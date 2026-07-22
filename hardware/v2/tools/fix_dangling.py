#!/usr/bin/env python3
"""One-off repair for the handful of dangling-stub track ends left by
surgical_fix.py before its endpoint-snapping fix (build_grid_and_search used
to return grid-quantized endpoints instead of the exact requested
src/dst, which is invisible when routing to/from a pad -- the gap lands
inside the pad's copper -- but produces a real disconnected stub when
routing between two bare mid-net waypoints, as surgical_fix.py's local
segment repair does).

For each given (net, x, y) dangling point: finds the track segment with an
endpoint there, finds whichever OTHER track endpoint (same net) is nearest
within a small search radius, and snaps the dangling endpoint exactly onto
it, closing the gap without changing the route's shape.
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pcbnew

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"

# (net_name, x_mm, y_mm) from the DRC report's dangling-track positions
DANGLING_POINTS = [
    ("DSA_CLK", 51.6961, 80.4461),
    ("DSA_CLK", 51.5661, 80.5161),
    ("PHY_XO", 118.15, 74.27),
    ("QSPI_SD1", 42.84, 86.7725),
    ("QSPI_SD1", 44.0, 85.5625),
    ("QSPI_SD1", 42.699999, 86.6625),
]


def mm(v):
    return pcbnew.ToMM(v)


def to_iu(v):
    return pcbnew.FromMM(v)


def main():
    board = pcbnew.LoadBoard(str(PCB_PATH))
    fixed = 0

    for net_name, x, y in DANGLING_POINTS:
        target = pcbnew.VECTOR2I(to_iu(x), to_iu(y))
        tol_iu = to_iu(0.05)

        # find the track whose start or end is at (x,y)
        dangling_track = None
        dangling_is_start = None
        for t in board.GetTracks():
            if t.GetNetname() != net_name or isinstance(t, pcbnew.PCB_VIA):
                continue
            s, e = t.GetStart(), t.GetEnd()
            if abs(s.x - target.x) <= tol_iu and abs(s.y - target.y) <= tol_iu:
                dangling_track = t
                dangling_is_start = True
                break
            if abs(e.x - target.x) <= tol_iu and abs(e.y - target.y) <= tol_iu:
                dangling_track = t
                dangling_is_start = False
                break
        if dangling_track is None:
            print(f"{net_name} @ ({x},{y}): could not find the track, skipping")
            continue

        my_layer = dangling_track.GetLayer()
        my_uuid = dangling_track.m_Uuid.AsString()

        # find the nearest other endpoint (same net, same layer, different
        # track) within a generous snap radius
        best = None
        best_d = None
        search_r_iu = to_iu(0.5)
        for t in board.GetTracks():
            if t.GetNetname() != net_name:
                continue
            if t.m_Uuid.AsString() == my_uuid:
                continue
            candidates = []
            if isinstance(t, pcbnew.PCB_VIA):
                candidates.append(("via", t.GetPosition()))
            else:
                if t.GetLayer() == my_layer:
                    candidates.append(("start", t.GetStart()))
                    candidates.append(("end", t.GetEnd()))
            for kind, pos in candidates:
                dx, dy = pos.x - target.x, pos.y - target.y
                d = (dx * dx + dy * dy) ** 0.5
                if d <= search_r_iu and (best_d is None or d < best_d):
                    best_d = d
                    best = pos

        # also check pads (same net) within the snap radius
        for fp in board.GetFootprints():
            for pad in fp.Pads():
                if pad.GetNetname() != net_name:
                    continue
                pos = pad.GetPosition()
                dx, dy = pos.x - target.x, pos.y - target.y
                d = (dx * dx + dy * dy) ** 0.5
                if d <= search_r_iu and (best_d is None or d < best_d):
                    best_d = d
                    best = pos

        if best is None:
            print(f"{net_name} @ ({x},{y}): no nearby connection point found within 0.5mm, "
                  f"leaving as-is (needs manual look)")
            continue

        if dangling_is_start:
            dangling_track.SetStart(best)
        else:
            dangling_track.SetEnd(best)
        print(f"{net_name} @ ({x:.4f},{y:.4f}): snapped {'start' if dangling_is_start else 'end'} "
              f"to ({mm(best.x):.4f},{mm(best.y):.4f}), gap was {mm(best_d):.4f}mm")
        fixed += 1

    board.Save(str(PCB_PATH))
    print(f"\nFixed {fixed}/{len(DANGLING_POINTS)}. Saved: {PCB_PATH}")


if __name__ == "__main__":
    main()
