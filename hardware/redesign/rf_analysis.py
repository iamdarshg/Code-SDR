"""Reproduce the V2 receiver gain, noise and anti-alias-filter budgets.

The component figures are conservative design values taken from the cited
manufacturer data sheets near the applicable operating bands.  They are not a
substitute for production S-parameter, tolerance and blocker simulations.
"""

from __future__ import annotations

import csv
import math
from dataclasses import dataclass
from pathlib import Path


OUT_DIR = Path(__file__).resolve().parent
NOISE_CSV = OUT_DIR / "NOISE_BUDGET.csv"
FILTER_CSV = OUT_DIR / "FILTER_RESPONSE.csv"
REPORT_MD = OUT_DIR / "RF_ANALYSIS.md"

BANDWIDTH_HZ = 40_000_000.0
THERMAL_NOISE_DBM_HZ = -174.0
REQUIRED_SNR_DB = 10.0
ADC_FULL_SCALE_DBM = 7.0  # 2 Vpp differential into 100 ohm
ADC_SNR_DB = 58.5  # conservative AD9215 planning value
DSA_RANGE_DB = 31.5


@dataclass(frozen=True)
class Stage:
    name: str
    component: str
    gain_db: float
    noise_figure_db: float
    source_note: str


COMMON_IF = (
    Stage(
        "IF balun",
        "TC1-1-13M+",
        -1.7,
        1.7,
        "manufacturer typical near 2.5 GHz plus rounding",
    ),
    Stage("IF selector", "BGS12WN6", -0.7, 0.7, "loss budget"),
    Stage("2436 MHz BAW", "QPQ1909", -1.2, 1.2, "worst useful-band loss budget"),
    Stage("IF gain 1", "GRF2013", 18.5, 1.3, "manufacturer typical"),
    Stage("digital attenuation, minimum", "PE4312C-Z", -1.7, 1.7, "loss budget"),
    Stage("IF gain 2", "GRF2013", 18.5, 1.3, "manufacturer typical"),
    Stage("second-mixer balun", "2450BL15B0100001E", -1.0, 1.0, "loss budget"),
    Stage("second mixer", "LT5560", 2.0, 11.0, "conservative at 2436 MHz"),
    Stage("46.5 MHz anti-alias LPF", "9-pole LC", -0.5, 0.5, "finite-Q allowance"),
    Stage("ADC driver", "AD8351", 21.0, 6.0, "R72=33 ohm gain setting"),
)


def low_path() -> tuple[Stage, ...]:
    return (
        Stage("protection and 2.5 GHz LPF", "PESD1V0R1BCSF + LFCN-2500+", -1.0, 1.0, "loss budget"),
        Stage("low-band LNA", "GRF4001", 15.4, 0.75, "manufacturer representative"),
        Stage("first mixer", "ADL5801", -6.1, 10.6, "manufacturer 2500 MHz worst-band typical"),
        Stage("mixer output balun", "2450BL15B0200001E", -1.0, 1.0, "loss budget"),
    ) + COMMON_IF[1:]


def high_path(
    lna_gain: float,
    lna_nf: float,
    label: str,
    second_lna: bool = False,
) -> tuple[Stage, ...]:
    front_end = (
        Stage("input protection/match", "port match", -0.7, 0.7, "loss budget"),
        Stage(f"{label} LNA", "GRF2101", lna_gain, lna_nf, "manufacturer custom-tune data"),
    )
    if second_lna:
        front_end += (
            Stage(
                f"{label} LNA 2",
                "GRF2101",
                lna_gain,
                lna_nf,
                "second manufacturer-tuned stage",
            ),
        )
    return front_end + (
        Stage("post-LNA RF selector", "QPC6144", -1.7, 1.7, "loss budget"),
        Stage("first mixer", "LTC5548", -7.5, 7.5, "conservative conversion-loss estimate"),
    ) + COMMON_IF


PATHS: dict[str, tuple[Stage, ...]] = {
    "LOW_0.1_2.5": low_path(),
    "HIGH_A_2.3_4.2": high_path(17.0, 1.0, "2.3-4.2 GHz"),
    "HIGH_B_4.2_6.8": high_path(14.9, 1.2, "4.2-6.8 GHz"),
    "HIGH_C_6.8_9.2": high_path(15.0, 1.34, "6.8-9.2 GHz"),
    "HIGH_D_8.0_10": high_path(12.3, 1.47, "8.0-10 GHz", second_lna=True),
}


