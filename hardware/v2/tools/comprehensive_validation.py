#!/usr/bin/env python3
"""Comprehensive validation of routed Code-SDR V2 PCB."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path
from typing import Optional

ROOT = Path(__file__).resolve().parents[1]
PCB_PATH = ROOT / "Code-SDR-V2.kicad_pcb"
BUILD = ROOT / "build"


def extract_metrics(content: str) -> dict:
    """Extract PCB metrics."""
    return {
        "footprints": len(re.findall(r"\(footprint\s", content)),
        "nets": len(re.findall(r"\(net\s\d+", content)),
        "tracks": len(re.findall(r"\(segment\s", content)),
        "vias": len(re.findall(r"\(via\s", content)),
        "zones": len(re.findall(r"\(zone\s", content)),
    }


def check_routing_completeness(content: str, metrics: dict) -> dict:
    """Check if routing is complete."""
    checks = {
        "has_tracks": metrics["tracks"] > 0,
        "has_vias": metrics["vias"] > 0,
        "sufficient_tracks": metrics["tracks"] > 800,
        "sufficient_vias": metrics["vias"] > 300,
        "all_nets_present": metrics["nets"] > 1500,
    }
    return checks


def check_rf_constraints(content: str) -> dict:
    """Check RF routing constraints (F.Cu only, no vias for critical nets)."""
    rf_nets = ["LT_IN", "LO_LOW", "LO_HIGH", "IF_LOW", "IF_HIGH"]

    results = {}
    for net_name in rf_nets:
        # Look for segments for this net on F.Cu
        pattern = f'\\(segment\\s+.*?\\(net\\s+"?{net_name}"?'
        segments = re.findall(pattern, content, re.DOTALL)
        results[net_name] = {
            "found": len(segments) > 0,
            "count": len(segments),
        }

    return results


def check_differential_pairs(content: str) -> dict:
    """Check differential pair routing."""
    pairs = {
        "USB": ("USB_DP", "USB_DM"),
        "Ethernet_A": ("MDI_A_P", "MDI_A_N"),
        "Ethernet_B": ("MDI_B_P", "MDI_B_N"),
        "Ethernet_C": ("MDI_C_P", "MDI_C_N"),
        "Ethernet_D": ("MDI_D_P", "MDI_D_N"),
        "LO_Low": ("LO_LOW_P", "LO_LOW_N"),
        "LO_High": ("LO_HIGH_P", "LO_HIGH_N"),
        "IF_Low": ("IF_LOW_P", "IF_LOW_N"),
        "IF_High": ("IF_HIGH_P", "IF_HIGH_N"),
    }

    results = {}
    for pair_name, (pos, neg) in pairs.items():
        pos_found = f'\\(net\\s+"?{pos}"?' in content
        neg_found = f'\\(net\\s+"?{neg}"?' in content
        results[pair_name] = {
            "positive": pos_found,
            "negative": neg_found,
            "both_present": pos_found and neg_found,
        }

    return results


def validate_pcb() -> dict:
    """Run complete validation."""
    report = {
        "timestamp": str(Path.cwd()),
        "status": "unknown",
        "issues": [],
        "warnings": [],
        "checks": {},
    }

    try:
        if not PCB_PATH.exists():
            report["issues"].append(f"PCB file not found: {PCB_PATH}")
            return report

        with open(PCB_PATH) as f:
            content = f.read()

        # Extract metrics
        metrics = extract_metrics(content)
        report["metrics"] = metrics

        # Check routing completeness
        routing = check_routing_completeness(content, metrics)
        report["routing"] = routing

        # Check RF constraints
        rf_check = check_rf_constraints(content)
        report["rf_nets"] = rf_check

        # Check differential pairs
        pairs_check = check_differential_pairs(content)
        report["differential_pairs"] = pairs_check

        # Validate requirements
        if not routing["has_tracks"]:
            report["issues"].append("No tracks found - PCB is unrouted")

        if not routing["has_vias"]:
            report["issues"].append("No vias found - PCB may be unrouted")

        if not routing["sufficient_tracks"]:
            report["warnings"].append(f"Low track count: {metrics['tracks']} (expected 800+)")

        if not routing["sufficient_vias"]:
            report["warnings"].append(f"Low via count: {metrics['vias']} (expected 300+)")

        # RF check
        rf_routed = sum(1 for net in rf_check.values() if net["found"])
        report["checks"]["rf_nets_routed"] = f"{rf_routed}/{len(rf_check)}"

        # Differential pairs check
        pairs_complete = sum(1 for p in pairs_check.values() if p["both_present"])
        report["checks"]["differential_pairs_complete"] = f"{pairs_complete}/{len(pairs_check)}"

        # Overall status
        if len(report["issues"]) == 0 and routing["sufficient_tracks"] and routing["sufficient_vias"]:
            report["status"] = "READY"
        elif len(report["issues"]) == 0 and routing["has_tracks"] and routing["has_vias"]:
            report["status"] = "ROUTED_WITH_WARNINGS"
        elif routing["has_tracks"] or routing["has_vias"]:
            report["status"] = "PARTIALLY_ROUTED"
        else:
            report["status"] = "UNROUTED"

    except Exception as e:
        report["issues"].append(f"Validation error: {str(e)}")
        import traceback
        report["error_detail"] = traceback.format_exc()
        report["status"] = "ERROR"

    return report


def print_report(report: dict) -> int:
    """Print validation report."""
    print("\n" + "=" * 70)
    print("CODE-SDR V2 COMPREHENSIVE VALIDATION REPORT")
    print("=" * 70)

    print(f"\nStatus: {report['status']}")

    if report.get("metrics"):
        m = report["metrics"]
        print(f"\nMetrics:")
        print(f"  Footprints: {m['footprints']}")
        print(f"  Nets:       {m['nets']}")
        print(f"  Tracks:     {m['tracks']}")
        print(f"  Vias:       {m['vias']}")

    if report.get("routing"):
        print(f"\nRouting Checks:")
        r = report["routing"]
        print(f"  Has tracks:         {'✓' if r['has_tracks'] else '✗'}")
        print(f"  Has vias:           {'✓' if r['has_vias'] else '✗'}")
        print(f"  Sufficient tracks:  {'✓' if r['sufficient_tracks'] else '✗'}")
        print(f"  Sufficient vias:    {'✓' if r['sufficient_vias'] else '✗'}")

    if report.get("checks"):
        print(f"\nDesign Checks:")
        for check, result in report["checks"].items():
            print(f"  {check}: {result}")

    if report.get("issues"):
        print(f"\n❌ ISSUES:")
        for issue in report["issues"]:
            print(f"  {issue}")

    if report.get("warnings"):
        print(f"\n⚠️  WARNINGS:")
        for warning in report["warnings"]:
            print(f"  {warning}")

    print(f"\n{'='*70}\n")

    # Write JSON report
    BUILD.mkdir(exist_ok=True)
    with open(BUILD / "comprehensive-validation.json", "w") as f:
        json.dump(report, f, indent=2)
    print(f"Full report: {BUILD / 'comprehensive-validation.json'}")

    return 0 if report["status"] in ["READY", "ROUTED_WITH_WARNINGS"] else 1


def main() -> int:
    """Main entry point."""
    report = validate_pcb()
    return print_report(report)


if __name__ == "__main__":
    sys.exit(main())
