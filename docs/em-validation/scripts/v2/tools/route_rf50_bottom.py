"""DRC-gated bottom-microstrip crossings for otherwise non-planar RF nets."""

from __future__ import annotations

import json
import subprocess
from pathlib import Path

import pcbnew

from export_rf50_dsn import RF50_NETS
from generate_pcb import enforce_project_rules


ROOT = Path(__file__).resolve().parents[1]
BOARD = ROOT / "Code-SDR-V2.kicad_pcb"
BUILD = ROOT / "build" / "rf50-bottom"
CLI = Path(r"C:\Program Files\KiCad\9.0\bin\kicad-cli.exe")
WIDTH = 0.23
VIA_DIAMETER = 0.45
VIA_DRILL = 0.20

# (front pad, first via, bottom waypoints including both vias, second via,
# front pad).  The stackup is symmetric: B.Cu-to-In2 is the same 0.13 mm
# dielectric as F.Cu-to-In1, so the controlled width remains 0.23 mm.
ROUTES = {
    "LO_LOW_P": {
        "front_a": [(22.820, 61.750), (23.600, 61.750)],
        "bottom": [(23.600, 61.750), (24.500, 60.850), (24.500, 31.250)],
        "front_b": [(24.500, 31.250), (25.800, 31.250),
                    (26.300, 31.750), (27.0625, 31.750)],
    },
    "LO_LOW_N": {
        "front_a": [(22.820, 63.250), (23.600, 63.250)],
        "bottom": [(23.600, 63.250), (25.100, 63.250),
                   (25.100, 32.750)],
        "front_b": [(25.100, 32.750), (25.800, 32.750),
                    (26.300, 32.250), (27.0625, 32.250)],
    },
    "RF_HB_LNA_OUT": {
        "front_a": [(38.000, 12.620), (38.000, 13.400)],
        "bottom": [(38.000, 13.400), (39.000, 14.400), (70.500, 14.400),
                   (71.500, 15.400), (71.500, 22.750)],
        "front_b": [(71.500, 22.750), (72.225, 22.750)],
    },
    "RF_HD_LNA_OUT": {
        "front_a": [(75.320, 13.200), (76.000, 13.200)],
        "bottom": [(76.000, 13.200), (77.000, 14.200), (77.000, 22.500),
                   (75.000, 24.500), (73.250, 24.500)],
        "front_b": [(73.250, 24.500), (73.250, 23.775)],
    },
}


def add_track(board, net_name, layer, start, end):
    item = pcbnew.PCB_TRACK(board)
    item.SetNet(board.FindNet(net_name))
    item.SetLayer(layer)
    item.SetWidth(pcbnew.FromMM(WIDTH))
    item.SetStart(pcbnew.VECTOR2I(pcbnew.FromMM(start[0]), pcbnew.FromMM(start[1])))
    item.SetEnd(pcbnew.VECTOR2I(pcbnew.FromMM(end[0]), pcbnew.FromMM(end[1])))
    board.Add(item)


def add_polyline(board, net_name, layer, points):
    for first, second in zip(points, points[1:]):
        add_track(board, net_name, layer, first, second)


def add_via(board, net_name, point):
    item = pcbnew.PCB_VIA(board)
    item.SetNet(board.FindNet(net_name))
    item.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(point[0]), pcbnew.FromMM(point[1])))
    item.SetWidth(pcbnew.FromMM(VIA_DIAMETER))
    item.SetDrill(pcbnew.FromMM(VIA_DRILL))
    item.SetLayerPair(pcbnew.F_Cu, pcbnew.B_Cu)
    board.Add(item)


def drc(tag):
    BUILD.mkdir(parents=True, exist_ok=True)
    enforce_project_rules()
    report = BUILD / f"{tag}.json"
    subprocess.run([
        str(CLI), "pcb", "drc", str(BOARD), "--format", "json",
        "--severity-all", "--all-track-errors", "--schematic-parity",
        "--output", str(report),
    ], check=False)
    return json.loads(report.read_text(encoding="utf-8"))


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("net", choices=sorted(ROUTES))
    args = parser.parse_args()
    net_name = args.net
    if net_name not in RF50_NETS:
        raise RuntimeError(f"Refusing non-RF50 net {net_name}")
    baseline = drc(f"{net_name}-baseline")
    allowed = len(baseline.get("violations", []))
    board = pcbnew.LoadBoard(str(BOARD))
    spec = ROUTES[net_name]
    add_polyline(board, net_name, pcbnew.F_Cu, spec["front_a"])
    add_via(board, net_name, spec["bottom"][0])
    add_polyline(board, net_name, pcbnew.B_Cu, spec["bottom"])
    add_via(board, net_name, spec["bottom"][-1])
    add_polyline(board, net_name, pcbnew.F_Cu, spec["front_b"])
    pcbnew.SaveBoard(str(BOARD), board)
    report = drc(net_name)
    safe = len(report.get("violations", [])) <= allowed and not report.get("schematic_parity", [])
    print(f"{'ACCEPT' if safe else 'REJECT'} {net_name}")
    if not safe:
        # Exact rollback: the net was incomplete and had no copper before this
        # single-net process was invoked.
        for item in list(board.GetTracks()):
            if item.GetNetname() == net_name:
                board.Remove(item)
        pcbnew.SaveBoard(str(BOARD), board)
        raise RuntimeError(f"{net_name}: bottom route failed DRC")


if __name__ == "__main__":
    main()
