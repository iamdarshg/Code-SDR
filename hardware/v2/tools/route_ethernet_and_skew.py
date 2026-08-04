"""Candidate-only manual routing contract for Ethernet and skew correction."""

from __future__ import annotations

import argparse
import heapq
import shutil
from collections import defaultdict
from pathlib import Path


def ethernet_route_contract() -> dict[str, object]:
    """Return the fixed MDI dimensions approved for the U30-to-J30 route."""
    return {
        "width_mm": 0.20,
        "allow_vias": True,
        "max_signal_vias_per_net": 2,
        "layers": ("F.Cu", "B.Cu"),
        "via_diameter_mm": 0.50,
        "pad_neck_width_mm": 0.15,
        "pad_neck_length_mm": 2.0,
    }


MDI_NETS = tuple(f"MDI_{pair}_{polarity}" for pair in "ABCD" for polarity in ("P", "N"))
ROUTING_ORDER = (
    "MDI_D_P", "MDI_C_N", "MDI_D_N", "MDI_C_P",
    "MDI_B_P", "MDI_A_N", "MDI_B_N", "MDI_A_P",
)
GRID_MM = 0.20
TRACK_MM = 0.20
VIA_MM = 0.50
DRILL_MM = 0.20
CLEARANCE_MM = 0.20
NECK_MM = 0.15
NECK_LENGTH_MM = 2.0


def _mm(value: float, pcbnew: object) -> int:
    return pcbnew.FromMM(value)


def _point(x: float, y: float, pcbnew: object) -> object:
    return pcbnew.VECTOR2I(_mm(x, pcbnew), _mm(y, pcbnew))


def _pad_locations(board: object, pcbnew: object) -> dict[str, tuple[tuple[float, float], tuple[float, float]]]:
    pads: dict[str, list[tuple[float, float]]] = defaultdict(list)
    for footprint in board.GetFootprints():
        if footprint.GetReference() not in {"U30", "J30"}:
            continue
        for pad in footprint.Pads():
            if pad.GetNetname() in MDI_NETS:
                pos = pad.GetPosition()
                pads[pad.GetNetname()].append((pcbnew.ToMM(pos.x), pcbnew.ToMM(pos.y)))
    result: dict[str, tuple[tuple[float, float], tuple[float, float]]] = {}
    for net_name, locations in pads.items():
        if len(locations) != 2:
            raise RuntimeError(f"expected two endpoints for {net_name}, got {locations}")
        result[net_name] = tuple(sorted(locations, key=lambda point: point[1]))  # U30 then J30
    return result


def _blocked_rectangles(
    board: object, layer: int, pcbnew: object, exempt_net: str,
) -> list[tuple[float, float, float, float]]:
    margin = CLEARANCE_MM + TRACK_MM / 2
    blocked: list[tuple[float, float, float, float]] = []
    for footprint in board.GetFootprints():
        for pad in footprint.Pads():
            if pad.GetNetname() == exempt_net:
                continue
            if not pad.GetLayerSet().Contains(layer):
                continue
            box = pad.GetBoundingBox()
            blocked.append((
                pcbnew.ToMM(box.GetLeft()) - margin,
                pcbnew.ToMM(box.GetTop()) - margin,
                pcbnew.ToMM(box.GetRight()) + margin,
                pcbnew.ToMM(box.GetBottom()) + margin,
            ))
    for item in board.GetTracks():
        if item.GetNetname() == exempt_net:
            continue
        if isinstance(item, pcbnew.PCB_VIA) or item.GetLayer() == layer:
            box = item.GetBoundingBox()
            blocked.append((
                pcbnew.ToMM(box.GetLeft()) - margin,
                pcbnew.ToMM(box.GetTop()) - margin,
                pcbnew.ToMM(box.GetRight()) + margin,
                pcbnew.ToMM(box.GetBottom()) + margin,
            ))
    return blocked


def _grid_point(point: tuple[float, float]) -> tuple[int, int]:
    return (round(point[0] / GRID_MM), round(point[1] / GRID_MM))


def _world_point(point: tuple[int, int]) -> tuple[float, float]:
    return (point[0] * GRID_MM, point[1] * GRID_MM)


