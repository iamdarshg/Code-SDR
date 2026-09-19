// ============================================================================
// v2_clock_pll - 100 MHz board reference -> 125 MHz Ethernet clock.
//
// The V2 board has exactly one clock tree (SiT8209 100 MHz -> CDCLVC1104 fanout)
// and no 125 MHz source, so RGMII's 125 MHz must come from the CrossLink PLL.
// CrossLink has one general-purpose PLL, which is exactly what this uses.
//
// In simulation a behavioural 125 MHz source is generated so the datapath can be
// verified. In hardware the body must be the Diamond-generated PLL (Clarity
// Designer / EHXPLLL) with the same port names; that primitive is deliberately
// not hand-written here because its exact parameter set must come from the
// vendor tool, not be guessed.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_clock_pll (
    input  wire clk_100m_in,
    input  wire rst_n,
    output wire clk_125m_eth,
    output wire locked
);

`ifdef V2_USE_VENDOR_PLL
    // ---------------------------------------------------------------- hardware
    // CrossLink sysCLOCK PLL. 100 MHz -> 125 MHz (5/4).
    //
    // The primitive below is Lattice's 40 nm sysCLOCK PLL (EHXPLLL), which
    // CrossLink shares with ECP5. Reference: "CrossLink sysCLOCK PLL/DLL Design
    // and Usage Guide", FPGA-TN-02015 - confirm the exact parameter names and the
    // valid CLKI/CLKOP ranges against that guide (or regenerate this block with
    // Clarity Designer) before the first build.
    localparam integer CLKIN_PERIOD = 10;           // 100 MHz input, ns
    // VCO = f_in * CLKFB_DIV / CLKI_DIV with CLKOP feedback, then
    // CLKOP = VCO / CLKOP_DIV. For 100 -> 125 MHz that needs VCO = 500 MHz:
    //   CLKFB_DIV = 5  -> VCO = 500 MHz
    //   CLKOP_DIV = 4  -> CLKOP = 125 MHz
    // CLKFB_DIV must NOT be tied to CLKOP_DIV: doing that gives VCO = 400 MHz
    // and CLKOP = 100 MHz, i.e. an Ethernet MAC clocked at the ADC rate.
    localparam integer CLKFB_DIV    = 5;            // VCO = 100 * 5 / 1 = 500 MHz
    localparam integer CLKOP_DIV    = 4;            // 500 MHz VCO / 4 = 125 MHz

    EHXPLLL #(
        .CLKI_DIV        (1),
        .CLKFB_DIV       (CLKFB_DIV),
        .CLKOP_DIV       (CLKOP_DIV),
        .FEEDBK_PATH     ("CLKOP"),
        .CLKOP_CPHASE    (0),
        .CLKOP_FPHASE    (0),
        .OUTDIVIDER_MUXA ("DIVA"),
        .OUTDIVIDER_MUXB ("DIVB"),
        .OUTDIVIDER_MUXC ("DIVC"),
        .OUTDIVIDER_MUXD ("DIVD"),
        .PLL_LOCK_MODE   (0),
        .STDBY_ENABLE    ("FALSE"),
        .REFIN_RESET     ("FALSE"),
        .SYNCI_RESET     ("FALSE"),
        .INTFB_WAKE      ("FALSE")
    ) u_pll (
        .CLKI    (clk_100m_in),
        .CLKFB   (clk_125m_eth),       // CLKOP feedback
        .RST     (~rst_n),
        .RSTK    (1'b0),
        .STDBY   (1'b0),
        .PHASESEL0(1'b0), .PHASESEL1(1'b0),
        .PHASEDIR(1'b0),  .PHASESTEP(1'b0), .PHASELOADREG(1'b0),
        .PLLWAKESYNC(1'b0),
        .ENCLKOP (1'b1), .ENCLKOS(1'b0), .ENCLKOS2(1'b0), .ENCLKOS3(1'b0),
        .CLKOP   (clk_125m_eth),
        .CLKOS   (),
        .CLKOS2  (),
        .CLKOS3  (),
        .LOCK    (locked),
        .INTLOCK ()
    );
`else
    // ------------------------------------------------------------- simulation
    reg clk_r = 1'b0;
    always #4 clk_r = ~clk_r;                 // 125 MHz

    reg [4:0] lock_cnt;
    always @(posedge clk_r or negedge rst_n) begin
        if (!rst_n) lock_cnt <= 5'd0;
        else if (lock_cnt != 5'd31) lock_cnt <= lock_cnt + 5'd1;
    end

    assign clk_125m_eth = clk_r;
    assign locked       = (lock_cnt == 5'd31);
`endif

endmodule

`default_nettype wire
