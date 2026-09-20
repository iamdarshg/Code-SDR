from __future__ import annotations

import json
import math
import re
import shutil
import subprocess
import uuid
from collections import Counter
from pathlib import Path

PROJECT = Path("hardware/v2")
ROOT = PROJECT / "build" / "corridor-route"
BOARD = PROJECT / "Code-SDR-V2.kicad_pcb"
CANDIDATES = PROJECT / "tools" / "corridor_candidates.json"
TARGET = 16
NET_RE = re.compile(r'^\s*\(net\s+(\d+)\s+"([^"]+)"\)', re.MULTILINE)
ROOT.mkdir(parents=True, exist_ok=True)


def run_drc(output: Path) -> dict:
    subprocess.run(
        [
            "kicad-cli", "pcb", "drc", "--format", "json", "--severity-all",
            "--all-track-errors", "--schematic-parity", "--units", "mm",
            "-o", str(output), str(BOARD),
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
    return {
        "opens": len(report.get("unconnected_items", [])),
        "signal_opens": sum(count for net, count in by_net.items() if net not in {"GND", "CHASSIS_GND"}),
        "nonlibrary_violations": sum(1 for violation in report.get("violations", []) if not violation.get("type", "").startswith("lib_footprint_")),
        "parity": len(report.get("schematic_parity", [])),
        "by_net": dict(by_net),
    }


def net_codes(board_text: str) -> dict[str, int]:
    result = {name: int(code) for code, name in NET_RE.findall(board_text)}
    if not result:
        raise RuntimeError("No net declarations found")
    return result


def segment_block(a, b, width: float, layer: str, net: int) -> str:
    return (
        "\n\t(segment\n"
        f"\t\t(start {a[0]:.6f} {a[1]:.6f})\n"
        f"\t\t(end {b[0]:.6f} {b[1]:.6f})\n"
        f"\t\t(width {width:.6f})\n"
        f"\t\t(layer \"{layer}\")\n"
        f"\t\t(net {net})\n"
        f"\t\t(uuid \"{uuid.uuid4()}\")\n"
        "\t)"
    )


def via_block(point, net: int) -> str:
    return (
        "\n\t(via\n"
        f"\t\t(at {point[0]:.6f} {point[1]:.6f})\n"
        "\t\t(size 0.450000)\n"
        "\t\t(drill 0.200000)\n"
        "\t\t(layers \"F.Cu\" \"B.Cu\")\n"
        f"\t\t(net {net})\n"
        f"\t\t(uuid \"{uuid.uuid4()}\")\n"
        "\t)"
    )


def add_candidate(board_text: str, candidate: dict) -> str:
    code = net_codes(board_text)[candidate["net"]]
    path = [tuple(point) for point in candidate["path"]]
    blocks = []
    if candidate["vias"]:
        blocks.extend((via_block(path[0], code), via_block(path[-1], code)))
    blocks.extend(
        segment_block(a, b, float(candidate["width"]), candidate["layer"], code)
        for a, b in zip(path, path[1:])
        if math.dist(a, b) > 1e-6
    )
    index = board_text.rfind("\n)")
    if index < 0:
        raise RuntimeError("PCB closing parenthesis not found")
    return board_text[:index] + "".join(blocks) + board_text[index:]


board_text = BOARD.read_text().replace(
    "Inductor_SMD:L_0805_2012Metric_Pad1.15x1.40mm_HandSolder",
    "CodeSDR:Inductor_0805_HandSolder",
)
BOARD.write_text(board_text)
baseline = run_drc(ROOT / "drc-baseline.json")
current = metrics(baseline)
print("BASELINE", json.dumps(current, sort_keys=True), flush=True)
accepted = []
rejected = []
for index, candidate in enumerate(json.loads(CANDIDATES.read_text())):
    if current["signal_opens"] <= TARGET:
        break
    net = candidate["net"]
    old_target = current["by_net"].get(net, 0)
    if old_target <= 0:
        continue
    trial = add_candidate(board_text, candidate)
    BOARD.write_text(trial)
    report = run_drc(ROOT / "drc-candidate.json")
    score = metrics(report)
    keep = (
        score["by_net"].get(net, 0) < old_target
        and score["signal_opens"] < current["signal_opens"]
        and score["nonlibrary_violations"] <= current["nonlibrary_violations"]
        and score["parity"] <= current["parity"]
    )
    record = {**candidate, "index": index, "before_target_opens": old_target, "metrics": score}
    print(net, "ACCEPT" if keep else "REJECT", json.dumps(score, sort_keys=True), flush=True)
    if keep:
        board_text = trial
        current = score
        accepted.append(record)
    else:
        BOARD.write_text(board_text)
        rejected.append(record)
BOARD.write_text(board_text)
final_report = run_drc(ROOT / "drc-final.json")
final_metrics = metrics(final_report)
shutil.copy2(BOARD, ROOT / "Code-SDR-V2.kicad_pcb")
(ROOT / "summary.json").write_text(
    json.dumps({"before": metrics(baseline), "after": final_metrics, "accepted": accepted, "rejected": rejected}, indent=2) + "\n"
)
print("FINAL", json.dumps(final_metrics, sort_keys=True), flush=True)