def _is_blocked(point: tuple[int, int], blocked: list[tuple[float, float, float, float]]) -> bool:
    x, y = _world_point(point)
    return any(left <= x <= right and top <= y <= bottom for left, top, right, bottom in blocked)


def _is_via_blocked(point: tuple[int, int], blocked: list[tuple[float, float, float, float]]) -> bool:
    x, y = _world_point(point)
    extra = (VIA_MM - TRACK_MM) / 2 + 0.02
    return any(
        left - extra <= x <= right + extra and top - extra <= y <= bottom + extra
        for left, top, right, bottom in blocked
    )


def _astar(start: tuple[float, float], goal: tuple[float, float], blocked: list[tuple[float, float, float, float]]) -> list[tuple[float, float]]:
    begin, end = _grid_point(start), _grid_point(goal)
    bounds = (int(68 / GRID_MM), int(86 / GRID_MM), int(68 / GRID_MM), int(98 / GRID_MM))
    queue: list[tuple[int, int, tuple[int, int]]] = [(0, 0, begin)]
    previous: dict[tuple[int, int], tuple[int, int] | None] = {begin: None}
    distances = {begin: 0}
    sequence = 0
    while queue:
        _, _, current = heapq.heappop(queue)
        if current == end:
            path: list[tuple[float, float]] = []
            while current is not None:
                path.append(_world_point(current))
                current = previous[current]
            return list(reversed(path))
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nxt = (current[0] + dx, current[1] + dy)
            if not (bounds[0] <= nxt[0] <= bounds[1] and bounds[2] <= nxt[1] <= bounds[3]):
                continue
            if nxt != end and _is_blocked(nxt, blocked):
                continue
            trial = distances[current] + 1
            if trial >= distances.get(nxt, 10**9):
                continue
            distances[nxt] = trial
            previous[nxt] = current
            sequence += 1
            heuristic = abs(nxt[0] - end[0]) + abs(nxt[1] - end[1])
            heapq.heappush(queue, (trial + heuristic, sequence, nxt))
    raise RuntimeError(f"no B.Cu path from {start} to {goal}")


def _astar_multilayer(
    start: tuple[float, float],
    goal: tuple[float, float],
    blocked_by_layer: tuple[list[tuple[float, float, float, float]], list[tuple[float, float, float, float]]],
) -> list[tuple[float, float, int]]:
    begin_xy, end_xy = _grid_point(start), _grid_point(goal)
    begin = (*begin_xy, 0)
    bounds = (int(66 / GRID_MM), int(94 / GRID_MM), int(67 / GRID_MM), int(99 / GRID_MM))
    queue: list[tuple[int, int, tuple[int, int, int]]] = [(0, 0, begin)]
    previous: dict[tuple[int, int, int], tuple[int, int, int] | None] = {begin: None}
    distances = {begin: 0}
    sequence = 0
    while queue:
        _, _, current = heapq.heappop(queue)
        if (current[0], current[1]) == end_xy:
            path: list[tuple[float, float, int]] = []
            while current is not None:
                world = _world_point((current[0], current[1]))
                path.append((world[0], world[1], current[2]))
                current = previous[current]
            return list(reversed(path))
        neighbours = [
            (current[0] + 1, current[1], current[2], 1),
            (current[0] - 1, current[1], current[2], 1),
            (current[0], current[1] + 1, current[2], 1),
            (current[0], current[1] - 1, current[2], 1),
            (current[0], current[1], 1 - current[2], 18),
        ]
        for x, y, layer, step_cost in neighbours:
            nxt = (x, y, layer)
            if not (bounds[0] <= x <= bounds[1] and bounds[2] <= y <= bounds[3]):
                continue
            xy = (x, y)
            if xy != end_xy and _is_blocked(xy, blocked_by_layer[layer]):
                continue
            if layer != current[2] and abs(x - begin[0]) + abs(y - begin[1]) < 5:
                continue
            if layer != current[2] and (
                _is_via_blocked(xy, blocked_by_layer[0]) or _is_via_blocked(xy, blocked_by_layer[1])
            ):
                continue
            trial = distances[current] + step_cost
            if trial >= distances.get(nxt, 10**9):
                continue
            distances[nxt] = trial
            previous[nxt] = current
            sequence += 1
            heuristic = abs(x - end_xy[0]) + abs(y - end_xy[1])
            heapq.heappush(queue, (trial + heuristic, sequence, nxt))
    raise RuntimeError(f"no two-layer path from {start} to {goal}")


