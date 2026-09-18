# Code-SDR V2 — Power Estimation

**These are datasheet-typical estimates, not measurements.** Nothing here has been
on a bench; there is no fabricated board. Every figure must be replaced by a
first-article measurement before the design is called validated. Rails and refs
are from `HARDWARE.md` section 9.

## Rails (as placed)

| Rail | Source | Loads |
|---|---|---|
| `+5V_RF` | J1 5 V input via F1 (2.5 A PTC), D1 | RF/IF amps, mixers, ADC driver |
| `SW_3V3` | U1 TLV62569DBVR | RP2040, flash, PHY I/O, FPGA I/O, digital |
| `SW_1V2` | U2 TLV62569DBVR | FPGA core/PLL, PHY core |
| `+2V5_AUX` | U3 TLV75525PDBVR | FPGA auxiliary |
| `+3V3_ANA` | U4 TPS7A8300ARGWT (low noise) | synthesisers, RF path, ADC, clocks |

## Estimated load per rail

Datasheet-typical active currents; the FPGA figure is design-dependent and the RF
figure assumes **one** lane enabled at a time (which firmware must enforce).

| Rail | Device | Ref | Est. current | Basis |
|---|---|---|---|---|
| `+5V_RF` | ADL5801 mixer | U51 | 100 mA | datasheet typical |
| | LTC5548 mixer | U63 | 200 mA | datasheet typical |
| | LT5560 mixer | U73 | 20 mA | datasheet typical |
| | GRF4001 LNA | U50/U60 | 60 mA each | datasheet typical |
| | GRF2101 LNA | U61/62/66/67 | 60 mA each | datasheet typical |
| | GRF2013 IF amp | U71/U75 | 80 mA each | datasheet typical |
| | AD8351 ADC driver | U74 | 30 mA | datasheet typical |
| | ADF4360-1 synth | U41 | 50 mA | datasheet typical |
| | PE4312 DSA | U72 | 1 mA | negligible |
| | **`+5V_RF` subtotal** | | **≈ 0.9 – 1.1 A** | one lane at a time |
| `+3V3_ANA` | LMX2592 synth | U40 | 120 mA | datasheet typical |
| | AD9215 ADC (AVDD) | U21 | 40 mA | ~120 mW |
| | clock tree (SiT8209 + buffers) | Y40/U42 | 60 mA | datasheet typical |
| | **`+3V3_ANA` subtotal** | | **≈ 0.22 A** | |
| `SW_3V3` | RP2040 | U10 | 50 mA | core + I/O, no radio |
| | W25Q16 flash | U11 | 5 mA | idle |
| | KSZ9031 (I/O) | U30 | 60 mA | 1000BASE-T |
| | FPGA VCCIO0/1/2 | U20 | 50 mA | I/O dependent |
| | **`SW_3V3` subtotal** | | **≈ 0.17 A** | |
| `SW_1V2` | FPGA core + PLL | U20 | 100 mA | design dependent |
| | KSZ9031 core | U30 | 150 mA | 1000BASE-T |
| | **`SW_1V2` subtotal** | | **≈ 0.25 A** | |
| `+2V5_AUX` | FPGA auxiliary | U20 | 20 mA | |

## Totals and implications

| Rail | Voltage | Est. current | Est. power |
|---|---|---|---|
| `+5V_RF` | 5.0 V | ~1.0 A | ~5.0 W |
| `SW_3V3` | 3.3 V | ~0.17 A | ~0.56 W |
| `+3V3_ANA` | 3.3 V | ~0.22 A | ~0.73 W |
| `SW_1V2` | 1.2 V | ~0.25 A | ~0.30 W |
| `+2V5_AUX` | 2.5 V | ~0.02 A | ~0.05 W |
| **Total** | | | **≈ 6.6 W** |

Notes that matter for bring-up:

1. **The RF rail dominates.** ~5 W of the ~6.6 W is the analog front end. Firmware
   must enable exactly one RF lane (`LNA_EN_*` / `RF_H*_ENABLE` / `MIXER_EN_HIGH`)
   and never all of them — concurrent operation is a thermal/electrical question
   for first-article test, and this design deliberately does not assume it works.
2. **The 5 V PTC is 2.5 A** (F1). The estimate sits at ~1 A, so there is headroom,
   but a stuck-on enable could approach it — the fault detection in the RP2040
   firmware watches for that.
3. **The 1.5 V headroom on the 5 V rail** (5 V in, `SW_3V3`/`SW_1V2` bucks, one
   LDO) is why `+5V_RF` exists at all rather than running the amps from 3.3 V.
4. **No thermal modelling is claimed.** ~6.6 W in an unknown enclosure is a
   first-article measurement, not a calculation.

## How to replace this with real numbers

Per rail, at first article:
1. Current: series shunt + DMM, or a bench supply's readback, at each rail with
   the design in its worst-case combination (raw mode, one RF lane, link up).
2. FPGA: Diamond's Power Calculator (`Power Estimator 3.14`) with the real
   placed-and-routed design and the measured toggle rates.
3. RF: measure per lane, confirm only one is ever enabled, and record die/case
   temperature under sustained operation.
