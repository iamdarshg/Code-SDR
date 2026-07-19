# Code-SDR hardware validation and V2 redesign

## Decision

`Wideband-Test-V1` is **DO NOT FABRICATE**. The corrected design is the
hierarchical KiCad project at [`hardware/v2`](v2).

V2 is one SDR receiver/control board. It contains no ESC, motor driver, phase
current measurement or high-current switching. Any ESC remains a distinct
external board with its own power, grounding, safety and validation boundary.

The design target is continuous tuning from 100 MHz through 10 GHz with a
40 MHz real instantaneous channel. Cost is the first optimization priority.
The analog design and CAD checks can be completed in this repository, but a
10 GHz design is not production-qualified until first articles have passed
VNA, phase-noise, noise-figure, blocker and thermal tests.

## Reproducible V1 audit

The redesign began from commit `7d617d1`. A fresh KiCad 9.0.9 audit of the
legacy project produced:

| V1 check | Result |
|---|---:|
| Schematic ERC findings | 521 |
| PCB DRC violations | 656 |
| Unconnected PCB items | 499 |
| Schematic/PCB parity findings | 10 |
| Fitted references | 285 |

The ERC total includes 449 library-symbol consistency findings; even when those
are separated, the remaining connectivity and power findings still prevent
fabrication. The older 94-count audit used a narrower reporting configuration.

The failures included invalid CrossLink and KSZ9031 rails, a nonfunctional
RP2040 implementation, incorrect ADC DNC/reference/clock connections,
incorrect LMX2592 reference and NC connections, RF parts outside their
ratings, no valid 100 MHz-10 GHz frequency-conversion path, symbol/footprint
pin errors and a substantially unrouted board. These are architectural faults,
not value-tuning issues.

`hardware/validate_hardware.ps1` preserves the V1 audit. No other branch or
open PR contained a corrected receiver, rail tree or PCB that could be safely
reused; the other branch/PR work was RTL/software. Closed, unmerged PR #28 changes only
the Ethernet/UDP and processing RTL. It contains no voltage, schematic,
footprint or PCB correction and still requires a V2-specific 100 MHz/RGMII
port before it can be loaded on this board.

## Implemented V2 architecture

```text
LOW SMA, 0.1-2.5 GHz
  ESD -> 2.5 GHz LPF -> GRF4001 -> ADL5801 -------------------+
                                                              |
HIGH A SMA, 2.3-4.2 GHz -> GRF4001 ----------------------+     |
HIGH B SMA, 4.2-6.8 GHz -> GRF2101 ----------------------+     |
HIGH C SMA, 6.8-9.2 GHz -> GRF2101 ----------------------+-> QPC6144
HIGH D SMA, 8.0-10 GHz -> GRF2101 -> GRF2101 ------------+     |
                                                            LTC5548
                                                              |
             +-------------------- 2.43625 GHz IF ------------+
             |
 BGS12 selector -> QPQ1909 -> GRF2013 -> PE4312 -> GRF2013
   -> LT5560 with fixed 2.410 GHz LO
   -> 6.25-46.25 MHz, 9-pole differential LPF
   -> AD8351 -> AD9215 at 100 MSPS -> CrossLink FPGA -> RGMII
```

The five RF inputs are intentional. Selecting after the LNAs avoids a lossy,
expensive 10 GHz input switch and preserves sensitivity. The extra D-port LNA
costs about $1.14 and fixes the otherwise weakest 9.2-10 GHz noise budget.

The high-band matched LNAs are useful band selectors, but they are not
precision brick-wall preselectors. For blocker-rich or calibrated image-free
measurements, place an external bandpass filter ahead of the selected high
input. Four fitted on-board wideband filter banks were rejected because they
would add cost and pre-LNA insertion loss.

## Hierarchical schematic

The root schematic contains eight functional subsheets:

1. `01_power` - protected 5 V input and 3.3 V digital, 1.2 V core, 2.5 V
   auxiliary and low-noise 3.3 V analog rails.
