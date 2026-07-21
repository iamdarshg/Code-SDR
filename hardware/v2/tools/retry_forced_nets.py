#!/usr/bin/env python3
"""Retry the nets that fell back to a direct/forced connection in the first
autoroute pass, now that the rest of the board is routed and stable.
Removes their existing tracks/vias and re-searches with a bigger budget.
"""

from __future__ import annotations

import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import auto_router as ar
import pcbnew

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"


def main():
    t_start = time.time()
    board = pcbnew.LoadBoard(str(PCB_PATH))

    forced_file = ROOT / "build" / "autoroute_forced_nets.txt"
    names = sorted(set(l.strip() for l in forced_file.read_text().splitlines() if l.strip()))
    print(f"Retrying {len(names)} distinct nets: {names}")

    fps = board.GetFootprints()
    pads_by_net = {}
    for fp in fps:
        for pad in fp.Pads():
            net = pad.GetNetname()
            if net == "":
                continue
            pads_by_net.setdefault(net, []).append(pad)

    obstacles = ar.Obstacles(board)

    still_forced = []
    total_new_tracks = 0
    total_new_vias = 0

    for name in names:
        net = board.FindNet(name)
        if net is None:
            continue
        # remove existing tracks/vias on this net
        to_remove = [t for t in board.GetTracks() if t.GetNetname() == name]
        for t in to_remove:
            board.Remove(t)
        print(f"Removed {len(to_remove)} existing segments/vias for {name}")

        pads = pads_by_net.get(name, [])
        if len(pads) < 2:
            continue
        allow_vias = not ar.is_rf_net(name)
        width_mm, _, _ = ar.net_electrical_class(name)
        edges = ar.nearest_neighbor_chain(pads)

        extra_attempts = [
            dict(extra_pad_mm=60.0, track_width_mm=width_mm, clearance_mm=0.12, pitch_mm=0.2),
            dict(extra_pad_mm=100.0, track_width_mm=0.1, clearance_mm=0.1, pitch_mm=0.3),
            dict(extra_pad_mm=160.0, track_width_mm=0.1, clearance_mm=0.08, pitch_mm=0.4),
        ]

        for (pa, pb) in edges:
            posa = pa.GetPosition()
            posb = pb.GetPosition()
            src = (pcbnew.ToMM(posa.x), pcbnew.ToMM(posa.y))
            dst = (pcbnew.ToMM(posb.x), pcbnew.ToMM(posb.y))
            src_layers = ar.pad_layer_set(pa)
            dst_layers = ar.pad_layer_set(pb)
            if not allow_vias:
                src_layers = {pcbnew.F_Cu} if pcbnew.F_Cu in src_layers else src_layers
                dst_layers = {pcbnew.F_Cu} if pcbnew.F_Cu in dst_layers else dst_layers
            if src == dst:
                continue
            path, width_used = ar.route_edge_with_fallback(
                obstacles, src, dst, allow_vias, name, src_layers=src_layers, dst_layers=dst_layers,
                preferred_width_mm=width_mm)
            if path is None:
                for kw in extra_attempts:
                    path = ar.build_grid_and_search(obstacles, src, dst, allow_vias, name,
                                                     src_layers=src_layers, dst_layers=dst_layers, **kw)
                    if path is not None:
                        width_used = kw["track_width_mm"]
                        break
            if path is None:
                path = ar.direct_fallback_path(src, dst, allow_vias, src_layers=src_layers, dst_layers=dst_layers)
                width_used = width_mm
                still_forced.append(name)
            path = ar.simplify_path(path)
            nt, nv = ar.add_track_path(board, path, net, width_used)
            total_new_tracks += nt
            total_new_vias += nv

    print(f"\nRetry done. still_forced={len(still_forced)}: {still_forced}")
    print(f"New tracks: {total_new_tracks}  New vias: {total_new_vias}")

    board.Save(str(PCB_PATH))
    print(f"Saved: {PCB_PATH}")
    print(f"Elapsed: {time.time()-t_start:.0f}s")

    with open(ROOT / "build" / "autoroute_forced_nets.txt", "w") as f:
        f.write("\n".join(still_forced))


if __name__ == "__main__":
    main()
