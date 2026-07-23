#!/usr/bin/env python3
"""Post-process the autorouted board: refill zones, report metrics."""

from __future__ import annotations

import re
from pathlib import Path

import pcbnew

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"


def main():
    board = pcbnew.LoadBoard(str(PCB_PATH))

    filler = pcbnew.ZONE_FILLER(board)
    zones = board.Zones()
    print(f"Refilling {len(zones)} zone(s)...")
    filler.Fill(zones)

    board.Save(str(PCB_PATH))
    print("Saved after zone refill.")

    with open(PCB_PATH) as f:
        content = f.read()
    tracks = len(re.findall(r"\(segment\s", content))
    vias = len(re.findall(r"\(via\s", content))
    print(f"Tracks: {tracks}  Vias: {vias}")


if __name__ == "__main__":
    main()
