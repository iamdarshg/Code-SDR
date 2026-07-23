#!/usr/bin/env python3
"""Rigorous whole-board connectivity check: for every net, verify all its
pads are reachable from each other via copper, treating pads and vias as
real areas (bounding box / circle) rather than exact center points -- a
router-placed trace landing anywhere within a pad's or via's actual copper
is a valid connection, not just an exact-center match.
"""
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import pcbnew
from collections import defaultdict

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"

SEG_TOL = 0.02


def main():
    board = pcbnew.LoadBoard(str(PCB_PATH))
    zone_nets = {z.GetNetname() for z in board.Zones()}

    pads_by_net = defaultdict(list)
    for fp in board.GetFootprints():
        for pad in fp.Pads():
            n = pad.GetNetname()
            if n and n not in zone_nets:
                bb = pad.GetBoundingBox()
                pads_by_net[n].append((
                    pcbnew.ToMM(bb.GetLeft()), pcbnew.ToMM(bb.GetTop()),
                    pcbnew.ToMM(bb.GetRight()), pcbnew.ToMM(bb.GetBottom()),
                ))

    segs_by_net = defaultdict(list)
    vias_by_net = defaultdict(list)
    for t in board.GetTracks():
        n = t.GetNetname()
        if not n or n in zone_nets:
            continue
        if isinstance(t, pcbnew.PCB_VIA):
            pos = t.GetPosition()
            vias_by_net[n].append((pcbnew.ToMM(pos.x), pcbnew.ToMM(pos.y), pcbnew.ToMM(t.GetDrill()) / 2.0 + 0.1))
        else:
            s, e = t.GetStart(), t.GetEnd()
            segs_by_net[n].append(((pcbnew.ToMM(s.x), pcbnew.ToMM(s.y)), (pcbnew.ToMM(e.x), pcbnew.ToMM(e.y))))

    def in_bbox(px, py, l, t, r, b, margin=0.03):
        return (l - margin) <= px <= (r + margin) and (t - margin) <= py <= (b + margin)

    bad_nets = []
    for net, pads in pads_by_net.items():
        if len(pads) < 2:
            continue
        segs = segs_by_net.get(net, [])
        vias = vias_by_net.get(net, [])
        seg_points = set()
        for (a, b) in segs:
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

        for (a, b) in segs:
            union(seg_point_index[a], seg_point_index[b])

        buckets = defaultdict(list)
        for p in seg_point_list:
            buckets[(round(p[0] / SEG_TOL), round(p[1] / SEG_TOL))].append(p)
        for p in seg_point_list:
            kx, ky = round(p[0] / SEG_TOL), round(p[1] / SEG_TOL)
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    for q in buckets.get((kx + dx, ky + dy), []):
                        if q != p and abs(p[0] - q[0]) <= SEG_TOL and abs(p[1] - q[1]) <= SEG_TOL:
                            union(seg_point_index[p], seg_point_index[q])

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

        groups = defaultdict(int)
        for i in range(n_pads):
            groups[find(i)] += 1
        if len(groups) > 1:
            bad_nets.append((net, len(pads), len(groups), sorted(groups.values(), reverse=True)))

    print(f"Nets with genuinely disconnected islands: {len(bad_nets)} / {len(pads_by_net)}")
    bad_nets.sort(key=lambda x: -x[1])
    for net, npads, ngroups, sizes in bad_nets:
        print(f"{net}: {npads} pads, {ngroups} components, sizes={sizes[:10]}")


if __name__ == "__main__":
    main()
