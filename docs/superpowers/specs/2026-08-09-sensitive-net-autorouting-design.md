# Sensitive-Net Autorouting Design

## Objective

Automatically complete and tune only the PCB connections whose function depends on differential balance, source-synchronous timing, controlled impedance, or low analog phase error. Preserve every existing footprint and every route outside the selected net set. Work on timestamped candidate copies and replace the live PCB only after all promotion gates pass.

## Authoritative inputs

- Live board: `hardware/v2/Code-SDR-V2.kicad_pcb`
- Project constraints: `hardware/v2/Code-SDR-V2.kicad_pro` and `hardware/v2/Code-SDR-V2.kicad_dru`
- Existing pair inventory: `hardware/v2/tools/freerouting_contract.py`
- RP2040 USB guidance: `https://datasheets.raspberrypi.com/rp2040/hardware-design-with-rp2040.pdf`
- KSZ9031 RGMII timing: `https://ww1.microchip.com/downloads/aemDocuments/documents/UNG/ProductDocuments/DataSheets/KSZ9031RNX-Data-Sheet-DS00002117J.pdf`
- AD9215 timing: `https://www.analog.com/media/en/technical-documentation/data-sheets/ad9215.pdf`
- ADF4360-1 differential RF outputs: `https://www.analog.com/media/en/technical-documentation/data-sheets/adf4360-1.pdf`
- LT5560 differential ports: `https://www.analog.com/media/en/technical-documentation/data-sheets/5560f.pdf`
- AD8351 differential ADC driver: `https://www.analog.com/media/en/technical-documentation/data-sheets/ad8351.pdf`
- LMX2592 differential synthesizer outputs: `https://www.ti.com/product/LMX2592`

## Selected routing scope

The router may remove, replace, or tune copper only on these nets:

- USB: `USB_DP_CONN`, `USB_DM_CONN`, `USB_DP`, `USB_DM`
- Ethernet MDI cleanup: `MDI_A_P/N`, `MDI_B_P/N`, `MDI_C_P/N`, `MDI_D_P/N`
- RGMII TX: `RGMII_TXD0..3`, `RGMII_TX_EN`, `RGMII_TXC` and their `_FPGA` segments
- RGMII RX: `RGMII_RXD0..3`, `RGMII_RX_DV`, `RGMII_RXC` and their `_FPGA` segments
- ADC digital timing: `ADC_D0..9`, `ADC_OR`, `ADC_CLK`
- LO/RF differential paths: `LMX_LO_LOW_P/N_RAW`, `ADF_LO2_P/N_RAW`, `LO2_P/N`, `LO_LOW_P/N`, `IF_LOW_P/N`, `IF_HIGH_P/N`, `LT_IN_P/N`
- IF/ADC differential paths: `IF2_P/N`, `IF2_F1_P/N`, `IF2_F2_P/N`, `IF2_F3_P/N`, `IF2_F4_P/N`, `IF2_LPF_P/N`, `AD8351_IN_P/N`, `AD8351_OUT_P/N`, `ADC_VIN_P/N`

All other nets, footprints, board outline geometry, copper-zone outlines, net assignments, and stack-up settings are immutable.

## Electrical targets

Manufacturer documents generally specify interface timing or differential use rather than a PCB length number. The following conservative design-level budgets translate those requirements into routable geometric constraints on this four-layer board:

| Group | Maximum mismatch or spread | Additional rule |
|---|---:|---|
| 2.4 GHz and higher LO/RF differential segments | 0.25 mm | Same layers and identical via count |
| IF2 and ADC analog differential segments | 0.50 mm | Same layers and identical via count |
| USB D+/D- end-to-end | 0.50 mm | 90 ohm differential geometry, continuous ground reference |
| Ethernet MDI pairs | 1.00 mm | Same layers and identical via count |
| RGMII TX data/control group | 2.00 mm total-length spread | Include both sides of each 33-ohm series resistor |
| RGMII RX data/control group | 2.00 mm total-length spread | Include both sides of each 33-ohm series resistor |
| ADC D0-D9 and `ADC_OR` | 2.00 mm total-length spread | Same layer/via policy where practical |
| `ADC_CLK` | No matching-to-data requirement | Shortest clear route, preferably without vias |

The KSZ9031 internal RGMII delays remain the intended clock/data phase mechanism; the PCB router must not add a deliberate clock-delay line. FPGA capture timing must account for the AD9215 clock-to-output delay, so `ADC_CLK` is not blindly length-matched to the ADC data traces.

## Routing architecture

1. Create a timestamped byte-for-byte backup of the live PCB and a separate candidate.
2. Snapshot all immutable board objects and non-selected copper.
3. Remove copper only for selected nets that are open, outside tolerance, or have unequal differential via counts. Leave already compliant selected routes intact when they do not obstruct required work.
4. Route differential nets as coupled objects: find one obstacle-clear center path, derive both conductors from it, and use paired layer changes.
5. Route RGMII and ADC digital groups through a shared routing corridor using consistent topology. Count total length across the series-resistor split nets.
6. Add compact serpentine tuning in reserved low-noise areas after connectivity is complete.
7. Refill zones, run KiCad DRC, measure connectivity and electrical-length proxies, and compare immutable snapshots.
8. Promote the candidate atomically only if every gate passes. Otherwise retain the live board unchanged and emit a failure report identifying the blocking nets.

## Failure handling and rollback

- Every run creates `live-before-sensitive-autoroute.kicad_pcb` in its timestamped build directory before any candidate work.
- Router timeout or no-path results leave the live board untouched.
- Any new short, clearance violation, unexpected unconnected item, footprint movement, non-selected copper change, zone-outline change, stack-up change, or layer-count change rejects the candidate.
- A promoted board is followed by a fresh live-file verification. If that differs from candidate verification, restore the backup immediately.

## Promotion gates

- All selected signal endpoints are connected.
- Every differential and timing group meets the table above.
- Every differential pair has equal via counts and the same layer-transition topology.
- USB and other controlled-impedance geometry remains consistent with the board's calculated/fabricator stack-up rules.
- No new DRC violations or non-selected unconnected items relative to the backup.
- All non-selected tracks, vias, footprints, board drawings, zone outlines, nets, and net classes compare equal to the backup.
- Board remains four copper layers and below 100 cm2.
- The candidate, measurements, DRC report, preservation report, and backup are retained in the timestamped build directory.

## Non-goals

- Routing ordinary power, control, SPI, status, or low-speed GPIO nets.
- Moving or rotating components.
- Changing the layer stack, board outline, schematic, architecture, or supply voltages.
- Claiming whole-board completion while unrelated ratsnest connections remain.