def analyse_stages(stages: tuple[Stage, ...]) -> tuple[list[dict[str, object]], float, float]:
    rows: list[dict[str, object]] = []
    total_noise_factor = 0.0
    gain_before = 1.0
    cumulative_gain_db = 0.0
    for order, stage in enumerate(stages, start=1):
        stage_noise_factor = 10.0 ** (stage.noise_figure_db / 10.0)
        if order == 1:
            total_noise_factor = stage_noise_factor
        else:
            total_noise_factor += (stage_noise_factor - 1.0) / gain_before
        cumulative_gain_db += stage.gain_db
        gain_before *= 10.0 ** (stage.gain_db / 10.0)
        rows.append(
            {
                "order": order,
                "stage": stage.name,
                "component": stage.component,
                "gain_db": stage.gain_db,
                "noise_figure_db": stage.noise_figure_db,
                "cumulative_gain_db": cumulative_gain_db,
                "cumulative_nf_db": 10.0 * math.log10(total_noise_factor),
                "source_note": stage.source_note,
            }
        )
    return rows, cumulative_gain_db, 10.0 * math.log10(total_noise_factor)


def filter_insertion_db(freq_mhz: float) -> float:
    """Loss relative to DC for one 50-ohm half of the differential ladder."""
    series_l_h = (200e-9, 360e-9, 390e-9, 360e-9, 200e-9)
    shunt_c_f = (100e-12, 110e-12, 110e-12, 100e-12)
    omega = 2.0 * math.pi * freq_mhz * 1e6

    # ABCD matrix, alternating series L and shunt C.
    a, b, c, d = 1 + 0j, 0j, 0j, 1 + 0j
    for index, inductance in enumerate(series_l_h):
        z = 1j * omega * inductance
        a, b, c, d = a, a * z + b, c, c * z + d
        if index < len(shunt_c_f):
            y = 1j * omega * shunt_c_f[index]
            a, b, c, d = a + b * y, b, c + d * y, d

    source_r = 46.15
    load_r = 50.0
    denominator = a * load_r + b + source_r * (c * load_r + d)
    normalized_transfer = (source_r + load_r) / denominator
    return 20.0 * math.log10(abs(normalized_transfer))


