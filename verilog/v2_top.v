// ============================================================================
// Code-SDR V2 package-pin RTL top (source-level implementation only)
// Target: LIF-MD6000-6UMG64I + AD9215 + RP2040 SPI + KSZ9031 RGMII PHY.
//
// This module exposes exactly the schematic/package nets in the release task.
// It does not instantiate the historical processing pipeline because that
// hierarchy expects phantom status/GMII I/O and would violate the exact
// package-I/O requirement. rgmii_phy_adapter is a source-level DDR conversion;
// vendor I/O cells and measured skew strategy are explicit hardware TODOs.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_top (
    // Board reference/sample clock. Schematic net ADC_CLK -> ball D9.
    input  wire        clk_100m_in,
    // Global active-low reset. Schematic net FPGA_RESET_N -> ball H2.
    input  wire        rst_n,

    // AD9215 parallel sample bus. ADC_D0..ADC_D9 ->
    // D10,E9,E10,F9,F10,G9,G10,H9,H10,J10.
    input  wire [9:0]  adc_data,
    // AD9215 over-range flag. Schematic net ADC_OR -> ball K9.
    input  wire        adc_ovr,

    // RP2040 slave SPI configuration/status port.
    input  wire        spi_mosi,       // FPGA_SPI_MOSI -> G1
    input  wire        spi_cs_n,       // FPGA_SPI_CS_N -> H1
    input  wire        spi_clk,        // FPGA_SPI_SCK  -> J1
    output wire        spi_miso,       // FPGA_SPI_MISO -> K1

    // KSZ9031 RGMII media-independent interface.
    output wire [3:0]  rgmii_txd,      // TXD0..TXD3 -> E1,E2,F1,F2
    output wire        rgmii_tx_en,    // RGMII_TX_EN_FPGA -> J3
    output wire        rgmii_txc,      // RGMII_TXC_FPGA -> J7
    input  wire [3:0]  rgmii_rxd,      // RXD0..RXD3 -> K8,J5,K10,K4
    input  wire        rgmii_rx_dv,    // RGMII_RX_DV_FPGA -> K5
    input  wire        rgmii_rxc,      // RGMII_RXC_FPGA -> K6

    // KSZ9031 serial management interface and interrupt.
    output wire        mdc,            // MDC -> J6
    inout  wire        mdio,           // MDIO -> K7
    input  wire        phy_int_n       // PHY_INT_N -> K2
);

    // Clock/reset assumptions:
    // clk_100m_in is assumed to be the same physical board reference as ADC_CLK
    // unless constraints prove otherwise. clock_manager keeps the ADC domain on
    // this direct reference and generates the Ethernet domain when its vendor PLL
    // wrapper is available. reset_n = rst_n & pll_locked intentionally holds all
    // instantiated legacy logic until the generated Ethernet clock qualifies.
    wire clk_100m_adc;
    wire clk_125m_eth;
    wire unused_clock_manager_clock;
    wire reset_n;
    wire pll_locked;

    clock_manager u_clock_manager (
        .clk_100m_in(clk_100m_in),
        .rst_n(rst_n),
        .clk_600m(unused_clock_manager_clock),
        .clk_1200m_fft(unused_clock_manager_clock),
        .clk_250m_eth(unused_clock_manager_clock),
        .clk_125m_eth(clk_125m_eth),
        .clk_100m_adc(clk_100m_adc),
        .reset_n(reset_n),
        .locked(pll_locked)
    );

    // AD9215 has no data-valid pin. Every non-reset ADC clock presents a valid
    // conversion result, so validity is tied high at the adc_interface boundary.
    wire [31:0] adc_samples;
    wire        adc_sample_valid;
    wire        adc_overflow_detect;

    adc_interface u_adc_interface (
        .clk_adc(clk_100m_adc),
        .rst_n(reset_n),
        .adc_data(adc_data),
        .adc_valid(1'b1),
        .adc_ovr(adc_ovr),
        .adc_samples(adc_samples),
        .sample_valid(adc_sample_valid),
        .overflow_detect(adc_overflow_detect)
    );

    // RP2040 controls are intentionally unconnected at this first integration
    // boundary. This preserves the register interface without inventing extra
    // package pins or claiming downstream consumers are already integrated.
    wire [31:0] rp_frequency_word_unused;
    wire [7:0]  rp_gain_control_unused;
    wire [3:0]  rp_filter_select_unused;
    wire        rp_enable_control_unused;
    wire        rp_streaming_mode_unused;
    wire [7:0]  rp_bandwidth_limit_unused;
    wire [2:0]  rp_processing_mode_unused;
    wire [7:0]  rp_modulation_type_unused;
    wire [7:0]  rp_filter_bandwidth_unused;
    wire        rp_clock_gating_en_unused;
    wire [7:0]  rp_thermal_scaling_unused;
    wire        rp_resource_opt_en_unused;
    wire [7:0]  rp_power_profile_unused;
    wire [15:0] system_status;
    wire        rgmii_link_up;

    // Raw status only: active-low interrupt, overflow, sample validity, lock.
    assign system_status = {12'd0, phy_int_n, adc_overflow_detect,
                            adc_sample_valid, pll_locked};

    rp2040_interface u_rp2040_interface (
        .spi_clk(spi_clk),
        .spi_mosi(spi_mosi),
        .spi_cs_n(spi_cs_n),
        .spi_miso(spi_miso),
        .frequency_word(rp_frequency_word_unused),
        .gain_control(rp_gain_control_unused),
        .filter_select(rp_filter_select_unused),
        .enable_control(rp_enable_control_unused),
        .streaming_mode(rp_streaming_mode_unused),
        .bandwidth_limit(rp_bandwidth_limit_unused),
        .processing_mode(rp_processing_mode_unused),
        .modulation_type(rp_modulation_type_unused),
        .filter_bandwidth(rp_filter_bandwidth_unused),
        .clock_gating_en(rp_clock_gating_en_unused),
        .thermal_scaling(rp_thermal_scaling_unused),
        .resource_opt_en(rp_resource_opt_en_unused),
        .power_profile(rp_power_profile_unused),
        .status_reg(system_status),
        .pll_locked(pll_locked),
        .eth_link_status(rgmii_link_up),
        .rst_n(reset_n)
    );

    // Internal GMII-style bytes. They are deliberately not connected to
    // ethernet_mac yet because no proven RGMII-to-125 MHz CDC exists here.
    wire [7:0] gmii_tx_d;
    wire       gmii_tx_en;
    wire [7:0] gmii_rx_d;
    wire       gmii_rx_dv;

    // TODO(phy-management): implement MDIO read/write FSM plus KSZ9031 ID/reset/
    // delay-register programming and link/interrupt handling. MDC is held low
    // only to avoid an undriven pin; it performs no PHY bring-up.
    assign mdc = 1'b0;

    // MDIO is released by default; no management transaction is implemented.
    assign mdio = 1'bz;

    rgmii_phy_adapter u_rgmii_phy_adapter (
        .rxc(rgmii_rxc),
        .rx_ctl(rgmii_rx_dv),
        .rxd(rgmii_rxd),
        .txc_out(rgmii_txc),
        .tx_ctl_out(rgmii_tx_en),
        .txd_out(rgmii_txd),
        .gmii_tx_clk(clk_125m_eth),
        .reset_n(reset_n),
        .gmii_tx_d(gmii_tx_d),
        .gmii_tx_en(gmii_tx_en),
        .gmii_rx_d(gmii_rx_d),
        .gmii_rx_dv(gmii_rx_dv),
        .link_status(rgmii_link_up)
    );

endmodule

// ============================================================================
// 4-bit DDR RGMII to internal GMII-style byte adapter.
// ============================================================================
module rgmii_phy_adapter (
    input  wire       rxc,
    input  wire       rx_ctl,
    input  wire [3:0] rxd,
    output wire       txc_out,
    output wire       tx_ctl_out,
    output wire [3:0] txd_out,
    input  wire       gmii_tx_clk,
    input  wire       reset_n,
    input  wire [7:0] gmii_tx_d,
    input  wire       gmii_tx_en,
    output reg [7:0]  gmii_rx_d,
    output reg        gmii_rx_dv,
    output reg        link_status
);
    reg [3:0] rx_even;
    reg [3:0] rx_odd;
    reg       ctl_even;
    reg       ctl_odd;
    reg       txc_q;
    reg [3:0] gmii_tx_low;
    reg [3:0] gmii_tx_high;
    reg       gmii_tx_valid_low;
    reg       gmii_tx_valid_high;

    // RX capture assumptions:
    // rx_even samples RGMII at each RXC rising edge; rx_odd samples at each
    // falling edge. Together they form one GMII-style byte per full period.
    // These behavioral edge-sensitive registers express intent for simulation
    // and review. Production must use CrossLink DDR input cells plus setup/hold
    // constraints covering both edges and the selected KSZ9031 RX delay mode.
    always @(posedge rxc or negedge reset_n) begin
        if (!reset_n) begin
            rx_even <= 4'h0;
            ctl_even <= 1'b0;
        end else begin
            rx_even <= rxd;
            ctl_even <= rx_ctl;
        end
    end

    always @(negedge rxc or negedge reset_n) begin
        if (!reset_n) begin
            rx_odd <= 4'h0;
            ctl_odd <= 1'b0;
        end else begin
            rx_odd <= rxd;
            ctl_odd <= rx_ctl;
        end
    end

    // Byte assembly assumption: low nibble is posedge capture and high nibble
    // is immediately preceding negedge capture. Validate ordering against the
    // KSZ9031 RGMII bit order and board trace relationship before hardware use.
    always @(posedge rxc or negedge reset_n) begin
        if (!reset_n) begin
            gmii_rx_d <= 8'h00;
            gmii_rx_dv <= 1'b0;
        end else begin
            gmii_rx_d <= {rx_odd, rx_even};
            gmii_rx_dv <= ctl_even & ctl_odd;
        end
    end

    // Generate the 62.5 MHz RGMII transmit reference from the assumed stable
    // 125 MHz GMII-style clock. The registered divider intentionally creates skew
    // relative to data changed directly on either source-clock edge.
    // TODO(skew): replace this behavioral divider with a target-driven, statically
    // constrained DDR clock-output path in the production flow.
    always @(posedge gmii_tx_clk or negedge reset_n) begin
        if (!reset_n)
            txc_q <= 1'b0;
        else
            txc_q <= ~txc_q;
    end

    assign txc_out = txc_q;

    // Register alternate TX nibbles/control samples. TODO(skew): derive the exact
    // internal-delay/board-delay split from LIF-MD6000 I/O timing and the selected
    // KSZ9031 RGMII delay configuration.
    always @(posedge gmii_tx_clk or negedge reset_n) begin
        if (!reset_n) begin
            gmii_tx_high <= 4'h0;
            gmii_tx_valid_high <= 1'b0;
        end else begin
            gmii_tx_high <= gmii_tx_d[7:4];
            gmii_tx_valid_high <= gmii_tx_en;
        end
    end

    always @(negedge gmii_tx_clk or negedge reset_n) begin
        if (!reset_n) begin
            gmii_tx_low <= 4'h0;
            gmii_tx_valid_low <= 1'b0;
        end else begin
            gmii_tx_low <= gmii_tx_d[3:0];
            gmii_tx_valid_low <= gmii_tx_en;
        end
    end

    // One process describes both DDR output edges and avoids multiple drivers.
    // TODO(skew): replace this generic dual-edge behavioral process with CrossLink
    // DDR output primitives and documented/statically constrained TXC-to-data/
    // control skew. It does not model physical API/board delay and is not a
    // validated timing implementation.
    always @(posedge gmii_tx_clk or negedge reset_n) begin
        if (!reset_n) begin
            txd_out <= gmii_tx_low;
            tx_ctl_out <= gmii_tx_valid_low;
        end
    end

    always @(negedge gmii_tx_clk) begin
        if (reset_n) begin
            txd_out <= gmii_tx_high;
            tx_ctl_out <= gmii_tx_valid_high;
        end
    end

    // TODO(link-status): replace this safe default with decoded MDIO BMSR link
    // state once PHY management is implemented.
    always @(posedge gmii_tx_clk or negedge reset_n) begin
        if (!reset_n)
            link_status <= 1'b0;
        else
            link_status <= 1'b0;
    end
endmodule

// Restore the compilation-unit default deliberately; `default_nettype none
// must not leak into unrelated files included after this one.
`default_nettype wire
