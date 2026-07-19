from __future__ import annotations

import json
import math
import re
import shutil
import subprocess
from collections import Counter
from pathlib import Path

import pcbnew

project = Path("hardware/v2")
root = project / "build" / "finish-sweep"
board_path = project / "Code-SDR-V2.kicad_pcb"
accepted_path = root / "accepted.kicad_pcb"
drc_path = root / "drc-current.json"
candidate_drc = root / "candidate-drc.json"

# Reconcile the routed artifact with the project-local footprint now on main.
text = board_path.read_text()
text = text.replace(
    "Inductor_SMD:L_0805_2012Metric_Pad1.15x1.40mm_HandSolder",
    "CodeSDR:Inductor_0805_HandSolder",
)
board_path.write_text(text)


def run_drc(output: Path) -> dict:
    subprocess.run(
        [
            "kicad-cli",
            "pcb",
            "drc",
            "--format",
            "json",
            "--severity-all",
            "--all-track-errors",
            "--schematic-parity",
            "--units",
            "mm",
            "-o",
            str(output),
            str(board_path),
        ],
        check=False,
    )
    if not output.is_file():
        raise RuntimeError(f"DRC did not create {output}")
    return json.loads(output.read_text())


def entry_net(entry: dict) -> str:
    text = " ".join(item.get("description", "") for item in entry.get("items", []))
    match = re.search(r"\[([^\]]+)\]", text)
    return match.group(1) if match else ""


def metrics(report: dict) -> dict:
    by_net = Counter(entry_net(entry) for entry in report.get("unconnected_items", []))
    nonlibrary = sum(
        1
        for violation in report.get("violations", [])
        if not violation.get("type", "").startswith("lib_footprint_")
    )
    return {
        "opens": len(report.get("unconnected_items", [])),
        "signal_opens": sum(
            count
            for net, count in by_net.items()
            if net not in {"GND", "CHASSIS_GND"}
        ),
        "nonlibrary_violations": nonlibrary,
        "library_violations": sum(
            1
            for violation in report.get("violations", [])
            if violation.get("type", "").startswith("lib_footprint_")
        ),
        "parity": len(report.get("schematic_parity", [])),
        "by_net": dict(by_net),
    }


def point_mm(vector):
    return (pcbnew.ToMM(vector.x), pcbnew.ToMM(vector.y))


def close(a, b, tolerance=0.002):
    return math.dist(a, b) <= tolerance


def find_net(board, net_name):
    for footprint in board.GetFootprints():
        for pad in footprint.Pads():
            if pad.GetNetname() == net_name:
                return pad.GetNet()
    for item in board.GetTracks():
        if item.GetNetname() == net_name:
            return item.GetNet()
    raise KeyError(f"Net not found on board: {net_name}")


def add_track(board, net_name, a, b, width, layer=pcbnew.F_Cu):
    if math.dist(a, b) < 1e-5:
        return
    track = pcbnew.PCB_TRACK(board)
    track.SetStart(pcbnew.VECTOR2I_MM(*a))
    track.SetEnd(pcbnew.VECTOR2I_MM(*b))
    track.SetWidth(pcbnew.FromMM(width))
    track.SetLayer(layer)
    track.SetNet(find_net(board, net_name))
    board.Add(track)


def add_via(board, net_name, point):
    via = pcbnew.PCB_VIA(board)
    via.SetPosition(pcbnew.VECTOR2I_MM(*point))
    via.SetWidth(pcbnew.FromMM(0.45))
    via.SetDrill(pcbnew.FromMM(0.20))
    via.SetLayerPair(pcbnew.F_Cu, pcbnew.B_Cu)
    via.SetNet(find_net(board, net_name))
    board.Add(via)


def width_for(net: str) -> float:
    if net.startswith(("RF_HC", "RF_HD")) or net in {
        "LMX_LO_HIGH_N_RAW",
        "LMX_LO_HIGH_P_RAW",
        "LO_HIGH_P",
        "LO_HIGH_N_TERM",
    }:
        return 0.30
    if net.startswith(("RF_", "LO_", "LMX_LO", "PE4312", "IF_")):
        return 0.25
    if net.startswith(("ADC_", "RGMII", "REF_100M")):
        return 0.10
    if net.startswith("+") or net.startswith("FPGA_VCC"):
        return 0.15
    return 0.10


