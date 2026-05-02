// ============================================================================
// LIF-MD6000-6UMG64I FPGA Processing Pipeline - Top Level Module
// ============================================================================
// This module implements the complete SDR processing pipeline with
// ADC interface, digital downconversion, FFT processing, and Ethernet output
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

    // Dynamic configuration outputs (driven by rp2040_interface)
    output wire [2:0]  processing_mode_out,
    output wire [7:0]  modulation_type_out,
    output wire [7:0]  filter_bandwidth_out,
    output wire        clock_gating_en_out,
    output wire [7:0]  thermal_scaling_out,
    output wire        resource_opt_en_out,
    output wire [7:0]  power_profile_out,
    
    // Ethernet GMII interface (KSZ9031RNXCC)
    output wire [7:0]  gmii_tx_d,       // GMII transmit data
    output wire        gmii_tx_en,      // GMII transmit enable
    output wire        gmii_tx_er,      // GMII transmit error
    input  wire [7:0]  gmii_rx_d,       // GMII receive data
    input  wire        gmii_rx_dv,      // GMII receive data valid
    input  wire        gmii_rx_er,      // GMII receive error
    output wire        gmii_crs,        // GMII carrier sense
    output wire        gmii_col,        // GMII collision detect
    output wire        gmii_tx_clk,     // GMII transmit clock (125 MHz)
    output wire        gmii_rx_clk,     // GMII receive clock (125 MHz)
    
    // Status and control outputs
    output wire        pll_locked,      // PLL lock status
    output wire        eth_link_status, // Ethernet link status
    output wire [15:0] system_status,   // System status register
    output wire [31:0] packet_counter   // Packet counter
);

    // ========================================================================
    // Internal wires - Explicitly declared
    // ========================================================================
    
    wire clk_600m;
    wire clk_1200m_fft;
    wire clk_125m_eth_standard;
    wire clk_250m_eth_boosted;
    wire adc_clock;
    wire reset_n;
    wire pll_locked_int;

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
    wire        streaming_mode_unused;
    wire [7:0]  bandwidth_limit_unused;

    wire [31:0] adc_data_proc;
    wire        adc_valid_proc;
    wire        overflow_detect;

    wire [31:0] adc_data_cross;
    wire        adc_fifo_full;
    wire        adc_fifo_empty;

    wire [31:0] ddc_i_data;
    wire [31:0] ddc_q_data;
    wire        ddc_valid;

    wire [15:0] nco_sine;
    wire [15:0] nco_cosine;
    wire        nco_valid;

    wire [31:0] windowed_i_data;
    wire [31:0] windowed_q_data;
    wire        window_valid;
    wire        window_valid_q;
    wire [23:0] fft_real_data;
    wire [23:0] fft_imag_data;
    wire        fft_valid;
    wire [11:0] fft_index;
    wire        fft_overflow_flag;
    wire        fft_processing_active;

    wire [31:0] eth_packet_data;
    wire [15:0] eth_packet_len;
    wire        eth_packet_valid;
    wire        app_ready;

    wire        eth_packet_ack;
    wire        eth_link_status_int;
    wire [31:0] packet_counter_int;

    // ========================================================================
    // Clock manager instance
    // ========================================================================

    clock_manager u_clock_manager (
        .clk_100m_in      (clk_100m_in),
        .rst_n            (rst_n),
        .clk_600m         (clk_600m),
        .clk_1200m_fft    (clk_1200m_fft),
        .clk_125m_eth     (clk_125m_eth_standard),
        .clk_250m_eth     (clk_250m_eth_boosted),
        .clk_105m_adc     (adc_clock),
        .reset_n          (reset_n),
        .locked           (pll_locked_int)
    );

    // Fixed 125MHz GMII clock per specification
    wire clk_ethernet = clk_125m_eth_standard;
    
    // ========================================================================
    // Pipeline Modules
    // ========================================================================
    adc_interface u_adc_interface (
        .clk_adc          (adc_clock),
        .rst_n            (reset_n),
        .adc_data         (adc_data),
        .adc_valid        (adc_valid),
        .adc_ovr          (adc_ovr),
        .adc_samples      (adc_data_proc),
        .sample_valid     (adc_valid_proc),
        .overflow_detect  (overflow_detect)
    );

    async_fifo #(
        .WIDTH            (32),
        .DEPTH            (256)
    ) u_adc_async_fifo (
        .wr_clk           (adc_clock),
        .rd_clk           (clk_600m),
        .wr_rst_n         (reset_n),
        .rd_rst_n         (reset_n),
        .din              (adc_data_proc),
        .wr_en            (adc_valid_proc),
        .rd_en            (1'b1),
        .dout             (adc_data_cross),
        .full             (adc_fifo_full),
        .empty            (adc_fifo_empty)
    );
    
    digital_downconverter u_ddc (
        .clk              (clk_600m),
        .rst_n            (reset_n),
        .adc_data         (adc_data_cross),
        .data_valid       (!adc_fifo_empty),
        .nco_sine         (nco_sine),
        .nco_cosine       (nco_cosine),
        .gain_control     (gain_control),
        .i_component      (ddc_i_data),
        .q_component      (ddc_q_data),
        .ddc_valid        (ddc_valid)
    );

    nco_generator u_nco_generator (
        .clk              (clk_600m),
        .rst_n            (reset_n),
        .frequency_word   (frequency_word),
        .enable           (enable_control),
        .sine_out         (nco_sine),
        .cosine_out       (nco_cosine),
        .valid_out        (nco_valid)
    );

    wire mode_spectrum_en = (processing_mode == 3'd0);
    wire fft_pipeline_en = mode_spectrum_en && enable_control && !clock_gating_en;

    hamming_window #(
        .WIDTH            (32),
        .FFT_SIZE         (4096)
    ) u_hamming_window_i (
        .clk              (clk_600m),
        .rst_n            (reset_n),
        .data_in          (ddc_i_data),
        .data_valid       (ddc_valid && fft_pipeline_en),
        .data_out         (windowed_i_data),
        .output_valid     (window_valid)
    );

    hamming_window #(
        .WIDTH            (32),
        .FFT_SIZE         (4096)
    ) u_hamming_window_q (
        .clk              (clk_600m),
        .rst_n            (reset_n),
        .data_in          (ddc_q_data),
        .data_valid       (ddc_valid && fft_pipeline_en),
        .data_out         (windowed_q_data),
        .output_valid     (window_valid_q)
    );

    fft_processor #(
        .FFT_SIZE         (4096),
        .DATA_WIDTH       (24)
    ) u_fft_processor (
        .clk              (clk_1200m_fft),
        .rst_n            (reset_n),
        .real_in          (windowed_i_data[23:0]),
        .imag_in          (windowed_q_data[23:0]),
        .data_valid       (window_valid && window_valid_q && fft_pipeline_en),
        .real_out         (fft_real_data),
        .imag_out         (fft_imag_data),
        .fft_valid        (fft_valid),
        .fft_index        (fft_index),
        .overflow_flag    (fft_overflow_flag),
        .processing_active (fft_processing_active)
    );

    wire mode_iq_stream_en = (processing_mode == 3'd1);
    wire mode_audio_demod_en = (processing_mode == 3'd2);
    wire mode_invalid = (processing_mode > 3'd2);

    wire [15:0] demodulated_audio;
    wire        audio_valid;

    wire [31:0] am_envelope = $signed(ddc_i_data) * $signed(ddc_i_data) + $signed(ddc_q_data) * $signed(ddc_q_data);
    wire [15:0] am_audio = am_envelope[30:15];

    reg [31:0] ddc_i_prev, ddc_q_prev;
    wire [31:0] fm_phase_diff = $signed(ddc_i_data) * $signed(ddc_q_prev) - $signed(ddc_q_data) * $signed(ddc_i_prev);
    wire [15:0] fm_audio = fm_phase_diff[25:10];

    reg [15:0] fsk_threshold;
    wire [15:0] fsk_audio = (am_envelope[30:15] > fsk_threshold) ? 16'h7FFF : 16'h0000;

    always @(posedge clk_600m or negedge reset_n) begin
        if (!reset_n) begin
            ddc_i_prev <= 32'd0;
            ddc_q_prev <= 32'd0;
            fsk_threshold <= 16'h4000;
        end else if (ddc_valid) begin
            ddc_i_prev <= ddc_i_data;
            ddc_q_prev <= ddc_q_data;
            fsk_threshold <= (fsk_threshold * 7 + am_envelope[30:15]) / 8;
        end
    end

    wire [15:0] selected_audio = (modulation_type == 8'h01) ? am_audio :
                                 (modulation_type == 8'h02) ? fm_audio :
                                 (modulation_type == 8'h03) ? fsk_audio :
                                 16'h0000;

    assign audio_valid = mode_audio_demod_en && ddc_valid;
    assign demodulated_audio = selected_audio;

    // Fixed spectrum mode data packing: Use upper 16 bits of 24-bit FFT components
    wire [31:0] fft_data_packed = {fft_real_data[23:8], fft_imag_data[23:8]};

    wire [31:0] selected_data = (mode_spectrum_en) ? fft_data_packed :
                               (mode_iq_stream_en) ? {ddc_i_data[15:0], ddc_q_data[15:0]} :
                               (mode_audio_demod_en) ? {demodulated_audio, 16'h0} : 32'h0;

    wire selected_valid = (mode_spectrum_en) ? fft_valid :
                         (mode_iq_stream_en) ? ddc_valid :
                         (mode_audio_demod_en) ? audio_valid : 1'b0;

    wire [3:0] selected_len = (mode_spectrum_en) ? 4'd4 :
                             (mode_iq_stream_en) ? 4'd4 :
                             (mode_audio_demod_en) ? 4'd2 : 4'd0;

    udp_ip_stack u_udp_ip_stack (
        .clk              (clk_ethernet),
        .rst_n            (reset_n && !mode_invalid),
        .app_data         (selected_data),
        .app_len          (selected_len),
        .app_valid        (selected_valid),
        .app_ready        (app_ready),
        .src_ip           (32'hC0A80002),
        .dst_ip           (32'hC0A80001),
        .src_port         (16'd10000),
        .dst_port         (16'd10001),
        .mac_data         (eth_packet_data),
        .mac_len          (eth_packet_len),
        .mac_valid        (eth_packet_valid)
    );

    ethernet_mac u_ethernet_mac (
        .clk              (clk_ethernet),
        .rst_n            (reset_n),
        .gmii_tx_d        (gmii_tx_d),
        .gmii_tx_en       (gmii_tx_en),
        .gmii_tx_er       (gmii_tx_er),
        .gmii_rx_d        (gmii_rx_d),
        .gmii_rx_dv       (gmii_rx_dv),
        .gmii_rx_er       (gmii_rx_er),
        .packet_data      (eth_packet_data),
        .packet_len       (eth_packet_len),
        .packet_valid     (eth_packet_valid),
        .packet_ack       (eth_packet_ack),
        .link_status      (eth_link_status_int),
        .packet_counter   (packet_counter_int)
    );
    
    wire [15:0] system_status_int;

    rp2040_interface u_rp2040_interface (
        .spi_clk          (spi_clk),
        .spi_mosi         (spi_mosi),
        .spi_cs_n         (spi_cs_n),
        .spi_miso         (spi_miso),
        .frequency_word   (frequency_word),
        .gain_control     (gain_control),
        .filter_select    (filter_select),
        .enable_control   (enable_control),
        .streaming_mode   (streaming_mode_unused),
        .bandwidth_limit  (bandwidth_limit_unused),
        .processing_mode  (processing_mode),
        .modulation_type  (modulation_type),
        .filter_bandwidth (filter_bandwidth),
        .clock_gating_en  (clock_gating_en),
        .thermal_scaling  (thermal_scaling),
        .resource_opt_en  (resource_opt_en),
        .power_profile    (power_profile),
        .status_reg       (system_status_int),
        .pll_locked       (pll_locked_int),
        .eth_link_status  (eth_link_status_int)
    );

    assign system_status_int = {
        mode_invalid,
        clock_gating_en,
        processing_mode[2:1],
        eth_link_status_int,
        pll_locked_int,
        overflow_detect,
        selected_valid,
        ddc_valid,
        adc_fifo_full,
        adc_fifo_empty,
        processing_mode[0],
        enable_control
    };
    
    assign pll_locked = pll_locked_int;
    assign eth_link_status = eth_link_status_int;
    assign packet_counter = packet_counter_int;
    assign system_status = system_status_int;

    assign processing_mode_out = processing_mode;
    assign modulation_type_out = modulation_type;
    assign filter_bandwidth_out = filter_bandwidth;
    assign clock_gating_en_out = clock_gating_en;
    assign thermal_scaling_out = thermal_scaling;
    assign resource_opt_en_out = resource_opt_en;
    assign power_profile_out = power_profile;

    assign gmii_crs = 1'b0;
    assign gmii_col = 1'b0;
    assign gmii_tx_clk = clk_ethernet;
    assign gmii_rx_clk = clk_ethernet;

endmodule
