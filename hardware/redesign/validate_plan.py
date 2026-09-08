"""Check tuning arithmetic for default and optional HF assembly populations.

Coverage is the union of RF frequencies, not full 40 MHz channels centered at
every endpoint. These checks do not establish conversion gain or image rejection.
"""

from __future__ import annotations

import csv
import math
from pathlib import Path


ROOT = Path(__file__).resolve().parent
PLAN_PATH = ROOT / "FREQUENCY_PLAN.csv"

TARGET_RF_MIN_MHZ = 10.0
DEFAULT_RF_MIN_MHZ = 100.0
TARGET_RF_MAX_MHZ = 10_000.0
FIRST_IF_MHZ = 2_426.25
SECOND_LO_MHZ = 2_400.0
SECOND_IF_MHZ = 26.25
USEFUL_BW_MHZ = 40.0
ADC_SAMPLE_RATE_MHZ = 100.0
LMX2592_MIN_MHZ = 20.0
LMX2592_MAX_MHZ = 9_800.0
ADF4360_1_MIN_MHZ = 2_050.0
ADF4360_1_MAX_MHZ = 2_450.0
ADF4360_REFERENCE_MHZ = 100.0
ADF4360_R_DIVIDER = 20
ADF4360_PRESCALER = 16
ADF4360_B_COUNTER = 30
ADF4360_A_COUNTER = 0
QPQ1909_MIN_MHZ = 2_402.5
QPQ1909_MAX_MHZ = 2_493.5
# Qorvo 030320 p2: attenuation is integrated over 5 MHz, not pointwise.
QPQ1909_LOW_STOPBAND_MAX_MHZ = 2_390.0
IMAGE_REJECTION_VERIFIED = False

MIXER_RF_LIMITS_MHZ = {
    "ADL5801": (10.0, 6_000.0),
    "LTC5548": (2_000.0, 14_000.0),
}
MIXER_LO_LIMITS_MHZ = {
    "ADL5801": (10.0, 6_000.0),
    "LTC5548": (1_000.0, 12_000.0),
}


def read_rows() -> list[dict[str, str]]:
    with PLAN_PATH.open(newline="", encoding="utf-8") as handle:
        return list(csv.DictReader(handle))


def value(row: dict[str, str], key: str) -> float:
    return float(row[key])


def assert_continuous_coverage(
    rows: list[dict[str, str]], rf_min_mhz: float = TARGET_RF_MIN_MHZ
) -> None:
    assert rows, "no frequency-plan rows"
    intervals = sorted((value(row, "rf_min_mhz"), value(row, "rf_max_mhz")) for row in rows)
    assert intervals[0][0] <= rf_min_mhz
    covered_to = intervals[0][1]
    for start, end in intervals[1:]:
        assert start <= covered_to, f"coverage gap from {covered_to} to {start} MHz"
        covered_to = max(covered_to, end)
    assert covered_to >= TARGET_RF_MAX_MHZ


def validate_populations(rows: list[dict[str, str]]) -> None:
    assert len({row["path"] for row in rows}) == len(rows), "duplicate path"
    assert all(row["population"] in {"default", "hf_bypass", "shared"} for row in rows)
    default = [r for r in rows if r["population"] in {"default", "shared"}]
    hf = [r for r in rows if r["population"] in {"hf_bypass", "shared"}]
    assert_continuous_coverage(default, DEFAULT_RF_MIN_MHZ)
    assert_continuous_coverage(hf, TARGET_RF_MIN_MHZ)
    low = [r for r in rows if r["path"] == "LOW"]
    optional = [r for r in rows if r["path"] == "LOW_HF_OPTION"]
    assert len(low) == len(optional) == 1
    assert low[0]["population"] == "default"
    assert value(low[0], "rf_min_mhz") >= DEFAULT_RF_MIN_MHZ, "GRF4001 HF coverage unproved"
    assert optional[0]["population"] == "hf_bypass"
    assert value(optional[0], "rf_min_mhz") == TARGET_RF_MIN_MHZ
    assert value(optional[0], "rf_max_mhz") == value(low[0], "rf_max_mhz")