2. `02_controller` - RP2040, flash, USB and SWD control only.
3. `03_fpga_adc` - CrossLink ctfBGA80, AD9215 and clock/data interface.
4. `04_ethernet` - KSZ9031RNX RGMII PHY and integrated-magnetics RJ45.
5. `05_reference_lo` - common 100 MHz oscillator/fanout, LMX2592 and fixed
   ADF4360-1 second LO.
6. `06_rf_low` - low-band protection, LPF, LNA, mixer and active-output balun.
7. `07_rf_high` - four characterized LNA ports, QPC6144 and LTC5548.
8. `08_if_chain` - IF selection/filtering, two gain stages, DSA, second mixer,
   anti-alias filter and ADC driver.

The schematic/model generation sources are in `hardware/v2/tools`. The
generated sheet structure is the release source; V1 is retained only for
comparison.

## Frequency plan

First IF is 2436.25 MHz. The 40 MHz wanted band is 2416.25-2456.25 MHz, inside
the QPQ1909 passband. The fixed ADF4360-1 LO is 2410 MHz, generated exactly
from the shared 100 MHz reference with `R=20`, 5 MHz PFD and integer
`N=482` (`P=16`, `B=30`, `A=2`). The second IF is 26.25 MHz.

At 100 MSPS the wanted real ADC band is 6.25-46.25 MHz, leaving 3.75 MHz to
Nyquist. The implemented filter is down approximately 20.6 dB at 53.75 MHz,
35.7 dB at 60 MHz and 87 dB at 100 MHz in the ideal component model.

| Selected path | RF range | First-LO mode and range |
|---|---:|---:|
| LOW | 0.100-2.500 GHz | high side, 2.53625-4.93625 GHz |
| A | 2.300-4.200 GHz | high side, 4.73625-6.63625 GHz |
| B | 4.200-6.800 GHz | high side, 6.63625-9.23625 GHz |
| C high-side segment | 6.800-7.350 GHz | high side, 9.23625-9.78625 GHz |
| C low-side segment | 7.150-9.200 GHz | low side, 4.71375-6.76375 GHz |
| D | 8.000-10.000 GHz | low side, 5.56375-7.56375 GHz |

The overlaps make coverage continuous. The LMX2592 upper requirement is
9786.25 MHz, 13.75 MHz below its 9800 MHz rated limit.

The QPC6144 physical routing maps RF4/RF3/RF2/RF1 to C/A/B/D respectively.
With `V3=0`, firmware uses:

| Path | HSW_V2 | HSW_V1 |
|---|---:|---:|
| C / RF4 | 0 | 0 |
| D / RF1 | 0 | 1 |
| B / RF2 | 1 | 0 |
| A / RF3 | 1 | 1 |

Only the selected LNA path should be enabled. The D selection enables both
cascaded D LNAs.

## Calculated gain, noise and throughput

The reproducible calculation is `hardware/redesign/rf_analysis.py`. It uses
40 MHz noise bandwidth, conservative in-band component figures and the fitted
21 dB AD8351 setting.

| Path | Gain at minimum DSA loss | Cascaded NF | 10 dB-SNR sensitivity |
|---|---:|---:|---:|
| LOW | 62.2 dB | 3.27 dB | -84.7 dBm |
| A | 60.3 dB | 3.15 dB | -84.8 dBm |
| B | 58.2 dB | 3.98 dB | -84.0 dBm |
| C | 58.3 dB | 4.03 dB | -83.9 dBm |
| D | 67.9 dB | 2.50 dB | -85.5 dBm |

The worst nominal analog noise is about 15.7 dB above the ADC's integrated
noise, so the ADC does not set the receiver noise floor. PE4312 provides
31.5 dB of ADC-level adjustment and powers up at maximum attenuation.
Compression and blocker limits require measurements; ADC full-scale
calculations alone do not guarantee upstream linearity.

The ADC's raw output is 100 MSPS x 10 bit = 1.0 Gbit/s before Ethernet
overhead, so raw full-rate samples cannot be GbE payload. FPGA firmware must
filter and decimate/resample or reduce sample width before packetization.

## Major electrical corrections

