"""Reproduce conditional V2 receiver budgets and the corrected IF2 ladder.

Gain/NF assumptions are not verified for the actual mixer/driver loading. The
finite-port, constant-Q calculations are sensitivity models, not measured parts.
"""

from __future__ import annotations

import csv
import math
from dataclasses import dataclass, replace
from pathlib import Path


OUT_DIR = Path(__file__).resolve().parent
NOISE_CSV = OUT_DIR / "NOISE_BUDGET.csv"
FILTER_CSV = OUT_DIR / "FILTER_RESPONSE.csv"
REPORT_MD = OUT_DIR / "RF_ANALYSIS.md"
SUMMARY_CSV = OUT_DIR / "RF_BUDGET_SCENARIOS.csv"

BANDWIDTH_HZ = 40_000_000.0
THERMAL_NOISE_DBM_HZ = -174.0
REQUIRED_SNR_DB = 10.0
ADC_FULL_SCALE_DBM = 7.0  # 2 Vpp differential into 100 ohm
ADC_SNR_DB = 58.5  # conservative AD9215 planning value
DSA_RANGE_DB = 31.5
SERIES_L_H = (200e-9, 360e-9, 390e-9, 360e-9, 200e-9)
# Actual C262/C264/C266/C268 bridge P to N; a half-circuit sees twice C.
CROSS_C_F = (51e-12, 56e-12, 56e-12, 51e-12)
SHUNT_C_HALF_F = tuple(2 * capacitance for capacitance in CROSS_C_F)
FILTER_FREQUENCIES_MHZ = (6.25, 26.25, 35, 40, 46.25, 48, 50, 53.75, 60, 75, 83.5, 100)
BUDGET_STATUS = "conditional_unverified_mixer_driver_gain_and_noise"


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
    Stage("2426 MHz BAW", "QPQ1909", -1.2, 1.2, "planning value; channel-integrated typical, not worst-band bound"),
    Stage("IF gain 1", "GRF2013", 18.5, 1.3, "manufacturer typical"),
    Stage("digital attenuation, minimum", "PE4312C-Z", -1.7, 1.7, "loss budget"),
    Stage("IF gain 2", "GRF2013", 18.5, 1.3, "manufacturer typical"),
    Stage("second-mixer balun", "2450BL15B0100001E", -1.0, 1.0, "loss budget"),
    Stage("second mixer", "LT5560", 2.0, 11.0, "UNVERIFIED +2 dB/11 dB NF under actual R79/R80 100-ohm loading; no manufacturer guarantee"),
    Stage("anti-alias LPF", "9-pole LC", 0.0, 0.0, "replaced by the selected finite-port/Q response scenario"),
    Stage("ADC driver", "AD8351", 21.0, 6.0, "UNVERIFIED power-gain/NF equivalents; R72=33 ohm voltage gain depends on actual ADC load"),
)


def low_path(hf_bypass: bool = False) -> tuple[Stage, ...]:
    front_end = (
        Stage("protection and 2.5 GHz LPF", "PESD1V0R1BCSF + LFCN-2500+", -1.0, 1.0, "loss budget"),
    )
    if hf_bypass:
        front_end += (Stage("passive HF bypass", "R153/R156; U50 isolated", -0.5, 0.5,
                            "unverified coupling/jumper loss allowance; separate HF calibration required"),)
    else:
        front_end += (Stage("low-band LNA", "GRF4001", 15.4, 0.75,
                            "manufacturer representative at RF >=100 MHz only"),)
    return front_end + (
        Stage("first mixer", "ADL5801", -6.1, 10.6, "UNVERIFIED high-IF upconversion placeholder; characterize actual RF frequency and output tune"),
        Stage("mixer output balun", "2450BL15B0200001E", -1.0, 1.0, "loss budget"),
    ) + COMMON_IF[1:]


