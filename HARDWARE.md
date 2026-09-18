# Code-SDR V2 Hardware and Coding Interfaces

Source of truth: `hardware/v2/Code-SDR-V2.kicad_pcb` (board rev 2.1,
title block: `CONTROL/SDR BOARD ONLY - ESC IS A DISTINCT EXTERNAL BOARD`).
Every claim below is parsed from placed footprints, pad nets, and the PCB
setup block. Anything not present in copper is marked as such.

Parsed totals: 373 placed footprints, 11,510 track segments, 1,152 vias,
4 zones, 4 copper layers (`F.Cu`, `In1.Cu`, `In2.Cu`, `B.Cu`). Outer
dielectric is 0.13 mm FR4 prepreg per the PCB setup block. Footprint
placement spans roughly 0-99 mm in both axes. Zone fills, return-path
treatment, and length/skew validation must be confirmed in CAD before
fabrication; they are not claimed here.

The following legacy claims are void against this PCB: 105 MSPS sample
clock, 8-bit GMII, `LIF-MD6000-6UMG64I`, dsPIC33 firmware, USB bulk SDR
streaming, and any GNU Radio / ExtIO completion status. Do not use them.

## 1. Compute and control

| Ref | Value | Footprint | Role on PCB |
|-----|-------|-----------|-------------|
| U10 | RP2040 | QFN-56 | Sole controller (GPIO/SPI/USB/QSPI/SWD) |
| U11 | W25Q16JVSNIQ | SOIC-8 | QSPI flash for RP2040 |
| U12 | USBLC6-2SC6 | SOT-23-6 | USB ESD protection |
| Y10 | 12 MHz ABM8-272-T3 | 3225-4 | RP2040 crystal (`RP_XIN`/`RP_XOUT`) |
| J10 | USB micro-B | Molex-105017 | Data/control only (`USB_DM_CONN`, `USB_DP_CONN`, `USB_VBUS_SENSE`, chassis GND) |
| J11 | Cortex SWD 10-pin | 2x05 P1.27mm | `SWD_IO`, `SWD_CLK`, `RP_RUN`, `+3V3_DIG`, GND |
| SW1 | BOOTSEL | TL3342 | Boot select button |
| SW2 | RESET | TL3342 | Reset button |

## 2. FPGA and data converters

| Ref | Value | Footprint | Role on PCB |
|-----|-------|-----------|-------------|
| U20 | LIF-MD6000-6JMG80I | CodeSDR 80-ball | FPGA (note: JMG80I, not UMG64I) |
| U21 | AD9215BCPZ-105 | LFCSP-32 | 10-bit parallel ADC |

ADC bus (pad nets, U21 to U20 balls D10,E9,E10,F9,F10,G9,G10,H9,H10,J10):
`ADC_D0`-`ADC_D9`, `ADC_CLK` (U21.1 to U20.**D9**), `ADC_OR` (U21.21 to U20.K9),
`ADC_PDWN` (driven by U10.34), `ADC_MODE` (strap net, no RP2040 pad).
There is no data-valid net anywhere on the PCB, so gateware must treat each
`ADC_CLK` edge as one sample and use `ADC_OR` only as an overload flag.
(Correction: the sample clock lands on U20.**D9**, not D7 as an earlier revision
of this document stated; the generator `tools/gen_lpf.py` derives the pin map
directly from the PCB and flags exactly this class of mismatch. U20.D7 has no
net.)
`ADC_SENSE`/`ADC_VREF`/`ADC_REFB`/`ADC_REFT`/`ADC_VCM`/`ADC_VIN_P`/`ADC_VIN_N`
are wired per the ADC footprint.

## 3. Clocks

| Ref | Value | Wiring on PCB |
|-----|-------|---------------|
| Y40 | SiT8209 100.000000 MHz | Output `REF_100M_OSC` into U42.1 |
| U42 | CDCLVC1104PWR | Fanout to `REF_100M_ADC`, `REF_100M_ADF`, `REF_100M_LMX` |
| Y30 | 25 MHz ECS-250 | `PHY_XI`/`PHY_XO` at U30 only (PHY crystal, independent of the 100 MHz tree) |
| Y10 | 12 MHz | RP2040 crystal only |

The ADC and both synthesisers share the 100 MHz tree. The PHY keeps its own
25 MHz crystal. `PHY_CLK125_NDO` is a PHY output net (test point, no load).