def critical_rf(net: str) -> bool:
    return net.startswith(("RF_HC", "RF_HD")) or net in {
        "LMX_LO_HIGH_N_RAW",
        "LMX_LO_HIGH_P_RAW",
        "LO_HIGH_P",
        "LO_HIGH_N_TERM",
    }


def geometric_paths(a, b):
    ax, ay = a
    bx, by = b
    dx = bx - ax
    dy = by - ay
    paths = [[a, b], [a, (bx, ay), b], [a, (ax, by), b]]
    if abs(dx) > 1e-6 and abs(dy) > 1e-6:
        step = min(abs(dx), abs(dy))
        paths.append(
            [a, (ax + math.copysign(step, dx), ay + math.copysign(step, dy)), b]
        )
    length = max(math.hypot(dx, dy), 1e-6)
    nx, ny = -dy / length, dx / length
    for offset in (
        0.5,
        -0.5,
        1.0,
        -1.0,
        1.5,
        -1.5,
        2.0,
        -2.0,
        3.0,
        -3.0,
        4.0,
        -4.0,
    ):
        q1 = (ax + dx * 0.30 + nx * offset, ay + dy * 0.30 + ny * offset)
        q2 = (ax + dx * 0.70 + nx * offset, ay + dy * 0.70 + ny * offset)
        paths.append([a, q1, q2, b])
    unique = []
    seen = set()
    for path in paths:
        key = tuple((round(x, 4), round(y, 4)) for x, y in path)
        if key not in seen:
            seen.add(key)
            unique.append(path)
    return unique


def candidate_variants(net, a, b):
    for path in geometric_paths(a, b):
        yield ("F.Cu", path, False)
    if not critical_rf(net):
        for path in geometric_paths(a, b):
            yield ("In2.Cu", path, True)


baseline_report = run_drc(root / "drc-baseline.json")
current = metrics(baseline_report)
print("BASELINE", json.dumps(current, sort_keys=True), flush=True)
shutil.copy2(board_path, accepted_path)
shutil.copy2(root / "drc-baseline.json", drc_path)
accepted = [{"kind": "baseline", "metrics": current}]
rejected = []


def accept_if_better(label, mutate):
    current_score = metrics(json.loads(drc_path.read_text()))
    board = pcbnew.LoadBoard(str(accepted_path))
    details = mutate(board)
    pcbnew.ZONE_FILLER(board).Fill(board.Zones())
    pcbnew.SaveBoard(str(board_path), board)
    report = run_drc(candidate_drc)
    score = metrics(report)
    better = (
        score["nonlibrary_violations"] <= current_score["nonlibrary_violations"]
        and score["signal_opens"] <= current_score["signal_opens"]
        and score["parity"] <= current_score["parity"]
        and (
            score["opens"] < current_score["opens"]
            or score["nonlibrary_violations"]
            < current_score["nonlibrary_violations"]
            or score["parity"] < current_score["parity"]
        )
    )
    print(
        label,
        "ACCEPT" if better else "REJECT",
        json.dumps(score, sort_keys=True),
        flush=True,
    )
    record = {"kind": label, "details": details, "metrics": score}
    if better:
        shutil.copy2(board_path, accepted_path)
        shutil.copy2(candidate_drc, drc_path)
        accepted.append(record)
        return True, score
    shutil.copy2(accepted_path, board_path)
    rejected.append(record)
    return False, current_score


def fix_clock(board):
    targets = [
        ((58.75, 22.325), (59.4667, 22.325)),
        ((59.4667, 22.325), (59.4667, 24.0849)),
        ((59.4667, 24.0849), (65.6755, 30.2937)),
    ]
    removed = []
    for item in list(board.GetTracks()):
        if item.GetNetname() != "REF_100M_ADC" or not hasattr(item, "GetStart"):
            continue
        start, end = point_mm(item.GetStart()), point_mm(item.GetEnd())
        for a, b in targets:
            if (close(start, a) and close(end, b)) or (
                close(start, b) and close(end, a)
            ):
                removed.append((start, end))
                board.Remove(item)
                break
    add_track(
        board,
        "REF_100M_ADC",
        (58.75, 22.325),
        (59.80, 22.325),
        0.13,
        pcbnew.F_Cu,
    )
    add_track(
        board,
        "REF_100M_ADC",
        (59.80, 22.325),
        (59.80, 24.0849),
        0.13,
        pcbnew.F_Cu,
    )
    add_track(
        board,
        "REF_100M_ADC",
        (59.80, 24.0849),
        (65.6755, 30.2937),
        0.13,
        pcbnew.F_Cu,
    )
    return {"removed": removed, "detour_x_mm": 59.80}


