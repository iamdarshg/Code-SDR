"""Apply the RF-ground connection policy and refill all PCB copper zones."""

from pathlib import Path

import pcbnew


BOARD_PATH = Path(__file__).resolve().parents[1] / "Code-SDR-V2.kicad_pcb"


def main() -> None:
    board = pcbnew.LoadBoard(str(BOARD_PATH))
    for footprint in board.GetFootprints():
        for pad in footprint.Pads():
            if pad.GetNetname() == "GND":
                pad.SetZoneConnection(pcbnew.ZONE_CONNECTION_FULL)
    for zone in board.Zones():
        if zone.GetNetname() == "GND":
            zone.SetPadConnection(pcbnew.ZONE_CONNECTION_FULL)
    pcbnew.ZONE_FILLER(board).Fill(board.Zones())
    pcbnew.SaveBoard(str(BOARD_PATH), board)
    print(f"Refilled {len(list(board.Zones()))} zones with solid GND attachment")


if __name__ == "__main__":
    main()
