#!/usr/bin/env python3
"""For a genuinely stuck net (still falling back to a direct/straight-line
connection even after the oriented-rect obstacle fix), the usual cause is
order-dependence: some OTHER net sharing the same crowded footprint(s) got
routed first and claimed the only viable escape corridor from a fine-pitch
part (BGA/QFN), starving whichever net is processed last.

This script: for the target net(s) given on the command line (or via
build/priority_reroute_nets.txt, one name per line), finds every other
already-routed net that shares a pad with the SAME footprint(s) as the
target, removes both the target's and those siblings' existing copper, then
re-routes with the target net(s) FIRST (so it gets first pick of the tight
corridor) and the siblings afterward (they route around the target's new
copper -- since they succeeded once before under worse conditions, they
should generally still succeed).

Skips GND and the 4 main power rails as "siblings" -- those are handled by
their own dedicated declutter-and-reflow pass and touch far too many
footprints board-wide to usefully re-route here.
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

SKIP_SIBLINGS = {"GND", "+3V3_ANA", "+3V3_DIG", "+1V2_CORE", "+5V_RF"}
# High-fanout hub footprints already handled by their own dedicated pass --
# including them in sibling discovery would pull in 100+ unrelated nets
# board-wide instead of just the actually-congested destination-side
# footprint, since nearly every net on the board touches U10 (the RP2040).
HUB_FOOTPRINTS_EXCLUDE = {"U10"}


def main():
    targets_file = ROOT / "build" / "priority_reroute_nets.txt"
    targets = sorted(set(l.strip() for l in targets_file.read_text().splitlines() if l.strip()))
    print(f"Priority targets: {targets}")

    board = pcbnew.LoadBoard(str(PCB_PATH))

    def pads_for_net(net_name):
        out = []
        for fp in board.GetFootprints():
            for pad in fp.Pads():
                if pad.GetNetname() == net_name:
                    out.append(pad)
        return out

    # find footprints touched by target nets
    fps_touched = set()
    for fp in board.GetFootprints():
        for pad in fp.Pads():
            if pad.GetNetname() in targets:
                fps_touched.add(fp.GetReference())
    fps_touched -= HUB_FOOTPRINTS_EXCLUDE
    print(f"Footprints touched by targets (excluding hubs): {sorted(fps_touched)}")

    siblings = set()
    for fp in board.GetFootprints():
        if fp.GetReference() not in fps_touched:
            continue
        for pad in fp.Pads():
            nn = pad.GetNetname()
            if nn and nn not in targets and nn not in SKIP_SIBLINGS:
                siblings.add(nn)
    print(f"Sibling nets to declutter+reflow ({len(siblings)}): {sorted(siblings)}")

    order = list(targets) + sorted(siblings)
    to_remove = [t for t in board.GetTracks() if t.GetNetname() in set(order)]
    print(f"Removing {len(to_remove)} existing segments/vias across {len(order)} nets")
    for t in to_remove:
        board.Remove(t)

    obstacles = ar.Obstacles(board)
    still_forced = []
    total_new_tracks = 0
    total_new_vias = 0
    t_start = time.time()

    # diff-pair coupling among the ordered list (both sides must be present)
    order_set = set(order)
    handled_as_partner = set()
    units = []
    for name in order:
        if name in handled_as_partner:
            continue
        info = ar.diff_pair_partner_suffix(name)
        if info is None:
            units.append((name,))
            continue
        base, my_suf, partner_suf = info
        partner = base + partner_suf
        if partner in order_set and partner not in handled_as_partner:
            if my_suf in ("_P", "_DP"):
                units.append((name, partner))
            else:
                units.append((partner, name))
            handled_as_partner.add(name)
            handled_as_partner.add(partner)
        else:
            units.append((name,))

    for idx, unit in enumerate(units):
        if idx > 0 and idx % 15 == 0:
            board.Save(str(PCB_PATH))
            print(f"[checkpoint] saved after {idx}/{len(units)} units")
        try:
            if len(unit) == 2:
                name_p, name_n = unit
                net_p = board.FindNet(name_p)
                net_n = board.FindNet(name_n)
                if net_p is None or net_n is None:
                    continue
                pads_p = pads_for_net(name_p)
                pads_n = pads_for_net(name_n)
                if len(pads_p) != 2 or len(pads_n) != 2:
                    unit = (name_p,)  # fall through to solo handling below for name_p
                    units.append((name_n,))
                else:
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
                    continue

            name = unit[0]
            net = board.FindNet(name)
            if net is None:
                continue
            pads = pads_for_net(name)
            if len(pads) < 2:
                continue
            allow_vias = not ar.is_rf_net(name)
            width_mm, _, _ = ar.net_electrical_class(name)
            if len(pads) > 6:
                edges = ar.minimum_spanning_tree_edges(pads)
            else:
                edges = ar.nearest_neighbor_chain(pads)

            extra_attempts = [
                dict(extra_pad_mm=60.0, track_width_mm=width_mm, clearance_mm=0.15, pitch_mm=0.15,
                     margin_cap_mm=60.0, max_cells=800000, max_pops=600000),
                dict(extra_pad_mm=100.0, track_width_mm=0.1, clearance_mm=0.12, pitch_mm=0.18,
                     margin_cap_mm=100.0, max_cells=1500000, max_pops=1000000),
                dict(extra_pad_mm=160.0, track_width_mm=0.1, clearance_mm=0.1, pitch_mm=0.2,
                     margin_cap_mm=170.0, max_cells=2500000, max_pops=1800000),
                # last-ditch: much finer pitch, for razor-thin gaps between
                # fine-pitch BGA/LGA pads that a coarser grid can't resolve
                # at all. Track width must never drop below the board's
                # 0.1mm minimum manufacturable width (a previous version of
                # this attempt used 0.08mm, which "succeeded" at pathfinding
                # but created guaranteed track_width DRC violations instead).
                dict(extra_pad_mm=20.0, track_width_mm=0.1, clearance_mm=0.08, pitch_mm=0.05,
                     margin_cap_mm=20.0, max_cells=1500000, max_pops=1200000),
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
            print(f"ERROR processing unit {unit}: {e!r} -- skipping, leaving as forced")
            still_forced.extend(unit)
            continue

    print(f"\nPriority reroute done. still_forced={len(still_forced)}: {still_forced}")
    print(f"New tracks: {total_new_tracks}  New vias: {total_new_vias}")
    board.Save(str(PCB_PATH))
    print(f"Saved: {PCB_PATH}")
    print(f"Elapsed: {time.time()-t_start:.0f}s")


if __name__ == "__main__":
    main()
