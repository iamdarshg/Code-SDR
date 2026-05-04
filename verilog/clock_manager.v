// ============================================================================
// Clock Manager Module
// ============================================================================
// Clock distribution and PLL targets for LIF-MD6000-6UMG64I FPGA
// Configured for 105 MHz (ADC/FFT) and 125 MHz (Ethernet) domains.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module clock_manager (
    input  wire clk_100m_in,     // Primary 100 MHz input clock
    input  wire rst_n,          // System reset (active low)

    // Clock outputs
    output wire clk_600m,       // Placeholder for high-speed serial
    output wire clk_1200m_fft,  // Placeholder for core internal
    output wire clk_125m_eth,   // 125 MHz Ethernet clock
    output wire clk_250m_eth,   // 250 MHz Ethernet clock
    output wire clk_105m_adc,   // 105 MHz ADC/FFT clock

    // Reset and Status
    output wire reset_n,        // Synchronized reset (active low)
    output wire locked          // PLL lock status (active high)
);

    wire clk_105m_int;
    wire clk_125m_int;
    wire clk_250m_int;
    wire clk_locked_int;

`ifdef SIMULATION
    // Behavioral Clock Generation for Simulation
    reg r_clk_125m_eth = 0;
    reg r_clk_105m_adc = 0;
    reg r_clk_250m_eth = 0;

    always #4.000 r_clk_125m_eth = ~r_clk_125m_eth; // 125 MHz
    always #4.762 r_clk_105m_adc = ~r_clk_105m_adc; // 105 MHz
    always #2.000 r_clk_250m_eth = ~r_clk_250m_eth; // 250 MHz

    assign clk_105m_int = r_clk_105m_adc;
    assign clk_125m_int = r_clk_125m_eth;
    assign clk_250m_int = r_clk_250m_eth;
    assign clk_locked_int = 1'b1;
`else
    // ========================================================================
    // Lattice CrossLink-NX (LIF-MD6000) PLL Configuration
    // ========================================================================
    // To implement in hardware, use the Radiant IP Catalog to generate:
    // 1. pll_sys_105: 100MHz In -> 105MHz Out (DIVR=19, DIVF=20, DIVQ=4 for example)
    // 2. pll_eth_125: 100MHz In -> 125MHz Out (DIVR=3, DIVF=4, DIVQ=4 for example)
    // ========================================================================

    // For elaboration cleanliness during source-only synthesis:
    assign clk_105m_int = clk_100m_in;
    assign clk_125m_int = clk_100m_in;
    assign clk_250m_int = clk_100m_in;
    assign clk_locked_int = 1'b1;
`endif

    assign clk_105m_adc   = clk_105m_int;
    assign clk_125m_eth   = clk_125m_int;
    assign clk_250m_eth   = clk_250m_int;
    assign clk_600m       = clk_105m_int;
    assign clk_1200m_fft  = clk_105m_int;

    // Reset Synchronizer
    reg [2:0] reset_sync;
    always @(posedge clk_100m_in or negedge rst_n) begin
        if (!rst_n) reset_sync <= 3'b000;
        else reset_sync <= {reset_sync[1:0], 1'b1};
    end

    assign reset_n = reset_sync[2] && clk_locked_int;
    assign locked = clk_locked_int;

endmodule
