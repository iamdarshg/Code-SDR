"""Independent connectivity and copper-scope audit for the RF50-only route."""

from __future__ import annotations

from collections import Counter
import math
from pathlib import Path

import pcbnew

from export_rf50_dsn import RF50_NETS


BOARD = Path(__file__).resolve().parents[1] / "Code-SDR-V2.kicad_pcb"
TARGET_WIDTH_MM = 0.23
TARGET_VIA_DIAMETER_MM = 0.45
TARGET_VIA_DRILL_MM = 0.20


def main() -> None:
    board = pcbnew.LoadBoard(str(BOARD))
    tracks = list(board.GetTracks())
    problems: list[str] = []
    widths: Counter[float] = Counter()
    for item in tracks:
        name = item.GetNetname()
        if isinstance(item, pcbnew.PCB_VIA):
            if name not in RF50_NETS and name != "GND":
                problems.append(f"non-RF50 via on {name}")
            diameter = pcbnew.ToMM(item.GetWidth(pcbnew.F_Cu))
            drill = pcbnew.ToMM(item.GetDrillValue())
            if abs(diameter - TARGET_VIA_DIAMETER_MM) > 1e-6 or abs(drill - TARGET_VIA_DRILL_MM) > 1e-6:
                problems.append(f"wrong via {diameter:.3f}/{drill:.3f} mm on {name}")
            continue
        width = round(pcbnew.ToMM(item.GetWidth()), 6)
        widths[width] += 1
        if name not in RF50_NETS:
            problems.append(f"non-RF50 track on {name}")
        if item.GetLayer() not in {pcbnew.F_Cu, pcbnew.B_Cu}:
            problems.append(f"unsupported track layer on {name}")
        if abs(width - TARGET_WIDTH_MM) > 1e-6:
            problems.append(f"wrong width {width:.6f} mm on {name}")

    incomplete = []
    for name in sorted(RF50_NETS):
        pads = [p for f in board.GetFootprints() for p in f.Pads() if p.GetNetname() == name]
        net_tracks = [t for t in tracks if t.GetNetname() == name]
        parent = list(range(len(net_tracks)))

        def find(index):
            while parent[index] != index:
                parent[index] = parent[parent[index]]
                index = parent[index]
            return index

        def union(a, b):
            ra, rb = find(a), find(b)
            if ra != rb:
                parent[rb] = ra

        endpoint_map = {}
        for index, track in enumerate(net_tracks):
            for endpoint in (track.GetStart(), track.GetEnd()):
                key = (endpoint.x, endpoint.y)
                if key in endpoint_map:
                    union(index, endpoint_map[key])
                else:
                    endpoint_map[key] = index

        def distance_to_segment(point, track):
            px, py = pcbnew.ToMM(point.x), pcbnew.ToMM(point.y)
            a, b = track.GetStart(), track.GetEnd()
            ax, ay = pcbnew.ToMM(a.x), pcbnew.ToMM(a.y)
            bx, by = pcbnew.ToMM(b.x), pcbnew.ToMM(b.y)
            dx, dy = bx - ax, by - ay
            if dx == 0 and dy == 0:
                return math.hypot(px - ax, py - ay)
            scale = max(0.0, min(1.0, ((px - ax) * dx + (py - ay) * dy) / (dx * dx + dy * dy)))
            return math.hypot(px - (ax + scale * dx), py - (ay + scale * dy))

        def item_width_mm(item):
            return pcbnew.ToMM(
                item.GetWidth(pcbnew.F_Cu)
                if isinstance(item, pcbnew.PCB_VIA)
                else item.GetWidth()
            )

        # Include legal T-junctions where one track ends on another track's
        # interior rather than on an identical endpoint coordinate.
        for first_index, first_track in enumerate(net_tracks):
            for second_index in range(first_index + 1, len(net_tracks)):
                second_track = net_tracks[second_index]
                tolerance = (item_width_mm(first_track) + item_width_mm(second_track)) / 2
                if any(
                    distance_to_segment(endpoint, second_track) <= tolerance + 1e-6
                    for endpoint in (first_track.GetStart(), first_track.GetEnd())
                ) or any(
                    distance_to_segment(endpoint, first_track) <= tolerance + 1e-6
                    for endpoint in (second_track.GetStart(), second_track.GetEnd())
                ):
                    union(first_index, second_index)

        roots = []
        for pad in pads:
            hits = [
                index for index, track in enumerate(net_tracks)
                if pad.HitTest(track.GetStart()) or pad.HitTest(track.GetEnd())
            ]
            if not hits:
                roots.append(None)
            else:
                for other in hits[1:]:
                    union(hits[0], other)
                roots.append(find(hits[0]))
        present = {root for root in roots if root is not None}
        if any(root is None for root in roots) or len(present) != 1:
            incomplete.append(name)

    print(f"tracks={len(tracks)} vias={sum(isinstance(t, pcbnew.PCB_VIA) for t in tracks)} widths={dict(widths)}")
    print(f"incomplete={incomplete}")
    if problems:
        raise RuntimeError("RF50 copper audit failed:\n" + "\n".join(problems[:100]))
    if incomplete:
        raise RuntimeError(f"RF50 connectivity incomplete: {len(incomplete)} nets")
    print(f"RF50 audit passed: {len(RF50_NETS)} nets connected")


if __name__ == "__main__":
    main()
