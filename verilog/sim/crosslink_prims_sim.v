// ============================================================================
// crosslink_prims_sim.v - SIMULATION-ONLY behavioural models of the Lattice
// CrossLink I/O primitives used by v2_rgmii when V2_USE_VENDOR_DDR is defined.
//
// These models exist so the hardware DDR path can be simulated. They are NOT
// synthesisable targets and must NOT be added to the Diamond project - Diamond
// provides the real primitives. Include this file in testbenches only.
//
// Model semantics follow the Lattice sysI/O convention:
//   ODDRX1F : D0 on the rising edge of SCLK, D1 on the falling edge
//   IDDRX1F : Q0 captured on the rising edge, Q1 on the falling edge
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module ODDRX1F (
    input  wire D0,
    input  wire D1,
    input  wire SCLK,
    input  wire RST,
    output reg  Q
);
    always @(posedge SCLK or posedge RST)
        if (RST) Q <= 1'b0;
        else     Q <= D0;

    always @(negedge SCLK or posedge RST)
        if (RST) Q <= 1'b0;
        else     Q <= D1;
endmodule

module IDDRX1F (
    input  wire D,
    input  wire SCLK,
    input  wire RST,
    output reg  Q0,
    output reg  Q1
);
    always @(posedge SCLK or posedge RST)
        if (RST) Q0 <= 1'b0;
        else     Q0 <= D;

    always @(negedge SCLK or posedge RST)
        if (RST) Q1 <= 1'b0;
        else     Q1 <= D;
endmodule

`default_nettype wire
