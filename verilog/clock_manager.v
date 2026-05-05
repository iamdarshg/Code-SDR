// ============================================================================
// Clock Manager Module
// ============================================================================
// Hardware clock plan for issue #12:
//   - clk_105m_adc: ADC, DDC, windowing, and FFT receive path.
//   - clk_125m_eth: GMII/UDP/Ethernet domain.
//
// Hardware builds must define USE_LIFMD6000_PLL after generating the
// board-specific lifmd6000_clock_pll wrapper. Without that define this module
// is an explicit simulation/source-only stub and does not claim hardware timing.
// ============================================================================

`timescale 1ns/1ps

module clock_manager (
    input  wire clk_100m_in,
    input  wire rst_n,

    output wire clk_600m,
    output wire clk_1200m_fft,
    output wire clk_125m_eth,
    output wire clk_250m_eth,
    output wire clk_105m_adc,

    output wire reset_n,
    output wire locked
);

`ifdef USE_LIFMD6000_PLL
    wire pll_clk_105m_adc;
    wire pll_clk_125m_eth;
    wire pll_locked;

    lifmd6000_clock_pll u_lifmd6000_clock_pll (
        .clk_in(clk_100m_in),
        .reset_n(rst_n),
        .clk_105m_adc(pll_clk_105m_adc),
        .clk_125m_eth(pll_clk_125m_eth),
        .locked(pll_locked)
    );

    assign clk_105m_adc = pll_clk_105m_adc;
    assign clk_125m_eth = pll_clk_125m_eth;
    assign locked = pll_locked;
`elsif SIMULATION
    assign clk_105m_adc = clk_100m_in;
    assign clk_125m_eth = clk_100m_in;
    assign locked = rst_n;
`else
    initial begin
        $error("Define USE_LIFMD6000_PLL for synthesis or SIMULATION for behavioral clock pass-through");
    end
    assign clk_105m_adc = clk_100m_in;
    assign clk_125m_eth = clk_100m_in;
    assign locked = 1'b0;
`endif

    // Legacy outputs kept for older modules; issue #12 does not use them.
    assign clk_600m = clk_105m_adc;
    assign clk_1200m_fft = clk_105m_adc;
    assign clk_250m_eth = clk_125m_eth;
    assign reset_n = rst_n & locked;

endmodule