accept_if_better("REF_100M_ADC_clearance_detour", fix_clock)


def remove_outer_pours(board):
    removed = []
    for zone in list(board.Zones()):
        if zone.GetNetname() == "GND" and zone.GetLayer() in (
            pcbnew.F_Cu,
            pcbnew.B_Cu,
        ):
            removed.append(board.GetLayerName(zone.GetLayer()))
            board.Remove(zone)
    return {"removed_layers": removed}


accept_if_better("remove_disconnected_outer_GND_pours", remove_outer_pours)


def connect_shields(board):
    pads = [
        (45.1, 101.2375),
        (47.0, 101.2375),
        (49.0, 101.2375),
        (50.9, 101.2375),
    ]
    for a, b in zip(pads, pads[1:]):
        add_track(board, "CHASSIS_GND", a, b, 0.50, pcbnew.F_Cu)
    add_track(
        board,
        "CHASSIS_GND",
        pads[0],
        (45.1, 99.50),
        0.50,
        pcbnew.F_Cu,
    )
    add_via(board, "CHASSIS_GND", (45.1, 99.50))
    add_track(
        board,
        "CHASSIS_GND",
        (45.1, 99.50),
        (45.5, 98.5375),
        0.50,
        pcbnew.In2_Cu,
    )
    return {"pads": pads, "via": (45.1, 99.50)}


accept_if_better("connect_USB_shield_chain", connect_shields)

report = json.loads(drc_path.read_text())
entries = []
for entry in report.get("unconnected_items", []):
    net = entry_net(entry)
    items = entry.get("items", [])
    if net in {"", "GND", "CHASSIS_GND"} or len(items) != 2:
        continue
    a = (float(items[0]["pos"]["x"]), float(items[0]["pos"]["y"]))
    b = (float(items[1]["pos"]["x"]), float(items[1]["pos"]["y"]))
    distance = math.dist(a, b)
    if distance <= 13.6:
        entries.append((distance, net, a, b))
entries.sort()
print("SHORT_GAP_COUNT", len(entries), flush=True)

attempted = set()
for distance, net, a, b in entries:
    key = (
        net,
        round(a[0], 3),
        round(a[1], 3),
        round(b[0], 3),
        round(b[1], 3),
    )
    if key in attempted:
        continue
    attempted.add(key)
    old_target = metrics(json.loads(drc_path.read_text()))["by_net"].get(net, 0)
    if old_target == 0:
        continue
    for index, (layer_name, path, use_vias) in enumerate(
        candidate_variants(net, a, b)
    ):
        layer = pcbnew.F_Cu if layer_name == "F.Cu" else pcbnew.In2_Cu

        def mutate(
            board,
            net=net,
            path=path,
            use_vias=use_vias,
            layer=layer,
            distance=distance,
            index=index,
            layer_name=layer_name,
        ):
            if use_vias:
                add_via(board, net, path[0])
                add_via(board, net, path[-1])
            for p1, p2 in zip(path, path[1:]):
                add_track(board, net, p1, p2, width_for(net), layer)
            return {
                "net": net,
                "distance_mm": round(distance, 4),
                "variant": index,
                "layer": layer_name,
                "vias": use_vias,
                "path": path,
            }

        improved, score = accept_if_better(f"close_{net}_{index}", mutate)
        if improved and score["by_net"].get(net, 0) < old_target:
            break

shutil.copy2(accepted_path, board_path)
final_report = run_drc(root / "drc-finish-sweep.json")
final_metrics = metrics(final_report)
shutil.copy2(board_path, root / "Code-SDR-V2.kicad_pcb")
(root / "accepted-patches.json").write_text(json.dumps(accepted, indent=2) + "\n")
(root / "rejected-patches.json").write_text(json.dumps(rejected, indent=2) + "\n")
(root / "finish-sweep-summary.json").write_text(
    json.dumps(
        {
            "before": metrics(baseline_report),
            "after": final_metrics,
            "accepted_patch_count": len(accepted) - 1,
            "rejected_patch_count": len(rejected),
        },
        indent=2,
    )
    + "\n"
)
print("FINAL", json.dumps(final_metrics, sort_keys=True), flush=True)
