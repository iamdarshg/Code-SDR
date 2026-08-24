# Code-SDR V2 first-article validation

## Scope and release boundary

This plan applies only to the receive-only Code-SDR V2 assembly in
`hardware/v2`. Passing the pre-fabrication gates makes the design orderable as
a first article; it does not establish production RF performance. RF
qualification requires measurements on assembled hardware and recorded raw
data for every unit tested.

## Pre-fabrication evidence required

- KiCad 9 ERC: zero violations.
- KiCad 9 PCB DRC with all-track errors and schematic parity enabled: zero
  violations, zero unconnected items, and zero parity findings.
- Visual review of all eight schematic sheets and both PCB sides.
- RF50 audit confirming net connectivity, intended outer-layer width, launch
  geometry, return-via placement, and no stub or unintended plane transition.
- Timing-group review for ADC clock/data, reference clocks, RGMII, and USB.
- Recalculated frequency-plan report covering 0.1-10 GHz without a tuning hole.
- Fabricator-approved impedance coupon/field solve for RF50, USB, and any RGMII
  geometry controlled by impedance.
- Exact fitted BOM with manufacturer part number, package, rating, lifecycle,
  and sourcing evidence; QPC6144 requires a written quotation.
- Reviewed Gerber, drill, placement, assembly, schematic, STEP, and manifest
  outputs generated from the same committed PCB revision.

## Probe points

The PCB has no dedicated `TP` footprints. For the first article, use the
listed accessible connector or capacitor pads with a fine insulated probe;
never probe RF or switch-node pads with an uninsulated clip. Pad 2 of each
listed capacitor is ground.

| Function | Probe point | Expected initial result |
|---|---|---|
| Input | J1.1 to J1.2 | 5.00 V regulated input |
| Fused input | C1.1 to C1.2 | within 100 mV of J1.1 after F1 |
| Filtered RF rail | C2.1 to C2.2 | 5.0 V nominal |
| Digital rail | C4.1 to C4.2 | 3.3 V nominal |
| FPGA/PHY core rail | C7.1 to C7.2 | 1.2 V nominal |
| FPGA auxiliary rail | C10.1 to C10.2 | 2.5 V nominal |
| Analog rail | C12.1 to C12.2 | 3.3 V nominal |
| RP2040 debug | J11.1/J11.3 | 3.3 V/GND reference |
| RP2040 reset | J11.10/J11.3 | high after reset release |
| Ethernet | J30 | cable/link and packet tests only; do not probe MDI live |
| Low-band RF input | J50 | calibrated RF source/VNA port |
| High-band A input | J60 | calibrated RF source/VNA port |
| High-band B input | J61 | calibrated RF source/VNA port |
| High-band C input | J62 | calibrated RF source/VNA port |
| High-band D input | J63 | calibrated RF source/VNA port |

## Exact initial power-up sequence

1. Inspect the bare and assembled board under magnification. Reject solder
   bridges, tombstones, reversed polarized parts, unsoldered exposed pads,
   damaged RF launches, or debris before applying power.
2. With J1 disconnected, measure resistance from each of C1.1, C2.1, C4.1,
   C7.1, C10.1, and C12.1 to ground. Record values after they settle. Stop for
   a hard short or a result inconsistent with the schematic/BOM; do not
   repeatedly power a suspect rail.
3. Connect SWD at J11 with target power disabled. Confirm pin 1 orientation.
   Prepare firmware that keeps `LNA_EN_LOW`, `LNA_EN_H1..H4`,
   `MIXER_EN_HIGH`, and all unnecessary RF paths disabled; holds the PHY and
   FPGA in reset until their rails are valid; and drives the RF switch to a
   documented safe state.
4. Set a bench supply to 5.00 V, current limit 0.10 A, output off. Connect
   positive to J1.1 and return to J1.2. Do not power through USB.
5. Enable the supply. If it immediately enters constant-current mode, remove
   power and diagnose the short. Otherwise record input current and all six
   rail voltages within five seconds. Increase the limit in 0.10 A steps only
   when measured current and rail behavior are normal; never exceed 2.5 A (F1)
   or the characterized first-article operating limit.
6. Verify 3.3 V digital, 1.2 V core, 2.5 V auxiliary, 3.3 V analog, and 5 V RF
   rails for value, startup monotonicity, ripple, and overshoot. Use a ground
   spring for oscilloscope measurements.
7. Check U1-U4, the FPGA, PHY, ADC, both synthesizers, mixers, and LNAs with a
   thermal camera at 10 s, 30 s, 2 min, and 10 min. Remove power for unexpected
   heating or temperature rise.
8. Release and identify the RP2040 over SWD. Program only the safe-state image,
   reset it, and confirm all RF enables remain inactive.
9. Release the FPGA, confirm configuration completion, verify the 100 MHz ADC
   clock/domain, and run the ADC static-pattern/overflow checks before enabling
   RF gain.
10. Release the PHY, read its identifier and strap/configuration registers over
    MDIO, establish a 1 Gb/s RGMII link, and run bidirectional packet, CRC,
    sequence, and sustained-throughput tests.
11. Enable one RF path at a time. Re-record supply current and temperature for
    each state. Never enable all gain paths merely to simplify testing.

## Clock, PLL, and digital validation

- Measure the 100 MHz reference at every fanout destination for frequency,
  level, duty cycle, and additive jitter consistent with the device limits.
- For LMX2592 and ADF4360-1, record lock status, programmed divider values,
  output power, phase noise, reference spurs, fractional/integer spurs, and
  unlock recovery at every band edge and transition overlap.
- Exercise ADC ramp/static patterns and capture at least 10^9 samples without
  unexplained overflow, framing error, or bit error.
- Verify RGMII setup/hold by static timing in the vendor flow and by sustained
  traffic at temperature and supply corners. An Ethernet link light alone is
  not a pass.
- Verify RP2040 controls against the schematic net names, including LO serial
  interfaces, lock inputs, LNA enables, DSA controls, high-band switch truth
  table, IF selection, ADC power-down, FPGA reset/configuration, and PHY reset.

## RF characterization required after fabrication

Calibrate cables, adapters, attenuators, and VNA/reference planes before every
measurement set. Store Touchstone/CSV/raw instrument files with serial number,
firmware/bitstream hash, temperature, supply voltage, path state, frequency,
and instrument configuration.

- VNA: connector launches, each filter/balun/transformer path, return loss,
  insertion loss, and path isolation.
- Gain and noise figure: each gain state and RF path over its entire assigned
  range, including both sides of every transition overlap.
- Linearity: input P1dB, output P1dB where applicable, two-tone IIP3, blocker
  response, and recovery without damaging the input chain.
- Spectral purity: phase noise, LO feedthrough/leakage, reference spurs,
  synthesizer spurs, images, harmonics, and ADC aliases.
- Image rejection and anti-alias performance using wanted/image tones at the
  first and second IF relationships.
- Full tuning sweep from 100 MHz through 10 GHz with no uncovered frequency.
  At minimum retain detailed records at 0.1, 0.5, 1.0, 2.3, 2.4, 2.5, 4.2,
  6.8, 8.0, 9.2, and 10.0 GHz and at every calculated worst-case transition.
- Thermals and current at nominal and worst supported ambient for every normal
  receive state. Verify regulator and exposed-pad temperatures and derating.

## Pass/fail record

The validation record must identify the board serial number and commit, fitted
BOM revision, CAM manifest SHA256, firmware and FPGA hashes, instruments and
calibration dates, operator, date, ambient, raw-data paths, each measured result,
and signed disposition. Deviations require a documented review; they are not
silently converted into a pass.
