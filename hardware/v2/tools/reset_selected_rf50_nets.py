"""Remove every track belonging to explicitly named RF50 nets."""

from __future__ import annotations

import sys
from pathlib import Path

import pcbnew

from export_rf50_dsn import RF50_NETS


BOARD = Path(__file__).resolve().parents[1] / "Code-SDR-V2.kicad_pcb"


def main() -> None:
    requested = set(sys.argv[1:])
    invalid = requested - RF50_NETS
    if invalid:
        raise RuntimeError("Refusing non-RF50 nets: " + ", ".join(sorted(invalid)))
    board = pcbnew.LoadBoard(str(BOARD))
    removed = 0
    for item in list(board.GetTracks()):
        if item.GetNetname() in requested:
            board.Remove(item)
            removed += 1
    pcbnew.SaveBoard(str(BOARD), board)
    print(f"Removed {removed} segments from {sorted(requested)}")


if __name__ == "__main__":
    main()
