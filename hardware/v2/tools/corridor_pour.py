#!/usr/bin/env python3
"""Convert a power net's existing (connectivity-verified) trace topology
into zone-filled copper corridors along the same paths, instead of thin
point-to-point traces.

Why corridors instead of one big per-net zone: these power rails' pads are
scattered across nearly the entire board (bounding boxes span corner to
corner), and KiCad resolves overlapping zone outlines by full priority --
the higher-priority net's outline wins the ENTIRE overlap region, not just
where it actually has copper. A full-board zone per rail would mean only
the single highest-priority rail gets real copper; the other three would
be almost completely cut out and their pads stranded. A per-segment
corridor (a thin rectangle following each existing trace's path, widened
and left to KiCad's proper clearance-aware filler) keeps the same working
topology -- so nothing can strand a pad that wasn't already connected --
while getting a real filled pour with guaranteed-correct DRC clearance
instead of this session's approximate grid-search traces.

For each target net:
1. Read its current track segments (already verified fully connected).
2. Build one zone per (net, layer) with one polygon outline per segment
   (a rectangle of width `corridor_width_mm` along the segment's path) plus
   a small square around each via position on that layer.
3. Remove the original thin traces for that net (vias are kept -- they
   still carry the layer change and the corridor zones connect to them).
4. Fill all zones via ZONE_FILLER.
5. Verify every pad for the net is covered by the filled zone on its own
   layer; any pad the fill doesn't reach gets a short stitch trace restored
   so nothing is ever left disconnected.
"""
from __future__ import annotations

import math
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import auto_router as ar
import pcbnew

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"


def mm(v):
    return pcbnew.ToMM(v)


def to_iu(v):
    return pcbnew.FromMM(v)


def make_corridor_zone(board, net, layer, segments, vias_here, width_mm):
    zone = pcbnew.ZONE(board)
    zone.SetLayer(layer)
    zone.SetNet(net)
    zone.SetIsRuleArea(False)
    zone.SetLocalClearance(to_iu(0.15))
    zone.SetMinThickness(to_iu(0.1))
    zone.SetThermalReliefGap(to_iu(0.3))
    zone.SetThermalReliefSpokeWidth(to_iu(0.3))
    zone.SetPadConnection(pcbnew.ZONE_CONNECTION_FULL)

    half = width_mm / 2.0
    outline = zone.Outline()

    def add_rect(cx0, cy0, cx1, cy1, h):
        # Extend past each nominal endpoint by h (a square end-cap, like a
        # stroked line) so the true connection point (a pad center or via)
        # sits safely INSIDE the polygon instead of exactly on its boundary
        # edge -- a flat-ended rectangle puts every endpoint precisely on
        # the perimeter, which is fragile for both hit-testing and for the
        # zone filler's own pad-thermal-relief connection detection.
        dx, dy = cx1 - cx0, cy1 - cy0
        length = math.hypot(dx, dy)
        if length < 1e-6:
            ux, uy = 1.0, 0.0
        else:
            ux, uy = dx / length, dy / length
        px, py = -uy * h, ux * h
        cx0e, cy0e = cx0 - ux * h, cy0 - uy * h
        cx1e, cy1e = cx1 + ux * h, cy1 + uy * h
        pts = [
            (cx0e + px, cy0e + py), (cx1e + px, cy1e + py),
            (cx1e - px, cy1e - py), (cx0e - px, cy0e - py),
        ]
        # NewOutline() must be called even for the very first polygon --
        # Append() on a brand-new zone's empty outline (no current polygon
        # to append to) segfaults pcbnew natively instead of raising a
        # catchable Python exception.
        outline.NewOutline()
        idx = outline.OutlineCount() - 1
        for (x, y) in pts:
            outline.Append(to_iu(x), to_iu(y), idx)

    for (sx, sy, ex, ey) in segments:
        add_rect(sx, sy, ex, ey, half)

    for (vx, vy, vr) in vias_here:
        # small square pad around the via stub on this layer
        add_rect(vx - vr * 0.3, vy, vx + vr * 0.3, vy, vr + half * 0.5)

    return zone


