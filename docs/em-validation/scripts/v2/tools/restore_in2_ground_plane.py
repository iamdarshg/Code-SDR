"""Restore the intended solid In2 GND reference using preservation gates."""

from __future__ import annotations

import hashlib
import json
import shutil
import subprocess
from datetime import datetime
from pathlib import Path

import pcbnew

ROOT = Path(__file__).resolve().parents[1]
BOARD = ROOT / "Code-SDR-V2.kicad_pcb"
BUILD = ROOT / "build" / "restore-in2-ground"


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def drc(board: Path, report: Path) -> dict:
    subprocess.run(
        [
            "kicad-cli", "pcb", "drc", str(board), "--format", "json",
            "--severity-all", "--all-track-errors", "--schematic-parity",
            "--units", "mm", "-o", str(report),
        ],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        timeout=180,
    )
    if not report.is_file():
        raise RuntimeError(f"KiCad did not write {report}")
    return json.loads(report.read_text(encoding="utf-8"))


def main() -> None:
    BUILD.mkdir(parents=True, exist_ok=True)
    run = BUILD / datetime.now().strftime("%Y%m%d-%H%M%S-%f")
    run.mkdir()
    candidate = run / BOARD.name
    shutil.copy2(BOARD, candidate)
    live_hash = sha256(BOARD)

    baseline = drc(BOARD, run / "baseline-drc.json")
    board = pcbnew.LoadBoard(str(candidate))
    forbidden = sorted({
        item.GetNetname()
        for item in board.GetTracks()
        if not isinstance(item, pcbnew.PCB_VIA)
        and item.GetLayer() == pcbnew.In2_Cu
        and item.GetNetname() != "GND"
    })
    if forbidden:
        raise RuntimeError(
            "Refusing to make In2 GND while signal copper remains: "
            + ", ".join(forbidden)
        )

    zones = list(board.Zones())
    template = next(
        (zone for zone in zones
         if zone.GetLayer() == pcbnew.In1_Cu and zone.GetNetname() == "GND"),
        None,
    )
    if template is None:
        raise RuntimeError("Solid In1 GND template zone missing")
    in2_ground = pcbnew.ZONE(board)
    in2_ground.CopyFrom(template)
    for zone in zones:
        if zone.GetLayer() == pcbnew.In2_Cu:
            board.Remove(zone)
    gnd = board.FindNet("GND")
    if gnd is None:
        raise RuntimeError("GND net missing")
    in2_ground.SetLayer(pcbnew.In2_Cu)
    in2_ground.SetNet(gnd)
    board.Add(in2_ground)
    pcbnew.ZONE_FILLER(board).Fill(board.Zones())
    pcbnew.SaveBoard(str(candidate), board)

    result = drc(candidate, run / "candidate-drc.json")
    errors = []
    if len(result.get("violations", [])) > len(baseline.get("violations", [])):
        errors.append("DRC violation count increased")
    if result.get("schematic_parity", []):
        errors.append("schematic parity is not clean")
    if len(result.get("unconnected_items", [])) >= len(baseline.get("unconnected_items", [])):
        errors.append("unconnected count did not improve")
    if sha256(BOARD) != live_hash:
        errors.append("live board changed during candidate validation")
    if errors:
        raise RuntimeError("In2 GND candidate rejected: " + "; ".join(errors))

    shutil.copy2(candidate, BOARD)
    print(
        "ACCEPT In2=solid GND "
        f"opens {len(baseline.get('unconnected_items', []))} -> "
        f"{len(result.get('unconnected_items', []))}"
    )


if __name__ == "__main__":
    main()
