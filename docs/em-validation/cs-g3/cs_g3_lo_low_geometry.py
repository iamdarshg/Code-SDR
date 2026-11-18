"""CS-G3 geometry: LO_LOW_P/N tracks, the planes above/below them, and the
nearest GND stitching vias / plane-bridging caps at each split crossing.
Run with KiCad's python: $KICAD_PY cs_g3_lo_low_geometry.py <board> <out.json>"""
import json, sys, math
import pcbnew
b = pcbnew.LoadBoard(sys.argv[1]); mm = pcbnew.ToMM
L = {b.GetLayerName(l): l for l in range(pcbnew.PCB_LAYER_ID_COUNT) if b.IsLayerEnabled(l)}
out = {"stackup_copper": [n for n in L if n.endswith(".Cu")]}
nets = ["LO_LOW_P", "LO_LOW_N"]
tracks = {n: [] for n in nets}; vias = {n: [] for n in nets}
for t in b.GetTracks():
    n = t.GetNetname().split("/")[-1]
    if n not in nets: continue
    if t.GetClass() == "PCB_VIA":
        vias[n].append([round(mm(t.GetPosition().x), 3), round(mm(t.GetPosition().y), 3)])
    else:
        tracks[n].append({"layer": b.GetLayerName(t.GetLayer()), "w": round(mm(t.GetWidth()), 3),
                          "a": [round(mm(t.GetStart().x), 3), round(mm(t.GetStart().y), 3)],
                          "b": [round(mm(t.GetEnd().x), 3), round(mm(t.GetEnd().y), 3)],
                          "len": round(mm(t.GetLength()), 3)})
out["tracks"], out["vias"] = tracks, vias
# zones on inner layers: which net fills under each sample point along the tracks
zones = [z for z in b.Zones() if not z.GetIsRuleArea()]
def nets_at(layer_name, x, y):
    p = pcbnew.VECTOR2I(pcbnew.FromMM(x), pcbnew.FromMM(y)); lid = L[layer_name]; hit = []
    for z in zones:
        if z.IsOnLayer(lid) and z.HitTestFilledArea(lid, p):
            hit.append(z.GetNetname())
    return sorted(set(hit)) or ["<none>"]
samples = {}
for n in nets:
    seq = []
    for s in tracks[n]:
        ax, ay = s["a"]; bx, by = s["b"]; k = max(2, int(s["len"] / 0.25))
        for i in range(k + 1):
            x, y = ax + (bx - ax) * i / k, ay + (by - ay) * i / k
            seq.append({"xy": [round(x, 3), round(y, 3)], "trace_layer": s["layer"],
                        **{ly: nets_at(ly, x, y) for ly in ("In1.Cu", "In2.Cu")}})
    samples[n] = seq
out["reference_samples"] = samples
# transitions in the adjacent plane under each trace
def adj(layer): return {"B.Cu": "In2.Cu", "F.Cu": "In1.Cu"}.get(layer)
trans = {}
for n in nets:
    tr, prev = [], None
    for s in samples[n]:
        a = adj(s["trace_layer"]); cur = tuple(s[a]) if a else ("<inner trace>",)
        if prev is not None and cur != prev[1]:
            tr.append({"at": s["xy"], "from": list(prev[1]), "to": list(cur)})
        prev = (s["xy"], cur)
    trans[n] = tr
out["adjacent_plane_transitions"] = trans
# GND vias and caps between plane nets near each transition
gnd_vias = [t for t in b.GetTracks() if t.GetClass() == "PCB_VIA" and t.GetNetname().split("/")[-1] == "GND"]
def near_gnd(x, y):
    d = sorted(math.hypot(mm(v.GetPosition().x) - x, mm(v.GetPosition().y) - y) for v in gnd_vias)
    return [round(v, 3) for v in d[:3]]
caps = []
for fp in b.GetFootprints():
    if not fp.GetReference().startswith("C"): continue
    pn = sorted({p.GetNetname().split("/")[-1] for p in fp.Pads()})
    caps.append((fp.GetReference(), pn, mm(fp.GetPosition().x), mm(fp.GetPosition().y), fp.GetValue()))
for n in nets:
    for t in trans[n]:
        x, y = t["at"]; t["nearest_gnd_vias_mm"] = near_gnd(x, y)
        planes = {p.split("/")[-1] for p in t["from"] + t["to"] if p != "<none>"}
        br = [(r, round(math.hypot(cx - x, cy - y), 2), v, pn) for r, pn, cx, cy, v in caps
              if len(pn) == 2 and set(pn) <= (planes | {"GND"}) and len(set(pn) & planes) >= 1
              and math.hypot(cx - x, cy - y) < 10]
        t["caps_within_10mm_bridging_these_planes_or_gnd"] = sorted(br, key=lambda r: r[1])[:6]
# pair spacing
P, N = tracks["LO_LOW_P"], tracks["LO_LOW_N"]
def segd(p, s):
    (ax, ay), (bx, by) = s["a"], s["b"]; dx, dy = bx - ax, by - ay; L2 = dx * dx + dy * dy or 1e-12
    u = max(0, min(1, ((p[0] - ax) * dx + (p[1] - ay) * dy) / L2)); return math.hypot(ax + u * dx - p[0], ay + u * dy - p[1])
gaps = [min(segd(s["xy"], q) for q in N if q["layer"] == s["trace_layer"]) for s in samples["LO_LOW_P"] if any(q["layer"] == s["trace_layer"] for q in N)]
out["pair_centre_distance_mm"] = {"min": round(min(gaps), 3), "median": round(sorted(gaps)[len(gaps)//2], 3), "max": round(max(gaps), 3)} if gaps else None
import re
txt = open(sys.argv[1]).read(); k = txt.find("(stackup"); blk = txt[k:txt.find("(copper_finish", k)]
out["stackup"] = [{"name": m.group(1), "thk_mm": float(m.group(2))} for m in re.finditer(r'\(layer "([^"]+)"[^()]*?\(type "[^"]+"\)\s*(?:\(color "[^"]*"\)\s*)?\(thickness ([0-9.]+)', blk)]
json.dump(out, open(sys.argv[2], "w"), indent=1)
print(json.dumps({k: out[k] for k in ("adjacent_plane_transitions", "pair_centre_distance_mm", "vias")}, indent=1))
print(json.dumps([[s["layer"], s["w"], s["len"]] for n in nets for s in tracks[n]]))
print(json.dumps(out["stackup"]))