## 4. Synthesisers

| Ref | Value | Control nets from U10 |
|-----|-------|-----------------------|
| U40 | LMX2592RHAT | `SYNTH_SCK` (U10.6 to U40.16), `SYNTH_SDI` (U10.7 to U40.17), `LMX_CSB` (U10.8 to U40.24), `LMX_CE` (U10.9 to U40.1), `LMX_LOCK` back (U40.20 to U10.38) |
| U41 | ADF4360-1BCPZ | `ADF_CLK` (U10.12 to U41.17), `ADF_DATA` (U10.13 to U41.18), `ADF_LE` (U10.11 to U41.19), `ADF_LOCK` back (U41.20 to U10.39) |

Both parts run off the fanned-out 100 MHz reference (`REF_100M_LMX`,
`REF_100M_ADF`). Bit-bang or SPI-peripheral drivers must use exactly these
nets; polarities, word widths, and lock-polling sequences are datasheet
matters, not PCB matters, and are not defined here.

## 5. RF front end (all nets as placed)

Five SMA inputs, all edge-mount Molex 73251-1153:

| Conn | Net | Lane |
|------|-----|------|
| J50 | `RF_LOW_IN` | Low band |
| J60 | `RF_HA_IN` | High A |
| J61 | `RF_HB_IN` | High B |
| J62 | `RF_HC_IN` | High C |
| J63 | `RF_HD_IN` | High D |

Low lane: `RF_LOW_IN` through FL50 (LFCN-2500+ low-pass) as `RF_LOW_FILT`
into U50 (GRF4001, enable net `LNA_EN_LOW_IC`), then U51 (ADL5801 mixer,
LO `LO_LOW_P`/`LO_LOW_N`, output `IF_LOW_P`/`IF_LOW_N`), then T50 balun to
`IF_LOW_SE`.

High lanes: U60 (GRF4001, `RF_HA_ENABLE`), U61/U62/U66 (GRF2101,
`RF_HB/HC/HD_ENABLE`), plus second cascaded U67 (GRF2101, `RF_HD2_ENABLE`)
on the D lane. All four lanes meet at U64 (QPC6144 switch, select nets
`HSW_V1_CHIP`/`HSW_V2_CHIP`, output `RF_HIGH_SWITCHED`) into U63 (LTC5548
mixer, enable `MIXER_EN_HIGH`, LO `LTC_LO_IN`, output `IF_HIGH_P`/`IF_HIGH_N`)
and T60 (TC1-1-13M+) to `IF_HIGH_SE`.

Enable-name warning for firmware: U10 drives `LNA_EN_LOW` and `LNA_EN_H1`-`H4`,
but the LNA footprints land on `LNA_EN_LOW_IC`, `RF_HA_ENABLE`,
`RF_HB_ENABLE`, `RF_HC_ENABLE`, `RF_HD_ENABLE`, `RF_HD2_ENABLE`. The joins
live off-footprint (series parts); resolve them in the schematic, not by
renaming nets. The same applies to `HSW_V1`/`HSW_V2` (U10.31/32) versus
`HSW_V1_CHIP`/`HSW_V2_CHIP` (U64.17/18).

## 6. IF chain (all nets as placed)

`IF_LOW_SE` / `IF_HIGH_SE` into U70 (BGS12WN6, select `IF_PATH_SEL` U10.30,
output `IF_SELECTED`) through FL70 (QPQ1909, output `IF_BAW_OUT`) into U71
(GRF2013, enable `GRF2013_ENABLE`) and the `IF_DSA_GAINED` node, through U72
(PE4312C-Z DSA, `PE4312_RF1`/`PE4312_RF2`) into U75 (second GRF2013, enable
`GRF2013B_ENABLE`), then U73 (LT5560 second mixer, LO `LO2_P`/`LO2_N`,
output `IF2_P`/`IF2_N`), a discrete L/C ladder (L80-L98, C201-C294 series),
and U74 (AD8351 ADC driver, output `AD8351_OUT_P`/`N` to `ADC_VIN_P`/`N`).

DSA serial port: `DSA_DATA` (U10.27 to U72.3 `DSA_DATA_PIN`), `DSA_CLK`
(U10.28 to U72.4), `DSA_LE` (U10.18 to U72.5); `DSA_C16` (U10 pad to U72.1)
is wired and must be driven per the PE4312 datasheet.