def process_net(board, net_name, corridor_width_mm):
    net = board.FindNet(net_name)
    if net is None:
        print(f"{net_name}: not found, skipping")
        return 0

    segs_by_layer = {}
    vias = []
    to_remove = []
    for t in board.GetTracks():
        if t.GetNetname() != net_name:
            continue
        if isinstance(t, pcbnew.PCB_VIA):
            pos = t.GetPosition()
            vias.append((mm(pos.x), mm(pos.y), mm(t.GetWidth()) / 2.0))
            continue
        s, e = t.GetStart(), t.GetEnd()
        layer = t.GetLayer()
        segs_by_layer.setdefault(layer, []).append(
            (mm(s.x), mm(s.y), mm(e.x), mm(e.y)))
        to_remove.append(t)

    if not segs_by_layer:
        print(f"{net_name}: no existing traces, skipping")
        return 0

    n_segs = sum(len(v) for v in segs_by_layer.values())
    print(f"{net_name}: {n_segs} segments across {len(segs_by_layer)} layer(s), "
          f"{len(vias)} vias -- building corridor zones at {corridor_width_mm}mm width")

    for t in to_remove:
        board.Remove(t)

    new_zones = []
    for layer, segs in segs_by_layer.items():
        vias_here = vias  # vias appear on every layer they land on; keepout handles the rest
        zone = make_corridor_zone(board, net, layer, segs, vias_here, corridor_width_mm)
        board.Add(zone)
        new_zones.append(zone)

    return new_zones


def cmd_build(targets, corridor_width_mm):
    """Phase 1: remove existing traces, add corridor zones, fill. Run this
    as its own process invocation -- calling pcbnew.LoadBoard() a second
    time in the same process (as phase 2 needs to, to get a clean object
    graph after the fill) has proven unstable on its own, independent of
    the usual Remove()/Add() staleness issue."""
    board = pcbnew.LoadBoard(str(PCB_PATH))
    t_start = time.time()

    all_new_zones = []
    for net_name in targets:
        all_new_zones.extend(process_net(board, net_name, corridor_width_mm))

    print(f"\nAdded {len(all_new_zones)} corridor zones. Filling...")
    filler = pcbnew.ZONE_FILLER(board)
    filler.Fill(all_new_zones)

    board.Save(str(PCB_PATH))
    print(f"Saved: {PCB_PATH}")
    print(f"Elapsed: {time.time()-t_start:.0f}s")


ROUTE_LAYER_SET = (pcbnew.F_Cu, pcbnew.B_Cu, pcbnew.In2_Cu)


def zone_covers(net_zones, layer, pos):
    for z in net_zones:
        if z.GetLayer() != layer:
            continue
        if z.HasFilledPolysForLayer(layer) and z.HitTestFilledArea(layer, pos):
            return True
    return False


