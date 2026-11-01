"""Add the symmetric RF reference planes and return vias for layer changes."""

from __future__ import annotations

import json
import math
import shutil
import subprocess
from pathlib import Path

import pcbnew

from generate_pcb import add_ground_zone, enforce_project_rules


ROOT = Path(__file__).resolve().parents[1]
BOARD = ROOT / "Code-SDR-V2.kicad_pcb"
BUILD = ROOT / "build" / "rf-reference"
CLI = Path(r"C:\Program Files\KiCad\9.0\bin\kicad-cli.exe")
VIA_DIAMETER = 0.45
VIA_DRILL = 0.20


def segment_distance(point, start, end):
    px, py = point
    ax, ay = start
    bx, by = end
    dx, dy = bx - ax, by - ay
    if dx == 0 and dy == 0:
        return math.hypot(px - ax, py - ay)
    scale = max(0.0, min(1.0, ((px - ax) * dx + (py - ay) * dy) / (dx * dx + dy * dy)))
    return math.hypot(px - (ax + scale * dx), py - (ay + scale * dy))


def drc(tag):
    BUILD.mkdir(parents=True, exist_ok=True)
    enforce_project_rules()
    report = BUILD / f"{tag}.json"
    subprocess.run([
        str(CLI), "pcb", "drc", str(BOARD), "--format", "json",
        "--severity-all", "--all-track-errors", "--schematic-parity",
        "--output", str(report),
    ], check=False)
    return json.loads(report.read_text(encoding="utf-8"))


def main():
    BUILD.mkdir(parents=True, exist_ok=True)
    backup = BUILD / "before.kicad_pcb"
    shutil.copy2(BOARD, backup)
    baseline = drc("baseline")
    allowed = len(baseline.get("violations", []))
    board = pcbnew.LoadBoard(str(BOARD))
    gnd = board.FindNet("GND")
    if gnd is None:
        raise RuntimeError("GND net missing")

    # Full inner reference planes; 0.25 mm outline inset clears the board edge.
    if not any(zone.GetLayer() == pcbnew.In1_Cu for zone in board.Zones()):
        add_ground_zone(board, gnd, pcbnew.In1_Cu, 0.25)
    if not any(zone.GetLayer() == pcbnew.In2_Cu for zone in board.Zones()):
        add_ground_zone(board, gnd, pcbnew.In2_Cu, 0.25)
    for footprint in board.GetFootprints():
        for pad in footprint.Pads():
            if pad.GetNetname() == "GND":
                pad.SetLocalZoneConnection(pcbnew.ZONE_CONNECTION_FULL)

    signal_vias = [
        item for item in board.GetTracks()
        if isinstance(item, pcbnew.PCB_VIA) and item.GetNetname() != "GND"
    ]
    pads = [pad for footprint in board.GetFootprints() for pad in footprint.Pads()]
    existing = list(board.GetTracks())
    added = []

    def clear(point):
        x, y = point
        if not (0.5 < x < 98.5 and 0.5 < y < 98.5):
            return False
        iu = pcbnew.VECTOR2I(pcbnew.FromMM(x), pcbnew.FromMM(y))
        for pad in pads:
            box = pad.GetBoundingBox()
            box.Inflate(pcbnew.FromMM(0.45))
            if box.Contains(iu):
                return False
        for item in existing + added:
            if isinstance(item, pcbnew.PCB_VIA):
                other = item.GetPosition()
                if math.hypot(x - pcbnew.ToMM(other.x), y - pcbnew.ToMM(other.y)) < 0.68:
                    return False
            else:
                first, second = item.GetStart(), item.GetEnd()
                if segment_distance(
                    point,
                    (pcbnew.ToMM(first.x), pcbnew.ToMM(first.y)),
                    (pcbnew.ToMM(second.x), pcbnew.ToMM(second.y)),
                ) < 0.55:
                    return False
        return True

    for signal in signal_vias:
        centre = signal.GetPosition()
        cx, cy = pcbnew.ToMM(centre.x), pcbnew.ToMM(centre.y)
        count = 0
        for angle in (0, 180, 90, 270, 45, 225, 135, 315):
            radians = math.radians(angle)
            point = (cx + 0.82 * math.cos(radians), cy + 0.82 * math.sin(radians))
            if not clear(point):
                continue
            via = pcbnew.PCB_VIA(board)
            via.SetNet(gnd)
            via.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(point[0]), pcbnew.FromMM(point[1])))
            via.SetWidth(pcbnew.FromMM(VIA_DIAMETER))
            via.SetDrill(pcbnew.FromMM(VIA_DRILL))
            via.SetLayerPair(pcbnew.F_Cu, pcbnew.B_Cu)
            board.Add(via)
            added.append(via)
            count += 1
            if count == 2:
                break
        if count < 1:
            raise RuntimeError(f"No return via fits beside signal via at {cx:.3f},{cy:.3f}")

    pcbnew.ZONE_FILLER(board).Fill(board.Zones())
    pcbnew.SaveBoard(str(BOARD), board)
    report = drc("final")
    safe = len(report.get("violations", [])) <= allowed and not report.get("schematic_parity", [])
    print(f"{'ACCEPT' if safe else 'REJECT'} planes=2 return_vias={len(added)}")
    if not safe:
        shutil.copy2(backup, BOARD)
        raise RuntimeError("Reference-plane candidate failed DRC")


if __name__ == "__main__":
    main()
