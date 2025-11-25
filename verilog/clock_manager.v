// ============================================================================
// Clock Manager Module for LIF-MD6000-6UMG64I Cyclone V
// ============================================================================
// High-performance clock management with PLLs for maximum frequency operation
// ============================================================================

`timescale 1ns/1ps

module clock_manager (
    input  wire        clk_100m_in,     // External 100 MHz reference clock
    input  wire        rst_n,           // System reset (active low)
    output wire        clk_600m,        // 600 MHz high-speed processing clock
    output wire        clk_1200m_fft,   // 1.2 GHz FFT dedicated clock
    output wire        clk_250m_eth,    // 250 MHz Ethernet/UDP clock
    output wire        clk_105m_adc,    // 105 MHz ADC sampling clock (PLL generated)
    output wire        reset_n,         // System reset (active low)
    output wire        locked           // PLL lock status
);

    // ========================================================================
    // PLL Instantiations (Cyclone V altpll)
    // ========================================================================

    // Primary high-speed PLL: Generate 600MHz from 100MHz input
    wire clk_600m_raw;
    wire pll1_locked;

    system_pll pll_system (
        .areset(~rst_n),
        .inclk0(clk_100m_in),
        .c0(clk_600m_raw),     // 600 MHz (6x multiplier)
        .locked(pll1_locked)
    );

    // FFT high-frequency PLL: Generate 1.2GHz from 100MHz
    wire clk_1200m_raw;
    wire pll2_locked;

    fft_pll pll_fft (
        .areset(~rst_n),
        .inclk0(clk_100m_in),
        .c0(clk_1200m_raw),    // 1.2 GHz (12x multiplier)
        .locked(pll2_locked)
    );

    // Ethernet PLL: Generate 250MHz from 100MHz
    wire clk_250m_raw;
    wire pll3_locked;

    ethernet_pll pll_ethernet (
        .areset(~rst_n),
        .inclk0(clk_100m_in),
        .c0(clk_250m_raw),     // 250 MHz (2.5x multiplier)
        .locked(pll3_locked)
    );

    // ADC PLL: Generate 105MHz from 100MHz
    wire clk_105m_raw;
    wire pll4_locked;

    adc_pll pll_adc (
        .areset(~rst_n),
        .inclk0(clk_100m_in),
        .c0(clk_105m_raw),     // 105 MHz (1.05x, approximated as needed)
        .locked(pll4_locked)
    );

    // ========================================================================
    // Global Clock Networks (Cyclone V Global Clock Buffers)
    // ========================================================================

    // High-performance processing clock (600MHz)
    wire clk_600m_buffered;
    altclkctrl clkctrl_600m (
        .ena(1'b1),
        .inclk({2'b00, clk_600m_raw, clk_600m_raw, clk_600m_raw}),
        .outclk(clk_600m_buffered)
    );
    assign clk_600m = clk_600m_buffered;

    // FFT dedicated clock (1.2GHz) - Use dedicated high-speed clock network
    wire clk_1200m_buffered;
    altclkctrl clkctrl_1200m (
        .ena(pll2_locked),
        .inclk({2'b00, clk_1200m_raw, clk_1200m_raw, clk_1200m_raw}),
        .outclk(clk_1200m_buffered)
    );
    assign clk_1200m_fft = clk_1200m_buffered;

    // Ethernet clock (250MHz)
    wire clk_250m_buffered;
    altclkctrl clkctrl_250m (
        .ena(pll3_locked),
        .inclk({2'b00, clk_250m_raw, clk_250m_raw, clk_250m_raw}),
        .outclk(clk_250m_buffered)
    );
    assign clk_250m_eth = clk_250m_buffered;

    // ADC clock (105MHz)
    wire clk_105m_buffered;
    altclkctrl clkctrl_105m (
        .ena(pll4_locked),
        .inclk({2'b00, clk_105m_raw, clk_105m_raw, clk_105m_raw}),
        .outclk(clk_105m_buffered)
    );
    assign clk_105m_adc = clk_105m_buffered;

    // ========================================================================
    // PLL Lock Status and Master Reset Logic
    // ========================================================================

    // Master PLL lock (all PLLs must be locked)
    wire all_plls_locked = pll1_locked & pll2_locked & pll3_locked & pll4_locked;
    assign locked = all_plls_locked;

    // ========================================================================
    // Reset Synchronization across all domains
    // ========================================================================

    // Synchronize reset with each clock domain
    reg [3:0] reset_sync_600m;
    reg [3:0] reset_sync_1200m;
    reg [3:0] reset_sync_250m;
    reg [3:0] reset_sync_105m;

    //
    // 600MHz domain reset sync
    //
    always @(posedge clk_600m or negedge rst_n) begin
        if (!rst_n || !all_plls_locked) begin
            reset_sync_600m <= 4'b0000;
        end else begin
            reset_sync_600m <= {reset_sync_600m[2:0], 1'b1};
        end
    end

    //
    // 1.2GHz domain reset sync
    //
    always @(posedge clk_1200m_fft or negedge rst_n) begin
        if (!rst_n || !all_plls_locked) begin
            reset_sync_1200m <= 4'b0000;
        end else begin
            reset_sync_1200m <= {reset_sync_1200m[2:0], 1'b1};
        end
    end

    //
    // 250MHz domain reset sync
    //
    always @(posedge clk_250m_eth or negedge rst_n) begin
        if (!rst_n || !all_plls_locked) begin
            reset_sync_250m <= 4'b0000;
        end else begin
            reset_sync_250m <= {reset_sync_250m[2:0], 1'b1};
        end
    end

    //
    // 105MHz domain reset sync
    //
    always @(posedge clk_105m_adc or negedge rst_n) begin
        if (!rst_n || !all_plls_locked) begin
            reset_sync_105m <= 4'b0000;
        end else begin
            reset_sync_105m <= {reset_sync_105m[2:0], 1'b1};
        end
    end

    // Master reset output (uses 600MHz domain as reference)
    assign reset_n = reset_sync_600m[3];

endmodule
