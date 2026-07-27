"""Re-place blocked RF groups and DRC-gate their 50-ohm F.Cu routes.

Only nets listed in :mod:`export_rf50_dsn` may receive copper.  The emitted
geometry is the board stackup's nominal 50-ohm width (0.23 mm), uses F.Cu
only, and never adds vias.
"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
from pathlib import Path

import pcbnew

from auto_router import Obstacles, add_track_path, build_grid_and_search, simplify_path
from export_rf50_dsn import RF50_NETS
from generate_pcb import enforce_project_rules


ROOT = Path(__file__).resolve().parents[1]
BOARD = ROOT / "Code-SDR-V2.kicad_pcb"
BUILD = ROOT / "build" / "rf50-reflow"
CLI = Path(r"C:\Program Files\KiCad\9.0\bin\kicad-cli.exe")
WIDTH_MM = 0.23
CLEARANCE_MM = 0.18


HIGH_MOVES = {
    # Rotate the QPC6144 so all four RF inputs face the source bank and the
    # common port faces the mixer coupling capacitor.  This removes the
    # single-layer topological crossing that stranded channel A.
    "U64": (74.0, 22.0, 270.0),
    "C232": (79.0, 22.25, 0.0),
    "U63": (84.0, 22.0, 0.0),
    "C233": (88.0, 21.25, 180.0),
    # Compact non-critical switch support parts outside the RF approach.
    "C260": (88.0, 17.0, 0.0),
    "C261": (91.0, 17.0, 0.0),
    "R144": (88.0, 19.0, 0.0),
    "R145": (91.0, 19.0, 0.0),
    "C93": (84.0, 25.5, 0.0),
    "C94": (88.0, 25.5, 0.0),
}

HIGH_NETS = [
    "RF_HA_LNA_OUT",
    "RF_HB_LNA_OUT",
    "RF_HC_LNA_OUT",
    "RF_HD_LNA_OUT",
    "RF_HIGH_SWITCHED",
    "LTC_RF_IN",
    "LO_HIGH_P",
    "LTC_LO_IN",
]

HB_MOVES = {
    "C216": (38.0, 7.0, 270.0),
    "C2110": (40.5, 8.2, 0.0),
    "U61": (38.0, 9.6, 270.0),
    "R105": (38.0, 12.3, 270.0),
}

HB_NETS = [
    "RF_HB_IN", "RF_HB_LNA_IN", "RF_HB_LNA_OUT_RAW", "RF_HB_LNA_OUT",
]

LO_MOVES = {
    "R53": (16.2, 64.6, 0.0),
    "C202": (17.8, 66.5, 270.0),
    "R52": (20.0, 64.6, 180.0),
    "C201": (19.2, 66.5, 270.0),
    "R54": (21.2, 60.2, 270.0),
    "C203": (22.5, 61.75, 0.0),
    "R55": (22.5, 65.2, 90.0),
    "C204": (22.5, 63.25, 0.0),
    # Compact nearby non-RF support parts away from the controlled launches.
    "R56": (9.0, 69.0, 0.0),
    "C71": (11.5, 69.0, 0.0),
    "R51": (14.0, 69.0, 0.0),
    "C70": (16.5, 69.0, 0.0),
    "C272": (19.0, 69.5, 0.0),
    "C67": (27.0, 61.0, 0.0),
}

LO_NETS = [
    "LMX_LO_HIGH_N_RAW", "LMX_LO_HIGH_P_RAW", "LO_HIGH_N_TERM", "LO_HIGH_P",
    "LMX_LO_LOW_P_RAW", "LMX_LO_LOW_N_RAW", "LO_LOW_P", "LO_LOW_N",
]

LO_N_FIX_MOVES = {"R55": (22.5, 65.2, 90.0)}
LO_N_FIX_NETS = ["LMX_LO_LOW_N_RAW", "LO_LOW_N"]

LOW_MIXER_MOVES = {
    "U51": (29.0, 32.0, 0.0),
    "T50": (33.0, 32.0, 0.0),
    "C282": (32.0, 29.0, 0.0),
    "C283": (34.0, 29.0, 0.0),
}
LOW_MIXER_NETS = ["LO_LOW_P", "LO_LOW_N", "IF_LOW_SE", "GRF2013_RFOUT_VDD"]

LOW_IF_MOVES = {
    "T50": (38.0, 28.0, 180.0),
    "C282": (36.0, 26.0, 0.0),
    "C283": (38.5, 26.0, 0.0),
}
LOW_IF_NETS = ["IF_LOW_SE"]

# Pad-centre to open-corridor escapes.  All escape segments retain the full
# 0.23 mm controlled width; the values merely choose the outward side of
# fine-pitch/closely packed pads so the maze search does not start inside a
# neighbouring-pad keepout.
HIGH_ESCAPES = {
    "RF_HA_LNA_OUT": ((18.20, 10.50), (71.50, 21.25)),
    "RF_HB_LNA_OUT": ((38.00, 11.30), (71.50, 22.75)),
    "RF_HC_LNA_OUT": ((61.00, 11.30), (73.25, 19.50)),
    "RF_HD_LNA_OUT": ((76.00, 13.20), (73.25, 24.50)),
    "RF_HIGH_SWITCHED": ((76.50, 21.75), (78.00, 22.25)),
    "LTC_RF_IN": ((80.00, 22.25), (81.80, 22.75)),
    "LTC_LO_IN": ((86.20, 21.25), (87.68, 21.25)),
    "LO_HIGH_N_TERM": ((17.80, 67.50), (22.20, 69.60)),
    "LO_HIGH_P": ((19.20, 67.50), (89.00, 21.25)),
    "LO_LOW_P": ((23.50, 61.75), (26.30, 31.75)),
    "LO_LOW_N": ((23.50, 63.25), (26.30, 32.25)),
    "IF_LOW_SE": ((31.40, 31.50), (43.40, 30.00)),
}

HIGH_EXPLICIT = {
    "LTC_LO_IN": [(85.45, 21.25), (87.68, 21.25)],
    "RF_HB_IN": [
        (38.00, 0.50), (38.00, 4.90), (37.754, 4.90),
        (37.447, 5.207), (37.447, 6.388), (37.740, 6.680),
        (38.00, 6.680),
    ],
    "RF_HB_LNA_IN": [
        (37.50, 8.875), (37.50, 7.820), (38.00, 7.320),
        (39.30, 7.320), (40.18, 8.200),
    ],
    "RF_HB_LNA_OUT_RAW": [
        (37.50, 10.325), (37.50, 11.200), (38.00, 11.700),
        (38.00, 11.980),
    ],
    "IF_SELECTED": [
        (43.00, 31.350), (43.00, 31.900), (43.50, 32.400),
        (45.50, 32.400), (45.50, 31.000), (46.355, 31.000),
    ],
    "GRF2013_RFOUT_VDD": [
        (50.580, 29.000), (51.20, 28.380), (52.20, 28.380),
        (52.20, 31.500), (51.425, 31.500), (52.20, 31.500),
        (52.80, 32.100), (52.80, 33.908), (53.472, 34.580),
    ],
    "LO_LOW_N": [
        (27.0625, 32.250), (26.300, 32.250), (24.900, 36.050),
        (24.900, 54.550), (24.600, 54.850), (24.600, 57.550),
        (24.400, 57.750), (24.300, 60.350), (24.300, 62.250),
        (23.500, 63.050), (23.500, 63.250), (22.820, 63.250),
    ],
    "IF_LOW_SE": [
        (38.880, 28.500), (39.500, 28.500), (40.200, 29.200),
        (42.700, 29.200), (43.400, 29.900), (43.400, 30.650),
    ],
    "ADF_LO2_N_RAW": [
        (31.5625, 61.2500), (31.0443, 61.2500), (30.7352, 61.5591),
        (30.7352, 61.9661), (33.8005, 65.0314), (33.8005, 65.8816),
        (34.7093, 66.7904), (37.3710, 66.7904), (38.5024, 67.9218),
        (38.5024, 69.5494), (37.5800, 70.4718), (37.5800, 70.9000),
        (37.0215, 71.4585), (33.4685, 71.4585), (32.9100, 70.9000),
    ],
}

BRANCHED_EXPLICIT = {
    "LMX_LO_HIGH_N_RAW": [
        [(17.800, 63.4375), (17.800, 66.180)],
        [(17.800, 64.600), (16.710, 64.600)],
    ],
    "LMX_LO_HIGH_P_RAW": [
        [(18.300, 63.4375), (18.300, 64.000), (19.200, 64.900), (19.200, 66.180)],
        [(18.900, 64.600), (19.490, 64.600)],
    ],
    "LMX_LO_LOW_P_RAW": [
        [(19.4875, 61.750), (22.180, 61.750)],
        [(21.200, 61.750), (21.200, 60.710)],
    ],
    "LMX_LO_LOW_N_RAW": [
        [(19.4875, 62.250), (20.500, 62.250), (21.500, 63.250), (22.180, 63.250)],
        [(21.500, 63.250), (21.500, 64.200), (21.990, 64.690), (22.500, 64.690)],
    ],
}


def run_drc(tag: str) -> dict:
    BUILD.mkdir(parents=True, exist_ok=True)
    enforce_project_rules()
    report = BUILD / f"{tag}.json"
    subprocess.run(
        [
            str(CLI), "pcb", "drc", str(BOARD), "--format", "json",
            "--severity-all", "--all-track-errors", "--schematic-parity",
            "--output", str(report),
        ],
        check=False,
    )
    return json.loads(report.read_text(encoding="utf-8"))


def footprint(board: pcbnew.BOARD, reference: str) -> pcbnew.FOOTPRINT:
    for item in board.GetFootprints():
        if item.GetReference() == reference:
            return item
    raise KeyError(reference)


def move_footprints(board: pcbnew.BOARD, moves) -> None:
    for reference, (x, y, rotation) in moves.items():
        item = footprint(board, reference)
        item.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(x), pcbnew.FromMM(y)))
        item.SetOrientationDegrees(rotation)


def remove_net_tracks(board: pcbnew.BOARD, net_names) -> None:
    allowed = set(net_names)
    if not allowed <= RF50_NETS:
        raise RuntimeError(f"Refusing to touch non-RF50 nets: {sorted(allowed - RF50_NETS)}")
    for item in list(board.GetTracks()):
        if item.GetNetname() in allowed:
            board.Remove(item)


def pads_for(board: pcbnew.BOARD, net_name: str) -> list[pcbnew.PAD]:
    return [
        pad for fp in board.GetFootprints() for pad in fp.Pads()
        if pad.GetNetname() == net_name
    ]


def pad_point(pad: pcbnew.PAD) -> tuple[float, float]:
    point = pad.GetPosition()
    return pcbnew.ToMM(point.x), pcbnew.ToMM(point.y)


def find_path(board: pcbnew.BOARD, net_name: str, source, destination):
    attempts = [
        # Exact production clearance first; progressively widen the search
        # field and refine the grid without relaxing impedance or clearance.
        (8.0, 0.10, 25.0, 500_000, 700_000),
        (20.0, 0.10, 45.0, 900_000, 1_100_000),
        (35.0, 0.08, 70.0, 1_400_000, 1_600_000),
    ]
    for extra, pitch, cap, pops, cells in attempts:
        path = build_grid_and_search(
            Obstacles(board), source, destination, False, net_name,
            extra_pad_mm=extra,
            track_width_mm=WIDTH_MM,
            clearance_mm=CLEARANCE_MM,
            pitch_mm=pitch,
            src_layers={pcbnew.F_Cu},
            dst_layers={pcbnew.F_Cu},
            margin_cap_mm=cap,
            max_pops=pops,
            max_cells=cells,
        )
        if path is not None:
            return simplify_path(path)
    return None


def add_segment(board: pcbnew.BOARD, net_name: str, start, end) -> None:
    item = pcbnew.PCB_TRACK(board)
    item.SetNet(board.FindNet(net_name))
    item.SetLayer(pcbnew.F_Cu)
    item.SetWidth(pcbnew.FromMM(WIDTH_MM))
    item.SetStart(pcbnew.VECTOR2I(pcbnew.FromMM(start[0]), pcbnew.FromMM(start[1])))
    item.SetEnd(pcbnew.VECTOR2I(pcbnew.FromMM(end[0]), pcbnew.FromMM(end[1])))
    board.Add(item)


def route_two_pad_net(board: pcbnew.BOARD, net_name: str, escapes=None) -> tuple[int, int]:
    if net_name not in RF50_NETS:
        raise RuntimeError(f"Refusing non-RF50 net {net_name}")
    if net_name == "GRF2013_RFOUT_VDD":
        source = (51.425, 31.500)
        branch = (52.20, 31.500)
        targets = [
            ((54.10, 34.580), (53.4725, 34.580)),
            ((49.90, 29.000), (50.580, 29.000)),
        ]
        add_segment(board, net_name, source, branch)
        count = 1
        for escape, pad in targets:
            path = find_path(board, net_name, branch, escape)
            if path is None:
                raise RuntimeError(f"{net_name}: no safe branch to {pad}")
            added, vias = add_track_path(board, path, board.FindNet(net_name), WIDTH_MM)
            if vias:
                raise RuntimeError(f"{net_name}: unexpected branch via")
            add_segment(board, net_name, escape, pad)
            count += added + 1
        return count, 0
    if net_name in BRANCHED_EXPLICIT:
        count = 0
        for polyline in BRANCHED_EXPLICIT[net_name]:
            for first, second in zip(polyline, polyline[1:]):
                add_segment(board, net_name, first, second)
                count += 1
        return count, 0
    pads = pads_for(board, net_name)
    if net_name in HIGH_EXPLICIT:
        points = HIGH_EXPLICIT[net_name]
        for first, second in zip(points, points[1:]):
            add_segment(board, net_name, first, second)
        return len(points) - 1, 0
    if len(pads) != 2:
        raise RuntimeError(f"{net_name}: expected two pads, found {len(pads)}")
    start = pad_point(pads[0])
    end = pad_point(pads[1])
    if escapes:
        # Choose the supplied escape nearest each actual endpoint; pad list
        # order is not stable across KiCad saves.
        first, second = escapes
        if sum((start[i] - first[i]) ** 2 for i in (0, 1)) > sum((start[i] - second[i]) ** 2 for i in (0, 1)):
            first, second = second, first
        route_start, route_end = first, second
    else:
        route_start, route_end = start, end
    path = find_path(board, net_name, route_start, route_end)
    if path is None:
        raise RuntimeError(f"{net_name}: no clearance-respecting F.Cu path")
    if escapes:
        add_segment(board, net_name, start, route_start)
        add_segment(board, net_name, route_end, end)
    tracks, vias = add_track_path(board, path, board.FindNet(net_name), WIDTH_MM)
    return tracks + (2 if escapes else 0), vias


def apply_group(moves, nets, tag: str, place_only: bool = False) -> None:
    BUILD.mkdir(parents=True, exist_ok=True)
    accepted = BUILD / f"{tag}-accepted.kicad_pcb"
    original = BUILD / f"{tag}-original.kicad_pcb"
    shutil.copy2(BOARD, original)

    baseline = run_drc(f"{tag}-baseline")
    allowed = len(baseline.get("violations", []))
    print(f"{tag}: baseline violations={allowed}", flush=True)
    board = pcbnew.LoadBoard(str(BOARD))
    print(f"{tag}: board loaded", flush=True)
    move_footprints(board, moves)
    print(f"{tag}: footprints moved", flush=True)
    # Move footprints before bulk track deletion.  Accessing footprints after
    # board.Remove() can invalidate KiCad 9 SWIG item wrappers.
    remove_net_tracks(board, nets)
    print(f"{tag}: selected RF tracks removed", flush=True)
    pcbnew.SaveBoard(str(BOARD), board)
    print(f"{tag}: placement saved", flush=True)
    placement_report = run_drc(f"{tag}-placement")
    if len(placement_report.get("violations", [])) > allowed or placement_report.get("schematic_parity", []):
        shutil.copy2(original, BOARD)
        raise RuntimeError(
            f"{tag}: placement adds DRC errors "
            f"({len(placement_report.get('violations', []))} vs {allowed})"
        )
    shutil.copy2(BOARD, accepted)

    if place_only:
        return

    for net_name in nets:
        try:
            tracks, vias = route_two_pad_net(board, net_name, HIGH_ESCAPES.get(net_name))
        except RuntimeError as error:
            print(f"NO_PATH {error}", flush=True)
            continue
        if vias:
            raise RuntimeError(f"{net_name}: unexpected via count {vias}")
        pcbnew.SaveBoard(str(BOARD), board)
        report = run_drc(f"{tag}-{net_name}")
        safe = (
            len(report.get("violations", [])) <= allowed
            and not report.get("schematic_parity", [])
        )
        print(f"{'ACCEPT' if safe else 'REJECT'} {net_name} segments={tracks}", flush=True)
        if safe:
            shutil.copy2(BOARD, accepted)
        else:
            # Avoid reloading the same board repeatedly in one KiCad 9 Python
            # process (LoadBoard can return an opaque SwigPyObject).  This net
            # began with no tracks, so deleting it is an exact rollback.
            remove_net_tracks(board, [net_name])
            pcbnew.SaveBoard(str(BOARD), board)
    pcbnew.SaveBoard(str(BOARD), board)
    shutil.copy2(BOARD, accepted)


def route_one(net_name: str) -> None:
    """Route and DRC-gate one net in a fresh KiCad Python process."""
    baseline = run_drc(f"one-{net_name}-baseline")
    allowed = len(baseline.get("violations", []))
    board = pcbnew.LoadBoard(str(BOARD))
    tracks, vias = route_two_pad_net(board, net_name, HIGH_ESCAPES.get(net_name))
    if vias:
        raise RuntimeError(f"{net_name}: unexpected via count {vias}")
    pcbnew.SaveBoard(str(BOARD), board)
    report = run_drc(f"one-{net_name}")
    safe = len(report.get("violations", [])) <= allowed and not report.get("schematic_parity", [])
    print(f"{'ACCEPT' if safe else 'REJECT'} {net_name} segments={tracks}", flush=True)
    if not safe:
        remove_net_tracks(board, [net_name])
        pcbnew.SaveBoard(str(BOARD), board)
        raise RuntimeError(f"{net_name}: candidate failed DRC")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "group", nargs="?",
        choices=("high", "hb", "lo", "lo_n_fix", "low_mixer", "low_if"),
    )
    parser.add_argument("--net", choices=sorted(RF50_NETS))
    parser.add_argument("--place-only", action="store_true")
    args = parser.parse_args()
    if args.net:
        route_one(args.net)
    elif args.group == "high":
        apply_group(HIGH_MOVES, HIGH_NETS, "high", place_only=args.place_only)
    elif args.group == "hb":
        apply_group(HB_MOVES, HB_NETS, "hb", place_only=args.place_only)
    elif args.group == "lo":
        apply_group(LO_MOVES, LO_NETS, "lo", place_only=args.place_only)
    elif args.group == "lo_n_fix":
        apply_group(LO_N_FIX_MOVES, LO_N_FIX_NETS, "lo-n-fix", place_only=args.place_only)
    elif args.group == "low_mixer":
        apply_group(LOW_MIXER_MOVES, LOW_MIXER_NETS, "low-mixer", place_only=args.place_only)
    elif args.group == "low_if":
        apply_group(LOW_IF_MOVES, LOW_IF_NETS, "low-if", place_only=args.place_only)
    else:
        parser.error("specify a group or --net")


if __name__ == "__main__":
    main()