def _compress(points: list[tuple[float, float]]) -> list[tuple[float, float]]:
    if len(points) < 3:
        return points
    result = [points[0]]
    for previous, current, following in zip(points, points[1:], points[2:]):
        if (current[0] - previous[0]) * (following[1] - current[1]) != (current[1] - previous[1]) * (following[0] - current[0]):
            result.append(current)
    result.append(points[-1])
    return result


def _compress_multilayer(points: list[tuple[float, float, int]]) -> list[tuple[float, float, int]]:
    if len(points) < 3:
        return points
    result = [points[0]]
    for previous, current, following in zip(points, points[1:], points[2:]):
        same_layer = previous[2] == current[2] == following[2]
        collinear = (
            (current[0] - previous[0]) * (following[1] - current[1])
            == (current[1] - previous[1]) * (following[0] - current[0])
        )
        if not (same_layer and collinear):
            result.append(current)
    result.append(points[-1])
    return result


def _add_track(
    board: object, net: object, layer: int, start: tuple[float, float], end: tuple[float, float],
    pcbnew: object, width_mm: float = TRACK_MM,
) -> None:
    track = pcbnew.PCB_TRACK(board)
    track.SetNet(net)
    track.SetLayer(layer)
    track.SetWidth(_mm(width_mm, pcbnew))
    track.SetStart(_point(*start, pcbnew))
    track.SetEnd(_point(*end, pcbnew))
    board.Add(track)


def _add_via(board: object, net: object, location: tuple[float, float], pcbnew: object) -> None:
    via = pcbnew.PCB_VIA(board)
    via.SetNet(net)
    via.SetPosition(_point(*location, pcbnew))
    via.SetLayerPair(pcbnew.F_Cu, pcbnew.B_Cu)
    via.SetWidth(_mm(VIA_MM, pcbnew))
    via.SetDrill(_mm(DRILL_MM, pcbnew))
    board.Add(via)


def route_ethernet_candidate(source: Path, candidate: Path, nets_to_route: tuple[str, ...] = ROUTING_ORDER) -> None:
    """Route every MDI net through an F/B candidate-only escape."""
    import pcbnew

    shutil.copy2(source, candidate)
    board = pcbnew.LoadBoard(str(candidate))
    locations = _pad_locations(board, pcbnew)
    nets = board.GetNetInfo().NetsByName()
    for net_name in nets_to_route:
        source_pad, destination_pad = locations[net_name]
        source_escape = (source_pad[0] - NECK_LENGTH_MM, source_pad[1])
        net = nets[net_name]
        blocked = (
            _blocked_rectangles(board, pcbnew.F_Cu, pcbnew, net_name),
            _blocked_rectangles(board, pcbnew.B_Cu, pcbnew, net_name),
        )
        path = _compress_multilayer(_astar_multilayer(source_escape, destination_pad, blocked))
        first = path[0]
        _add_track(board, net, pcbnew.F_Cu, source_pad, (first[0], first[1]), pcbnew, NECK_MM)
        for current, following in zip(path, path[1:]):
            if current[2] != following[2]:
                _add_via(board, net, (current[0], current[1]), pcbnew)
            else:
                layer = pcbnew.F_Cu if current[2] == 0 else pcbnew.B_Cu
                _add_track(board, net, layer, (current[0], current[1]), (following[0], following[1]), pcbnew)
        last = path[-1]
        last_layer = pcbnew.F_Cu if last[2] == 0 else pcbnew.B_Cu
        _add_track(board, net, last_layer, (last[0], last[1]), destination_pad, pcbnew)
    pcbnew.ZONE_FILLER(board).Fill(board.Zones())
    pcbnew.SaveBoard(str(candidate), board)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("candidate", type=Path)
    parser.add_argument("--net", action="append", choices=MDI_NETS)
    args = parser.parse_args()
    route_ethernet_candidate(args.source, args.candidate, tuple(args.net or ROUTING_ORDER))


if __name__ == "__main__":
    main()
