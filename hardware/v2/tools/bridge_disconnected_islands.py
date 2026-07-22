#!/usr/bin/env python3
"""Fix genuine electrical opens: nets whose pads split into more than one
connected copper island (found by check_full_connectivity.py, which
treats pads/vias as real areas rather than exact points -- so these are
real gaps, not measurement artifacts).

For each affected net, repeatedly finds the closest pair of points
between any two remaining islands and bridges them with a short straight
track segment (plus a via if the two points are on different layers),
then merges those islands, until the whole net is one component. Gaps
found so far are all small (tens of micrometers to ~0.1mm) -- leftover
seams from repeated declutter/reroute/nudge passes on this board -- so a
short direct bridge is appropriate rather than a full pathfind.
"""
from __future__ import annotations

import sys
from pathlib import Path
from collections import defaultdict

sys.path.insert(0, str(Path(__file__).resolve().parent))
import auto_router as ar
import pcbnew

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"
SEG_TOL = 0.02
MAX_BRIDGE_MM = 2.0  # sanity cap -- anything needing a longer bridge gets flagged, not auto-connected


def mm(v):
    return pcbnew.ToMM(v)


def to_iu(v):
    return pcbnew.FromMM(v)


def analyze_net(board, net_name, zone_nets):
    pads = []
    pad_objs = []
    for fp in board.GetFootprints():
        for pad in fp.Pads():
            if pad.GetNetname() == net_name:
                bb = pad.GetBoundingBox()
                pads.append((mm(bb.GetLeft()), mm(bb.GetTop()), mm(bb.GetRight()), mm(bb.GetBottom())))
                pad_objs.append(pad)

    segs = []
    vias = []
    for t in board.GetTracks():
        if t.GetNetname() != net_name:
            continue
        if isinstance(t, pcbnew.PCB_VIA):
            pos = t.GetPosition()
            vias.append((mm(pos.x), mm(pos.y), mm(t.GetDrill()) / 2.0 + 0.1))
        else:
            s, e = t.GetStart(), t.GetEnd()
            segs.append(((mm(s.x), mm(s.y), t.GetLayer()), (mm(e.x), mm(e.y), t.GetLayer())))

    seg_points = set()
    for a, b in segs:
        seg_points.add(a); seg_points.add(b)
    seg_point_list = list(seg_points)

    n_pads, n_segpts = len(pads), len(seg_point_list)
    total = n_pads + n_segpts + len(vias)
    parent = list(range(total))

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    def union(a, b):
        ra, rb = find(a), find(b)
        if ra != rb:
            parent[ra] = rb

    seg_point_index = {p: n_pads + i for i, p in enumerate(seg_point_list)}
    via_base = n_pads + n_segpts

    for a, b in segs:
        union(seg_point_index[a], seg_point_index[b])

    for p in seg_point_list:
        for q in seg_point_list:
            if p != q and p[2] == q[2] and abs(p[0] - q[0]) <= SEG_TOL and abs(p[1] - q[1]) <= SEG_TOL:
                union(seg_point_index[p], seg_point_index[q])

    def in_bbox(px, py, l, t, r, b, margin=0.03):
        return (l - margin) <= px <= (r + margin) and (t - margin) <= py <= (b + margin)

    for i, (l, t, r, b) in enumerate(pads):
        for p in seg_point_list:
            if in_bbox(p[0], p[1], l, t, r, b):
                union(i, seg_point_index[p])

    for vi, (vx, vy, vr) in enumerate(vias):
        vid = via_base + vi
        for p in seg_point_list:
            if (p[0] - vx) ** 2 + (p[1] - vy) ** 2 <= (vr + 0.03) ** 2:
                union(vid, seg_point_index[p])
        for i, (l, t, r, b) in enumerate(pads):
            cx = min(max(vx, l), r)
            cy = min(max(vy, t), b)
            if (cx - vx) ** 2 + (cy - vy) ** 2 <= (vr + 0.03) ** 2:
                union(i, vid)
    for vi1 in range(len(vias)):
        for vi2 in range(vi1 + 1, len(vias)):
            x1, y1, r1 = vias[vi1]
            x2, y2, r2 = vias[vi2]
            if (x1 - x2) ** 2 + (y1 - y2) ** 2 <= (r1 + r2 + 0.03) ** 2:
                union(via_base + vi1, via_base + vi2)

    # collect representative "anchor" points per group (pad center preferred,
    # else via, else any segment endpoint) for bridging
    groups = defaultdict(list)  # root -> list of (x, y, layer_or_None, kind)
    for i, (l, t, r, b) in enumerate(pads):
        cx, cy = (l + r) / 2.0, (t + b) / 2.0
        groups[find(i)].append((cx, cy, None, "pad"))
    for p in seg_point_list:
        groups[find(seg_point_index[p])].append((p[0], p[1], p[2], "seg"))
    for vi, (vx, vy, vr) in enumerate(vias):
        groups[find(via_base + vi)].append((vx, vy, None, "via"))

    return groups