- CrossLink changed to the routable 0.65 mm ctfBGA80 package. Its used core,
  auxiliary, three I/O-bank and GPLL supplies have separate checklist-value
  ferrite/filter islands and one 100 nF bypass per supply ball. The unused
  D-PHY analog/PLL supplies are intentionally unpowered as explicitly allowed
  by the vendor checklist.
- CrossLink CRESET_B, CDONE and SPI_SS use the required 4.7 kΩ pull-ups to
  VCCIO0; the processor-driven configuration clock has 33 Ω source damping.
- KSZ9031RNX pin map, 1.2 V core, ISET, straps, RGMII series damping, reset,
  MDIO pulls, crystal and integrated-magnetics RJ45 corrected. AVDDH, AVDDL
  and AVDDL_PLL now use the vendor-checklist ferrite partitions, bulk
  capacitance and per-pin local bypassing instead of sharing unfiltered digital
  rails.
- RP2040 rebuilt as control-only circuitry with valid core regulator use,
  crystal, flash, USB ESD topology, reset/boot and unique GPIO assignments.
- AD9215 DNC, AVDD/DRVDD, MODE, SENSE/VREF/REFT/REFB and decoupling corrected.
- One 100 MHz SiT8209 and CDCLVC1104 fanout now clock the ADC and both PLLs;
  the PHY retains its independent 25 MHz crystal.
- LMX2592 reference, CE, NC pins, per-rail bypassing and passive loop network
  corrected. The final loop must still be verified with PLLatinum Sim.
- ADL5801 receives its correct 50:200 ohm center-tapped active-output balun.
- LTC5548 uses the datasheet-recommended TC1-1-13M+ 1:1 IF transformer; this
  saves about $7.46 against MABA-011108 at the public 1,000-piece tiers.
- LTC5548 LO/GND pin error and LT5560 input-center-tap bias error corrected.
- GRF3042, used outside the original 5 V implementation, replaced by two
  5 V-compatible GRF2013 IF gain stages.
- Expensive HMC542B replaced by the production PE4312C-Z.
- Multi-pF RF ESD parts replaced by 0.1 pF RF-qualified protection.
- The ADC anti-alias network replaced by a calculated nine-pole differential
  filter with the four calculated shunt capacitors connected directly between
  the differential legs; its response is exported to `FILTER_RESPONSE.csv`.
- The cascaded 8-10 GHz LNAs use separate 2.9 kΩ enable/bias-setting
  resistors. Sharing one resistor would under-bias both devices because the
  GRF2101 enable current is part of its quiescent-current setting.
- Startup pulldowns keep mixers/LNAs disabled, the DSA at maximum attenuation,
  the FPGA in reset until valid and the QPC selector deterministic.

## Power architecture

Main input is a protected regulated 5 V supply at J1. USB is data/control only.
Normal firmware must enable only the selected RF path to keep analog-regulator
load and temperature bounded.

| Rail | Design capacity | Principal loads |
|---|---:|---|
| +5V_RF | 5 V / 1.0 A | ADL5801, two GRF2013 stages, AD8351 |
| +3V3_DIG | 3.3 V / 2.0 A | FPGA I/O, RP2040, flash, PHY I/O |
| +1V2_CORE | 1.2 V / 2.0 A | Filtered FPGA core/GPLL islands and PHY core |
| +2V5_AUX | 2.5 V / 0.5 A | FPGA auxiliary rail |
| +3V3_ANA | 3.3 V / 1.0 A | LMX/ADF, selected RF path, ADC, clocks and RF controls |

TLV767's exposed pad requires the specified ground copper and thermal vias.
At elevated ambient, measure the 3.3 V analog LDO junction estimate and disable
unused RF paths. If all RF paths must be powered concurrently, replace this
rail with a low-noise buck plus post-filter/LDO architecture; that is not the
normal receiver operating mode.

## PCB implementation requirements

V2 is a 160 x 100 mm, single-sided-assembly, four-layer board:

`F.Cu / 0.18 mm prepreg / solid In1 GND / 1.09 mm core / In2 power+slow /
0.18 mm prepreg / B.Cu`

