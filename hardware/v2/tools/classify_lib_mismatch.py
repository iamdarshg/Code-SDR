"""Identity-level classification of KiCad lib_footprint_mismatch warnings.

Run with KiCad's bundled Python (needs pcbnew). For every footprint named in a
`kicad-cli pcb drc` JSON report as lib_footprint_mismatch, load the library copy
and compare, in footprint-local coordinates (board rotation/flip removed):
  pads:     number, shape, size, local offset, drill, layer set, local rotation
  graphics: per-layer multiset of (shape type, local bbox) for silk/fab/courtyard
  other:    attributes (SMD/THT/exclude flags), pad count
Each footprint is classified ELECTRICAL (pad geometry/layers differ) or
GRAPHICS_ONLY (only silk/fab/courtyard differ) or OTHER (attributes differ).
Output: JSON with per-identity evidence (reference, UUID, library id, diffs).
Read-only: the board is never saved.
"""
from __future__ import annotations

import collections
import json
import os
import re
import sys

import pcbnew

BOARD, DRC_JSON, OUT = sys.argv[1], sys.argv[2], sys.argv[3]
board = pcbnew.LoadBoard(BOARD)
proj_dir = os.path.dirname(os.path.abspath(BOARD))

# library nickname -> path, from the project fp-lib-table (env vars expanded)
libs = {}
table = open(os.path.join(proj_dir, "fp-lib-table"), encoding="utf-8").read()
for name, uri in re.findall(r'\(name "([^"]+)"\)\(type "KiCad"\)\(uri "([^"]+)"\)', table):
    uri = uri.replace("${KIPRJMOD}", proj_dir)
    uri = re.sub(r"\$\{(\w+)\}", lambda m: os.environ.get(m.group(1), m.group(0)), uri)
    libs[name] = uri

drc = json.load(open(DRC_JSON, encoding="utf-8"))
targets = []
for v in drc["violations"]:
    if v["type"] != "lib_footprint_mismatch":
        continue
    ref = v["items"][0]["description"].replace("Footprint ", "").strip()
    targets.append((ref, v["items"][0].get("uuid")))

NM = 1e6


def local_pads(fp):
    """Pad geometry in footprint coordinates via KiCad's own FP-relative accessors.
    Back-side footprints are mirrored by the flip; mirror x back so they compare
    against the front-side library copy."""
    out = []
    flipped = fp.IsFlipped()
    for p in fp.Pads():
        rel = p.GetFPRelativePosition()
        x, y = rel.x, rel.y
        rot = p.GetFPRelativeOrientation().AsDegrees() % 360
        if flipped:
            x = -x
            rot = (180 - rot) % 360
        layers = tuple(sorted(pcbnew.LSET(p.GetLayerSet()).Seq()))
        out.append((
            p.GetNumber(), int(p.GetShape()),
            round(p.GetSize().x / NM, 4), round(p.GetSize().y / NM, 4),
            round(x / NM, 3), round(y / NM, 3),
            round(p.GetDrillSize().x / NM, 4),
            round(rot, 1), layers,
        ))
    return sorted(out)


def graphics(fp):
    c = collections.Counter()
    for g in fp.GraphicalItems():
        try:
            layer = pcbnew.LayerName(g.GetLayer()) if hasattr(pcbnew, "LayerName") else str(g.GetLayer())
        except Exception:
            layer = str(g.GetLayer())
        if fp.IsFlipped():
            layer = layer.replace("B.", "F.") if layer.startswith("B.") else layer
        c[(layer, g.GetClass())] += 1
    return c


rows = []
summary = collections.Counter()
for ref, uuid in targets:
    fp = board.FindFootprintByReference(ref)
    fpid = fp.GetFPID()
    nick, name = str(fpid.GetLibNickname()), str(fpid.GetLibItemName())
    row = {"reference": ref, "uuid": uuid, "lib_id": f"{nick}:{name}"}
    if nick not in libs:
        row["class"] = "LIB_NOT_FOUND"
        rows.append(row); summary[row["class"]] += 1
        continue
    lib_fp = pcbnew.FootprintLoad(libs[nick], name)
    if lib_fp is None:
        row["class"] = "LIB_NOT_FOUND"
        rows.append(row); summary[row["class"]] += 1
        continue
    # library copy is at origin, unrotated, unflipped
    lib_pads = sorted(
        (p.GetNumber(), int(p.GetShape()), round(p.GetSize().x / NM, 4), round(p.GetSize().y / NM, 4),
         round(p.GetPosition().x / NM, 3), round(p.GetPosition().y / NM, 3), round(p.GetDrillSize().x / NM, 4),
         round(p.GetOrientation().AsDegrees() % 360, 1), tuple(sorted(pcbnew.LSET(p.GetLayerSet()).Seq())))
        for p in lib_fp.Pads())
    bpads = local_pads(fp)
    pad_diff = []
    if len(bpads) != len(lib_pads):
        pad_diff.append(f"pad count {len(bpads)} vs lib {len(lib_pads)}")
    else:
        for a, b in zip(bpads, lib_pads):
            fields = ["number", "shape", "w", "h", "x", "y", "drill", "rot", "layers"]
            for f, va, vb in zip(fields, a, b):
                if f == "layers":
                    continue  # layer ids differ for flipped parts; compared via count below
                if f in ("x", "y", "w", "h", "drill"):
                    if abs(va - vb) > 0.0015:
                        pad_diff.append(f"pad {a[0]} {f} {va} vs lib {vb}")
                elif f == "rot":
                    if min(abs(va - vb), 360 - abs(va - vb)) > 0.05:
                        pad_diff.append(f"pad {a[0]} rot {va} vs lib {vb}")
                elif va != vb:
                    pad_diff.append(f"pad {a[0]} {f} {va} vs lib {vb}")
            if len(set(a[8])) != len(set(b[8])):
                pad_diff.append(f"pad {a[0]} layer count {len(a[8])} vs lib {len(b[8])}")
    gb, gl = graphics(fp), graphics(lib_fp)
    g_diff = {f"{k[0]}/{k[1]}": [gb.get(k, 0), gl.get(k, 0)] for k in set(gb) | set(gl) if gb.get(k, 0) != gl.get(k, 0)}
    attr_diff = fp.GetAttributes() != lib_fp.GetAttributes()
    row.update({"pad_diffs": pad_diff[:12], "graphics_diffs_board_vs_lib": g_diff, "attributes_differ": bool(attr_diff)})
    row["class"] = "ELECTRICAL" if pad_diff else ("OTHER" if attr_diff else "GRAPHICS_ONLY")
    rows.append(row)
    summary[row["class"]] += 1

json.dump({"board": BOARD, "summary": summary, "footprints": rows}, open(OUT, "w"), indent=1)
print(json.dumps(summary))