def bridge_two_points(board, net, name, p1, p2):
    x1, y1, l1, k1 = p1
    x2, y2, l2, k2 = p2
    layer = l1 if l1 is not None else (l2 if l2 is not None else pcbnew.F_Cu)
    width_mm, _, _ = ar.net_electrical_class(name)

    if l1 is not None and l2 is not None and l1 != l2:
        # need a via partway -- put it at p2, run the bridge on l1, then a via
        seg = pcbnew.PCB_TRACK(board)
        seg.SetStart(pcbnew.VECTOR2I(to_iu(x1), to_iu(y1)))
        seg.SetEnd(pcbnew.VECTOR2I(to_iu(x2), to_iu(y2)))
        seg.SetWidth(to_iu(width_mm))
        seg.SetLayer(l1)
        seg.SetNet(net)
        board.Add(seg)
        via = pcbnew.PCB_VIA(board)
        via.SetPosition(pcbnew.VECTOR2I(to_iu(x2), to_iu(y2)))
        via.SetWidth(to_iu(ar.VIA_DIAMETER_MM))
        via.SetDrill(to_iu(ar.VIA_DRILL_MM))
        via.SetNet(net)
        board.Add(via)
    else:
        seg = pcbnew.PCB_TRACK(board)
        seg.SetStart(pcbnew.VECTOR2I(to_iu(x1), to_iu(y1)))
        seg.SetEnd(pcbnew.VECTOR2I(to_iu(x2), to_iu(y2)))
        seg.SetWidth(to_iu(width_mm))
        seg.SetLayer(layer)
        seg.SetNet(net)
        board.Add(seg)


def main():
    board = pcbnew.LoadBoard(str(PCB_PATH))
    zone_nets = {z.GetNetname() for z in board.Zones()}

    target_nets = sys.argv[1:]
    if not target_nets:
        print("usage: bridge_disconnected_islands.py NET1 NET2 ...")
        return

    fixed = []
    unresolved = []

    for net_name in target_nets:
        net = board.FindNet(net_name)
        if net is None:
            print(f"{net_name}: net not found, skipping")
            continue

        # iteratively bridge nearest islands until 1 group remains, re-analyzing
        # from scratch each time since board state changed
        for _ in range(20):
            groups = analyze_net(board, net_name, zone_nets)
            if len(groups) <= 1:
                break
            group_list = list(groups.values())
            best = None
            for gi in range(len(group_list)):
                for gj in range(gi + 1, len(group_list)):
                    for p1 in group_list[gi]:
                        for p2 in group_list[gj]:
                            d = ((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2) ** 0.5
                            if best is None or d < best[0]:
                                best = (d, p1, p2)
            if best is None:
                break
            d, p1, p2 = best
            if d > MAX_BRIDGE_MM:
                print(f"{net_name}: closest remaining gap is {d:.3f}mm (> {MAX_BRIDGE_MM}mm cap), "
                      f"not auto-bridging -- needs a real reroute")
                unresolved.append(net_name)
                break
            bridge_two_points(board, net, net_name, p1, p2)
            print(f"{net_name}: bridged {d*1000:.0f}um gap between {p1[3]} and {p2[3]}")
        else:
            unresolved.append(net_name)
            continue

        groups = analyze_net(board, net_name, zone_nets)
        if len(groups) <= 1:
            fixed.append(net_name)
        else:
            unresolved.append(net_name)

    board.Save(str(PCB_PATH))
    print(f"\nFixed: {len(fixed)} {fixed}")
    print(f"Unresolved (need manual reroute): {len(unresolved)} {unresolved}")


if __name__ == "__main__":
    main()
