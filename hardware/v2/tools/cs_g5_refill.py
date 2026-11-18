"""Refill every copper zone of a KiCad board in place (pcbnew ZONE_FILLER) and report per-layer,
per-net filled area before/after. Only zone fills change; tracks/vias/pads are untouched.
usage: $KICAD_PY cs_g5_refill.py <board.kicad_pcb> <out.json>"""
import json, sys, pcbnew
b = pcbnew.LoadBoard(sys.argv[1])
def areas():
    out = {}
    for z in b.Zones():
        if z.GetIsRuleArea(): continue
        for lid in z.GetLayerSet().Seq():
            if not z.HasFilledPolysForLayer(lid): continue
            a = z.GetFilledPolysList(lid).Area() / 1e12  # nm^2 -> mm^2... (1e6 nm/mm)^2
            k = f"{b.GetLayerName(lid)}|{z.GetNetname()}"; out[k] = out.get(k, 0.0) + a
    return out
before = areas(); ntr = len(b.GetTracks())
filler = pcbnew.ZONE_FILLER(b); ok = filler.Fill(b.Zones())
after = areas()
pcbnew.SaveBoard(sys.argv[1], b)
diff = {k: round(after.get(k, 0) - before.get(k, 0), 4) for k in set(before) | set(after) if abs(after.get(k, 0) - before.get(k, 0)) > 1e-4}
rec = {"fill_ok": bool(ok), "tracks_and_vias": ntr, "changed_filled_area_mm2": diff, "total_change_mm2": round(sum(diff.values()), 4)}
json.dump(rec, open(sys.argv[2], "w"), indent=1); print(json.dumps(rec, indent=1))
