# Code-SDR V2 — Bring-Up & Fabrication Validation

A staged procedure that proves the fabrication and every chip, in an order where
each step's failure is unambiguous. **None of this has been run** — no board is
fabricated. Each step lists what it proves and how it fails.

Refs and nets are from `HARDWARE.md`; rails from `POWER.md`. Tools: bench supply
with current limit and readback, DMM, scope (≥100 MHz), hot-air/iron, and a PC.

---

## Stage 0 — before power

| # | Check | Pass | If it fails |
|---|---|---|---|
| 0.1 | Visual: no solder bridges, especially U20 (0.65 mm pitch), U10 (QFN-56), U30 (QFN-48) | clean | rework |
| 0.2 | Continuity: every rail to GND is **not** a short (expect > 50 Ω on a DMM) | > 50 Ω | locate and clear the bridge before applying power |
| 0.3 | Continuity: `VIN_5V` → F1 → `VIN_FUSED`; F1 not open | < 1 Ω | replace F1 if open |
| 0.4 | `ADC_CLK` (net 12) rings from U21.1 to U20.**D9** (not D7) | continuous | — |

---

## Stage 1 — power rails (no FPGA config)

Apply 5 V to J1 with the supply limited to **300 mA**.

| # | Rail | Expected | Notes |
|---|---|---|---|
| 1.1 | `SW_3V3` (U1) | 3.3 V ±5% | if low, check the buck feedback |
| 1.2 | `SW_1V2` (U2) | 1.2 V ±5% | |
| 1.3 | `+2V5_AUX` (U3) | 2.5 V ±5% | |
| 1.4 | `+3V3_ANA` (U4) | 3.3 V ±5%, low noise | the RF/ADC supply |
| 1.5 | `+5V_RF` | ~5 V | |
| 1.6 | Total draw at idle (all RF lanes off) | **< 300 mA** | a stuck RF enable shows here |

**Failure signature:** any rail near 0 V with the supply in current limit = a
short on that rail; go back to 0.2.

---

## Stage 2 — clocks

| # | Check | Probe | Pass |
|---|---|---|---|
| 2.1 | 100 MHz reference | Y40 output → U42.1 (`REF_100M_OSC`) | 100 MHz ±50 ppm, LVCMOS |
| 2.2 | ADC clock | `ADC_CLK` (U20.D9) | 100 MHz, clean edges |
| 2.3 | PHY 25 MHz crystal | `PHY_XI` (U30) | 25 MHz oscillating |
| 2.4 | RP2040 crystal | `RP_XIN` (Y10) | 12 MHz oscillating |

---

## Stage 3 — RP2040 bring-up (before touching the FPGA)

| # | Check | Pass |
|---|---|---|
| 3.1 | Flash `firmware/rp2040_dashboard` (Arduino-Pico core, USB stack = Pico SDK) | sketch uploads |
| 3.2 | USB CDC enumerates, dashboard banner prints | serial output |
| 3.3 | `FAULTS:` line appears | `active=0x00` (nothing wrong yet) |
| 3.4 | Overtemp path: warm the RP2040 (hot air, gentle) | `FAULT_OVERTEMP` raises, then clears |

**Confirms:** RP2040, its flash, USB, and the fault engine run.

---

## Stage 4 — FPGA configuration

| # | Check | Pass | If it fails |
|---|---|---|---|
| 4.1 | Place `raw.bin` on the RP2040 LittleFS as `/raw.bit` | file present | — |
| 4.2 | Send `0` on serial to trigger configuration | `# FPGA configuration OK (CDONE=1)` | **FAILED** → check `FPGA_SPI_*` (G1/K1/H1/J1) and `FPGA_RESET_N` (H2) |
| 4.3 | CDONE stays high | no `FAULT_FPGA_CONFIG` | if CDONE drops, the bitstream is wrong or the config clock is unstable |
| 4.4 | `FAULT_SPI_BUS` absent | telemetry reads sane | all-ones/zero read = SPI bus fault |

