#!/usr/bin/env python3
"""One-shot bulk removal of copper for the 4 big power nets plus their most
frequent DRC conflict-partner nets (per build/drc-report16.json). This
clears the corridors those partners were occupying so the power nets get
first pick when re-routed, then the partners are re-routed afterward and
naturally route around the power nets' new copper.

Does NOT re-route anything -- just removes tracks/vias. Re-routing happens
via the existing one-net-per-subprocess retry_forced_nets.py pattern
immediately after, in a specific order (power nets first).
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pcbnew

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"

POWER_NETS = ["+3V3_ANA", "+3V3_DIG", "+1V2_CORE", "+5V_RF"]

CONFLICT_PARTNERS = [
    "RF_HA_LNA_OUT", "REF_100M_ADC", "PHY_AVDDL_1V2", "DSA_LE",
    "LMX_LO_LOW_N_RAW", "LMX_CE", "HSW_V1", "LO_LOW_N", "ADC_VCM",
    "RGMII_TX_EN", "FPGA_SPI_MISO", "ADF_LOCK", "DSA_DATA", "RGMII_RXC",
    "DSA_CLK", "HSW_V2", "RF_HB_LNA_OUT", "IF2_P", "LNA_EN_H3",
    "IF_HIGH_N", "ADC_VIN_P", "RF_HD_LNA2_IN", "LO2_P", "ADC_VIN_N",
    "RF_HIGH_SWITCHED", "ADF_LO2_P_RAW", "LNA_EN_H4", "RGMII_RXD1",
    "ADL_RF_DRIVE", "RP_CORE",
]

ALL_TARGETS = POWER_NETS + CONFLICT_PARTNERS


def main():
    board = pcbnew.LoadBoard(str(PCB_PATH))
    target_set = set(ALL_TARGETS)
    removed_by_net = {n: 0 for n in ALL_TARGETS}

    to_remove = []
    for t in board.GetTracks():
        nn = t.GetNetname()
        if nn in target_set:
            to_remove.append(t)
            removed_by_net[nn] += 1

    for t in to_remove:
        board.Remove(t)

    print(f"Removed {len(to_remove)} track/via segments across {len(ALL_TARGETS)} nets:")
    for n in ALL_TARGETS:
        print(f"  {n}: {removed_by_net[n]}")

    board.Save(str(PCB_PATH))
    print(f"Saved (decluttered): {PCB_PATH}")

    # write the priority order file for the reroute loop: power nets first
    order_file = ROOT / "build" / "declutter_reroute_order.txt"
    order_file.write_text("\n".join(POWER_NETS + CONFLICT_PARTNERS) + "\n")
    print(f"Reroute order written to {order_file}")


if __name__ == "__main__":
    main()