def high_path(
    lna_gain: float,
    lna_nf: float,
    label: str,
    second_lna: bool = False,
    lna_component: str = "GRF2101",
) -> tuple[Stage, ...]:
    front_end = (
        Stage("input protection/match", "port match", -0.7, 0.7, "loss budget"),
        Stage(f"{label} LNA", lna_component, lna_gain, lna_nf, "planning value; exact tune and match require measurement"),
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
    "LOW_HF_OPTION_0.01_2.5": low_path(hf_bypass=True),
    "HIGH_A_2.3_4.2": high_path(15.4, 1.0, "2.3-4.2 GHz", lna_component="GRF4001"),
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


def filter_insertion_db(
    freq_mhz: float, inductor_q: float = math.inf, finite_ports: bool = False
) -> float:
    """Half-circuit voltage dB relative to ideal 100-ohm matched through.

    Constant Q uses coil ESR=wL/Q. Finite ports include LT5560 1200||0.7pF,
    R79=100 ohms, two 2.2uH supply chokes, R80=100 ohms, AD8351 5k||0.8pF
    and two 100nF couplers. The reference uses the same Norton source current;
    finite-port response includes source-shunt loss but excludes conversion gain.
    Capacitors, chokes and supply ground are ideal. No SRF/layout/tolerance model.
    """
    if not math.isfinite(freq_mhz) or freq_mhz < 0:
        raise ValueError("frequency must be finite and nonnegative")
    if math.isnan(inductor_q) or inductor_q <= 0:
        raise ValueError("inductor Q must be positive")
    if finite_ports and freq_mhz == 0:
        raise ValueError("finite-port AC model requires positive frequency")
    omega = 2.0 * math.pi * freq_mhz * 1e6

    # ABCD matrix, alternating series L and shunt C.
    a, b, c, d = 1 + 0j, 0j, 0j, 1 + 0j
    for index, inductance in enumerate(SERIES_L_H):
        z = omega * inductance / inductor_q + 1j * omega * inductance
        a, b, c, d = a, a * z + b, c, c * z + d
        if index < len(SHUNT_C_HALF_F):
            y = 1j * omega * SHUNT_C_HALF_F[index]
            a, b, c, d = a + b * y, b, c + d * y, d

    source_z: complex = 50 + 0j
    load_z: complex = 50 + 0j
    coupling: complex = 1 + 0j
    if finite_ports:
        # Differential port models divided by two for the half circuit.
        source_z = 0.5 / (1/100 + 1/1200 + 1j*omega*0.7e-12 + 1/(1j*omega*4.4e-6))
        driver_z = 1 / (1/5000 + 1j*omega*0.8e-12)
        coupling_z = 2 / (1j*omega*100e-9)
        load_z = 0.5 / (1/100 + 1/(driver_z + coupling_z))
        coupling = driver_z / (driver_z + coupling_z)
    denominator = a * load_z + b + source_z * (c * load_z + d)
    normalized_transfer = (2 * source_z / 50) * coupling * load_z / denominator
    return 20.0 * math.log10(abs(normalized_transfer))


def scenario_stages(stages: tuple[Stage, ...], frequency_mhz: float,
                    inductor_q: float) -> tuple[Stage, ...]:
    loss_db = -filter_insertion_db(frequency_mhz, inductor_q, finite_ports=True)
    return tuple(replace(stage, gain_db=-loss_db, noise_figure_db=loss_db,
                         source_note="conditional passive-loss/NF equivalent for finite-port/Q scenario; not a noise-wave model")
                 if stage.component == "9-pole LC" else stage for stage in stages)


def validate_filter_calculation() -> None:
    """Sanity gates on the calculation, never RF hardware acceptance."""
    wanted = [filter_insertion_db(6.25 + index * 0.004) for index in range(10001)]
    assert min(wanted) >= -0.30
    assert filter_insertion_db(53.75) < -22.0
    assert math.isclose(filter_insertion_db(0), 0.0, abs_tol=1e-12)
    for frequency in FILTER_FREQUENCIES_MHZ:
        ideal = filter_insertion_db(frequency, finite_ports=True)
        q50 = filter_insertion_db(frequency, 50, finite_ports=True)
        q30 = filter_insertion_db(frequency, 30, finite_ports=True)
        assert q30 < q50 < ideal
    # Independent full differential audit reference, rounded to 0.01 dB.
    assert abs(filter_insertion_db(46.25, 50, True) - (-2.63)) < 0.01


def write_dict_csv(path: Path, rows: list[dict[str, object]]) -> None:
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(rows[0]), lineterminator="\n")
        writer.writeheader()
        for row in rows:
            writer.writerow({key: f"{value:.3f}" if isinstance(value, float) else value
                             for key, value in row.items()})