**Confirms:** FPGA, config flash path, and the SPI control plane.

---

## Stage 5 — Ethernet / PHY bring-up

| # | Check | Pass | If it fails |
|---|---|---|---|
| 5.1 | `FAULT_PHY_ID` absent after config | PHY identity matched | mismatch → check `MDC` (J6) / `MDIO` (K7) and PHY power |
| 5.2 | `PHY_RESET_N` pulses at power-up (U10.37) | scope | |
| 5.3 | Plug into a gigabit switch; link LED | lit | check the RJ45 magnetics and `PHY_INT_N` (K2) |
| 5.4 | Dashboard `link=1`, `pll=1` | yes | link but no traffic → RGMII skew |
| 5.5 | **Ethernet loopback:** connect the board to a PC, run `software/sdr_spectrum.py --mode raw`, drop the FPGA into raw mode | frames arrive, `pkts` increments | no packets → RGMII TX (E1,E2,F1,F2,J3,J7) or the 125 MHz PLL |
| 5.6 | Packet integrity | `net loss` ≈ 0% and `fpga drops` not climbing | loss climbing = link capacity or a bad lane |

**Confirms:** KSZ9031, RGMII DDR, the PLL, the MAC/UDP path, and the whole raw
datastream. This is the single most informative test on the board.

**Bank check (do this in CAD, before fab):** every RGMII ball must be in Bank 1/2
— CrossLink DDR is unavailable in Bank 0.

---

## Stage 6 — ADC path

| # | Check | Pass |
|---|---|---|
| 6.1 | Inject a known tone into `RF_LOW_IN`; enable only `LNA_EN_LOW_IC` | spectrum peak at the expected frequency in `sdr_spectrum.py` |
| 6.2 | Sweep the injected level | peak tracks; `ADC_OR` (K9) asserts only near full scale |
| 6.3 | Disable all RF lanes | noise floor only, **current drops** (confirms one-lane-at-a-time) |

---

## Stage 7 — RF lane matrix

For **each** lane (`RF_HA_IN`..`RF_HD_IN`), one at a time:

| # | Check |
|---|---|
| 7.1 | Enable that lane only; confirm every other lane's current contribution is zero |
| 7.2 | Inject a tone; confirm it appears |
| 7.3 | Confirm `+5V_RF` current matches the single-lane estimate in `POWER.md` |

---

## Stage 8 — FFT mode

| # | Check | Pass |
|---|---|---|
| 8.1 | Store `/fft.bit`, send `1` | reconfigure OK |
| 8.2 | `software/sdr_spectrum.py --mode fft` | bins arrive; `scale_exp` reported |
| 8.3 | Inject a tone at a known offset | peak lands on the expected bin |

---

## Fault coverage summary

| Fault | Detected by | Where |
|---|---|---|
| FPGA config lost | CDONE low in `loop()` | `faults.h FAULT_FPGA_CONFIG` |
| SPI bus dead | telemetry reads all-ones/zero | `FAULT_SPI_BUS` |
| PHY bring-up failure | FPGA `phy_error` in telemetry bit 20 | `FAULT_PHY_ID` |
| Link down > 3 s | link bit | `FAULT_LINK_DOWN` |
| FPGA overrun | drop counter advancing | `FAULT_OVERRUN` |
| Controller hang | hardware watchdog (3 s) | `faults_watchdog_init` |
| Overtemperature | RP2040 internal sensor > 80 °C | `FAULT_OVERTEMP` |

Gaps that hardware must close, not firmware:

- **Rail faults** (U5–U9 supervisors) are hardware-only — they gate power, they do
  not report. A scope on the enable nets is the only observation.
- **Stuck RF enable** is inferred from `+5V_RF` current, not measured in firmware.
- **PHY disconnect** (cable out) reads as link-down, indistinguishable from a PHY
  fault over the serial console alone.