def main() -> None:
    input_noise_dbm = THERMAL_NOISE_DBM_HZ + 10.0 * math.log10(BANDWIDTH_HZ)
    adc_input_noise_dbm = ADC_FULL_SCALE_DBM - ADC_SNR_DB
    summary_rows: list[dict[str, float | str]] = []
    noise_rows: list[dict[str, object]] = []

    for path, stages in PATHS.items():
        rows, gain_db, nf_db = analyse_stages(stages)
        for row in rows:
            noise_rows.append({"path": path, **row})
        output_noise_dbm = input_noise_dbm + nf_db + gain_db
        summary_rows.append(
            {
                "path": path,
                "gain_db": gain_db,
                "nf_db": nf_db,
                "sensitivity_dbm": input_noise_dbm + nf_db + REQUIRED_SNR_DB,
                "output_noise_dbm": output_noise_dbm,
                "adc_noise_margin_db": output_noise_dbm - adc_input_noise_dbm,
                "adc_fs_input_min_dsa_dbm": ADC_FULL_SCALE_DBM - gain_db,
                "adc_fs_input_max_dsa_dbm": ADC_FULL_SCALE_DBM - gain_db + DSA_RANGE_DB,
            }
        )

    with NOISE_CSV.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(noise_rows[0]))
        writer.writeheader()
        for row in noise_rows:
            writer.writerow(
                {
                    key: f"{value:.3f}" if isinstance(value, float) else value
                    for key, value in row.items()
                }
            )

    filter_frequencies = (0, 6.25, 26.25, 40, 46.25, 48, 50, 53.75, 60, 75, 83.5, 100)
    with FILTER_CSV.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(("frequency_mhz", "relative_insertion_db"))
        for frequency in filter_frequencies:
            writer.writerow((f"{frequency:.2f}", f"{filter_insertion_db(frequency):.3f}"))

    summary_lines = "\n".join(
        "| {path} | {gain_db:.1f} | {nf_db:.2f} | {sensitivity_dbm:.1f} | "
        "{output_noise_dbm:.1f} | {adc_noise_margin_db:.1f} | "
        "{adc_fs_input_min_dsa_dbm:.1f} to {adc_fs_input_max_dsa_dbm:.1f} |".format(**row)
        for row in summary_rows
    )
    filter_lines = "\n".join(
        f"| {frequency:.2f} | {filter_insertion_db(frequency):.2f} |"
        for frequency in filter_frequencies[1:]
    )
    report = f"""# V2 RF gain, noise and bandwidth analysis

This is the reproducible nominal design budget for the routed V2 hardware. It
uses a 40 MHz useful channel, 100 MSPS ADC, 2436.25 MHz first IF and 26.25 MHz
second IF. Thermal input noise is **{input_noise_dbm:.2f} dBm** in 40 MHz.

| Selected input path | Gain at 0 dB DSA (dB) | Cascaded NF (dB) | 10 dB-SNR sensitivity (dBm) | ADC input noise (dBm) | Analog/ADC noise margin (dB) | ADC full-scale input over DSA range (dBm) |
|---|---:|---:|---:|---:|---:|---:|
{summary_lines}

The second D-port LNA fixes the original top-band sensitivity shortfall. The
second GRF2013 after the PE4312 is also intentional: every path keeps more than
15 dB nominal analog-noise margin above the ADC's own integrated noise. The
PE4312 powers up at maximum attenuation and provides 31.5 dB of ADC-level
control. Front-end compression and blocker limits must still be verified on
first articles; the last column is an ADC-level calculation, not a guarantee
that every upstream stage remains linear at that input.

## Anti-alias filter

The differential filter is a symmetric 9-pole, approximately 46.5 MHz
low-pass ladder. Each half uses series inductors
200/360/390/360/200 nH and shunt capacitors 100/110/110/100 pF. The table is
the ideal response relative to DC with the implemented source/load model;
the RF budget separately reserves 0.5 dB for component Q and layout loss.

| Frequency (MHz) | Relative insertion (dB) |
|---:|---:|
{filter_lines}

The wanted 6.25-46.25 MHz band stays within 0.13 dB ideal ripple. The response
is about -20.6 dB at the first upper alias edge (53.75 MHz), -35.7 dB at
60 MHz and -87 dB at 100 MHz.

## Frequency-plan limitations and digital bandwidth

The five RF connectors are deliberately separate and the four high-band LNAs
are selected after amplification. That is the lowest-cost way to retain noise
figure and continuous 100 MHz-10 GHz tuning. The matched high-band LNAs
provide useful image discrimination, but they are not precision brick-wall
preselectors. For blocker-rich or calibrated image-free measurements, fit an
external filter ahead of the selected A/B/C/D connector. Adding four wideband
on-board filter banks would exceed the cost-first target and add insertion
loss ahead of the LNAs.

The ADC produces 1.0 Gbit/s of raw samples, which cannot be carried as payload
over GbE. FPGA firmware must digitally filter and decimate/resample before
Ethernet framing (for example 80 MSPS x 10 bit or 80 MSPS x 8 bit). This is a
firmware throughput requirement, not an analog-bandwidth reduction.

## Evidence

- [ADL5801 data sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ADL5801.pdf)
- [LTC5548 data sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/5548f.pdf)
- [LT5560 data sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/5560f.pdf)
- [AD8351 data sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/AD8351.pdf)
- [AD9215 data sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ad9215.pdf)
- [GRF4001 data sheet](https://www.guerrilla-rf.com/includes/prodFiles/4001/GRF4001DS.pdf)
- [GRF2101 data sheet](https://www.guerrilla-rf.com/includes/prodFiles/2101/GRF2101DS.pdf)
- [GRF2013 data sheet](https://www.guerrilla-rf.com/includes/prodFiles/2013/GRF2013DS.pdf)
- [QPQ1909 data sheet](https://www.mouser.com/datasheet/2/412/QPQ1909_Data_Sheet-1815271.pdf)
- [PE4312 data sheet](https://www.psemi.com/pdf/datasheets/pe4312ds.pdf)
"""
    REPORT_MD.write_text(report, encoding="utf-8")

    worst_margin = min(float(row["adc_noise_margin_db"]) for row in summary_rows)
    worst_sensitivity = max(float(row["sensitivity_dbm"]) for row in summary_rows)
    assert worst_margin >= 8.0
    assert worst_sensitivity <= -83.5
    assert filter_insertion_db(46.25) >= -0.2
    assert filter_insertion_db(53.75) <= -20.0
    print(
        f"RF budget valid: worst 10 dB-SNR sensitivity {worst_sensitivity:.1f} dBm, "
        f"worst ADC-noise margin {worst_margin:.1f} dB"
    )


if __name__ == "__main__":
    main()
