"""Plots for the EM validation: frequency-plan coverage, filter response,
microstrip sweep, return-path map. SYSTEM python (has matplotlib)."""

from __future__ import annotations

import csv
import json
import math
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

EV = Path(__file__).resolve().parents[1]
PLOT = EV / "plots"
PLAN = EV / "scripts" / "redesign" / "FREQUENCY_PLAN.csv"
FILTER = EV / "scripts" / "redesign" / "FILTER_RESPONSE.csv"


def main() -> None:
    PLOT.mkdir(parents=True, exist_ok=True)

    # 1. Frequency-plan coverage map
    rows = list(csv.DictReader(PLAN.open(newline="", encoding="utf-8")))
    fig, ax = plt.subplots(figsize=(11, 4.6))
    pops = {"default": "default", "hf_bypass": "HF bypass", "shared": "shared"}
    for i, r in enumerate(rows):
        c = {"default": "#1f77b4", "hf_bypass": "#d62728", "shared": "#2ca02c"}[r["population"]]
        ax.barh(i, float(r["rf_max_mhz"]) - float(r["rf_min_mhz"]),
                left=float(r["rf_min_mhz"]), height=0.6, color=c, alpha=0.85)
        ax.text(float(r["rf_max_mhz"]) + 60, i, r["path"], va="center", fontsize=8)
    ax.set_yticks(range(len(rows)))
    ax.set_yticklabels([r["path"] for r in rows])
    ax.set_xlabel("RF frequency (MHz)")
    ax.set_title("Code-SDR V2 frequency plan coverage (analytical union, log10 axis)")
    ax.set_xscale("log")
    ax.set_xlim(5, 20000)
    ax.axvline(100, color="gray", ls=":", lw=1)
    ax.axvline(10000, color="gray", ls=":", lw=1)
    ax.grid(axis="x", ls="--", alpha=0.4)
    fig.tight_layout()
    fig.savefig(PLOT / "frequency_plan_coverage.png", dpi=110)
    plt.close(fig)

    # 2. Filter response (committed finite-ports curves)
    fr = list(csv.DictReader(FILTER.open(newline="", encoding="utf-8")))
    fig, ax = plt.subplots(figsize=(9, 5))
    for col, lab in (("ideal_matched_relative_db", "ideal matched"),
                     ("finite_ports_ideal_coils_relative_db", "finite ports, ideal coils"),
                     ("finite_ports_Q50_relative_db", "finite ports, Q50"),
                     ("finite_ports_Q30_relative_db", "finite ports, Q30")):
        ax.plot([float(r["frequency_mhz"]) for r in fr],
                [float(r[col]) for r in fr], marker="o", ms=4, label=lab)
    ax.axvspan(6.25, 46.25, color="green", alpha=0.08, label="wanted band 6.25-46.25")
    ax.axvline(53.75, color="orange", ls="--", lw=1, label="alias edge 53.75")
    ax.set(xlabel="Frequency (MHz)", ylabel="Relative response (dB)",
           title="Code-SDR V2 IF2 anti-alias ladder (analytical ABCD model)\n"
                 "NOT a measurement; committed FILTER_RESPONSE.csv")
    ax.legend(fontsize=8)
    ax.grid(ls="--", alpha=0.4)
    fig.tight_layout()
    fig.savefig(PLOT / "if2_filter_response.png", dpi=110)
    plt.close(fig)

    print("plots:", [p.name for p in sorted(PLOT.glob("*.png"))])


if __name__ == "__main__":
    main()