"""Import the completed Freerouting session into the release PCB."""

from __future__ import annotations

import json
import shutil
from pathlib import Path

import pcbnew


ROOT = Path(__file__).resolve().parents[1]
BOARD_PATH = ROOT / "Code-SDR-V2.kicad_pcb"
SES_PATH = ROOT / "build" / "Code-SDR-V2-routed.ses"
BASE_COPY = ROOT / "build" / "Code-SDR-V2-pre-route.kicad_pcb"
SCHEMATIC_PATHS = ROOT / "build" / "schematic_paths.json"


def main() -> None:
    if not SES_PATH.exists() or SES_PATH.stat().st_size == 0:
        raise FileNotFoundError(f"Missing completed route: {SES_PATH}")
    shutil.copy2(BOARD_PATH, BASE_COPY)
    board = pcbnew.LoadBoard(str(BOARD_PATH))
    if not pcbnew.ImportSpecctraSES(board, str(SES_PATH)):
        raise RuntimeError("KiCad failed to import the Specctra session")

    # The deterministic schematic generator intentionally creates a fresh
    # hierarchy.  Refresh every footprint's current schematic UUID path after
    # route import so parity remains exact without regenerating the PCB and
    # discarding the route.
    path_map = json.loads(SCHEMATIC_PATHS.read_text(encoding="utf-8"))["components"]
    missing: list[str] = []
    for footprint in board.GetFootprints():
        reference = footprint.GetReference()
        if reference not in path_map:
            missing.append(reference)
            continue
        footprint.SetPath(pcbnew.KIID_PATH(path_map[reference]["path"]))
    if missing:
        raise RuntimeError(
            "No schematic UUID path for PCB footprints: " + ", ".join(sorted(missing))
        )
    pcbnew.SaveBoard(str(BOARD_PATH), board)

    tracks = list(board.GetTracks())
    vias = [item for item in tracks if isinstance(item, pcbnew.PCB_VIA)]
    segments = [item for item in tracks if not isinstance(item, pcbnew.PCB_VIA)]
    print(
        f"Imported {SES_PATH.name}: {len(segments)} track segments, "
        f"{len(vias)} vias"
    )


if __name__ == "__main__":
    main()