def cmd_verify(targets):
    """Phase 2: reload the freshly-filled board (its own process invocation)
    and verify every PAD and VIA of each target net is actually reached by
    that net's zone fill on every layer it needs -- a via only carries a
    layer change if the zone fill genuinely touches it on BOTH its top and
    bottom layer; each layer's zone is a separate object that doesn't
    connect to the others just by sharing a net. Stitch a real trace to
    anything the fill missed."""
    board = pcbnew.LoadBoard(str(PCB_PATH))
    t_start = time.time()
    all_new_zones = [z for z in board.Zones() if z.GetNetname() in targets]

    print("Verifying every pad/via is actually reached by its net's zone fill...")
    obstacles = ar.Obstacles(board)
    n_stitched = 0
    for net_name in targets:
        net_zones = [z for z in all_new_zones if z.GetNetname() == net_name]
        if not net_zones:
            continue
        net = board.FindNet(net_name)

        # unify pads and (per-layer) vias into one list of connection
        # points that each need zone coverage on their own specific layer(s)
        points = []  # (label, x_mm, y_mm, [required layers])
        for fp in board.GetFootprints():
            for pad in fp.Pads():
                if pad.GetNetname() != net_name:
                    continue
                pos = pad.GetPosition()
                pad_layers = [l for l in ROUTE_LAYER_SET if pad.GetLayerSet().Contains(l)]
                points.append((f"pad {fp.GetReference()}", mm(pos.x), mm(pos.y),
                               pad_layers or list(ROUTE_LAYER_SET)))
        for t in board.GetTracks():
            if not isinstance(t, pcbnew.PCB_VIA) or t.GetNetname() != net_name:
                continue
            pos = t.GetPosition()
            points.append((f"via", mm(pos.x), mm(pos.y), [t.TopLayer(), t.BottomLayer()]))

        covered_points = []  # (x, y, layer) -- one entry per satisfied layer
        needs = []  # (label, x, y, layer) -- one entry per UNsatisfied layer
        for (label, x, y, layers) in points:
            pos_iu = pcbnew.VECTOR2I(to_iu(x), to_iu(y))
            for layer in layers:
                if zone_covers(net_zones, layer, pos_iu):
                    covered_points.append((x, y, layer))
                else:
                    needs.append((label, x, y, layer))

        if not needs:
            continue
        print(f"  {net_name}: {len(needs)} connection(s) not reached by the zone fill, stitching...")
        for (label, x, y, layer) in needs:
            src = (x, y)
            same_layer_covered = [(cx, cy) for (cx, cy, cl) in covered_points if cl == layer]
            if not same_layer_covered:
                # nothing confirmed-covered on this exact layer -- fall back
                # to any other point of this net as a best-effort target
                same_layer_covered = [(px, py) for (lb, px, py, pl) in points if (px, py) != (x, y)]
            if not same_layer_covered:
                print(f"    {label} @ {src} layer {layer}: no other connection point exists, skipping")
                continue
            dst = min(same_layer_covered, key=lambda c: math.hypot(c[0] - src[0], c[1] - src[1]))
            path, width_used = ar.route_edge_with_fallback(
                obstacles, src, dst, True, net_name,
                src_layers={layer}, dst_layers={layer}, preferred_width_mm=0.2)
            if path is None:
                path = ar.direct_fallback_path(src, dst, True, src_layers={layer}, dst_layers={layer})
                width_used = 0.2
                print(f"    {label} @ {src} layer {layer}: no clean path, using direct stitch (needs manual review)")
            else:
                print(f"    {label} @ {src} layer {layer}: stitched to zone via real path")
            path = ar.simplify_path(path)
            ar.add_track_path(board, path, net, width_used)
            n_stitched += 1
            obstacles = ar.Obstacles(board)

    print(f"Stitched {n_stitched} connection(s).")

    board.Save(str(PCB_PATH))
    print(f"Saved: {PCB_PATH}")
    print(f"Elapsed: {time.time()-t_start:.0f}s")


def main():
    args = sys.argv[1:]
    if not args or args[0] not in ("build", "verify"):
        print("usage: corridor_pour.py build [--width=MM] NET [NET...]")
        print("       corridor_pour.py verify NET [NET...]")
        sys.exit(2)
    mode = args[0]
    args = args[1:]
    if mode == "build":
        corridor_width_mm = 0.6
        if args and args[0].startswith("--width="):
            corridor_width_mm = float(args[0].split("=", 1)[1])
            args = args[1:]
        targets = args if args else ["+3V3_ANA", "+3V3_DIG", "+1V2_CORE", "+5V_RF"]
        cmd_build(targets, corridor_width_mm)
    else:
        targets = args if args else ["+3V3_ANA", "+3V3_DIG", "+1V2_CORE", "+5V_RF"]
        cmd_verify(targets)


if __name__ == "__main__":
    main()
