`default_nettype none
// ============================================================================
// LIF-MD6000 Clock PLL Wrapper
// ============================================================================
// Replace this wrapper body with the vendor-generated Radiant PLL instance for
// hardware builds. The synthesis scripts define USE_LIFMD6000_PLL and include
// this file so the clock-manager dependency is explicit in the committed flow.
// ============================================================================

`timescale 1ns/1ps

(* syn_black_box = 1, black_box = 1 *)
module lifmd6000_clock_pll (
    input  wire clk_in,
    input  wire reset_n,
    output wire clk_105m_adc,
    output wire clk_125m_eth,
    output wire locked
);

endmodule
`default_nettype wire
