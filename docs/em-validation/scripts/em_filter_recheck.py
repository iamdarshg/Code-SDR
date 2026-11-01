"""Independent re-derivation of the Code-SDR V2 IF2 anti-alias ladder.

The repo's rf_analysis.py computes an ABCD half-circuit model of the
differential 9-pole ladder (L 200/360/390/360/200 nH series; C262/C264/
C266/C268 bridge caps 51/56/56/51 pF -> half-circuit shunt 102/112/112/102 pF).
This script re-derives the same nominal response independently from the
documented values and compares point-by-point against the committed
FILTER_RESPONSE.csv (finite-ports, ideal-coil case). Agreement within a small
numerical tolerance confirms transcription/arithmetic consistency; it is not
an RF measurement.

Documented in hardware/redesign/rf_analysis.py and RF_ANALYSIS.md.
"""

from __future__ import annotations

import csv
import json
import math
from pathlib import Path

import numpy as np

EV = Path(__file__).resolve().parents[1]
FO = EV / "scripts" / "redesign" / "FILTER_RESPONSE.csv"
OUT = EV / "outputs"

SERIES_L_H = (200e-9, 360e-9, 390e-9, 360e-9, 200e-9)
BRIDGE_C_F = (51e-12, 56e-12, 56e-12, 51e-12)
SHUNT_C_HALF_F = tuple(2.0 * c for c in BRIDGE_C_F)


def insertion_db(freq_mhz: float) -> float:
    """Independent ABCD cascade, ideal coils, finite ports (LT5560 1200||0.7p,
    R79=100, 2x 2.2uH chokes, R80=100, AD8351 5k||0.8p, 2x 100nF couplers).
    Same Norton-reference normalization as the repo: voltage dB relative to
    the ideal 100-ohm matched-through reference and the same source current.
    """
    w = 2.0 * math.pi * freq_mhz * 1e6
    a, b, c, d = 1 + 0j, 0j, 0j, 1 + 0j
    for i, lh in enumerate(SERIES_L_H):
        z = 1j * w * lh
        a, b, c, d = a, a * z + b, c, c * z + d
        if i < len(SHUNT_C_HALF_F):
            y = 1j * w * SHUNT_C_HALF_F[i]
            a, b, c, d = a + b * y, b, c + d * y, d
    # finite-port terminations (half-circuit)
    source_z = 0.5 / (1/100 + 1/1200 + 1j*w*0.7e-12 + 1/(1j*w*4.4e-6))
    driver_z = 1 / (1/5000 + 1j*w*0.8e-12)
    coupling_z = 2 / (1j*w*100e-9)
    load_z = 0.5 / (1/100 + 1/(driver_z + coupling_z))
    coupling = driver_z / (driver_z + coupling_z)
    denom = a * load_z + b + source_z * (c * load_z + d)
    transfer = (2 * source_z / 50) * coupling * load_z / denom
    return 20.0 * math.log10(abs(transfer))


def main() -> None:
    committed: list[dict] = []
    with FO.open(newline="", encoding="utf-8") as fh:
        for row in csv.DictReader(fh):
            committed.append(row)

    rows = []
    max_dev = 0.0
    for r in committed:
        f = float(r["frequency_mhz"])
        expected = float(r["finite_ports_ideal_coils_relative_db"])
        got = insertion_db(f)
        dev = abs(got - expected)
        max_dev = max(max_dev, dev)
        rows.append({"frequency_mhz": f, "committed_db": expected,
                     "independent_db": round(got, 6), "abs_dev_db": round(dev, 6)})
        print(f"  {f:8.3f} MHz  committed {expected:8.3f}  independent {got:8.3f}  dev {dev:.6f} dB")

    tol = 6e-4  # committed CSV is rounded to 0.001 dB; half-ULP = 5e-4 dB
    passed = max_dev <= tol
    result = {"all_within_tolerance_db": bool(passed), "max_abs_dev_db": max_dev,
              "tolerance_db": tol, "tolerance_note": "CSV values stored with 3 decimals (0.001 dB quantization); half-ULP = 0.0005 dB",
              "rows": rows,
              "note": "Independent ABCD re-derivation vs committed CSV. "
                      "Arithmetic-consistency check only; not an RF measurement."}
    (OUT / "filter_independent_recheck.json").write_text(json.dumps(result, indent=1), encoding="utf-8")
    print(f"\nmax |dev| = {max_dev:.3e} dB  ->  {'PASS' if passed else 'FAIL'} "
          f"within {tol} dB (= 0.001 dB CSV quantization bound, transcription consistency)")


if __name__ == "__main__":
    main()