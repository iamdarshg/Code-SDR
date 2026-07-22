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

    def pads_for_net(net_name):
        """Re-scan footprints fresh each call instead of trusting a cached
        pad list -- heavy programmatic track add/remove has been observed to
        leave stale SWIG pad wrappers behind in this KiCad 9 binding."""
        out = []
        for fp in board.GetFootprints():
            for pad in fp.Pads():
                if pad.GetNetname() == net_name:
                    out.append(pad)
        return out

    obstacles = ar.Obstacles(board)

    still_forced = []
    total_new_tracks = 0
    total_new_vias = 0

    # detect diff-pair partners where BOTH members were requested together --
    # routing them independently (the old behavior) breaks the mirrored
    # coupling that keeps a differential pair's length/spacing matched, which
    # is exactly why some of these ended up as bare single-segment fallback
    # lines instead of a real coupled pair.
    names_set = set(names)
    handled_as_partner = set()
    pairs = []  # (p_name, n_name)
    solo_names = []
    for name in names:
        if name in handled_as_partner:
            continue
        info = ar.diff_pair_partner_suffix(name)
        if info is None:
            solo_names.append(name)
            continue
        base, my_suf, partner_suf = info
        partner = base + partner_suf
        if partner in names_set and partner not in handled_as_partner:
            if my_suf in ("_P", "_DP"):
                pairs.append((name, partner))
            else:
                pairs.append((partner, name))
            handled_as_partner.add(name)
            handled_as_partner.add(partner)
        else:
            solo_names.append(name)

    if pairs:
        print(f"Detected {len(pairs)} coupled diff pairs among requested nets: {pairs}")

    for (name_p, name_n) in pairs:
        try:
            net_p = board.FindNet(name_p)
            net_n = board.FindNet(name_n)
            if net_p is None or net_n is None:
                continue
            to_remove = [t for t in board.GetTracks() if t.GetNetname() in (name_p, name_n)]
            for t in to_remove:
                board.Remove(t)
            print(f"Removed {len(to_remove)} existing segments/vias for pair {name_p}/{name_n}")

            pads_p = pads_for_net(name_p)
            pads_n = pads_for_net(name_n)
            if len(pads_p) != 2 or len(pads_n) != 2:
                # not a clean 2-pad pair any more -- treat as solo nets instead
                solo_names.extend([name_p, name_n])
                continue
            allow_vias = not ar.is_rf_net(name_p)
            width_mm, _, gap_mm = ar.net_electrical_class(name_p)
            nt, nv, forced_p, forced_n = ar.route_diff_pair(
                obstacles, board, net_p, net_n, pads_p[0], pads_p[1], pads_n[0], pads_n[1],
                name_p, name_n, allow_vias, width_mm, gap_mm)
            total_new_tracks += nt
            total_new_vias += nv
            if forced_p:
                still_forced.append(name_p)
            if forced_n:
                still_forced.append(name_n)
        except Exception as e:
            print(f"ERROR processing pair {name_p}/{name_n}: {e!r} -- skipping, leaving as forced")
            still_forced.extend([name_p, name_n])
            continue

    names = solo_names
    for idx, name in enumerate(names):
        if idx > 0 and idx % 20 == 0:
            board.Save(str(PCB_PATH))
            print(f"[checkpoint] saved after {idx}/{len(names)} nets")
        try:
            net = board.FindNet(name)
            if net is None:
                continue
            # remove existing tracks/vias on this net
            to_remove = [t for t in board.GetTracks() if t.GetNetname() == name]
            for t in to_remove:
                board.Remove(t)
            print(f"Removed {len(to_remove)} existing segments/vias for {name}")

            pads = pads_for_net(name)
            if len(pads) < 2:
                continue
            allow_vias = not ar.is_rf_net(name)
            width_mm, _, _ = ar.net_electrical_class(name)
            # big spread-out nets (power rails) branch much more cleanly as
            # an MST than as a forced serial chain, which is prone to
            # zig-zagging across the whole board and self-crossing
            if len(pads) > 6:
                edges = ar.minimum_spanning_tree_edges(pads)
            else:
                edges = ar.nearest_neighbor_chain(pads)

            extra_attempts = [
                # never search below the board's true 0.1mm min clearance --
                # a coarser grid pitch needs *more* margin, not less, to
                # avoid landing a "just barely blocked" path that DRC then
                # flags as a marginal clearance violation.
                # margin_cap_mm must be raised explicitly here -- it used to
                # default to a hardcoded 30mm inside build_grid_and_search
                # regardless of extra_pad_mm, which silently defeated these
                # "search wider" attempts and pushed long nets straight to
                # direct_fallback_path (a straight line through everything).
                dict(extra_pad_mm=60.0, track_width_mm=width_mm, clearance_mm=0.15, pitch_mm=0.15,
                     margin_cap_mm=60.0, max_cells=800000, max_pops=600000),
                dict(extra_pad_mm=100.0, track_width_mm=0.1, clearance_mm=0.12, pitch_mm=0.18,
                     margin_cap_mm=100.0, max_cells=1500000, max_pops=1000000),
                dict(extra_pad_mm=160.0, track_width_mm=0.1, clearance_mm=0.1, pitch_mm=0.2,
                     margin_cap_mm=170.0, max_cells=2500000, max_pops=1800000),
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
        except Exception as e:
            print(f"ERROR processing net {name}: {e!r} -- skipping, leaving as forced")
            still_forced.append(name)
            continue

    print(f"\nRetry done. still_forced={len(still_forced)}: {still_forced}")
    print(f"New tracks: {total_new_tracks}  New vias: {total_new_vias}")

    board.Save(str(PCB_PATH))
    print(f"Saved: {PCB_PATH}")
    print(f"Elapsed: {time.time()-t_start:.0f}s")

    with open(ROOT / "build" / "autoroute_forced_nets.txt", "w") as f:
        f.write("\n".join(still_forced))


if __name__ == "__main__":
    main()
