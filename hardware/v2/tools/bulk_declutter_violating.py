#!/usr/bin/env python3
"""Bulk-remove copper for every currently-violating non-power signal net
(clearance/shorting_items/tracks_crossing, per the latest DRC report), then
write a priority-ordered reroute list (RF/analog first, general next, noisy
digital last; diff-pair partners grouped on one line so the retry loop
routes them coupled).

Keeps the ~66 currently-clean nets' copper in place as real obstacles for
the reroute pass -- a full from-scratch strip-and-reroute was tried earlier
this session and produced a WORSE result (50 stuck nets vs. the ~5 we have
now), so this reuses the working baseline as scaffolding instead of
discarding it.
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import auto_router as ar
import pcbnew

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"

SKIP = {"GND", "+3V3_ANA", "+3V3_DIG", "+1V2_CORE", "+5V_RF", "+2V5_AUX", "CHASSIS_GND"}


def main():
    drc_report = ROOT / "build" / "drc-report23.json"
    d = json.loads(drc_report.read_text())
    violating = set()
    for x in d.get("violations", []):
        if x.get("type") not in ("clearance", "shorting_items", "tracks_crossing"):
            continue
        for it in x.get("items", []):
            m = re.match(r"(?:Track|Via) \[([^\]]+)\]", it.get("description", ""))
            if m:
                violating.add(m.group(1))
    violating -= SKIP
    print(f"Violating non-power nets: {len(violating)}")

    board = pcbnew.LoadBoard(str(PCB_PATH))

    pads_by_net = {}
    for fp in board.GetFootprints():
        for pad in fp.Pads():
            n = pad.GetNetname()
            if n:
                pads_by_net.setdefault(n, []).append(pad)

    targets = sorted(n for n in violating if n in pads_by_net)
    print(f"Targets with pads on board: {len(targets)}")

    to_remove = [t for t in board.GetTracks() if t.GetNetname() in violating]
    print(f"Removing {len(to_remove)} existing segments/vias")
    for t in to_remove:
        board.Remove(t)
    board.Save(str(PCB_PATH))
    print(f"Saved decluttered board: {PCB_PATH}")

    # priority order: analog/RF first, general next, noisy digital last;
    # within each tier smaller nets (fewer pads) first, mirroring main()'s
    # tiering logic in auto_router.py
    def sort_key(n):
        return (ar.is_noisy_digital_net(n), not ar.is_analog_net(n), len(pads_by_net[n]))

    targets_set = set(targets)
    handled = set()
    groups = []
    for n in sorted(targets, key=sort_key):
        if n in handled:
            continue
        info = ar.diff_pair_partner_suffix(n)
        if info is None:
            groups.append((n,))
            handled.add(n)
            continue
        base, my_suf, partner_suf = info
        partner = base + partner_suf
        if partner in targets_set and partner not in handled:
            if my_suf in ("_P", "_DP"):
                groups.append((n, partner))
            else:
                groups.append((partner, n))
            handled.add(n)
            handled.add(partner)
        else:
            groups.append((n,))
            handled.add(n)

    out_file = ROOT / "build" / "mass_reroute_groups.txt"
    with open(out_file, "w") as f:
        for g in groups:
            f.write(",".join(g) + "\n")
    print(f"Wrote {len(groups)} priority-ordered groups to {out_file}")


if __name__ == "__main__":
    main()
