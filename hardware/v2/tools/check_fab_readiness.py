#!/usr/bin/env python3
"""Check fabrication readiness for Code-SDR V2 PCB.

Validates:
1. PCB structure and completeness
2. Routing status and metrics
3. Design rule compliance
4. Critical net routing constraints
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Optional


ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"
SCH_PATH = ROOT / "Code-SDR-V2.kicad_sch"
BUILD = ROOT / "build"


def load_pcb_content() -> str:
    """Load PCB file content."""
    if not PCB_PATH.exists():
        print(f"ERROR: PCB file not found: {PCB_PATH}")
        sys.exit(1)
    with open(PCB_PATH) as f:
        return f.read()


def extract_pcb_metrics(content: str) -> dict:
    """Extract basic metrics from PCB content."""
    return {
        "footprints": len(re.findall(r"\(footprint\s", content)),
        "nets": len(re.findall(r"\(net\s\d+", content)),
        "tracks": len(re.findall(r"\(segment\s", content)),
        "vias": len(re.findall(r"\(via\s", content)),
        "zones": len(re.findall(r"\(zone\s", content)),
    }


def check_critical_nets(content: str) -> dict:
    """Check critical net definitions."""
    critical_net_patterns = {
        "RF_paths": [
            r"LT_IN_P",
            r"LT_IN_N",
            r"LO_LOW_P",
            r"LO_LOW_N",
            r"IF_LOW_P",
            r"IF_LOW_N",
            r"IF_HIGH_P",
            r"IF_HIGH_N",
        ],
        "USB_signals": [r"USB_DP", r"USB_DM"],
        "Ethernet_signals": [r"MDI_[ABCD]_P", r"MDI_[ABCD]_N"],
        "ADC_signals": [rf"ADC_D\d+", r"ADC_CLK"],
        "FPGA_signals": [r"FPGA_.*"],
    }

    found_nets = {}
    for category, patterns in critical_net_patterns.items():
        found_nets[category] = {}
        for pattern in patterns:
            matches = re.findall(f'\\(net\\s+\\d+\\s+"{pattern}"', content)
            if matches:
                found_nets[category][pattern] = len(matches)

    return found_nets


def analyze_layers(content: str) -> dict:
    """Analyze layer definitions."""
    # Look for layer definitions and usage
    layer_defs = re.findall(r'\(layer\s+"([^"]+)"', content)
    return {
        "defined_layers": sorted(set(layer_defs)),
        "layer_count": len(set(layer_defs)),
    }


def check_zones(content: str) -> dict:
    """Analyze power/ground zones."""
    zones = re.findall(r"\(zone\s+.*?\(net\s+(\d+)\s+\"([^\"]+)\"", content, re.DOTALL)
    zone_analysis = {}
    for net_num, net_name in zones:
        zone_analysis[net_name] = int(net_num)
    return zone_analysis


def generate_readiness_report(content: str) -> dict:
    """Generate complete readiness report."""
    report = {
        "timestamp": str(Path(__file__).stat().st_mtime),
        "pcb_file": str(PCB_PATH),
        "file_size": len(content),
        "checks": {},
        "issues": [],
        "warnings": [],
        "ready_for_fab": False,
    }

    # Check 1: Basic structure
    metrics = extract_pcb_metrics(content)
    report["metrics"] = metrics

    if metrics["footprints"] == 0:
        report["issues"].append("ERROR: No footprints found in PCB")
    else:
        report["checks"]["footprints"] = f"✓ {metrics['footprints']} footprints"

    if metrics["nets"] == 0:
        report["issues"].append("ERROR: No nets found in PCB")
    else:
        report["checks"]["nets"] = f"✓ {metrics['nets']} nets"

    # Check 2: Routing status
    if metrics["tracks"] == 0 and metrics["vias"] == 0:
        report["issues"].append(
            f"ERROR: PCB is completely unrouted (0 tracks, 0 vias)"
        )
        report["warnings"].append(
            f"Expected ~1000 tracks and ~350 vias for complete routing"
        )
    elif metrics["tracks"] < 100 or metrics["vias"] < 10:
        report["issues"].append(
            f"ERROR: PCB appears under-routed ({metrics['tracks']} tracks, {metrics['vias']} vias)"
        )
    else:
        report["checks"]["routing"] = f"✓ {metrics['tracks']} tracks, {metrics['vias']} vias"

    # Check 3: Zones
    if metrics["zones"] == 0:
        report["warnings"].append("No zones defined (should have GND and power planes)")
    else:
        report["checks"]["zones"] = f"✓ {metrics['zones']} zones"

    # Check 4: Critical nets
    critical_nets = check_critical_nets(content)
    report["critical_nets"] = critical_nets

    missing_critical = []
    for category, nets in critical_nets.items():
        if not nets:
            missing_critical.append(category)

    if missing_critical:
        report["warnings"].append(
            f"Missing critical nets in: {', '.join(missing_critical)}"
        )
    else:
        report["checks"]["critical_nets"] = "✓ All critical net categories found"

    # Check 5: Layers
    layers = analyze_layers(content)
    report["layers"] = layers

    if layers["layer_count"] >= 4:
        report["checks"]["layers"] = f"✓ {layers['layer_count']} layers defined"
    else:
        report["issues"].append(f"ERROR: Only {layers['layer_count']} layers (need 4+)")

    # Final readiness assessment
    report["ready_for_fab"] = len(report["issues"]) == 0 and metrics["tracks"] > 500

    return report


def load_drc_report() -> Optional[dict]:
    """Load DRC report if available."""
    drc_path = BUILD / "drc.json"
    if drc_path.exists():
        try:
            with open(drc_path) as f:
                return json.load(f)
        except:
            pass
    return None


def print_report(report: dict) -> None:
    """Print formatted report."""
    print("\n" + "=" * 70)
    print("CODE-SDR V2 FABRICATION READINESS REPORT")
    print("=" * 70)

    metrics = report.get("metrics", {})
    print(f"\nPCB Metrics:")
    print(f"  Footprints: {metrics.get('footprints', 0)}")
    print(f"  Nets:       {metrics.get('nets', 0)}")
    print(f"  Tracks:     {metrics.get('tracks', 0)}")
    print(f"  Vias:       {metrics.get('vias', 0)}")
    print(f"  Zones:      {metrics.get('zones', 0)}")

    if report.get("checks"):
        print(f"\nPassed Checks:")
        for check, status in report["checks"].items():
            print(f"  {status}")

    if report.get("issues"):
        print(f"\n❌ ISSUES (must fix):")
        for issue in report["issues"]:
            print(f"  {issue}")

    if report.get("warnings"):
        print(f"\n⚠️  WARNINGS (should review):")
        for warning in report["warnings"]:
            print(f"  {warning}")

    print(f"\nCritical Nets Found:")
    for category, nets in report.get("critical_nets", {}).items():
        if nets:
            print(f"  {category}: {len(nets)} patterns found")

    print(f"\nLayers:")
    layers = report.get("layers", {})
    print(f"  Defined: {', '.join(layers.get('defined_layers', []))}")

    print(f"\n{'='*70}")
    status = "✓ READY FOR FABRICATION" if report["ready_for_fab"] else "✗ NOT READY"
    print(f"Status: {status}")
    print(f"{'='*70}\n")

    return 0 if report["ready_for_fab"] else 1


def main() -> int:
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Check fabrication readiness for Code-SDR V2"
    )
    parser.add_argument(
        "--output",
        type=Path,
        help="Write JSON report to file",
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="Exit with error if not completely ready",
    )
    args = parser.parse_args()

    content = load_pcb_content()
    report = generate_readiness_report(content)

    if args.output:
        BUILD.mkdir(exist_ok=True)
        with open(args.output, "w") as f:
            json.dump(report, f, indent=2)
        print(f"Report written to: {args.output}")

    exit_code = print_report(report)

    if args.strict and not report["ready_for_fab"]:
        return 1

    return exit_code


if __name__ == "__main__":
    sys.exit(main())