def main() -> None:
    validate_filter_calculation()
    input_noise_dbm = THERMAL_NOISE_DBM_HZ + 10.0 * math.log10(BANDWIDTH_HZ)
    adc_input_noise_dbm = ADC_FULL_SCALE_DBM - ADC_SNR_DB
    summary_rows: list[dict[str, object]] = []
    noise_rows: list[dict[str, object]] = []

    for frequency in (26.25, 46.25):
        for q in (math.inf, 50.0, 30.0):
            for path, stages in PATHS.items():
                context = {"path": path, "if2_frequency_mhz": frequency,
                           "inductor_q": "ideal" if math.isinf(q) else f"{q:g}",
                           "budget_status": BUDGET_STATUS}
                rows, gain_db, nf_db = analyse_stages(scenario_stages(stages, frequency, q))
                noise_rows.extend({**context, **row} for row in rows)
                output_noise_dbm = input_noise_dbm + nf_db + gain_db
                summary_rows.append({
                    **context,
                    "gain_db": gain_db,
                    "nf_db": nf_db,
                    "sensitivity_dbm": input_noise_dbm + nf_db + REQUIRED_SNR_DB,
                    "output_noise_dbm": output_noise_dbm,
                    "adc_noise_margin_db": output_noise_dbm - adc_input_noise_dbm,
                    "adc_fs_input_min_dsa_dbm": ADC_FULL_SCALE_DBM - gain_db,
                    "adc_fs_input_max_dsa_dbm": ADC_FULL_SCALE_DBM - gain_db + DSA_RANGE_DB,
                })

    write_dict_csv(NOISE_CSV, noise_rows)
    write_dict_csv(SUMMARY_CSV, summary_rows)
    filter_rows = [
        {"frequency_mhz": frequency,
         "ideal_matched_relative_db": filter_insertion_db(frequency),
         "finite_ports_ideal_coils_relative_db": filter_insertion_db(frequency, finite_ports=True),
         "finite_ports_Q50_relative_db": filter_insertion_db(frequency, 50, True),
         "finite_ports_Q30_relative_db": filter_insertion_db(frequency, 30, True)}
        for frequency in FILTER_FREQUENCIES_MHZ
    ]
    write_dict_csv(FILTER_CSV, filter_rows)
    summary_lines = "\n".join(
        "| {path} | {gain_db:.1f} | {nf_db:.2f} | {sensitivity_dbm:.1f} | "
        "{adc_noise_margin_db:.1f} | "
        "{adc_fs_input_min_dsa_dbm:.1f} to {adc_fs_input_max_dsa_dbm:.1f} |".format(**row)
        for row in summary_rows
        if row["if2_frequency_mhz"] == 26.25 and row["inductor_q"] == "50"
    )
    filter_lines = "\n".join(
        "| {frequency_mhz:.2f} | {ideal_matched_relative_db:.2f} | "
        "{finite_ports_ideal_coils_relative_db:.2f} | "
        "{finite_ports_Q50_relative_db:.2f} | {finite_ports_Q30_relative_db:.2f} |".format(**row)
        for row in filter_rows
    )
    ideal_worst = min(filter_insertion_db(6.25 + index * 0.004) for index in range(10001))
    q50_edge = filter_insertion_db(46.25, 50, True)
    q30_edge = filter_insertion_db(46.25, 30, True)
    report = f"""# V2 RF calculation: conditional budgets and corrected IF2 response

Generated by python hardware/redesign/rf_analysis.py. These are analytical
scenarios; **conversion gain, noise figure, sensitivity and ADC margin remain
unverified for the actual mixer, terminations, driver and PCB**. Passing the
script verifies calculation consistency only.

The plan uses LO2=2400 MHz, IF1=2426.25 MHz, IF2=26.25 MHz, 40 MHz useful
bandwidth and a 100 MSPS ADC. Thermal noise is {input_noise_dbm:.2f} dBm in 40 MHz.

## Conditional gain and noise examples

The table uses IF2=26.25 MHz and constant coil Q=50. Its Friis calculation assumes
LT5560 gain=+2 dB/NF=11 dB and AD8351 gain=21 dB/NF=6 dB. Those are placeholders,
not manufacturer guarantees at the installed loading. The ADC full-scale
reference of +7 dBm is a 2 Vpp differential **100-ohm equivalent**; the ADC is
not a physical 100-ohm termination. Driver voltage gain and power gain must be
reconciled with actual impedances before these numbers become system budgets.

| Selected input path | Conditional gain (dB) | Conditional NF (dB) | 10 dB-SNR sensitivity (dBm) | Analog/ADC noise margin (dB) | ADC-equivalent full-scale input over DSA range (dBm) |
|---|---:|---:|---:|---:|---:|
{summary_lines}

RF_BUDGET_SCENARIOS.csv retains {len(summary_rows)} scenarios: six RF paths,
26.25/46.25 MHz IF2, and ideal/Q50/Q30 coils. NOISE_BUDGET.csv retains every
stage with the same conditional status. Each scenario applies its single
frequency gain/NF to a 40 MHz noise bandwidth as a sensitivity calculation; it
does not integrate measured noise or gain across the channel. The passive filter
NF equals its modeled loss only as a planning approximation; a full noise-wave
and impedance analysis remains necessary.

The high-A path names its actual GRF4001; the B/C/D paths use tuned GRF2101.
The passive HF row removes GRF4001 gain and adds an explicit unverified 0.5 dB
bypass/coupling allowance. It must have separate calibration. No sensitivity or
ADC-margin acceptance assertion is made from these assumptions. ADC full-scale
arithmetic does not prove upstream compression/blocker limits.

## Corrected differential anti-alias ladder

Each leg has series inductors 200/360/390/360/200 nH. Actual capacitors
C262/C264/C266/C268 bridge P to N with **51/56/56/51 pF**. In the implemented half
circuit they become **102/112/112/102 pF** to ground, while the source and load
are each 50 ohms. Equivalently, the full differential circuit has twice each
series inductance, the actual bridge capacitors and 100-ohm ports.

The ideal matched response has worst wanted-band attenuation {-ideal_worst:.3f} dB
over 6.25-46.25 MHz. The first upper alias edge is 100-46.25=53.75 MHz.
This calculation preserves the full 40 MHz band; its alias attenuation does not
establish adequate rejection for an unspecified blocker requirement.

| Frequency (MHz) | Ideal matched (dB) | Finite ports, ideal coils (dB) | Finite ports, Q50 (dB) | Finite ports, Q30 (dB) |
|---:|---:|---:|---:|---:|
{filter_lines}

All response columns are voltage dB relative to the same ideal 100-ohm
matched-through reference and the same Norton source current. The finite-port
model includes the LT5560 differential output 1200 ohms parallel 0.7 pF,
R79=100 ohms, both 2.2 uH supply chokes, R80=100 ohms, the AD8351 input
5000 ohms parallel 0.8 pF, and both 100 nF input couplers.
The LT5560 model is from [ADI p19, Figure 10](https://www.analog.com/media/en/technical-documentation/data-sheets/5560f.pdf);
the AD8351 differential input and load dependence are documented in
[ADI pp3, 12-16](https://www.analog.com/media/en/technical-documentation/data-sheets/AD8351.pdf).

Q30/Q50 use constant ESR=2*pi*f*L/Q for each coil. They assume ideal capacitors,
ideal bias chokes/supply ground, and omit PCB coupling, tolerances and inductor
self resonance. These are sensitivity cases, not selected part models.
At 46.25 MHz the finite-port response is {q50_edge:.2f} dB for Q50 and
{q30_edge:.2f} dB for Q30; the former blanket 0.5 dB allowance is unsupported.
Select actual coils by frequency-dependent Q/SRF data and measure the terminated
filter at both wanted and alias edges.

R79 and R80 also load the high-impedance mixer. At low frequency an ideal
1200-ohm Norton model gives 1200||100||100=48 ohms; the power delivered to
R80 versus an optimally matched 1200-ohm load is
10*log10((48**2/100)/(1200/4))=-11.15 dB. This is a loading comparison,
**not a conversion-gain correction**. The filter normalization does not include
that entire mismatch relative to the manufacturer gain test. LT5560 +2 dB must
therefore be measured with the actual terminations before any total gain/NF
claim is accepted.

## Frequency plan and assembly optionality

validate_plan.py checks each complete population separately: the default
GRF4001 assembly has a 100 MHz-10 GHz tuning union; the optional passive
HF assembly extends that union to 10 MHz-10 GHz. The
[GRF4001 manufacturer range](https://www.guerrilla-rf.com/products/detail/sku/GRF4001)
starts at 100 MHz. No HF amplifier gain is inferred below it.

| Low-path item | Default 100-2500 MHz assembly | Optional 10-2500 MHz passive assembly |
|---|---|---|
| R153/R156 | DNP | Fit both 0-ohm links |
| C208/C210 | Fit 100 pF | DNP both to isolate U50 |
| LNA_EN_LOW | Normal control | Hold low; U50 disabled |
| C211/C212 | Fit 1 nF | Retain 1 nF |
| FL50/U51/T50/common IF | Normal population and matching | Same population and matching |
| High A/B/C/D paths | Retained | Retained through 10 GHz |

This is an assembly population, not an electronic bypass switch. The HF signal
uses J50 -> FL50 -> R153 -> HF_BYPASS -> R156 -> C211 -> U51.
The [ADL5801](https://www.analog.com/media/en/technical-documentation/data-sheets/ADL5801.pdf)
RF range reaches 10 MHz and its high IF needs external matching.
[LFCN-2500+](https://www.minicircuits.com/WebStore/dashboard.html?model=LFCN-2500%2B)
covers DC-2500 MHz. These establish plausible frequency support, not measured
HF input match or conversion gain. Coverage means receivable RF frequencies:
a full 40 MHz slice containing 10 MHz can span 10-50 MHz (center 30 MHz);
the top endpoint uses 9960-10000 MHz (center 9980 MHz).

LO2=2400 MHz uses the 5 MHz PFD with P=16, B=30, A=0, N=480.
The wanted IF1 is **2406.25-2446.25 MHz** and the second-mixer image is
**2353.75-2393.75 MHz**. QPQ1909's documented passband union is
2402.5-2493.5 MHz, so there is no passband overlap. However **2390-2393.75 MHz
of the image is in an unproved transition region**. Qorvo specifies low-side
attenuation through 2390 MHz using 5 MHz integrated measurements; those are not
pointwise rejection guarantees. The 1.2 dB BAW loss in the budget is a planning
value, not a guaranteed worst-band loss. See
[Qorvo QPQ1909 datasheet 030320 p2](https://www.mouser.com/datasheet/2/412/QPQ1909_Data_Sheet-1815271.pdf).
Measured S-parameters and image-injection tests are required for a rejection
claim. First-mixer images likewise need actual preselector/blocker validation.

The ADC produces 1.0 Gbit/s before framing. FPGA filtering/resampling and a
verified Ethernet payload budget are needed for streaming the desired channel;
the analog bandwidth itself remains 40 MHz.

## Additional component references

- [LTC5548](https://www.analog.com/media/en/technical-documentation/data-sheets/5548f.pdf)
- [AD9215](https://www.analog.com/media/en/technical-documentation/data-sheets/ad9215.pdf)
- [GRF2101](https://www.guerrilla-rf.com/includes/prodFiles/2101/GRF2101DS.pdf)
- [GRF2013](https://www.guerrilla-rf.com/includes/prodFiles/2013/GRF2013DS.pdf)
- [PE4312](https://www.psemi.com/pdf/datasheets/pe4312ds.pdf)
"""
    REPORT_MD.write_text(report, encoding="utf-8", newline="\n")
    print(f"RF calculation checks passed; wrote {len(summary_rows)} conditional budget "
          f"scenarios, {len(noise_rows)} stage rows and {len(filter_rows)} filter samples.")
    print("Gain/NF, sensitivity, ADC margin and image rejection remain unverified "
          "under actual mixer/driver loading; Q30/Q50 are sensitivity models.")


if __name__ == "__main__":
    main()
