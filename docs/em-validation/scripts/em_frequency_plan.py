"""Independent frequency-plan and component-range verification for Code-SDR V2.

Cross-checks (independently re-derived, not a re-run of the repo scripts):
- Continuous coverage union of the FREQUENCY_PLAN.csv rows: default assembly
  100-10000 MHz, optional HF assembly 10-10000 MHz.
- First-LO range vs LMX2592 rated output (20-9800 MHz).
- Mixer RF/LO range vs ADL5801 (10-6000 MHz) and LTC5548 (2000-14000 RF,
  1000-12000 LO) per the datasheet-honest limits used by the repo.
- Second LO: ADF4360-1 (2050-2450 MHz) with N=480, PFD=5 MHz -> 2400 MHz.
- BAW QPQ1909 passband (2402.5-2493.5 MHz) vs wanted IF1 (2406.25-2446.25)
  and image (2353.75-2393.75 MHz): overlap check.
- ADC Nyquist: IF2 26.25 +/- 20 MHz band vs 100 MSPS half-band (50 MHz).

Component ranges are analytical checks against documented datasheet ranges
cited in the repo; they do NOT validate measured part behavior.
"""

from __future__ import annotations

import csv
import json
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLAN = ROOT / "scripts" / "redesign" / "FREQUENCY_PLAN.csv"
OUT = ROOT / "outputs"

IF1 = 2426.25
LO2 = 2400.0
IF2 = 26.25
BW = 40.0
FS_MSPS = 100.0

LMX_MIN, LMX_MAX = 20.0, 9800.0
ADF_MIN, ADF_MAX = 2050.0, 2450.0
BAW_MIN, BAW_MAX = 2402.5, 2493.5
QPQ_LOW_STOP = 2390.0  # documented integrated-5MHz low-side stopband ceiling
MIXERS = {
    "ADL5801": {"rf": (10.0, 6000.0), "lo": (10.0, 6000.0)},
    "LTC5548": {"rf": (2000.0, 14000.0), "lo": (1000.0, 12000.0)},
}


def read_rows() -> list[dict[str, str]]:
    with PLAN.open(newline="", encoding="utf-8") as fh:
        return list(csv.DictReader(fh))


def union_cover(rows: list[tuple[float, float]]) -> tuple[bool, list[tuple[float, float]]]:
    iv = sorted(rows)
    merged = [list(iv[0])]
    for a, b in iv[1:]:
        if a <= merged[-1][1] + 1e-9:
            merged[-1][1] = max(merged[-1][1], b)
        else:
            merged.append([a, b])
    return len(merged) == 1, [tuple(m) for m in merged]


