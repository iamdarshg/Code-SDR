# Code-SDR — V2 wideband SDR

A single-board, 100 MHz – 10 GHz receiver with an FPGA gateware datapath, an
RP2040 controller, and host-side tooling.

**Current hardware is V2:** the modular receiver/control board in
[`hardware/v2`](hardware/v2). The first-generation `Wideband-Test-V1` design was
audited and found **unsafe to fabricate**; its RTL, firmware, software and design
documents have been removed from this tree. What remains of V1 is the audit
record and clearly-labelled history — see
[V1 / legacy status](#v1--legacy-status).

> **Status, stated plainly.** No board has been fabricated and nothing in this
> repository has touched silicon. All gateware claims below are simulation and
> synthesis estimates. Frame counts, pin constraints and vendor I/O primitives
> still need Lattice Diamond and a first article. See
> [`GATEWARE.md`](GATEWARE.md), [`RESOURCE_REPORT.md`](RESOURCE_REPORT.md) and
> [`BRINGUP.md`](BRINGUP.md).

---

## Hardware at a glance

Source of truth is copper:
[`hardware/v2/Code-SDR-V2.kicad_pcb`](hardware/v2/Code-SDR-V2.kicad_pcb), parsed
in [`HARDWARE.md`](HARDWARE.md). 99 × 99 mm, four copper layers, 373 placed
footprints.

| Block | Part | Role |
|---|---|---|
| Controller | RP2040 (U10) + W25Q16JV QSPI flash | Sole controller: SPI master, FPGA config, USB CDC telemetry |
| FPGA | LIF-MD6000-6JMG80I (U20) | 5,936 LUT4, 180 kbit EBR, no DSP blocks |
| ADC | AD9215BCPZ-105 (U21) | 10-bit parallel, clocked from the fixed 100 MHz board reference — i.e. **100 MSPS**. `-105` is the part rating, not the operating rate |
| Ethernet | KSZ9031RNXCA (U30), RJ45 with magnetics | **RGMII only** — there is no GMII on this PCB |
| Reference | SiT8209 100 MHz (Y40) → CDCLVC1104 fanout | Shared ADC + synthesiser reference; PHY keeps its own 25 MHz crystal |
| Synthesisers | LMX2592 (U40), ADF4360-1 (U41) | First and second LO |
| RF front end | 5 SMA lanes, GRF LNAs, ADL5801 / LTC5548 / LT5560 mixers | 100 MHz – 10 GHz, 40 MHz real IBW, 2436.25 MHz IF1 → 26.25 MHz ADC IF |
| Power | 5 V in only (J1), TLV62569 bucks + TPS7A8300 LNA | ≈ 6.6 W estimated, ≈ 5 W of it the RF rail — see [`POWER.md`](POWER.md) |

USB on this board is **data/control only**; there is no USB-powered or USB-bulk
SDR streaming path.

## Gateware at a glance

Two **compile-time exclusive** modes (`v2_top` builds one or the other; the
RP2040 holds both bitstreams and swaps them over SPI in ~0.1–0.2 s):

```
MODE = 0 (raw)   ADC 100 MSPS -> 8/10-bit pack -> CDC FIFO -> UDP/IP -> MAC -> RGMII
MODE = 1 (fft)   ADC 100 MSPS -> CIC /R -> CDC FIFO -> window -> N-pt FFT
                             -> bin packetizer -> UDP/IP -> MAC -> RGMII
```

| | Raw (MODE=0) | FFT (MODE=1) |
|---|---|---|
| Area | **2,400 / 5,936 LUT4** (Yosys + ABC) | not yet measurable; bounded analytically |
| Verified throughput | 801 Mbps wire at 8-bit / 100 MSPS, no drops | 7.8 MSPS sustained input at N=1024 |
| Host does | FFT, demod, recording | display only |

10-bit at 100 MSPS offers 1002 Mbps, which no gigabit link carries, so raw mode
keeps the full rate and drops ~0.9 % of **whole datagrams** via a delta-sigma
accumulator (exact `floor(K·frac/65536)` drop count, verified to the packet). Loss
is counted and reported in-band, never hidden. Full detail, including the CIC /
FFT rate table and the three real FFT bugs found: [`GATEWARE.md`](GATEWARE.md).

## Repository map

| Path | What it is |
|---|---|
| `hardware/v2/` | **V2 board source of truth** — KiCad 9 project, 8 schematic sheets, footprints, deterministic generators in `tools/` |
| `hardware/redesign/` | Frequency plan, noise/filter budgets, power tree, V1↔V2 BOM cost model (reproducible Python) |
| `hardware/HARDWARE_VALIDATION.md` | Why V1 is DO NOT FABRICATE; the implemented V2 architecture |
| `hardware/validate_hardware.ps1` | Runs the ERC/DRC audit against the V2 project |
| `hardware/KSZ9031RNXCC-TR/`, `hardware/MABA-011108/` | Vendored KiCad vendor libraries (retained, see cleanup report) |
| `verilog/` | V2 gateware `v2_*.v`, testbenches, `v2_top.lpf`, `sim/crosslink_prims_sim.v` |
| `firmware/rp2040_dashboard/` | The RP2040 sketch: SPI telemetry, fault engine, bitstream swap |
| `software/` | Host tools: `sdr_spectrum.py`, `sdr_dashboard.py`, shared `sdr_common.py` |
| `tools/` | `run_tests.py`, `lint_verilog.py`, `check_firmware.py`, `gen_lpf.py`, `gen_twiddles.py`, `r2sdf_reference.py` |
| `tests/` | One retained cocotb test (`async_fifo`); the real regression is `tools/run_tests.py` |
| `docs/em-validation/` | EM/RF validation proof package (analytical, fail-closed) |
| `docs/superpowers/` | Routing plans and specs behind the V2 copper |
| `reviews/`, `outputs/code-sdr-routing/` | V2 routing review records and routing deliverables |
| `openfpga_framework/` | Vendored upstream OpenFPGA framework — retained, not used by the V2 build (see cleanup report) |
| `HARDWARE.md` / `GATEWARE.md` / `RESOURCE_REPORT.md` / `POWER.md` / `BRINGUP.md` / `KNOWN_ISSUES.md` | PCB-parse, gateware, area, power, bring-up and defect register |
| `docs/CLEANUP_2026-11-04.md` | The V1 removal / README cleanup record |

## Quick start

Requirements: Python 3.8+ with `numpy scipy matplotlib` for the host tools;
Icarus Verilog for the RTL regression; Verilator for the lint gate; `arduino-cli`
with the earlephilhower RP2040 core for the firmware build.

```bash
pip install numpy scipy matplotlib

# Host viewer — works with no radio attached
python software/sdr_spectrum.py --simulate --save spectrum.png --frames 20

# Live, against a configured board (both modes transmit to UDP 10000)
python software/sdr_spectrum.py --mode raw
python software/sdr_spectrum.py --mode fft
python software/sdr_dashboard.py --port 10000 --serial COM7

# Host-tool unit checks (run from the repository root)
python software/test_dashboard.py
```

## Verification

These are the gates CI runs (`.github/workflows/rtl.yml`), and they are the only
commands that should be quoted as "the tests pass":

```bash
python tools/run_tests.py        # Icarus regression: 21 testbench runs
python tools/lint_verilog.py     # Verilator lint: 5 tops
python tools/check_firmware.py   # arduino-cli compile of the RP2040 sketch
```

Last run of this tree (2026-11-04, Windows, Icarus Verilog 12.0, Verilator 5.040,
arduino-cli):

```
run_tests.py       21/21 passed
lint_verilog.py    5/5 tops clean
check_firmware.py  PASS: firmware compiles (0 warning(s))
```

Hardware analyses are reproducible too:

```bash
python hardware/redesign/validate_plan.py    # frequency plan / LO / Nyquist
python hardware/redesign/rf_analysis.py      # Friis cascade, NF, anti-alias
python hardware/redesign/cost_model.py       # V1 vs V2 fitted BOM
python tools/gen_lpf.py                      # PCB -> verilog/v2_top_generated.lpf
```

## Documentation

| Document | Contents |
|---|---|
| [`HARDWARE.md`](HARDWARE.md) | Every claim parsed from placed footprints, pad nets and PCB setup; supersedes all other hardware prose |
| [`hardware/HARDWARE_VALIDATION.md`](hardware/HARDWARE_VALIDATION.md) | V1 audit (ERC/DRC counts), V2 corrections, limitations, first-article plan |
| [`GATEWARE.md`](GATEWARE.md) | Two modes, rate plan, throughput table, FFT correctness, what blocks production |
| [`RESOURCE_REPORT.md`](RESOURCE_REPORT.md) | Yosys/ABC area numbers and what is still unmeasured |
| [`POWER.md`](POWER.md) | Per-rail load estimates and the one-lane-at-a-time policy |
| [`BRINGUP.md`](BRINGUP.md) | Staged board bring-up, each step with its failure signature |
| [`KNOWN_ISSUES.md`](KNOWN_ISSUES.md) | Defect register: fixed (kept for the record) and open |
| [`docs/em-validation/EM_VALIDATION_REPORT.md`](docs/em-validation/EM_VALIDATION_REPORT.md) | EM/RF evidence package; every full-wave/physical claim fails closed |
| [`software/LEGACY.md`](software/LEGACY.md) | What the four removed V1 software trees attempted |

## V1 / legacy status

The first-generation design (`Wideband-Test-V1`, dsPIC33 firmware, 105 MSPS /
GMII RTL, USB-bulk host stack, GNU Radio module, ExtIO plugin) is **removed from
this tree**. It is not supported, not buildable here, and its pin, clock and
interface claims must not be reused: see the void list at the top of
[`HARDWARE.md`](HARDWARE.md).

Where the record survives:

- `hardware/HARDWARE_VALIDATION.md` — the reproducible V1 audit, kept as evidence.
- [`software/LEGACY.md`](software/LEGACY.md) — what each removed software tree
  attempted and why it was abandoned.
- [`KNOWN_ISSUES.md`](KNOWN_ISSUES.md) § Removed — the deleted P0 RTL cluster,
  module by module.
- [`ISSUE_11_VERIFICATION_REPORT.md`](ISSUE_11_VERIFICATION_REPORT.md) — the V1
  integration report, retained but marked historical.
- `git log -- <path>` — every removed tree is recoverable from history.
- [`docs/CLEANUP_2026-11-04.md`](docs/CLEANUP_2026-11-04.md) — the itemised
  removal list from the 2026-11-04 cleanup.

Deliberately **not** removed: vendored third-party libraries and the upstream
OpenFPGA framework, which are not V1 design work. They are classified in the
cleanup report.

## Licence

This repository contains **no `LICENSE` file**. Treat it as all rights reserved
until one is added; the badges and MIT text that earlier revisions of this README
carried referred to a file that is not present.

## References

- Lattice CrossLink: FPGA-TN-02015 (sysCLOCK PLL), FPGA-TN-02016 (sysI/O),
  FPGA-TN-02012 (high-speed I/O) — needed to confirm the vendor primitives that
  `verilog/v2_clock_pll.v` and `verilog/v2_rgmii.v` instantiate behind
  `V2_USE_VENDOR_PLL` / `V2_USE_VENDOR_DDR`.
- Microchip KSZ9031RNX datasheet — RGMII delay registers used by
  `v2_phy_manager.v`.
- Analog Devices AD9215 datasheet — 10-bit parallel timing.
