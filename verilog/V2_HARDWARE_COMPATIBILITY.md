# Code-SDR V2 RTL/hardware compatibility audit

Status: **not yet a releasable V2 FPGA image**.

The PCB uses a Lattice CrossLink `LIF-MD6000-6UMG64I`, a 100 MSPS parallel
AD9215 interface, and 4-bit DDR RGMII. The historical
`fpga_processing_pipeline` is a processing-core/test top, not a package-pin top
for this PCB. It must not be passed to a vendor flow as though it matched V2.

## Confirmed package-pin contract

| Schematic net | Ball | Direction at FPGA | Function |
|---|---|---|---|
| ADC_CLK | D9 | input | 100 MHz ADC/sample clock |
| ADC_D0..ADC_D9 | D10, E9, E10, F9, F10, G9, G10, H9, H10, J10 | input | parallel ADC data |
| ADC_OR | K9 | input | ADC over-range |
| FPGA_SPI_MOSI | G1 | input | RP2040 to FPGA |
| FPGA_SPI_CS_N | H1 | input | RP2040 to FPGA |
| FPGA_SPI_SCK | J1 | input | RP2040 to FPGA |
| FPGA_SPI_MISO | K1 | output | FPGA to RP2040 |
| FPGA_RESET_N | H2 | input | RP2040 reset control |
| RGMII_TXD0..3_FPGA | E1, E2, F1, F2 | output DDR | PHY transmit data |
| RGMII_TX_EN_FPGA | J3 | output DDR | PHY TX control |
| RGMII_TXC_FPGA | J7 | output | PHY transmit clock |
| RGMII_RXD0..3_FPGA | K8, J5, K10, K4 | input DDR | PHY receive data |
| RGMII_RX_DV_FPGA | K5 | input DDR | PHY RX control |
| RGMII_RXC_FPGA | K6 | input | PHY receive clock |
| MDC | J6 | output | PHY management clock |
| MDIO | K7 | bidirectional | PHY management data |
| PHY_INT_N | K2 | input | PHY interrupt |

`FPGA_CDONE` on J2 is the configuration-done function and is monitored by the
RP2040; it is not an arbitrary application output.

## Blocking mismatches in the historical top

- It exposes 8-bit SDR GMII data and GMII error pins. V2 physically has 4-bit
  DDR RGMII and no GMII error pins.
- It has no RGMII DDR input/output cells and no PHY clock-delay/skew contract.
- It omits MDC, bidirectional MDIO, and PHY interrupt handling.
- It expects `adc_valid`, but AD9215/V2 has no such pin; samples are valid each
  ADC clock unless reset/power-down policy says otherwise.
- It exposes status and mode buses that have no V2 package connections.
- The committed `lifmd6000_clock_pll` remains a black-box placeholder; no
  generated Radiant PLL implementation is present.
- The old `.xdc`, Vivado-style project Tcl, and Quartus/Cyclone-V Tcl are not a
  Lattice CrossLink implementation flow and contain no trustworthy V2 package
  constraints.
- No vendor timing report, fitted pin report, utilization report, or bitstream
  tied to this schematic revision exists.

## Work completed in this release branch

The processing-domain contract and host defaults were corrected from 105 MSPS
to 100 MSPS. The ADC domain now uses the 100 MHz board input directly; only the
125 MHz Ethernet domain calls for generated clocking. These source corrections
do not by themselves close the RGMII/package implementation gate.

## Required evidence before compatibility can pass

1. Add a V2 package top with exactly the schematic ports above and no phantom
   package I/O.
2. Implement RGMII DDR I/O with CrossLink-supported primitives and a documented
   TX/RX clock-skew strategy consistent with KSZ9031 register configuration.
3. Implement and test MDIO/MDC management, including PHY identification,
   reset, RGMII delay/skew setup, link status, and interrupt handling.
4. Tie ADC sample-valid behavior to the actual AD9215 interface and verify the
   package pin assignment against the schematic.
5. Generate the 125 MHz clock IP in the exact Lattice vendor flow and commit
   reproducible project/constraint sources allowed by the tool license.
6. Run HDL tests and lint, then Lattice synthesis/place/route. Retain the fitted
   pin report, unconstrained-path check, setup/hold summary, clock report,
   utilization, tool/version log, and bitstream SHA256.

Until all six items pass, the repository must not claim V2 RTL/firmware pinout
compatibility or a releasable FPGA image.
