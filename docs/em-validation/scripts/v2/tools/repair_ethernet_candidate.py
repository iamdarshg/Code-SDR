"""Apply localized clearance repairs and length tuning to an Ethernet candidate."""

from __future__ import annotations

import argparse
import shutil
from pathlib import Path


Point = tuple[float, float]


def _xy(item_point: object, pcbnew: object) -> Point:
    return (round(pcbnew.ToMM(item_point.x), 3), round(pcbnew.ToMM(item_point.y), 3))


def _point(point: Point, pcbnew: object) -> object:
    return pcbnew.VECTOR2I(pcbnew.FromMM(point[0]), pcbnew.FromMM(point[1]))


def _find_track(board: object, net_name: str, start: Point, end: Point, pcbnew: object) -> object:
    wanted = {start, end}
    for item in board.GetTracks():
        if isinstance(item, pcbnew.PCB_VIA) or item.GetNetname() != net_name:
            continue
        if {_xy(item.GetStart(), pcbnew), _xy(item.GetEnd(), pcbnew)} == wanted:
            return item
    raise RuntimeError(f"track not found: {net_name} {start} -> {end}")


def _find_via(board: object, net_name: str, at: Point, pcbnew: object) -> object:
    for item in board.GetTracks():
        if isinstance(item, pcbnew.PCB_VIA) and item.GetNetname() == net_name:
            if _xy(item.GetPosition(), pcbnew) == at:
                return item
    raise RuntimeError(f"via not found: {net_name} at {at}")


def repair_clearances(board: object, pcbnew: object) -> None:
    # Pull the D_N layer-change away from J30 MAG_CT3.
    old_via = (80.8, 84.8)
    new_via = (80.8, 84.2)
    _find_via(board, "MDI_D_N", old_via, pcbnew).SetPosition(_point(new_via, pcbnew))
    f_track = _find_track(board, "MDI_D_N", (80.8, 76.6), old_via, pcbnew)
    f_track.SetEnd(_point(new_via, pcbnew))
    b_track = _find_track(board, "MDI_D_N", old_via, (80.2, 84.8), pcbnew)
    b_track.SetStart(_point(new_via, pcbnew))

    # Lift the A_P dogleg away from J30 PHY_LED2.
    vertical_right = _find_track(board, "MDI_A_P", (77.4, 83.4), (77.4, 83.8), pcbnew)
    vertical_right.SetStart(_point((77.4, 83.4), pcbnew))
    vertical_right.SetEnd(_point((76.0, 83.4), pcbnew))
    horizontal = _find_track(board, "MDI_A_P", (77.4, 83.8), (75.2, 83.8), pcbnew)
    horizontal.SetStart(_point((76.0, 83.4), pcbnew))
    horizontal.SetEnd(_point((75.2, 83.4), pcbnew))
    vertical_left = _find_track(board, "MDI_A_P", (75.2, 83.8), (75.2, 88.8), pcbnew)
    vertical_left.SetStart(_point((75.2, 83.4), pcbnew))

    # A short neck-down is enough to clear the adjacent U30 MDI_B_P pad.
    b_neck = _find_track(board, "MDI_B_N", (82.2, 74.0), (84.0, 74.0), pcbnew)
    b_neck.SetWidth(pcbnew.FromMM(0.15))


def _replace_with_polyline(
    board: object,
    net_name: str,
    start: Point,
    end: Point,
    points: list[Point],
    pcbnew: object,
) -> None:
    old = _find_track(board, net_name, start, end, pcbnew)
    layer = old.GetLayer()
    width = old.GetWidth()
    net = old.GetNet()
    old.SetStart(_point(points[0], pcbnew))
    old.SetEnd(_point(points[1], pcbnew))
    for first, second in zip(points[1:], points[2:]):
        track = pcbnew.PCB_TRACK(board)
        track.SetNet(net)
        track.SetLayer(layer)
        track.SetWidth(width)
        track.SetStart(_point(first, pcbnew))
        track.SetEnd(_point(second, pcbnew))
        board.Add(track)


def tune_pair_lengths(board: object, pcbnew: object) -> None:
    # A_N: +6.8 mm, routed into the open left-side B.Cu channel.
    _replace_with_polyline(
        board, "MDI_A_N", (77.8, 83.8), (77.8, 90.2),
        [
            (77.8, 83.8), (77.4, 83.8), (77.4, 83.4),
            (74.4, 83.4), (74.4, 90.2), (77.8, 90.2),
        ], pcbnew,
    )
    # B_N: +1.2 mm, a shallow outward jog on B.Cu.
    _replace_with_polyline(
        board, "MDI_B_N", (82.6, 89.6), (82.6, 93.2),
        [(82.6, 89.6), (83.2, 89.6), (83.2, 93.2), (82.6, 93.2)], pcbnew,
    )
    # C_N: +3.2 mm using two 0.8 mm folds left of the pair on B.Cu.
    _replace_with_polyline(
        board, "MDI_C_N", (81.6, 75.4), (81.6, 84.8),
        [
            (81.6, 75.4), (80.8, 75.4), (80.8, 77.0), (81.6, 77.0),
            (81.6, 79.0), (80.8, 79.0), (80.8, 80.6), (81.6, 80.6),
            (81.6, 84.8),
        ], pcbnew,
    )
    # D_P: +0.8 mm beside its connector pad, clear of the MAG_CT4 keepout.
    _replace_with_polyline(
        board, "MDI_D_P", (79.0, 86.2), (79.0, 87.0),
        [(79.0, 86.2), (79.4, 86.2), (79.4, 87.0), (79.0, 87.0)], pcbnew,
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=Path)
    parser.add_argument("candidate", type=Path)
    args = parser.parse_args()
    source = args.source.resolve()
    candidate = args.candidate.resolve()

    import pcbnew

    candidate.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, candidate)
    board = pcbnew.LoadBoard(str(candidate))
    repair_clearances(board, pcbnew)
    tune_pair_lengths(board, pcbnew)
    if not pcbnew.SaveBoard(str(candidate), board):
        raise RuntimeError(f"KiCad failed to save {candidate}")


if __name__ == "__main__":
    main()
