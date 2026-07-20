#!/usr/bin/env python3
"""Complete routing pipeline orchestration for Code-SDR V2.

This script coordinates:
1. PCB validation and baseline metrics
2. DSN export for FreeRouting
3. SES import and integration
4. Post-route RF via fencing
5. Design validation and audits
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Optional


ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build"
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"
SCH_PATH = ROOT / "Code-SDR-V2.kicad_sch"
DSN_PATH = BUILD / "Code-SDR-V2.dsn"
SES_PATH = BUILD / "Code-SDR-V2-routed.ses"


def run_cmd(cmd: list[str], description: str, fatal: bool = False) -> int:
    """Run a shell command with logging."""
    print(f"\n{'='*60}")
    print(f"[CMD] {description}")
    print(f"{'='*60}")
    print(" ".join(cmd))
    try:
        result = subprocess.run(cmd, cwd=str(ROOT), check=False)
        if result.returncode != 0 and fatal:
            print(f"ERROR: Command failed with code {result.returncode}")
            return result.returncode
        return result.returncode
    except FileNotFoundError as e:
        print(f"ERROR: Command not found: {cmd[0]}")
        return 1


def validate_pcb_structure() -> bool:
    """Validate that PCB has required structure."""
    print("\n[VALIDATE] PCB Structure")

    if not PCB_PATH.exists():
        print(f"ERROR: PCB not found: {PCB_PATH}")
        return False

    # Would need pcbnew to fully validate, but we can do basic checks
    with open(PCB_PATH) as f:
        content = f.read()

    if not ("(footprint " in content):
        print("ERROR: No footprints found in PCB")
        return False

    if not ("(net " in content):
        print("ERROR: No nets found in PCB")
        return False

    print("✓ PCB structure looks valid")
    return True


def create_baseline_metrics() -> dict:
    """Create baseline metrics before routing."""
    print("\n[BASELINE] PCB Metrics")

    import hashlib
    import re

    with open(PCB_PATH) as f:
        content = f.read()

    metrics = {
        "sha256": hashlib.sha256(content.encode()).hexdigest(),
        "file_size": len(content),
        "footprints": len(re.findall(r"\(footprint\s", content)),
        "nets": len(re.findall(r"\(net\s\d+", content)),
        "tracks": len(re.findall(r"\(segment\s", content)),
        "vias": len(re.findall(r"\(via\s", content)),
        "zones": len(re.findall(r"\(zone\s", content)),
    }

    BUILD.mkdir(exist_ok=True)
    with open(BUILD / "baseline.json", "w") as f:
        json.dump(metrics, f, indent=2)

    print(f"Footprints: {metrics['footprints']}")
    print(f"Nets: {metrics['nets']}")
    print(f"Tracks: {metrics['tracks']}")
    print(f"Vias: {metrics['vias']}")
    print(f"Zones: {metrics['zones']}")

    if metrics["tracks"] == 0 and metrics["vias"] == 0:
        print("⚠ PCB is completely unrouted")

    return metrics


def run_erc() -> int:
    """Run schematic ERC."""
    return run_cmd(
        [
            "kicad-cli",
            "sch",
            "erc",
            "--format",
            "json",
            "--severity-error",
            "-o",
            str(BUILD / "erc.json"),
            str(SCH_PATH),
        ],
        "Schematic ERC validation",
    )


def export_dsn() -> int:
    """Export PCB to DSN for routing."""
    # Try to use KiCad's export
    code = run_cmd(
        [
            "kicad-cli",
            "pcb",
            "export",
            "dsn",
            "-o",
            str(DSN_PATH),
            str(PCB_PATH),
        ],
        "Export PCB to DSN",
    )

    if code != 0:
        print("\n[INFO] KiCad CLI export failed, trying programmatic export")
        try:
            import pcbnew

            board = pcbnew.LoadBoard(str(PCB_PATH))
            if not pcbnew.ExportSpecctraDSN(board, str(DSN_PATH)):
                print("ERROR: Failed to export DSN via pcbnew")
                return 1
            print(f"✓ DSN exported via pcbnew: {DSN_PATH}")
            return 0
        except ImportError:
            print("ERROR: pcbnew module not available, cannot export DSN")
            return 1

    return code


def run_drc() -> int:
    """Run DRC on current PCB state."""
    return run_cmd(
        [
            "kicad-cli",
            "pcb",
            "drc",
            "--format",
            "json",
            "--all-track-errors",
            "-o",
            str(BUILD / "drc.json"),
            str(PCB_PATH),
        ],
        "PCB Design Rule Check",
    )


def import_ses() -> int:
    """Import FreeRouting SES result."""
    if not SES_PATH.exists() or SES_PATH.stat().st_size == 0:
        print(f"ERROR: SES file missing or empty: {SES_PATH}")
        return 1

    print(f"\n[IMPORT] SES Result")
    print(f"SES file: {SES_PATH} ({SES_PATH.stat().st_size} bytes)")

    try:
        import pcbnew

        board = pcbnew.LoadBoard(str(PCB_PATH))

        # Backup original
        backup = BUILD / "Code-SDR-V2-pre-import.kicad_pcb"
        import shutil

        shutil.copy2(PCB_PATH, backup)
        print(f"Backup created: {backup}")

        # Import SES
        if not pcbnew.ImportSpecctraSES(board, str(SES_PATH)):
            print("ERROR: Failed to import SES")
            return 1

        # Save
        if not pcbnew.SaveBoard(str(PCB_PATH), board):
            print("ERROR: Failed to save board")
            return 1

        # Report
        tracks = [t for t in board.Tracks() if not isinstance(t, pcbnew.PCB_VIA)]
        vias = [t for t in board.Tracks() if isinstance(t, pcbnew.PCB_VIA)]
        print(f"✓ Imported: {len(tracks)} tracks, {len(vias)} vias")
        return 0

    except ImportError:
        print("ERROR: pcbnew module not available")
        return 1


def add_rf_fences() -> int:
    """Add ground via fences around RF routes."""
    script = Path(__file__).parent / "add_rf_via_fences.py"
    if not script.exists():
        print(f"WARNING: RF fence script not found: {script}")
        return 0

    return run_cmd(
        [sys.executable, str(script)],
        "Add RF via fences",
    )


def validate_route() -> int:
    """Validate final routed design."""
    script = Path(__file__).parent / "validate_route.py"
    if not script.exists():
        print(f"WARNING: Validation script not found: {script}")
        return 0

    return run_cmd(
        [sys.executable, str(script)],
        "Validate final route",
    )


def create_fab_report() -> dict:
    """Create fabrication readiness report."""
    print("\n[FAB_REPORT] Fabrication Readiness")

    import hashlib
    import re

    with open(PCB_PATH) as f:
        content = f.read()

    metrics = {
        "sha256": hashlib.sha256(content.encode()).hexdigest(),
        "footprints": len(re.findall(r"\(footprint\s", content)),
        "nets": len(re.findall(r"\(net\s\d+", content)),
        "tracks": len(re.findall(r"\(segment\s", content)),
        "vias": len(re.findall(r"\(via\s", content)),
        "zones": len(re.findall(r"\(zone\s", content)),
    }

    issues = []

    if metrics["tracks"] == 0 and metrics["vias"] == 0:
        issues.append("Board is completely unrouted")

    if metrics["tracks"] < 100:
        issues.append(f"Very few tracks ({metrics['tracks']})")

    # Check DRC results if available
    drc_path = BUILD / "drc.json"
    if drc_path.exists():
        with open(drc_path) as f:
            drc = json.load(f)
        unconnected = len(drc.get("unconnected_items", []))
        errors = len(drc.get("errors", []))
        if unconnected > 0:
            issues.append(f"DRC: {unconnected} unconnected items")
        if errors > 0:
            issues.append(f"DRC: {errors} errors")

    report = {
        "metrics": metrics,
        "ready": len(issues) == 0,
        "issues": issues,
    }

    with open(BUILD / "fab-readiness.json", "w") as f:
        json.dump(report, f, indent=2)

    print(f"Tracks: {metrics['tracks']}")
    print(f"Vias: {metrics['vias']}")
    print(f"Status: {'READY' if report['ready'] else 'NOT READY'}")
    if issues:
        print("Issues:")
        for issue in issues:
            print(f"  - {issue}")

    return report


def main() -> int:
    """Run complete routing pipeline."""
    parser = argparse.ArgumentParser(
        description="Complete routing pipeline for Code-SDR V2"
    )
    parser.add_argument(
        "--stage",
        choices=[
            "validate",
            "baseline",
            "erc",
            "export",
            "import",
            "fences",
            "validate-route",
            "drc",
            "report",
            "all",
        ],
        default="all",
        help="Which stage to run",
    )
    parser.add_argument(
        "--skip-errors",
        action="store_true",
        help="Continue even if individual stages fail",
    )
    args = parser.parse_args()

    BUILD.mkdir(exist_ok=True)

    stages = {
        "validate": validate_pcb_structure,
        "baseline": lambda: create_baseline_metrics() and 0 or 1,
        "erc": run_erc,
        "export": export_dsn,
        "import": import_ses,
        "fences": add_rf_fences,
        "validate-route": validate_route,
        "drc": run_drc,
        "report": lambda: create_fab_report() and 0 or 1,
    }

    if args.stage == "all":
        stage_list = [
            "validate",
            "baseline",
            "erc",
            "export",
            "import",
            "fences",
            "validate-route",
            "drc",
            "report",
        ]
    else:
        stage_list = [args.stage]

    print(f"Code-SDR V2 Routing Pipeline")
    print(f"{'='*60}")

    failed = []
    for stage in stage_list:
        code = stages[stage]()
        if code != 0:
            failed.append(stage)
            if not args.skip_errors:
                print(f"\n✗ Stage '{stage}' failed")
                return code
            else:
                print(f"\n⚠ Stage '{stage}' failed (continuing)")

    print(f"\n{'='*60}")
    if failed:
        print(f"⚠ {len(failed)} stages failed: {', '.join(failed)}")
        return 1
    else:
        print("✓ All stages completed successfully")
        return 0


if __name__ == "__main__":
    sys.exit(main())
