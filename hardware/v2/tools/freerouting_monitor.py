"""Report persistent one-pass Freerouting progress without changing the PCB."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path


KICAD_PYTHON = Path(r"C:\Program Files\KiCad\9.0\bin\python.exe")
NET_PATTERN = re.compile(r"\[([^\]]+)\]")


@dataclass(frozen=True)
class OpenSummary:
    total: int
    by_net: dict[str, int]


def summarize_unconnected(report: dict) -> OpenSummary:
    counts: Counter[str] = Counter()
    items = report.get("unconnected_items", [])
    for issue in items:
        descriptions = [entry.get("description", "") for entry in issue.get("items", [])]
        match = next((NET_PATTERN.search(text) for text in descriptions if NET_PATTERN.search(text)), None)
        counts[match.group(1) if match else "<unknown>"] += 1
    return OpenSummary(len(items), dict(sorted(counts.items())))


def _latest_run(work_root: Path) -> Path:
    runs = sorted((path for path in work_root.glob("*") if path.is_dir()), key=lambda path: path.name)
    if not runs:
        raise FileNotFoundError(f"No Freerouting runs below {work_root}")
    return runs[-1]


def _run_drc(board: Path, output: Path) -> dict:
    subprocess.run(
        ["kicad-cli", "pcb", "drc", str(board), "--format", "json", "--severity-all", "--all-track-errors",
         "--schematic-parity", "--units", "mm", "-o", str(output)],
        text=True, capture_output=True, check=False, timeout=300,
    )
    return json.loads(output.read_text(encoding="utf-8"))


def _process_state(run_dir: Path) -> str:
    state_path = run_dir / "run-state.json"
    if not state_path.exists():
        return "no launcher state recorded"
    state = json.loads(state_path.read_text(encoding="utf-8"))
    pid = state.get("pid")
    if not pid:
        return state.get("state", "unknown")
    query = subprocess.run(["tasklist", "/FI", f"PID eq {pid}", "/NH"], text=True, capture_output=True, check=False)
    if str(pid) in query.stdout:
        return f"running (PID {pid})"
    return f"finished (PID {pid} exited); inspect result.json and router logs"


def _print_status(run_dir: Path) -> None:
    from freerouting_contract import measure_pairs

    candidate = run_dir / "candidate" / "candidate.kicad_pcb"
    if not candidate.exists():
        raise FileNotFoundError(f"Candidate board is missing: {candidate}")
    baseline_path = run_dir / "baseline-drc.json"
    baseline = json.loads(baseline_path.read_text(encoding="utf-8")) if baseline_path.exists() else None
    current = _run_drc(candidate, run_dir / "status-drc.json")
    current_summary = summarize_unconnected(current)
    print(f"Run: {run_dir}")
    print(f"State: {_process_state(run_dir)}")
    if baseline:
        before = summarize_unconnected(baseline).total
        connected = before - current_summary.total
        percent = 100 * connected / before if before else 100.0
        print(f"Connections routed this pass: {connected}/{before} ({percent:.1f}%)")
    print(f"Connections remaining: {current_summary.total}")
    print("Remaining connections by net:")
    for net_name, count in current_summary.by_net.items():
        print(f"  {net_name}: {count}")
    print("Differential pair status (P/N mm, skew mm, tolerance mm, via P/N, connected):")
    for pair in measure_pairs(candidate).values():
        print(
            f"  {pair.name}: {pair.positive_length_mm:.3f}/{pair.negative_length_mm:.3f}, "
            f"skew {pair.skew_mm:.3f}, tol {pair.tolerance_mm:.3f}, "
            f"vias {pair.positive_vias}/{pair.negative_vias}, connected={pair.connected}"
        )


def main() -> None:
    if "pcbnew" not in sys.modules:
        try:
            import pcbnew  # noqa: F401
        except ImportError:
            subprocess.run([str(KICAD_PYTHON), str(Path(__file__).resolve()), *sys.argv[1:]], check=True)
            return
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work-root", type=Path, default=Path("hardware/v2/build/one-pass-freerouting"))
    parser.add_argument("--run", type=Path, help="specific run directory; default is newest")
    args = parser.parse_args()
    _print_status(args.run or _latest_run(args.work_root))


if __name__ == "__main__":
    main()
