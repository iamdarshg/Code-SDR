"""DRC-guided no-via length tuning for already-routed sensitive pairs."""

from __future__ import annotations

import argparse
import json
import math
import shutil
import subprocess
from pathlib import Path

import pcbnew


KICAD_CLI = Path(r"C:\Program Files\KiCad\9.0\bin\kicad-cli.exe")
PAIRS = (
    ("LO_LOW_P", "LO_LOW_N", 0.25),
    ("IF_LOW_P", "IF_LOW_N", 0.25),
    ("IF_HIGH_P", "IF_HIGH_N", 0.25),
    ("LT_IN_P", "LT_IN_N", 0.25),
    ("AD8351_OUT_P", "AD8351_OUT_N", 0.50),
    ("ADC_VIN_P", "ADC_VIN_N", 0.50),
)


def route_length(board: pcbnew.BOARD, net: str) -> float:
    return sum(
        pcbnew.ToMM(item.GetLength())
        for item in board.GetTracks()
        if item.GetNetname() == net and not isinstance(item, pcbnew.PCB_VIA)
    )


def pair_findings(report: Path, nets: tuple[str, str]) -> list[dict]:
    data = json.loads(report.read_text(encoding="utf-8"))
    findings: list[dict] = []
    for section in ("violations", "unconnected_items"):
        for finding in data.get(section, []):
            rendered = json.dumps(finding)
            if any(net in rendered for net in nets):
                findings.append(finding)
    return findings


def add_dogleg(
    board: pcbnew.BOARD,
    net_name: str,
    start: pcbnew.VECTOR2I,
    end: pcbnew.VECTOR2I,
    offset_mm: float,
) -> None:
    old = next(
        item
        for item in board.GetTracks()
        if item.GetNetname() == net_name
        and not isinstance(item, pcbnew.PCB_VIA)
        and item.GetStart() == start
        and item.GetEnd() == end
    )
    dx = pcbnew.ToMM(end.x - start.x)
    dy = pcbnew.ToMM(end.y - start.y)
    length = math.hypot(dx, dy)
    px, py = -dy / length, dx / length
    delta = pcbnew.VECTOR2I(
        pcbnew.FromMM(px * offset_mm), pcbnew.FromMM(py * offset_mm)
    )
    points = (start, start + delta, end + delta, end)
    layer, width, net = old.GetLayer(), old.GetWidth(), old.GetNet()
    board.Remove(old)
    for first, second in zip(points, points[1:]):
        track = pcbnew.PCB_TRACK(board)
        track.SetNet(net)
        track.SetLayer(layer)
        track.SetWidth(width)
        track.SetStart(first)
        track.SetEnd(second)
        board.Add(track)


def run_drc(board_path: Path, report_path: Path) -> None:
    subprocess.run(
        [
            str(KICAD_CLI), "pcb", "drc", "--severity-all", "--format", "json",
            "--output", str(report_path), str(board_path),
        ],
        check=False,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=Path)
    parser.add_argument("candidate", type=Path)
    parser.add_argument("--project", type=Path, required=True)
    args = parser.parse_args()
    source, candidate = args.source.resolve(), args.candidate.resolve()
    candidate.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, candidate)
    shutil.copy2(args.project.resolve(), candidate.with_suffix(".kicad_pro"))
    trial = candidate.with_name(candidate.stem + "-trial.kicad_pcb")
    trial_project = trial.with_suffix(".kicad_pro")
    shutil.copy2(args.project.resolve(), trial_project)
    report = trial.with_suffix(".drc.json")

    for positive, negative, limit_mm in PAIRS:
        current = pcbnew.LoadBoard(str(candidate))
        lengths = {net: route_length(current, net) for net in (positive, negative)}
        skew = abs(lengths[positive] - lengths[negative])
        if skew <= limit_mm:
            print(f"PASS {positive}/{negative}: {skew:.3f} mm")
            continue
        shorter = min(lengths, key=lengths.get)
        required = skew - limit_mm * 0.80
        amplitude = max(0.05, required / 2.0)
        tracks = sorted(
            (
                item for item in current.GetTracks()
                if item.GetNetname() == shorter and not isinstance(item, pcbnew.PCB_VIA)
            ),
            key=lambda item: item.GetLength(),
            reverse=True,
        )
        accepted = False
        for item in tracks:
            if pcbnew.ToMM(item.GetLength()) < 0.40:
                continue
            start, end = item.GetStart(), item.GetEnd()
            for sign in (1.0, -1.0):
                shutil.copy2(candidate, trial)
                board = pcbnew.LoadBoard(str(trial))
                try:
                    add_dogleg(board, shorter, start, end, sign * amplitude)
                except StopIteration:
                    continue
                pcbnew.SaveBoard(str(trial), board)
                run_drc(trial, report)
                if not pair_findings(report, (positive, negative)):
                    shutil.copy2(trial, candidate)
                    accepted = True
                    tuned = pcbnew.LoadBoard(str(candidate))
                    new_skew = abs(route_length(tuned, positive) - route_length(tuned, negative))
                    print(f"TUNED {positive}/{negative}: {skew:.3f} -> {new_skew:.3f} mm")
                    break
            if accepted:
                break
        if not accepted:
            # Dense pairs often cannot accept one deep fold. Distribute the
            # compensation over several shallow folds instead.
            for _ in range(8):
                current = pcbnew.LoadBoard(str(candidate))
                lengths = {net: route_length(current, net) for net in (positive, negative)}
                skew = abs(lengths[positive] - lengths[negative])
                if skew <= limit_mm:
                    accepted = True
                    break
                shorter = min(lengths, key=lengths.get)
                amplitude = min(0.30, max(0.06, (skew - limit_mm * 0.80) / 2.0))
                tracks = sorted(
                    (
                        item for item in current.GetTracks()
                        if item.GetNetname() == shorter
                        and not isinstance(item, pcbnew.PCB_VIA)
                        and pcbnew.ToMM(item.GetLength()) >= 0.60
                    ),
                    key=lambda item: item.GetLength(),
                    reverse=True,
                )
                step_accepted = False
                for item in tracks:
                    start, end = item.GetStart(), item.GetEnd()
                    for sign in (1.0, -1.0):
                        shutil.copy2(candidate, trial)
                        board = pcbnew.LoadBoard(str(trial))
                        try:
                            add_dogleg(board, shorter, start, end, sign * amplitude)
                        except StopIteration:
                            continue
                        pcbnew.SaveBoard(str(trial), board)
                        run_drc(trial, report)
                        if not pair_findings(report, (positive, negative)):
                            shutil.copy2(trial, candidate)
                            step_accepted = True
                            break
                    if step_accepted:
                        break
                if not step_accepted:
                    break
            final = pcbnew.LoadBoard(str(candidate))
            final_skew = abs(route_length(final, positive) - route_length(final, negative))
            if final_skew <= limit_mm:
                print(f"TUNED {positive}/{negative}: {skew:.3f} -> {final_skew:.3f} mm (distributed)")
            else:
                print(f"UNCHANGED {positive}/{negative}: best DRC-clean skew {final_skew:.3f} mm")

    trial.unlink(missing_ok=True)
    trial_project.unlink(missing_ok=True)
    report.unlink(missing_ok=True)


if __name__ == "__main__":
    main()
