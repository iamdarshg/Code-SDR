"""Move accidental inner-layer signals to B.Cu and restore solid In2 GND."""

from __future__ import annotations

import hashlib
import json
import shutil
import subprocess
from collections import Counter
from datetime import datetime
from pathlib import Path

import pcbnew

from export_rf50_dsn import RF50_NETS
ROOT = Path(__file__).resolve().parents[1]
BOARD = ROOT / "Code-SDR-V2.kicad_pcb"
BUILD = ROOT / "build" / "inner-plane-migration"


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


def rf_signature(board: pcbnew.BOARD) -> Counter[tuple[object, ...]]:
    result: Counter[tuple[object, ...]] = Counter()
    for item in board.GetTracks():
        if item.GetNetname() not in RF50_NETS:
            continue
        start, end = item.GetStart(), item.GetEnd()
        result[(
            type(item).__name__, item.GetNetname(), int(item.GetLayer()),
            int(start.x), int(start.y), int(end.x), int(end.y),
            int(item.GetWidth(pcbnew.F_Cu)) if isinstance(item, pcbnew.PCB_VIA)
            else int(item.GetWidth()),
            int(item.GetDrillValue()) if isinstance(item, pcbnew.PCB_VIA) else 0,
        )] += 1
    return result


def main() -> None:
    BUILD.mkdir(parents=True, exist_ok=True)
    run = BUILD / datetime.now().strftime("%Y%m%d-%H%M%S-%f")
    run.mkdir()
    candidate = run / BOARD.name
    shutil.copy2(BOARD, candidate)
    live_hash = sha256(BOARD)

    baseline = drc(BOARD, run / "baseline-drc.json")
    before_board = pcbnew.LoadBoard(str(BOARD))
    before_rf = rf_signature(before_board)
    board = pcbnew.LoadBoard(str(candidate))
    moved: Counter[str] = Counter()
    for item in board.GetTracks():
        if isinstance(item, pcbnew.PCB_VIA):
            continue
        if item.GetLayer() in (pcbnew.In1_Cu, pcbnew.In2_Cu) and item.GetNetname() != "GND":
            moved[item.GetNetname()] += 1
            item.SetLayer(pcbnew.B_Cu)
    if not moved:
        raise RuntimeError("No inner-layer signal segments found")

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

    checked = board
    remaining = sorted({
        item.GetNetname()
        for item in checked.GetTracks()
        if not isinstance(item, pcbnew.PCB_VIA)
        and item.GetLayer() in (pcbnew.In1_Cu, pcbnew.In2_Cu)
        and item.GetNetname() != "GND"
    })
    result = drc(candidate, run / "candidate-drc.json")
    errors: list[str] = []
    if remaining:
        errors.append("inner-layer signal copper remains: " + ", ".join(remaining))
    if rf_signature(checked) != before_rf:
        errors.append("protected RF50 copper changed")
    if len(result.get("violations", [])) > len(baseline.get("violations", [])):
        errors.append(
            f"DRC violations increased {len(baseline.get('violations', []))} -> "
            f"{len(result.get('violations', []))}"
        )
    if result.get("schematic_parity", []):
        errors.append("schematic parity is not clean")
    if len(result.get("unconnected_items", [])) >= len(baseline.get("unconnected_items", [])):
        errors.append(
            f"unconnected count did not improve {len(baseline.get('unconnected_items', []))} -> "
            f"{len(result.get('unconnected_items', []))}"
        )
    if sha256(BOARD) != live_hash:
        errors.append("live board changed during candidate validation")
    (run / "migration.json").write_text(
        json.dumps({"moved_segments": moved, "errors": errors}, indent=2),
        encoding="utf-8",
    )
    if errors:
        raise RuntimeError("Inner-plane candidate rejected: " + "; ".join(errors))

    shutil.copy2(candidate, BOARD)
    print(
        "ACCEPT inner->B.Cu and In2=solid GND "
        f"moved={dict(moved)} opens {len(baseline.get('unconnected_items', []))} -> "
        f"{len(result.get('unconnected_items', []))}"
    )


if __name__ == "__main__":
    main()