The finish is ENIG. RF uses grounded coplanar routing on F.Cu referenced to
the uninterrupted In1 plane. The >7 GHz connector/LNA/select paths must remain
on F.Cu without signal vias, with short runs and ground-via fencing. RGMII,
ADC data, clocks and USB require continuous reference and controlled geometry.

The stack values are a fabrication specification, not a generic promise of
50 ohms. The selected fabricator must field-solve the RF and differential
width/gap against its actual Dk, copper and solder mask before ordering.

## Release and first-article gates

1. Zero schematic ERC errors, PCB DRC violations, parity mismatches and
   unconnected items.
2. Gerber/drill/BOM/position outputs generated from the exact routed board.
3. Independent visual review of every schematic sheet and both PCB sides.
4. Written production quote for QPC6144; the cost model reports its ceiling.
5. Fabricator impedance review for RF, USB and RGMII geometries.
6. Power-up with RF paths disabled; verify all rails, sequencing, ripple,
   inrush and thermals.
7. Verify both PLLs for lock, output level, spurs and phase noise at every
   band edge.
8. VNA launches, every RF branch, BAW path and anti-alias filter.
9. Sweep gain, NF, P1dB, IIP3, images, LO leakage and spurs at 0.1, 0.5, 1,
   2.4, 4, 6, 7.2, 8.5, 9.2 and 10 GHz.
10. Record per-path gain, frequency and image calibration over the full range,
    followed by a 24-hour thermal soak.

## Primary references

- [AD9215](https://www.analog.com/media/en/technical-documentation/data-sheets/ad9215.pdf)
- [LMX2592](https://www.ti.com/lit/ds/symlink/lmx2592.pdf)
- [CrossLink](https://www.latticesemi.com/view_document?document_id=51662)
- [CrossLink hardware checklist](https://www.latticesemi.com/-/media/LatticeSemi/Documents/ApplicationNotes/AD2/FPGA-TN-02013-1-3-CrossLink-Hardware-Checklist.ashx?document_id=51654)
- [RP2040 hardware design guide](https://datasheets.raspberrypi.com/rp2040/hardware-design-with-rp2040.pdf)
- [KSZ9031RNX](https://ww1.microchip.com/downloads/aemDocuments/documents/UNG/ProductDocuments/DataSheets/KSZ9031RNX-Data-Sheet-DS00002117J.pdf)
- [KSZ9031 hardware checklist](https://ww1.microchip.com/downloads/en/DeviceDoc/KSZ9031RNX-HW-Design-Checklist-00003391.pdf)
- [ADL5801](https://www.analog.com/media/en/technical-documentation/data-sheets/ADL5801.pdf)
- [LTC5548](https://www.analog.com/media/en/technical-documentation/data-sheets/5548f.pdf)
- [LT5560](https://www.analog.com/media/en/technical-documentation/data-sheets/5560f.pdf)
- [AD8351](https://www.analog.com/media/en/technical-documentation/data-sheets/AD8351.pdf)
- [ADF4360-1](https://www.analog.com/media/en/technical-documentation/data-sheets/adf4360-1.pdf)
- [CDCLVC1104](https://www.ti.com/lit/ds/symlink/cdclvc1104.pdf)
- [GRF4001](https://www.guerrilla-rf.com/includes/prodFiles/4001/GRF4001DS.pdf)
- [GRF2101](https://www.guerrilla-rf.com/includes/prodFiles/2101/GRF2101DS.pdf)
- [GRF2013](https://www.guerrilla-rf.com/includes/prodFiles/2013/GRF2013DS.pdf)
- [QPQ1909](https://www.mouser.com/datasheet/2/412/QPQ1909_Data_Sheet-1815271.pdf)
- [PE4312](https://www.psemi.com/pdf/datasheets/pe4312ds.pdf)
- [QPC6144](https://www.qorvo.com/products/p/QPC6144)
- [TC1-1-13M+](https://www.minicircuits.com/pdfs/TC1-1-13M+.pdf)
