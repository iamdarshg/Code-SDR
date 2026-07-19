"""Add conservative through-ground-via fences beside critical RF routes."""

from __future__ import annotations

import math
from pathlib import Path

import pcbnew

from generate_pcb import router_class_for_net


ROOT = Path(__file__).resolve().parents[1]
BOARD_PATH = ROOT / "Code-SDR-V2.kicad_pcb"
PITCH_MM = 1.50
OFFSET_MM = 0.70
VIA_DIAMETER_MM = 0.45
VIA_DRILL_MM = 0.20
EDGE_MARGIN_MM = 0.40


def point_segment_distance_mm(
    point: pcbnew.VECTOR2I,
    start: pcbnew.VECTOR2I,
    end: pcbnew.VECTOR2I,
) -> float:
    px, py = pcbnew.ToMM(point.x), pcbnew.ToMM(point.y)
    ax, ay = pcbnew.ToMM(start.x), pcbnew.ToMM(start.y)
    bx, by = pcbnew.ToMM(end.x), pcbnew.ToMM(end.y)
    dx, dy = bx - ax, by - ay
    if dx == 0 and dy == 0:
        return math.hypot(px - ax, py - ay)
    fraction = max(0.0, min(1.0, ((px - ax) * dx + (py - ay) * dy) / (dx * dx + dy * dy)))
    return math.hypot(px - (ax + fraction * dx), py - (ay + fraction * dy))


def main() -> None:
    board = pcbnew.LoadBoard(str(BOARD_PATH))
    gnd = board.FindNet("GND")
    if gnd is None:
        raise RuntimeError("GND net not found")

    critical_tracks = [
        item
        for item in board.GetTracks()
        if not isinstance(item, pcbnew.PCB_VIA)
        and item.GetLayer() == pcbnew.F_Cu
        and router_class_for_net(item.GetNetname()) == "RF_CRITICAL"
    ]
    all_tracks = list(board.GetTracks())
    pads = [pad for footprint in board.GetFootprints() for pad in footprint.Pads()]
    accepted: list[pcbnew.VECTOR2I] = []
    width_mm = 160.0
    height_mm = 100.0

    def candidate_clear(point: pcbnew.VECTOR2I) -> bool:
        x, y = pcbnew.ToMM(point.x), pcbnew.ToMM(point.y)
        if not (
            EDGE_MARGIN_MM <= x <= width_mm - EDGE_MARGIN_MM
            and EDGE_MARGIN_MM <= y <= height_mm - EDGE_MARGIN_MM
        ):
            return False
        if any(
            math.hypot(x - pcbnew.ToMM(other.x), y - pcbnew.ToMM(other.y)) < 0.65
            for other in accepted
        ):
            return False
        via_radius = VIA_DIAMETER_MM / 2.0
        for pad in pads:
            box = pad.GetBoundingBox()
            box.Inflate(pcbnew.FromMM(via_radius + 0.12))
            if box.Contains(point):
                return False
        for item in all_tracks:
            if isinstance(item, pcbnew.PCB_VIA):
                other = item.GetPosition()
                required = via_radius + pcbnew.ToMM(item.GetWidth()) / 2.0 + 0.10
                if math.hypot(x - pcbnew.ToMM(other.x), y - pcbnew.ToMM(other.y)) < required:
                    return False
            else:
                required = via_radius + pcbnew.ToMM(item.GetWidth()) / 2.0 + 0.10
                if point_segment_distance_mm(point, item.GetStart(), item.GetEnd()) < required:
                    return False
        return True

    for track in critical_tracks:
        start, end = track.GetStart(), track.GetEnd()
        ax, ay = pcbnew.ToMM(start.x), pcbnew.ToMM(start.y)
        bx, by = pcbnew.ToMM(end.x), pcbnew.ToMM(end.y)
        dx, dy = bx - ax, by - ay
        length = math.hypot(dx, dy)
        if length < 0.75:
            continue
        ux, uy = dx / length, dy / length
        nx, ny = -uy, ux
        positions = max(1, math.ceil(length / PITCH_MM))
        for index in range(positions + 1):
            distance = min(length - 0.30, max(0.30, index * length / positions))
            cx, cy = ax + ux * distance, ay + uy * distance
            for side in (-1.0, 1.0):
                point = pcbnew.VECTOR2I(
                    pcbnew.FromMM(cx + side * nx * OFFSET_MM),
                    pcbnew.FromMM(cy + side * ny * OFFSET_MM),
                )
                if not candidate_clear(point):
                    continue
                via = pcbnew.PCB_VIA(board)
                via.SetPosition(point)
                via.SetWidth(pcbnew.FromMM(VIA_DIAMETER_MM))
                via.SetDrill(pcbnew.FromMM(VIA_DRILL_MM))
                via.SetLayerPair(pcbnew.F_Cu, pcbnew.B_Cu)
                via.SetNet(gnd)
                board.Add(via)
                all_tracks.append(via)
                accepted.append(point)

    # Refill all copper after adding stitching vias so the saved manufacturing
    # board contains current F.Cu/B.Cu pours and the solid In1 ground plane.
    pcbnew.ZONE_FILLER(board).Fill(board.Zones())
    pcbnew.SaveBoard(str(BOARD_PATH), board)
    print(
        f"Added {len(accepted)} GND fence vias beside "
        f"{len(critical_tracks)} critical RF segments"
    )


if __name__ == "__main__":
    main()