def validate_frequency_rows(rows: list[dict[str, str]]) -> None:
    for row in rows:
        rf_min, rf_max = value(row, "rf_min_mhz"), value(row, "rf_max_mhz")
        lo_min, lo_max = value(row, "first_lo_min_mhz"), value(row, "first_lo_max_mhz")
        mixer = row["first_mixer"]
        mixer_rf_min, mixer_rf_max = MIXER_RF_LIMITS_MHZ[mixer]
        mixer_lo_min, mixer_lo_max = MIXER_LO_LIMITS_MHZ[mixer]

        assert math.isclose(value(row, "first_if_mhz"), FIRST_IF_MHZ)
        assert math.isclose(value(row, "second_lo_mhz"), SECOND_LO_MHZ)
        assert math.isclose(value(row, "second_if_mhz"), SECOND_IF_MHZ)
        assert math.isclose(value(row, "useful_bw_mhz"), USEFUL_BW_MHZ)
        assert mixer_rf_min <= rf_min <= rf_max <= mixer_rf_max
        assert mixer_lo_min <= lo_min <= lo_max <= mixer_lo_max
        assert LMX2592_MIN_MHZ <= lo_min <= lo_max <= LMX2592_MAX_MHZ

        if row["first_lo_mode"] == "high-side":
            assert math.isclose(lo_min, rf_min + FIRST_IF_MHZ)
            assert math.isclose(lo_max, rf_max + FIRST_IF_MHZ)
            assert row["image_expression"] == f"RF+{2 * FIRST_IF_MHZ:g}"
        elif row["first_lo_mode"] == "low-side":
            assert math.isclose(lo_min, rf_min - FIRST_IF_MHZ)
            assert math.isclose(lo_max, rf_max - FIRST_IF_MHZ)
            assert row["image_expression"] == f"RF-{2 * FIRST_IF_MHZ:g}"
        else:
            raise AssertionError(f"unknown LO mode {row['first_lo_mode']}")


def validate_if_adc_and_fixed_lo() -> None:
    half_bw = USEFUL_BW_MHZ / 2.0
    assert QPQ1909_MIN_MHZ <= FIRST_IF_MHZ - half_bw
    assert FIRST_IF_MHZ + half_bw <= QPQ1909_MAX_MHZ
    assert SECOND_IF_MHZ - half_bw > 0.0
    assert SECOND_IF_MHZ + half_bw < ADC_SAMPLE_RATE_MHZ / 2.0
    assert ADF4360_1_MIN_MHZ <= SECOND_LO_MHZ <= ADF4360_1_MAX_MHZ
    assert math.isclose(FIRST_IF_MHZ - SECOND_LO_MHZ, SECOND_IF_MHZ)
    image_min, image_max = second_mixer_image_band()
    assert image_min < image_max < QPQ1909_MIN_MHZ, "second image overlaps BAW passband"

    pfd_mhz = ADF4360_REFERENCE_MHZ / ADF4360_R_DIVIDER
    n_divider = ADF4360_PRESCALER * ADF4360_B_COUNTER + ADF4360_A_COUNTER
    assert math.isclose(pfd_mhz, 5.0)
    assert n_divider == 480
    assert 0 <= ADF4360_A_COUNTER < ADF4360_PRESCALER
    assert ADF4360_B_COUNTER >= ADF4360_A_COUNTER
    assert math.isclose(n_divider * pfd_mhz, SECOND_LO_MHZ)


def second_mixer_image_band() -> tuple[float, float]:
    half_bw = USEFUL_BW_MHZ / 2.0
    return (SECOND_LO_MHZ - SECOND_IF_MHZ - half_bw,
            SECOND_LO_MHZ - SECOND_IF_MHZ + half_bw)


def main() -> None:
    rows = read_rows()
    validate_populations(rows)
    validate_frequency_rows(rows)
    validate_if_adc_and_fixed_lo()
    print(
        "Frequency arithmetic passed: default 100 MHz-10 GHz; optional passive HF "
        "assembly 10 MHz-10 GHz; 40 MHz useful bandwidth, 100 MSPS, "
        "3.75 MHz Nyquist guard."
    )
    print("LO2=2400 MHz; IF1=2426.25 MHz; desired IF1=2406.25-2446.25 MHz; "
          "second image=2353.75-2393.75 MHz. No BAW passband overlap; "
          "2390-2393.75 MHz transition attenuation and system image rejection unproved.")


if __name__ == "__main__":
    main()
