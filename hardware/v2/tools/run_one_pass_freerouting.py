"""Run one preservation-gated Freerouting pass against a candidate PCB copy.

The live board is copied only after every acceptance gate succeeds.  This
module deliberately has no top-level ``pcbnew`` import, so its dry-run and
argument handling remain usable with the system Python interpreter.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys
import urllib.request
from dataclasses import asdict, dataclass
from datetime import datetime
from pathlib import Path

from freerouting_contract import BoardSnapshot, PairMeasurement, compare_preservation, snapshot_board, transform_dsn


OFFICIAL_ROUTER_URL = (
    "https://github.com/freerouting/freerouting/releases/download/v2.3.0/"
    "freerouting-2.3.0.jar"
)
KICAD_PYTHON = Path(r"C:\Program Files\KiCad\9.0\bin\python.exe")
_router_invoked = False


@dataclass(frozen=True)
class RouteResult:
    run_dir: Path
    checkpoint_path: Path
    candidate_path: Path
    dsn_path: Path
    ses_path: Path
    accepted: bool
    router_invocations: int
    before_opens: int | None
    after_opens: int | None
    preservation_errors: tuple[str, ...]
    quality_errors: tuple[str, ...]
    before_pairs: dict[str, PairMeasurement]
    after_pairs: dict[str, PairMeasurement]


def _sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _new_run_dir(work_root: Path) -> Path:
    run_dir = work_root / datetime.now().strftime("%Y%m%d-%H%M%S-%f")
    run_dir.mkdir(parents=True, exist_ok=False)
    return run_dir


def _copy_inputs(board_path: Path, project_path: Path, run_dir: Path) -> tuple[Path, Path, Path]:
    checkpoint_dir = run_dir / "checkpoint"
    candidate_dir = run_dir / "candidate"
    checkpoint_dir.mkdir()
    candidate_dir.mkdir()
    checkpoint = checkpoint_dir / board_path.name
    checkpoint_project = checkpoint_dir / project_path.name
    candidate = candidate_dir / board_path.name
    candidate_project = candidate_dir / project_path.name
    shutil.copy2(board_path, checkpoint)
    shutil.copy2(project_path, checkpoint_project)
    shutil.copy2(checkpoint, candidate)
    shutil.copy2(checkpoint_project, candidate_project)
    # DRC/parity and library checks require the complete project context in
    # each isolated directory. A board/project pair alone silently drops the
    # custom .dru, local libraries, hierarchy, and schematic parity source.
    source_root = board_path.resolve().parent
    sidecar_files = [
        board_path.with_suffix(".kicad_dru"),
        board_path.with_suffix(".kicad_sch"),
        source_root / "fp-lib-table",
        source_root / "sym-lib-table",
        source_root / "CodeSDR.kicad_sym",
    ]
    for source in sidecar_files:
        if source.is_file():
            shutil.copy2(source, checkpoint_dir / source.name)
            shutil.copy2(source, candidate_dir / source.name)
    for name in ("sheets", "CodeSDR.pretty"):
        source = source_root / name
        if source.is_dir():
            shutil.copytree(source, checkpoint_dir / name)
            shutil.copytree(source, candidate_dir / name)
    (run_dir / "input-hashes.json").write_text(
        json.dumps(
            {
                "board": {"path": str(board_path), "sha256": _sha256(checkpoint)},
                "project": {"path": str(project_path), "sha256": _sha256(checkpoint_project)},
            },
            indent=2,
        ),
        encoding="utf-8",
    )
    return checkpoint, candidate, candidate_project


def _export_transformed_dsn(
    candidate: Path, dsn_path: Path, editable_nets: frozenset[str], route_only: bool = False,
) -> None:
    import pcbnew

    board = pcbnew.LoadBoard(str(candidate))
    if not pcbnew.ExportSpecctraDSN(board, str(dsn_path)):
        raise RuntimeError(f"KiCad failed to export DSN: {dsn_path}")
    dsn_path.write_text(
        transform_dsn(dsn_path.read_text(encoding="utf-8"), set(editable_nets), route_only=route_only),
        encoding="utf-8",
    )


def _download_router(run_dir: Path) -> Path:
    jar = run_dir / "freerouting-2.3.0.jar"
    urllib.request.urlretrieve(OFFICIAL_ROUTER_URL, jar)
    (run_dir / "router.json").write_text(
        json.dumps({"url": OFFICIAL_ROUTER_URL, "sha256": _sha256(jar)}, indent=2), encoding="utf-8"
    )
    return jar


def _invoke_router_once(
    router_jar: Path, dsn_path: Path, ses_path: Path, run_dir: Path, router_timeout: float | None,
) -> int:
    global _router_invoked
    if _router_invoked:
        raise RuntimeError("Freerouting has already been invoked by this process")
    if not router_jar.is_file():
        raise FileNotFoundError(f"Missing Freerouting JAR: {router_jar}")
    router_jar, dsn_path, ses_path, run_dir = (
        router_jar.resolve(), dsn_path.resolve(), ses_path.resolve(), run_dir.resolve()
    )
    command = [
        "java", "-Xmx2600m", "-jar", str(router_jar), "-de", str(dsn_path), "-do", str(ses_path),
        "-mp", "1", "-mt", "0", "-is", "prioritized", "--gui.enabled=false",
    ]
    _router_invoked = True
    (run_dir / "router-command.json").write_text(json.dumps(command, indent=2), encoding="utf-8")
    try:
        completed = subprocess.run(
            command, cwd=run_dir, text=True, capture_output=True, timeout=router_timeout
        )
    except subprocess.TimeoutExpired as exc:
        (run_dir / "router-stdout.log").write_text(exc.stdout or "", encoding="utf-8")
        (run_dir / "router-stderr.log").write_text(exc.stderr or "", encoding="utf-8")
        (run_dir / "router-result.json").write_text(
            json.dumps({"timed_out": True, "timeout_seconds": router_timeout, "invocations": 1}, indent=2),
            encoding="utf-8",
        )
        raise RuntimeError(f"Freerouting timed out after {router_timeout} seconds; see {run_dir}") from exc
    (run_dir / "router-stdout.log").write_text(completed.stdout, encoding="utf-8")
    (run_dir / "router-stderr.log").write_text(completed.stderr, encoding="utf-8")
    (run_dir / "router-result.json").write_text(
        json.dumps({"returncode": completed.returncode, "invocations": 1}, indent=2), encoding="utf-8"
    )
    if completed.returncode != 0:
        raise RuntimeError(f"Freerouting returned {completed.returncode}; see {run_dir}")
    if not ses_path.is_file() or ses_path.stat().st_size == 0:
        raise RuntimeError("Freerouting completed without a usable SES file")
    return 1


def _import_ses(candidate: Path, ses_path: Path) -> None:
    import pcbnew

    board = pcbnew.LoadBoard(str(candidate))
    if not pcbnew.ImportSpecctraSES(board, str(ses_path)):
        raise RuntimeError("KiCad failed to import the Freerouting session")
    pcbnew.SaveBoard(str(candidate), board)


def _run_drc(board_path: Path, report_path: Path) -> dict:
    board_path = board_path.resolve()
    report_path = report_path.resolve()
    command = [
        "kicad-cli", "pcb", "drc", str(board_path), "--format", "json", "--severity-all",
        "--all-track-errors", "--schematic-parity", "--units", "mm", "-o", str(report_path),
    ]
    # KiCad resolves the project-local .kicad_dru and fp-lib-table from the
    # project directory. Running from an isolated build directory silently
    # drops those rules and reports a different DRC contract.
    subprocess.run(
        command, cwd=board_path.parent, text=True, capture_output=True,
        check=False, timeout=180,
    )
    if not report_path.is_file():
        raise RuntimeError(f"KiCad DRC did not write {report_path}")
    return json.loads(report_path.read_text(encoding="utf-8"))


def _issue_count(report: dict, name: str) -> int:
    return len(report.get(name, []))


def _pair_quality_errors(before: BoardSnapshot, after: BoardSnapshot) -> list[str]:
    errors: list[str] = []
    for name, old in before.pair_measurements.items():
        new = after.pair_measurements[name]
        old_valid = old.connected and old.skew_mm <= old.tolerance_mm and old.positive_vias == old.negative_vias
        new_valid = new.connected and new.skew_mm <= new.tolerance_mm and new.positive_vias == new.negative_vias
        if old_valid and not new_valid:
            errors.append(f"completed pair regressed: {name}")
        if new.connected and not new_valid:
            errors.append(f"completed pair outside skew/via tolerance: {name}")
    return errors


def _write_report(result: RouteResult) -> None:
    def pairs(values: dict[str, PairMeasurement]) -> dict[str, dict]:
        return {name: asdict(value) for name, value in values.items()}

    payload = {
        "accepted": result.accepted,
        "router_invocations": result.router_invocations,
        "before_opens": result.before_opens,
        "after_opens": result.after_opens,
        "preservation_errors": result.preservation_errors,
        "quality_errors": result.quality_errors,
        "before_pairs": pairs(result.before_pairs),
        "after_pairs": pairs(result.after_pairs),
    }
    (result.run_dir / "result.json").write_text(json.dumps(payload, indent=2), encoding="utf-8")


def run_one_pass(
    board_path: Path,
    project_path: Path,
    work_root: Path,
    router_jar: Path | None,
    *,
    dry_run: bool = False,
    download_official_router: bool = False,
    router_timeout: float | None = None,
    route_only: bool = False,
    require_eligible_pairs: bool = False,
    editable_nets_override: frozenset[str] | None = None,
) -> RouteResult:
    """Create a candidate, run at most once, and copy it over only if accepted."""
    global _router_invoked
    _router_invoked = False
    board_path, project_path = map(Path, (board_path, project_path))
    work_root = Path(work_root).resolve()
    if not board_path.is_file() or not project_path.is_file():
        raise FileNotFoundError("board and project files must exist")
    run_dir = _new_run_dir(work_root)
    checkpoint, candidate, _candidate_project = _copy_inputs(board_path, project_path, run_dir)
    before = snapshot_board(checkpoint)
    editable_nets = editable_nets_override or before.editable_nets
    dsn = run_dir / "input.dsn"
    ses = run_dir / "output.ses"
    _export_transformed_dsn(candidate, dsn, editable_nets, route_only)
    if dry_run:
        result = RouteResult(
            run_dir, checkpoint, candidate, dsn, ses, False, 0, None, None, (), (),
            before.pair_measurements, before.pair_measurements,
        )
        _write_report(result)
        return result

    before_drc = _run_drc(checkpoint, run_dir / "baseline-drc.json")
    before_opens = _issue_count(before_drc, "unconnected_items")
    jar = _download_router(run_dir) if download_official_router else router_jar
    if jar is None:
        raise ValueError("provide --router-jar or --download-official-router")
    invocations = _invoke_router_once(Path(jar), dsn, ses, run_dir, router_timeout)
    _import_ses(candidate, ses)
    after = snapshot_board(candidate)
    after_drc = _run_drc(candidate, run_dir / "candidate-drc.json")
    after_opens = _issue_count(after_drc, "unconnected_items")
    preservation_errors = compare_preservation(before, candidate)
    quality_errors = _pair_quality_errors(before, after)
    if require_eligible_pairs:
        for name, old in before.pair_measurements.items():
            new = after.pair_measurements[name]
            if all(net in editable_nets for net in (*old.positive, *old.negative)) and not (
                new.connected and new.skew_mm <= new.tolerance_mm and new.positive_vias == new.negative_vias
            ):
                quality_errors.append(f"eligible pair remains incomplete or out of tolerance: {name}")
    if _issue_count(after_drc, "violations") > _issue_count(before_drc, "violations"):
        quality_errors.append("candidate adds KiCad DRC violations")
    if _issue_count(after_drc, "schematic_parity") > _issue_count(before_drc, "schematic_parity"):
        quality_errors.append("candidate adds schematic parity issues")
    before_editable_inner = sum(
        count for signature, count in before.internal_items.items()
        if signature[1] in editable_nets
    )
    after_editable_inner = sum(
        count for signature, count in after.internal_items.items()
        if signature[1] in editable_nets
    )
    # A correction pass may reroute an already-connected signal off a reserved
    # reference plane without changing the ratsnest count. Treat that as a
    # real electrical improvement, while still rejecting no-op candidates.
    if after_opens >= before_opens and after_editable_inner >= before_editable_inner:
        quality_errors.append(
            f"neither unconnected items ({before_opens} -> {after_opens}) nor "
            f"editable inner-layer segments ({before_editable_inner} -> {after_editable_inner}) improved"
        )
    accepted = not preservation_errors and not quality_errors
    if accepted:
        shutil.copy2(candidate, board_path)
    result = RouteResult(
        run_dir, checkpoint, candidate, dsn, ses, accepted, invocations, before_opens, after_opens,
        tuple(preservation_errors), tuple(quality_errors), before.pair_measurements, after.pair_measurements,
    )
    _write_report(result)
    return result


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--board", type=Path, required=True)
    parser.add_argument("--project", type=Path, required=True)
    parser.add_argument("--work-root", type=Path, required=True)
    parser.add_argument("--router-jar", type=Path)
    parser.add_argument("--download-official-router", action="store_true")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--router-timeout", type=float, help="seconds; omitted means no timeout")
    parser.add_argument("--route-only", action="store_true", help="route only unlocked differential nets")
    parser.add_argument("--require-eligible-pairs", action="store_true", help="reject unless all unlocked pairs meet tolerance")
    parser.add_argument("--editable-net", action="append", default=[], help="net to unlock; repeat as needed")
    return parser.parse_args()


def main() -> None:
    if not importlib_available("pcbnew"):
        if not KICAD_PYTHON.is_file():
            raise RuntimeError(f"KiCad Python not found: {KICAD_PYTHON}")
        subprocess.run(
            [str(KICAD_PYTHON), str(Path(__file__).resolve()), *sys.argv[1:]],
            check=True,
        )
        return
    args = _parse_args()
    result = run_one_pass(
        args.board, args.project, args.work_root, args.router_jar,
        dry_run=args.dry_run, download_official_router=args.download_official_router,
        router_timeout=args.router_timeout,
        route_only=args.route_only, require_eligible_pairs=args.require_eligible_pairs,
        editable_nets_override=frozenset(args.editable_net) or None,
    )
    print(json.dumps({"accepted": result.accepted, "run_dir": str(result.run_dir), "router_invocations": result.router_invocations}))
    if not args.dry_run and not result.accepted:
        raise SystemExit(2)


def importlib_available(module_name: str) -> bool:
    import importlib.util
    return importlib.util.find_spec(module_name) is not None


if __name__ == "__main__":
    main()
