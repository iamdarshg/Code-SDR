// ============================================================================
// v2_rgmii - GMII (8-bit, single edge) <-> RGMII (4-bit, DDR) conversion.
//
// The KSZ9031 on this board is RGMII-only (HARDWARE.md section 7); there are no
// GMII error/CRS/COL nets, so the internal side is a plain 8-bit byte interface
// clocked at 125 MHz (exactly 1000 Mbps).
//
// RGMII timing: RXD/TXD carry bits [3:0] on the rising edge and [7:4] on the
// falling edge; control carries the valid signal on both edges. Clock/data skew
// is handled by the KSZ9031's internal delay registers (programmed by
// v2_phy_manager), so this module adds no deliberate skew.
//
// The behavioural DDR processes below express intent for simulation. Production
// must use the LIF-MD6000 DDR input/output I/O cells plus setup/hold and
// output-delay constraints; that substitution is the one hardware TODO here.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_rgmii (
    input  wire       rst_n,

    // RGMII pins to the KSZ9031
    input  wire       rgmii_rxc,
    input  wire [3:0] rgmii_rxd,
    input  wire       rgmii_rx_ctl,
    output wire       rgmii_txc,
    output wire [3:0] rgmii_txd,
    output wire       rgmii_tx_ctl,

    // Internal GMII-style byte interface
    input  wire       clk_tx,          // 125 MHz from the PLL
    input  wire [7:0] gmii_tx_d,
    input  wire       gmii_tx_en,
    output reg  [7:0] gmii_rx_d,
    output reg        gmii_rx_dv
);

    // ------------------------------------------------------------------ TX DDR
    // Latch the byte ONCE per cycle and drive both nibbles from that single
    // register. Sampling gmii_tx_d directly on each edge would read the old byte
    // on the rising edge and the new byte on the falling edge (the source
    // updates on the same clock), splitting every byte across two frames - which
    // the loopback test caught.
    reg [7:0] tx_byte;
    reg       tx_en_r;

    always @(posedge clk_tx or negedge rst_n)
        if (!rst_n) begin tx_byte <= 8'h00; tx_en_r <= 1'b0; end
        else        begin tx_byte <= gmii_tx_d; tx_en_r <= gmii_tx_en; end

    // rising edge -> bits [3:0], falling edge -> bits [7:4], same byte.
    // TODO(hw): replace this behavioural mux with the LIF-MD6000 DDR output cell
    // and add the target's output-delay constraints.
    assign rgmii_txd    = clk_tx ? tx_byte[3:0] : tx_byte[7:4];
    assign rgmii_tx_ctl = tx_en_r;
    assign rgmii_txc    = clk_tx;

    // ------------------------------------------------------------------ RX DDR
    // Low nibble is present on the rising edge, high nibble on the falling edge.
    // The byte must therefore be assembled on the FALLING edge: pairing the high
    // nibble sampled now with the low nibble sampled at the preceding rising
    // edge. Assembling on the rising edge would mix in the previous byte's high
    // nibble (caught by the loopback test).
    reg [3:0] rx_lo;
    reg       ctl_lo;
    reg [3:0] rx_hi;

    always @(posedge rgmii_rxc or negedge rst_n)
        if (!rst_n) begin rx_lo <= 4'h0; ctl_lo <= 1'b0; end
        else        begin rx_lo <= rgmii_rxd; ctl_lo <= rgmii_rx_ctl; end

    always @(negedge rgmii_rxc or negedge rst_n)
        if (!rst_n) begin
            rx_hi     <= 4'h0;
            gmii_rx_d <= 8'h00;
            gmii_rx_dv<= 1'b0;
        end else begin
            rx_hi     <= rgmii_rxd;
            gmii_rx_d <= {rgmii_rxd, rx_lo};
            gmii_rx_dv<= ctl_lo & rgmii_rx_ctl;
        end

endmodule

`default_nettype wire
