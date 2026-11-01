"""Apply the RF-ground connection policy and refill all PCB copper zones."""

import argparse
from pathlib import Path

import pcbnew


BOARD_PATH = Path(__file__).resolve().parents[1] / "Code-SDR-V2.kicad_pcb"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("board", nargs="?", type=Path, default=BOARD_PATH)
    args = parser.parse_args()
    board_path = args.board.resolve()
    board = pcbnew.LoadBoard(str(board_path))
    for footprint in board.GetFootprints():
        for pad in footprint.Pads():
            if pad.GetNetname() == "GND":
                pad.SetLocalZoneConnection(pcbnew.ZONE_CONNECTION_FULL)
    for zone in board.Zones():
        if zone.GetNetname() == "GND":
            zone.SetPadConnection(pcbnew.ZONE_CONNECTION_FULL)
    pcbnew.ZONE_FILLER(board).Fill(board.Zones())
    pcbnew.SaveBoard(str(board_path), board)
    print(f"Refilled {len(list(board.Zones()))} zones with solid GND attachment")


if __name__ == "__main__":
    main()
