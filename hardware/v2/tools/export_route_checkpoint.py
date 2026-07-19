"""Export the current partially routed PCB as the next Freerouting DSN."""

from __future__ import annotations

import argparse
from pathlib import Path

import pcbnew

from generate_pcb import (
    BOARD_PATH,
    DSN_PATH,
    enforce_router_contract,
)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--board",
        type=Path,
        default=BOARD_PATH,
        help="PCB to export; defaults to the release board",
    )
    args = parser.parse_args()
    board_path = args.board.resolve()
    board = pcbnew.LoadBoard(str(board_path))
    if not pcbnew.ExportSpecctraDSN(board, str(DSN_PATH)):
        raise RuntimeError("KiCad failed to export the routed checkpoint DSN")
    enforce_router_contract()
    print(f"Exported {board_path.name} to {DSN_PATH}")


if __name__ == "__main__":
    main()
