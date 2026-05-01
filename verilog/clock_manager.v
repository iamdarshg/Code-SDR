// ============================================================================
// Clock Manager Module
// ============================================================================
// Clock distribution and PLL instantiation for LIF-MD6000-6UMG64I FPGA
// ============================================================================

`timescale 1ns/1ps

module clock_manager (
    input  wire clk_100m_in,     // Primary 100 MHz input clock
    input  wire rst_n,          // System reset (active low)

    // Clock outputs
    output wire clk_600m,       // 600 MHz clock for DDC and windowing
    output wire clk_1200m_fft,  // 1200 MHz clock for FFT processor
    output wire clk_125m_eth,   // 125 MHz Ethernet clock (standard)
    output wire clk_250m_eth,   // 250 MHz Ethernet clock (boosted)
    output wire clk_105m_adc,   // 105 MHz ADC clock

    // Reset and Status
    output wire reset_n,        // Synchronized reset (active low)
    output wire locked          // PLL lock status (active high)
);

    // ========================================================================
    // Behavioral Clock Generation for Simulation
    // ========================================================================

    // In actual hardware, this would be a PLL primitive (e.g. ALTPLL, ClockWizard)
    // For simulation, we generate clocks using simple ratios or delays.

    reg r_clk_600m = 0;
    reg r_clk_1200m_fft = 0;
    reg r_clk_125m_eth = 0;
    reg r_clk_250m_eth = 0;
    reg r_clk_105m_adc = 0;

    // Simulation-only clock generators
`ifdef SIMULATION
    // 600 MHz (1.666 ns period)
    always #0.833 r_clk_600m = ~r_clk_600m;

    // 1200 MHz (0.833 ns period)
    always #0.416 r_clk_1200m_fft = ~r_clk_1200m_fft;

    // 125 MHz (8 ns period)
    always #4.0 r_clk_125m_eth = ~r_clk_125m_eth;
    
    // 250 MHz (4 ns period)
    always #2.0 r_clk_250m_eth = ~r_clk_250m_eth;

    // 105 MHz (9.524 ns period)
    always #4.762 r_clk_105m_adc = ~r_clk_105m_adc;
`else
    // For synthesis, we pass through the input clock to allow bitstream generation
    // though actual PLL parameters must be set in the FPGA project.
    assign clk_600m       = clk_100m_in;
    assign clk_1200m_fft  = clk_100m_in;
    assign clk_125m_eth   = clk_100m_in;
    assign clk_250m_eth   = clk_100m_in;
    assign clk_105m_adc   = clk_100m_in;
`endif

    // Map registers to outputs in simulation
`ifdef SIMULATION
    assign clk_600m       = r_clk_600m;
    assign clk_1200m_fft  = r_clk_1200m_fft;
    assign clk_125m_eth   = r_clk_125m_eth;
    assign clk_250m_eth   = r_clk_250m_eth;
    assign clk_105m_adc   = r_clk_105m_adc;
`endif

    // ========================================================================
    // Reset Synchronization
    // ========================================================================

    reg [2:0] reset_sync_reg;

    always @(posedge clk_100m_in or negedge rst_n) begin
        if (!rst_n) begin
            reset_sync_reg <= 3'b000;
        end else begin
            reset_sync_reg <= {reset_sync_reg[1:0], 1'b1};
        end
    end

    assign reset_n = reset_sync_reg[2];
    assign locked = reset_sync_reg[2]; // Locked when reset sync is complete

endmodule
