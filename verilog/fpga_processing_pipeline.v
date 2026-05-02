// ============================================================================
// LIF-MD6000-6UMG64I FPGA Processing Pipeline - Top Level Module
// ============================================================================
// Updated to support 1024-pt FFT with sustained 105 MSPS throughput
// and safe CDC to Ethernet domain.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module fpga_processing_pipeline (
    // System clocks and reset
    input  wire        clk_100m_in,     // Primary 100 MHz input clock
    input  wire        rst_n,           // System reset (active low)
    
    // ADC interface
    input  wire [9:0]  adc_data,        // 10-bit parallel ADC data
    input  wire        adc_valid,       // ADC data valid flag
    input  wire        adc_ovr,         // ADC overflow flag
    
    // RP2040 SPI interface
    input  wire        spi_clk,         // SPI clock
    input  wire        spi_mosi,        // SPI master out, slave in
    input  wire        spi_cs_n,        // SPI chip select (active low)
    output wire        spi_miso,        // SPI master in, slave out

    // Dynamic configuration outputs
    output wire [2:0]  processing_mode_out,
    output wire [7:0]  modulation_type_out,
    output wire [7:0]  filter_bandwidth_out,
    output wire        clock_gating_en_out,
    output wire [7:0]  thermal_scaling_out,
    output wire        resource_opt_en_out,
    output wire [7:0]  power_profile_out,
    
    // Ethernet GMII interface
    output wire [7:0]  gmii_tx_d,
    output wire        gmii_tx_en,
    output wire        gmii_tx_er,
    input  wire [7:0]  gmii_rx_d,
    input  wire        gmii_rx_dv,
    input  wire        gmii_rx_er,
    output wire        gmii_crs,
    output wire        gmii_col,
    output wire        gmii_tx_clk,
    output wire        gmii_rx_clk,
    
    // Status and control outputs
    output wire        pll_locked,
    output wire        eth_link_status,
    output wire [15:0] system_status,
    output wire [31:0] packet_counter
);

    localparam FFT_SIZE = 1024;
    localparam ADDR_WIDTH = $clog2(FFT_SIZE);

    // ========================================================================
    // Internal Clocks & Reset
    // ========================================================================
    wire clk_105m;
    wire clk_125m_eth;
    wire reset_n;
    wire pll_locked_int;

    clock_manager u_clock_manager (
        .clk_100m_in      (clk_100m_in),
        .rst_n            (rst_n),
        .clk_600m         (),
        .clk_1200m_fft    (),
        .clk_125m_eth     (clk_125m_eth),
        .clk_250m_eth     (),
        .clk_105m_adc     (clk_105m),
        .reset_n          (reset_n),
        .locked           (pll_locked_int)
    );

    // ========================================================================
    // RP2040 Interface
    // ========================================================================
    wire [31:0] frequency_word;
    wire [7:0]  gain_control;
    wire [3:0]  filter_select;
    wire        enable_control;
    wire [2:0]  processing_mode;
    wire [7:0]  modulation_type;
    wire [7:0]  filter_bandwidth;
    wire        clock_gating_en;
    wire [7:0]  thermal_scaling;
    wire        resource_opt_en;
    wire [7:0]  power_profile;
    wire        eth_link_status_int;

    rp2040_interface u_rp2040_interface (
        .spi_clk          (spi_clk),
        .spi_mosi         (spi_mosi),
        .spi_cs_n         (spi_cs_n),
        .spi_miso         (spi_miso),
        .frequency_word   (frequency_word),
        .gain_control     (gain_control),
        .filter_select    (filter_select),
        .enable_control   (enable_control),
        .streaming_mode   (),
        .bandwidth_limit  (),
        .processing_mode  (processing_mode),
        .modulation_type  (modulation_type),
        .filter_bandwidth (filter_bandwidth),
        .clock_gating_en  (clock_gating_en),
        .thermal_scaling  (thermal_scaling),
        .resource_opt_en  (resource_opt_en),
        .power_profile    (power_profile),
        .status_reg       (system_status),
        .pll_locked       (pll_locked_int),
        .rst_n            (reset_n),
        .eth_link_status  (eth_link_status_int)
    );

    // ========================================================================
    // ADC & DDC Chain (105 MHz)
    // ========================================================================
    wire [31:0] adc_data_proc;
    wire        adc_valid_proc;
    wire        overflow_detect;

    adc_interface u_adc_interface (
        .clk_adc          (clk_105m),
        .rst_n            (reset_n),
        .adc_data         (adc_data),
        .adc_valid        (adc_valid),
        .adc_ovr          (adc_ovr),
        .adc_samples      (adc_data_proc),
        .sample_valid     (adc_valid_proc),
        .overflow_detect  (overflow_detect)
    );

    wire [15:0] nco_sine, nco_cosine;
    nco_generator u_nco_generator (
        .clk              (clk_105m),
        .rst_n            (reset_n),
        .frequency_word   (frequency_word),
        .enable           (enable_control),
        .sine_out         (nco_sine),
        .cosine_out       (nco_cosine),
        .valid_out        ()
    );

    wire [31:0] ddc_i, ddc_q;
    wire        ddc_valid;
    digital_downconverter u_ddc (
        .clk              (clk_105m),
        .rst_n            (reset_n),
        .adc_data         (adc_data_proc),
        .data_valid       (adc_valid_proc),
        .nco_sine         (nco_sine),
        .nco_cosine       (nco_cosine),
        .gain_control     (gain_control),
        .i_component      (ddc_i),
        .q_component      (ddc_q),
        .ddc_valid        (ddc_valid)
    );

    // Audio Demod (Internal bits)
    wire [31:0] am_env = $signed(ddc_i) * $signed(ddc_i) + $signed(ddc_q) * $signed(ddc_q);
    wire [15:0] am_aud = am_env[30:15];
    reg [31:0] i_prev, q_prev;
    wire [31:0] fm_pd = $signed(ddc_i) * $signed(q_prev) - $signed(ddc_q) * $signed(i_prev);
    wire [15:0] fm_aud = fm_pd[25:10];
    always @(posedge clk_105m) if (ddc_valid) begin i_prev <= ddc_i; q_prev <= ddc_q; end
    wire [15:0] sel_aud = (modulation_type == 8'h01) ? am_aud : (modulation_type == 8'h02) ? fm_aud : 16'd0;

    // Windowing and FFT
    wire [31:0] win_i, win_q;
    wire        win_v;
    hamming_window #(.FFT_SIZE(FFT_SIZE)) u_win_i (.clk(clk_105m), .rst_n(reset_n), .data_in(ddc_i), .data_valid(ddc_valid), .data_out(win_i), .output_valid(win_v));
    hamming_window #(.FFT_SIZE(FFT_SIZE)) u_win_q (.clk(clk_105m), .rst_n(reset_n), .data_in(ddc_q), .data_valid(ddc_valid), .data_out(win_q), .output_valid());

    wire [23:0] fft_r, fft_im;
    wire        fft_v;
    wire [ADDR_WIDTH-1:0] fft_id;
    wire [31:0] fft_fc;
    wire        fft_ov;

    fft_processor #(
        .FFT_SIZE         (FFT_SIZE),
        .DATA_WIDTH       (24)
    ) u_fft_processor (
        .clk              (clk_105m),
        .rst_n            (reset_n),
        .real_in          (win_i[23:0]),
        .imag_in          (win_q[23:0]),
        .data_valid       (win_v),
        .real_out         (fft_r),
        .imag_out         (fft_im),
        .fft_valid        (fft_v),
        .fft_index        (fft_id),
        .frame_count      (fft_fc),
        .overflow_flag    (fft_ov),
        .processing_active ()
    );

    // ========================================================================
    // Metadata-Enriched Multi-Word Packetizer
    // ========================================================================
    // We need more than one 32-bit word to carry all requested metadata.
    // Word 0: [31:22] bin_idx, [21] overflow, [20:0] frame_count[20:0]
    // Word 1: [31:16] real, [15:0] imag
    reg [31:0] packet_word;
    reg [1:0] word_cnt;
    reg packet_valid_int;

    wire mode_spectrum_en = (processing_mode == 3'd0);
    wire mode_iq_stream_en = (processing_mode == 3'd1);
    wire mode_audio_demod_en = (processing_mode == 3'd2);

    always @(posedge clk_105m or negedge reset_n) begin
        if (!reset_n) begin
            word_cnt <= 0;
            packet_valid_int <= 0;
        end else begin
            packet_valid_int <= 0;
            if (mode_spectrum_en && fft_v) begin
                if (word_cnt == 0) begin
                    packet_word <= {fft_id, fft_ov, 1'b0, fft_fc[19:0]};
                    word_cnt <= 1;
                    packet_valid_int <= 1;
                end else begin
                    packet_word <= {fft_r[23:8], fft_im[23:8]};
                    word_cnt <= 0;
                    packet_valid_int <= 1;
                end
            end else if (mode_iq_stream_en && ddc_valid) begin
                packet_word <= {ddc_i[15:0], ddc_q[15:0]};
                packet_valid_int <= 1;
                word_cnt <= 0;
            end else if (mode_audio_demod_en && ddc_valid) begin
                packet_word <= {sel_aud, 16'h0};
                packet_valid_int <= 1;
                word_cnt <= 0;
            end
        end
    end

    // ========================================================================
    // CDC Bridge: 105 MHz (FFT) -> 125 MHz (Ethernet)
    // ========================================================================
    wire [31:0] cdc_fifo_data;
    wire        cdc_fifo_empty;
    wire        cdc_fifo_full;
    wire        cdc_rd_en;

    async_fifo #(
        .WIDTH(32),
        .DEPTH(256)
    ) u_fft_cdc_fifo (
        .wr_clk(clk_105m),
        .rd_clk(clk_125m_eth),
        .wr_rst_n(reset_n),
        .rd_rst_n(reset_n),
        .din(packet_word),
        .wr_en(packet_valid_int),
        .rd_en(cdc_rd_en),
        .dout(cdc_fifo_data),
        .full(cdc_fifo_full),
        .empty(cdc_fifo_empty)
    );

    assign cdc_rd_en = !cdc_fifo_empty; // Aggressive read

    // ========================================================================
    // UDP/MAC Path (125 MHz)
    // ========================================================================
    wire [31:0] eth_data;
    wire [15:0] eth_len;
    wire        eth_v;
    wire        eth_ack;

    udp_ip_stack u_udp_ip_stack (
        .clk(clk_125m_eth),
        .rst_n(reset_n),
        .app_data(cdc_fifo_data),
        .app_len(16'd4),
        .app_valid(!cdc_fifo_empty),
        .app_ready(),
        .src_ip(32'hC0A80002), .dst_ip(32'hC0A80001),
        .src_port(16'd10000), .dst_port(16'd10001),
        .mac_data(eth_data), .mac_len(eth_len), .mac_valid(eth_v)
    );

    ethernet_mac u_ethernet_mac (
        .clk(clk_125m_eth),
        .rst_n(reset_n),
        .gmii_tx_d(gmii_tx_d), .gmii_tx_en(gmii_tx_en), .gmii_tx_er(gmii_tx_er),
        .gmii_rx_d(gmii_rx_d), .gmii_rx_dv(gmii_rx_dv), .gmii_rx_er(gmii_rx_er),
        .packet_data(eth_data), .packet_len(eth_len), .packet_valid(eth_v), .packet_ack(eth_ack),
        .link_status(eth_link_status_int), .packet_counter(packet_counter)
    );

    assign pll_locked = pll_locked_int;
    assign eth_link_status = eth_link_status_int;
    assign processing_mode_out = processing_mode;
    assign modulation_type_out = modulation_type;
    assign filter_bandwidth_out = filter_bandwidth;
    assign clock_gating_en_out = clock_gating_en;
    assign thermal_scaling_out = thermal_scaling;
    assign resource_opt_en_out = resource_opt_en;
    assign power_profile_out = power_profile;
    assign gmii_crs = 1'b0;
    assign gmii_col = 1'b0;
    assign gmii_tx_clk = clk_125m_eth;
    assign gmii_rx_clk = clk_125m_eth;

endmodule