## 7. Ethernet (RGMII only, no GMII on this PCB)

| Ref | Value | Role |
|-----|-------|------|
| U30 | KSZ9031RNXCA QFN-48 | RGMII PHY |
| J30 | JK0654219NL | RJ45 with integrated magnetics (`MDI_A/B/C/D_P/N`, centre taps `MAG_CT1-4`, LED nets) |

MAC-side nets are 4-bit DDR pairs: `RGMII_TXD0-3_FPGA` (balls
E1,E2,F1,F2), `RGMII_TX_EN_FPGA` (J3), `RGMII_TXC_FPGA` (J7),
`RGMII_RXD0-3_FPGA` (K8,J5,K10,K4), `RGMII_RX_DV_FPGA` (K5),
`RGMII_RXC_FPGA` (K6), each joined toward the PHY-side net of the same
base name. Management: `MDC` (U20.J6 to U30.36), `MDIO` (U20.K7 to U30.37),
`PHY_INT_N` (U20.K2 to U30.38). PHY reset `PHY_RESET_N` is driven by U10.37
(sequenced with `PHY_RESET_CT`). There are no GMII error, CRS, or COL nets.
Any 8-bit GMII logic will not fit this PCB; RGMII DDR cells plus a
TX/RX skew strategy and KSZ9031 delay-register setup are required before a
releasable bitstream.

## 8. FPGA configuration and host link

`FPGA_SPI_MOSI` (U10.3 to U20.G1), `FPGA_SPI_MISO` (U10.4 to U20.K1),
`FPGA_SPI_CS_N` (U10.5 to U20.H1), `FPGA_SPI_SCK_MCU` (U10.2) toward
`FPGA_SPI_SCK` (U20.J1), `FPGA_RESET_N` (U10.29 to U20.H2),
`FPGA_CDONE` (U20.J2 to U10.41). RP2040 is the SPI master and the
reset/done monitor. No SPI register map exists in copper; the
`verilog/rp2040_interface.v` address map is an RTL proposal, not hardware
truth, until firmware and gateware agree on it and it is tested on the board.

## 9. Power (input and rails as placed)

J1 is a 2-pin 5 V-only terminal block (`VIN_5V`, GND). There is no USB
power input: USB is data/control only. Protection and storage: F1 (2.5 A
PTC), D1 (SMAJ5.0A), bulk caps C11/C12/C346 (47 uF) and C1-C7 (22 uF).

| Ref | Value | Output net |
|-----|-------|------------|
| U1 | TLV62569DBVR | `SW_3V3` from `VIN_FUSED` |
| U2 | TLV62569DBVR | `SW_1V2` from `VIN_FUSED` |
| U3 | TLV75525PDBVR | `+2V5_AUX` (FPGA aux) |
| U4 | TPS7A8300ARGWT | `+3V3_ANA` (low-noise analog) |
| U5/U6/U7 | TPS389033 supervisors | `CORE_EN`-gated `FPGA_VCCIO0/1/2_3V3` |
| U8 | TPS389012 supervisor | `AUX_EN`-gated `FPGA_VCC_1V2` |
| U9 | TPS389012 supervisor | `PHY_RESET_CT`-sequenced `PHY_AVDDL_1V2` / `PHY_RESET_N` |

Load rails on copper: `+5V_RF` (mixers, IF amps, ADC driver),
`+3V3_DIG` (FPGA I/O, RP2040, flash, PHY I/O), `+1V2_CORE` (FPGA core/PLL
islands, PHY core), `+2V5_AUX` (FPGA aux), `+3V3_ANA` (synths, RF path,
ADC, clocks). Firmware must enable only the selected RF lane; concurrent
operation is a thermal/electrical question for first-article test, not a
coding default.

## 10. What the PCB does not define

Copper gives nets, footprints, and placement. It does not give SPI register
maps, LO frequency plans, decimation or packet formats, sample-rate or
bandwidth claims, host APIs, or pass/fail test results. Those live (or must
be created) in the schematic sources (`hardware/v2/sheets/`,
`hardware/v2/tools/`), the gateware (`verilog/v2_top.v` is a source-level
package top, not a fitted image), future firmware, and measured
first-article data. Anything elsewhere in this repo that contradicts
sections 1-9 is superseded by sections 1-9.
