// ============================================================================
// v2_top - Code-SDR V2 integrated top level.
//
// MODE selects which datapath is built (compile-time, because a 1024-point FFT
// plus the raw streamer would not fit together in 5,936 LUTs):
//
//   MODE = 0 (raw): ADC 100 MSPS -> 8/10-bit pack -> CDC -> UDP/IP -> MAC -> RGMII
//                   The host does the FFT. Full instantaneous bandwidth.
//
//   MODE = 1 (fft): ADC 100 MSPS -> CIC /32 -> CDC -> window -> 1024-pt FFT
//                   -> bin packetizer -> UDP/IP -> MAC -> RGMII
//                   Resolution on-board; ~1.5 MHz instantaneous bandwidth.
//
// The ADC always runs at 100 MSPS in both modes (its clock is a fixed 100 MHz
// board reference). Only the FFT path consumes a decimated rate.
//
// The RP2040 reconfigures between the two bitstreams over slave SPI, so the
// swap needs no host and no re-flash (see firmware/rp2040_dashboard).
//
// Board facts honoured (HARDWARE.md): RGMII only, PHY reset driven by the
// RP2040, MDC/MDIO/PHY_INT_N are FPGA pins, single 100 MHz clock tree so the
// 125 MHz Ethernet clock comes from the PLL.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_top #(
    parameter integer MODE         = 0,       // 0 = raw, 1 = on-FPGA 1024-pt FFT
    parameter integer SAMPLE_BITS  = 8,       // raw only: 8 or 10
    parameter integer DECIM        = 1,
    parameter integer MTU          = 1500,
    parameter integer FFT_RATE     = 16,      // CIC decimation for MODE=1 (fastest the FFT sustains)
    parameter integer FFT_W        = 16,
    parameter integer FFT_N        = 1024     // 256 / 512 / 1024
) (
    input  wire        rst_n,
    input  wire        clk_100m_in,

    input  wire [9:0]  adc_data,
    input  wire        adc_ovr,

    input  wire        spi_clk,
    input  wire        spi_mosi,
    input  wire        spi_cs_n,
    output wire        spi_miso,

    output wire [3:0]  rgmii_txd,
    output wire        rgmii_tx_ctl,
    output wire        rgmii_txc,
    input  wire [3:0]  rgmii_rxd,
    input  wire        rgmii_rx_ctl,
    input  wire        rgmii_rxc,

    output wire        mdc,
    inout  wire        mdio,
    input  wire        phy_int_n
);

    // ------------------------------------------------------------------ clocks
    wire clk_eth;
    wire eth_locked;
    wire reset_n = rst_n & eth_locked;

    v2_clock_pll u_pll (
        .clk_100m_in(clk_100m_in), .rst_n(rst_n),
        .clk_125m_eth(clk_eth), .locked(eth_locked)
    );

    // ------------------------------------------------------------- PHY manager
    wire link_up, phy_ready, phy_error, mdio_i, mdio_o, mdio_oe;
    assign mdio   = mdio_oe ? mdio_o : 1'bz;
    assign mdio_i = mdio;

    v2_phy_manager u_phy (
        .clk(clk_eth), .rst_n(reset_n), .phy_int_n(phy_int_n), .mdio_i(mdio_i),
        .mdc(mdc), .mdio_o(mdio_o), .mdio_oe(mdio_oe),
        .link_up(link_up), .ready(phy_ready), .error(phy_error)
    );

    // ------------------------------------------------------------------- config
    wire [1:0]  cfg_mode;
    wire [7:0]  cfg_bits;
    wire [7:0]  cfg_decim;
    wire [31:0] cfg_nco;
    wire        cfg_enable;
    wire [15:0] cfg_dst_port;

    // --------------------------------------------- common payload-path signals
    wire [7:0]  p_data;
    wire        p_valid, p_ready;
    wire        send;
    wire [15:0] payload_len;
    wire [31:0] packet_seq;
    wire        upstream_busy;
    wire [31:0] packets_sent;

    // ===================================================== MODE 0: raw streamer
    wire [31:0] raw_dropped, raw_ovf, raw_pkts;

    generate
    if (MODE == 0) begin : g_raw

        v2_raw_path #(
            .FIFO_ADDR_WIDTH(10), .SAMPLE_BITS(SAMPLE_BITS),
            .DECIM(DECIM), .MTU(MTU)
        ) u_raw (
            .clk_adc(clk_100m_in), .clk_eth(clk_eth), .rst_n(reset_n),
            .adc_data(adc_data),
            .upstream_busy(upstream_busy),
            .link_up(link_up),
            .decim_cfg(cfg_decim),
            .p_data(p_data), .p_valid(p_valid), .p_ready(p_ready),
            .send(send), .payload_len(payload_len), .packet_seq(packet_seq),
            .dropped_words(raw_dropped),
            .overflow_count(raw_ovf), .packets_sent(raw_pkts)
        );

        assign packets_sent = raw_pkts;

    end else begin : g_fft

        // ------------------------------------------------- ADC -> CIC (100 MHz)
        // The ADC always runs at 100 MSPS. rate_cfg selects how much of that
        // stream the FFT path consumes: 1 = all 100 MSPS, 16 = 6.25 MSPS (the
        // fastest a 1024-point FFT on this fabric can keep up with).
        wire cic_valid;
        wire signed [FFT_W-1:0] cic_out;
        wire [31:0] cic_in_n, cic_out_n;

        v2_cic_decimator #(
            .IN_WIDTH(10), .STAGES(3), .RATE(FFT_RATE), .OUT_WIDTH(FFT_W)
        ) u_cic (
            .clk(clk_100m_in), .rst_n(reset_n), .in_valid(1'b1),
            .din(adc_data[9:0]),
            .rate_cfg(cfg_decim),
            .out_valid(cic_valid), .dout(cic_out),
            .samples_in(cic_in_n), .samples_out(cic_out_n)
        );

        // -------------------------------- CDC + frame buffer at decimated rate
        // Deep enough to hold one whole frame, so the FFT can burst-load at one
        // sample per clock instead of being limited to the input sample rate.
        wire [FFT_W-1:0] fifo_dout;
        wire             fifo_empty;
        wire [11:0]      fifo_avail;
        wire             fifo_rd_en;

        v2_cdc_fifo #(.WIDTH(FFT_W), .ADDR_WIDTH(11)) u_cicfifo (
            .wr_clk(clk_100m_in), .wr_rst_n(reset_n), .wr_en(cic_valid),
            .din(cic_out), .full(),
            .rd_clk(clk_eth), .rd_rst_n(reset_n), .rd_en(fifo_rd_en),
            .dout(fifo_dout), .empty(fifo_empty), .rd_avail(fifo_avail)
        );

        // --------------------------------------------------------- FFT (clk_eth)
        localparam integer FFT_LOGN = $clog2(FFT_N);
    wire                fft_in_ready;
        wire                fft_in_valid;
        wire                fft_start;
        wire signed [FFT_W-1:0] fft_re, fft_im;
        wire [FFT_LOGN-1:0] fft_index;
        wire                fft_valid, fft_frame_done, fft_busy;
        wire [31:0]         fft_frames, fft_ovf_count;
        wire [4:0]          fft_scale_exp;
        wire                fft_overflow;

        v2_fft1024 #(
            .N(FFT_N), .LOGN(FFT_LOGN), .W(FFT_W),
            .TWID_FILE("verilog/twiddle_real.mem"),
            .TWID_FILEI("verilog/twiddle_imag.mem"),
            .WINDOW_FILE("verilog/window_coeff.mem"), .WINDOW_EN(1)
        ) u_fft (
            .clk(clk_eth), .rst_n(reset_n),
            .start(fft_start),
            .in_re(fifo_dout), .in_im({FFT_W{1'b0}}),
            .in_valid(fft_in_valid), .in_ready(fft_in_ready),
            .out_re(fft_re), .out_im(fft_im), .out_index(fft_index),
            .out_valid(fft_valid), .busy(fft_busy), .frame_done(fft_frame_done),
            .frame_count(fft_frames), .overflow_count(fft_ovf_count),
            .scale_exp(fft_scale_exp), .overflow(fft_overflow)
        );

        // Burst-load handshake: the FIFO has registered output, so a read issued
        // this cycle data is valid on the next, which is when in_valid asserts.
        reg rd_en_d;
        always @(posedge clk_eth or negedge reset_n)
            if (!reset_n) rd_en_d <= 1'b0;
            else          rd_en_d <= fifo_rd_en;

        assign fifo_rd_en   = fft_in_ready;              // one pop per load cycle
        assign fft_in_valid = rd_en_d;

        // start a frame only when a whole frame is buffered, so the burst load
        // never underruns
        assign fft_start = !fft_busy && (fifo_avail >= FFT_N);

        // ---------------------------------------------------- bin packetizer
        wire [31:0] fft_pkts, fft_bins_dropped;

        v2_fft_packetizer #(
            .BINS_PER_PKT(256), .HEADER_BYTES(16), .W(FFT_W)
        ) u_fftpkt (
            .clk(clk_eth), .rst_n(reset_n),
            .fft_re(fft_re), .fft_im(fft_im), .fft_valid(fft_valid),
            .fft_index({{(10-FFT_LOGN){1'b0}}, fft_index}), .fft_frame(fft_frames),
            .fft_scale_exp(fft_scale_exp), .fft_overflow(fft_overflow),
            .upstream_busy(upstream_busy),
            .p_data(p_data), .p_valid(p_valid), .p_ready(p_ready),
            .send(send), .payload_len(payload_len), .packet_seq(packet_seq),
            .packets_sent(fft_pkts), .bins_dropped(fft_bins_dropped)
        );

        assign packets_sent = fft_pkts;
        assign raw_dropped  = fft_bins_dropped;
        assign raw_ovf      = {27'd0, fft_overflow};
        assign raw_pkts     = fft_pkts;

    end
    endgenerate

    // -------------------------------------------------------------- UDP / IPv4
    wire [7:0]  m_data;
    wire        m_valid, m_ready;
    wire [15:0] m_len;
    wire [15:0] src_port = (MODE == 1) ? 16'd4661 : 16'd4660;

    v2_udp_ip_tx u_udp (
        .clk(clk_eth), .rst_n(reset_n),
        .src_ip(32'hC0A80002), .dst_ip(32'hC0A80001),
        .src_port(src_port), .dst_port(cfg_dst_port),
        .send(send), .payload_len(payload_len), .busy(upstream_busy),
        .p_data(p_data), .p_valid(p_valid), .p_ready(p_ready),
        .m_data(m_data), .m_valid(m_valid), .m_ready(m_ready), .m_len(m_len)
    );

    // ----------------------------------------------------------------- MAC (TX)
    wire [7:0] gmii_tx_d, gmii_rx_d;
    wire       gmii_tx_en, gmii_rx_dv;

    v2_eth_mac_tx u_mac (
        .clk(clk_eth), .rst_n(reset_n),
        .dst_mac(48'h02_00_00_00_00_01), .src_mac(48'h02_00_00_00_00_02),
        .s_data(m_data), .s_valid(m_valid), .s_len(m_len), .s_ready(m_ready),
        .gmii_tx_d(gmii_tx_d), .gmii_tx_en(gmii_tx_en),
        .packet_count(), .frame_count()
    );

    // -------------------------------------------------------------------- RGMII
    v2_rgmii u_rgmii (
        .rst_n(reset_n),
        .rgmii_rxc(rgmii_rxc), .rgmii_rxd(rgmii_rxd), .rgmii_rx_ctl(rgmii_rx_ctl),
        .rgmii_txc(rgmii_txc), .rgmii_txd(rgmii_txd), .rgmii_tx_ctl(rgmii_tx_ctl),
        .clk_tx(clk_eth),
        .gmii_tx_d(gmii_tx_d), .gmii_tx_en(gmii_tx_en),
        .gmii_rx_d(gmii_rx_d), .gmii_rx_dv(gmii_rx_dv)
    );

    // --------------------------------------------------------------- telemetry
    wire [7:0]  tele_addr;
    wire [31:0] tele_data;

    v2_telemetry u_tele (
        .clk(clk_eth), .rst_n(reset_n),
        .packets_sent(packets_sent),
        .dropped_words(raw_dropped),
        .overflow_count(raw_ovf),
        .seq_value(packet_seq),
        .sample_bits(cfg_bits),
        .decim((MODE == 1) ? {3'b0, FFT_RATE[4:0]} : cfg_decim),
        .mode({1'b0, MODE[0]}),
        .link_up(link_up), .pll_locked(eth_locked), .phy_error(phy_error),
        .effective_mbps(16'd0),
        .rd_addr(tele_addr), .rd_data(tele_data)
    );

    // ---------------------------------------------------------------- SPI regs
    v2_spi_regs u_spi (
        .clk(clk_eth), .rst_n(reset_n),
        .spi_clk(spi_clk), .spi_cs_n(spi_cs_n), .spi_mosi(spi_mosi),
        .spi_miso(spi_miso),
        .tele_addr(tele_addr), .tele_data(tele_data),
        .cfg_mode(cfg_mode), .cfg_sample_bits(cfg_bits), .cfg_decim(cfg_decim),
        .cfg_nco_freq(cfg_nco), .cfg_enable(cfg_enable), .cfg_dst_port(cfg_dst_port)
    );

endmodule

`default_nettype wire
