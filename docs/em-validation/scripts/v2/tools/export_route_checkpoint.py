"""Export the current routed PCB as the next constrained Specctra checkpoint."""

from __future__ import annotations

import pcbnew

from generate_pcb import BOARD_PATH, DSN_PATH, enforce_router_contract


def main() -> None:
    board = pcbnew.LoadBoard(str(BOARD_PATH))
    if not pcbnew.ExportSpecctraDSN(board, str(DSN_PATH)):
        raise RuntimeError(f"KiCad failed to export checkpoint DSN: {DSN_PATH}")
    enforce_router_contract()
    print(f"Exported constrained route checkpoint {DSN_PATH}")


if __name__ == "__main__":
    main()
