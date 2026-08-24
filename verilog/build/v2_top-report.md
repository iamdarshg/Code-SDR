# v2_top implementation report

## Status

DONE for the requested two-file RTL/source deliverable.

This explicitly means source-level completion only. No synthesis, place/route,
timing closure, hardware bring-up, or release claim is made. iverilog and yosys
were not found in PATH; therefore no Verilog parse/lint or synthesis success can
be claimed.

## Files created

- `verilog/v2_top.v`
- `verilog/build/v2_top-report.md`

No existing repository file was intentionally modified. The untracked `.superpowers/`
and `hardware/v2/tools/audit_opens.py` entries present before this task were left
untouched.

## Package/port contract implemented

All requested physical connections are represented by ordered HDL buses or
scalar ports, with one bus/scalar bit per supplied net/ball and inline comments:

| Net | Ball | Direction |
|---|---|---|
| ADC_CLK / clk_100m_in | D9 | input |
| ADC_D0..ADC_D9 | D10,E9,E10,F9,F10,G9,G10,H9,H10,J10 | input bus [9:0] |
| ADC_OR | K9 | input |
| FPGA_SPI_MOSI | G1 | input |
| FPGA_SPI_CS_N | H1 | input |
| FPGA_SPI_SCK | J1 | input |
| FPGA_SPI_MISO | K1 | output |
| FPGA_RESET_N | H2 | input |
| RGMII_TXD0..TXD3_FPGA | E1,E2,F1,F2 | output bus [3:0] |
| RGMII_TX_EN_FPGA | J3 | output |
| RGMII_TXC_FPGA | J7 | output |
| RGMII_RXD0..RXD3_FPGA | K8,J5,K10,K4 | input bus [3:0] |
| RGMII_RX_DV_FPGA | K5 | input |
| RGMII_RXC_FPGA | K6 | input |
| MDC | J6 | output |
| MDIO | K7 | inout |
| PHY_INT_N | K2 | input |

`FPGA_CDONE` on ball J2 is deliberately absent because it was outside the supplied
contract and is a configuration-done function, not application I/O.

A mechanical audit counted 32 FPGA direction bits across the 17 top-level HDL
ports: 12 ADC/reference/reset/data bits, four SPI bits, 12 RGMII bits, and three
management/interrupt bits. Grouping related nets into Verilog vectors (rather than
creating ADC_D0..D9/RGMII_TXD0..3/RGMII_RXD0..3 scalar ports) is the recorded
simplest-interface interpretation; every bit-to-ball relationship remains documented.

## Instantiated existing modules

- `clock_manager`
- `adc_interface`, with `adc_valid` tied to `1'b1`
- `rp2040_interface`

The historical processing pipeline was not instantiated: it exposes phantom status,
GMII CRS/COL/clocks, and other non-package signals, so including it would violate the
exact-I/O requirement.

## RGMII interpretation

`rgmii_phy_adapter` captures RX nibbles at opposite edges of `RGMII_RXC_FPGA`,
combines them into one byte per full receive-clock period, and exposes a GMII-style
data/valid pair. TX divides the assumed 125 MHz GMII-style clock by two for
`RGMII_TXC_FPGA`; data/control are behaviorally driven from both source-clock edges.
Explicit TODOs require CrossLink-supported DDR primitives, target constraints, and a
documented KSZ9031/LIF-MD6000 internal/board skew split before hardware use.

## Documented assumptions and simplest interpretations

1. `clk_100m_in` represents the same physical board reference as `ADC_CLK`. This is
   the minimal way to expose the required single board reference while reusing
   `clock_manager`; vendor constraints must confirm it.
2. `reset_n = rst_n & locked` follows the instantiated clock manager's qualified-reset
   convention. A dedicated CrossLink reset synchronizer remains TODO.
3. ADC samples are valid every ADC clock because AD9215 has no valid pin; overflow is
   tracked through real `ADC_OR`.
4. RP2040 control outputs are retained but unused rather than dropped or routed to
   nonexistent package pins. Status currently reports raw PHY interrupt, overflow,
   sample-valid, and PLL lock state.
5. RX low/high byte-nibble order assumes rising-edge sample first, followed by the
   preceding falling-edge sample. Validate against KSZ9031 RGMII bit ordering.
6. MDC is held low and MDIO released because implementing an unsafe management FSM
   would be worse than explicitly leaving management TODO. Link status defaults low.
7. Internal GMII bytes are not connected to `ethernet_mac` yet because no proven CDC
   into the generated 125 MHz Ethernet domain exists here.
8. Related package nets are grouped into natural ADC/SPI/RGMII HDL buses instead of
   individual scalar ports. This preserves all 32 physical directions across 17 HDL
   ports while avoiding a wider, error-prone top-level port list.

## TODOs before hardware/release

- Add Lattice Radiant/CrossLink DDR input/output primitives and package constraints.
- Define and constrain the complete RGMII TX/RX delay/skew strategy against actual
  KSZ9031 configuration and board delays.
- Implement MDIO/MDC management, PHY ID/reset/delay programming, link/interrupt
  handling, and a properly divided MDC.
- Add a proven RX-to-125 MHz CDC/FIFO and connect a package-safe MAC datapath.
- Connect RP2040 controls to the selected processing datapath without phantom I/O.
- Generate/commit reproducible CrossLink PLL/project sources and run lint plus vendor
  synthesis/place/route/timing/utilization/pin checks before any compatibility claim.