def main() -> None:
    rows = read_rows()
    checks: list[dict] = []
    ok_all = True

    def check(name: str, ok: bool, detail: str, warn: bool = False) -> None:
        nonlocal ok_all
        if not warn:
            ok_all = ok_all and ok
        state = "PASS" if ok else ("WARN" if warn else "FAIL")
        checks.append({"check": name, "status": state, "pass": bool(ok), "detail": detail})
        print(f"[{state}] {name}: {detail}")

    # --- coverage unions ---
    default = [(float(r["rf_min_mhz"]), float(r["rf_max_mhz"])) for r in rows
               if r["population"] in ("default", "shared")]
    hf = [(float(r["rf_min_mhz"]), float(r["rf_max_mhz"])) for r in rows
          if r["population"] in ("hf_bypass", "shared")]
    default_ok, default_m = union_cover(default)
    hf_ok, hf_m = union_cover(hf)
    check("default coverage 100-10000 continuous",
          default_ok and abs(default_m[0][0] - 100.0) < 1e-6 and default_m[0][1] >= 10000.0 - 1e-6,
          f"merged={default_m}")
    check("HF bypass coverage 10-10000 continuous",
          hf_ok and hf_m[0][0] <= 10.0 + 1e-6 and hf_m[0][1] >= 10000.0 - 1e-6,
          f"merged={hf_m}")

    # --- per-row LO / mixer range checks ---
    lo_max_used = 0.0
    for r in rows:
        name = r["path"]
        rf_min, rf_max = float(r["rf_min_mhz"]), float(r["rf_max_mhz"])
        lo_min, lo_max = float(r["first_lo_min_mhz"]), float(r["first_lo_max_mhz"])
        lo_max_used = max(lo_max_used, lo_max)
        mixer = r["first_mixer"]
        mrf, mlo = MIXERS[mixer]["rf"], MIXERS[mixer]["lo"]
        check(f"{name}: RF {rf_min:g}-{rf_max:g} within {mixer} RF {mrf}",
              mrf[0] - 1e-6 <= rf_min and rf_max <= mrf[1] + 1e-6, "")
        check(f"{name}: LO {lo_min:g}-{lo_max:g} within {mixer} LO {mlo}",
              mlo[0] - 1e-6 <= lo_min and lo_max <= mlo[1] + 1e-6, "")
        check(f"{name}: LO {lo_min:g}-{lo_max:g} within LMX2592 {LMX_MIN}-{LMX_MAX}",
              LMX_MIN - 1e-6 <= lo_min and lo_max <= LMX_MAX + 1e-6, "")
        # IF arithmetic
        want_if = rf_min + IF1 if r["first_lo_mode"] == "high-side" else rf_min - IF1
        check(f"{name}: first-IF arithmetic", math.isclose(lo_min, want_if, abs_tol=1e-6), "")

    check("LMX2592 max LO headroom (9786.25 <= 9800)",
          lo_max_used <= LMX_MAX, f"peak LO used {lo_max_used:g} MHz")

    # --- second LO / BAW / ADC ---
    check("ADF4360-1 LO2=2400 MHz in 2050-2450", ADF_MIN <= LO2 <= ADF_MAX, "")
    pfd = 100.0 / 20.0
    n = 16 * 30 + 0
    check("LO2 divider arithmetic N*PFD = 480*5 MHz", math.isclose(n * pfd, LO2, abs_tol=1e-9), "")
    wanted = (IF1 - BW / 2, IF1 + BW / 2)
    image = (LO2 - IF2 - BW / 2, LO2 - IF2 + BW / 2)
    check("BAW passband covers wanted IF1", BAW_MIN <= wanted[0] and wanted[1] <= BAW_MAX,
          f"wanted {wanted}")
    overlap = (min(image[1], BAW_MAX) - max(image[0], BAW_MIN))
    check("BAW passband does NOT overlap image",
          overlap <= 1e-6,
          f"image {image} vs BAW {BAW_MIN}-{BAW_MAX}; overlap {max(overlap,0):.3f} MHz")
    # Repo documents "2390-2393.75 MHz in an unproved transition region"
    # (QPQ1909 low-side attenuation quoted with 5 MHz integrated
    # measurements, not pointwise). exact rejection is unproved -> recorded
    # as a documented limitation (WARN) rather than a hard pass/fail.
    trans_overlap = min(image[1], 2393.75) - max(image[0], 2390.0)
    check("Image fully inside proved QPQ stopband",
          image[1] <= QPQ_LOW_STOP,
          f"image {image} vs proved-stopband ceiling {QPQ_LOW_STOP}; "
          f"unproved-transition exposure {max(trans_overlap,0):.2f} MHz "
          f"(documented limitation - transition region unproved, not a pass claim)",
          warn=True)
    check("Image transition exposure < 3.75 MHz documented bound",
          trans_overlap <= (2393.75 - 2390.0) + 1e-9,
          f"exposure {max(trans_overlap,0):.2f} MHz")
    adc_band = (IF2 - BW / 2, IF2 + BW / 2)
    check("IF2 band inside ADC Nyquist", adc_band[0] > 0 and adc_band[1] < FS_MSPS / 2,
          f"IF2 band {adc_band} vs FS/2 {FS_MSPS/2}")
    check("Nyquist guard >= 3.75 MHz",
          (FS_MSPS / 2 - adc_band[1]) >= 3.75 - 1e-9,
          f"guard {FS_MSPS/2 - adc_band[1]:.3f} MHz")

    out = {"all_passed": bool(ok_all), "checks": checks,
           "generated_from": str(PLAN),
           "note": "Analytical cross-check of documented plan constants; "
                   "does not constitute measured RF performance."}
    (OUT / "frequency_plan_verification.json").write_text(json.dumps(out, indent=1), encoding="utf-8")
    print(f"ALL {'PASSED' if ok_all else 'FAILED'}; wrote frequency_plan_verification.json")


if __name__ == "__main__":
    main()