// ============================================================================
// v2_telemetry - read-only telemetry register block for the RP2040 dashboard.
//
// Aggregates the datapath counters into a byte-addressed 32-bit read port that
// a SPI slave (v2_spi_regs) exposes to the RP2040. The RP2040 turns these into
// the user-visible dashboard: sample rate, bit depth, required Mbps, headroom,
// and dropped-word rate.
//
// Register map (byte address, 32-bit little-endian reads):
//   0x00  packets_sent
//   0x04  dropped_words      (samples lost because the link could not drain)
//   0x08  overflow_seen      (sticky: any drop has ever occurred)
//   0x0C  status             bit0 link_up, bit1 pll_locked,
//                            bits[3:2] mode, bit4 sticky_overflow,
//                            bits[11:5] sample_bits[6:0], bits[19:12] decim
//   0x10  effective Mbps     sample rate x bit depth (as computed in RTL)
//   0x14  sequence           packet sequence / frame counter
//
// All inputs are expected in the telemetry clock domain; counters that origin-
// ate elsewhere are synchronized by their source (dropped_words is gray-coded
// in v2_raw_path before crossing).
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_telemetry (
    input  wire        clk,
    input  wire        rst_n,

    // live counters / configuration
    input  wire [31:0] packets_sent,
    input  wire [31:0] dropped_words,
    input  wire [31:0] overflow_count,
    input  wire [31:0] seq_value,
    input  wire [7:0]  sample_bits,
    input  wire [7:0]  decim,
    input  wire [1:0]  mode,
    input  wire        link_up,
    input  wire        pll_locked,
    input  wire [15:0] effective_mbps,

    input  wire [7:0]  rd_addr,
    output reg  [31:0] rd_data
);

    reg sticky_overflow;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) sticky_overflow <= 1'b0;
        else if (overflow_count != 32'd0) sticky_overflow <= 1'b1;
    end

    always @* begin
        case (rd_addr[7:2])
            6'h00:   rd_data = packets_sent;
            6'h01:   rd_data = dropped_words;
            6'h02:   rd_data = {31'd0, sticky_overflow};
            6'h03:   rd_data = {12'd0, decim[7:0], sample_bits[6:0],
                                sticky_overflow, mode, pll_locked, link_up};
            6'h04:   rd_data = {16'd0, effective_mbps};
            default: rd_data = seq_value;
        endcase
    end

endmodule

`default_nettype wire
