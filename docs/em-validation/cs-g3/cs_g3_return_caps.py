"""Nearest decoupling caps from the LO_LOW split crossing to GND for each plane, and any DIG<->ANA bridge."""
import sys, math, json, pcbnew
b = pcbnew.LoadBoard(sys.argv[1]); mm = pcbnew.ToMM; X, Y = float(sys.argv[2]), float(sys.argv[3])
rows = []
for fp in b.GetFootprints():
    if not fp.GetReference().startswith("C"): continue
    pn = tuple(sorted({p.GetNetname().split("/")[-1] for p in fp.Pads()}))
    d = math.hypot(mm(fp.GetPosition().x) - X, mm(fp.GetPosition().y) - Y)
    rows.append((round(d, 2), fp.GetReference(), fp.GetValue(), pn))
out = {}
for key in [("+3V3_DIG", "GND"), ("+3V3_ANA", "GND"), ("+3V3_ANA", "+3V3_DIG")]:
    m = sorted(r for r in rows if r[3] == tuple(sorted(key)))
    out[" / ".join(key)] = {"count_on_board": len(m), "nearest": m[:4]}
print(json.dumps(out, indent=1))
