// ============================================================================
// LIF-MD6000-6UMG64I FPGA Processing Pipeline - Top Level Module
// ============================================================================
// Issue #12 receive path: 105 MHz ADC domain -> DDC/window/1024 FFT ->
// packetized UDP application stream across an async FIFO into 125 MHz Ethernet.
// ============================================================================

`timescale 1ns/1ps

module fpga_processing_pipeline (
    input  wire        clk_100m_in,
    input  wire        rst_n,

    input  wire [9:0]  adc_data,
    input  wire        adc_valid,
    input  wire        adc_ovr,

    input  wire        spi_clk,
    input  wire        spi_mosi,
    input  wire        spi_cs_n,
    output wire        spi_miso,

    output wire [2:0]  processing_mode_out,
    output wire [7:0]  modulation_type_out,

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

    output wire        pll_locked,
    output wire        eth_link_status,
    output wire [15:0] system_status,
    output wire [31:0] packet_counter
);

    wire clk_105m_adc;
    wire clk_125m_eth;
    wire clk_250m_unused;
    wire clk_600m_unused;
    wire clk_fft_unused;
    wire reset_n;

    clock_manager u_clock_manager (
        .clk_100m_in(clk_100m_in),
        .rst_n(rst_n),
        .clk_600m(clk_600m_unused),
        .clk_1200m_fft(clk_fft_unused),
        .clk_125m_eth(clk_125m_eth),
        .clk_250m_eth(clk_250m_unused),
        .clk_105m_adc(clk_105m_adc),
        .reset_n(reset_n),
        .locked(pll_locked)
    );

    wire [31:0] frequency_word;
    wire [7:0] gain_control;
    wire [3:0] filter_select;
    wire enable_control;
    wire rp_streaming_mode;
    wire [7:0] rp_bandwidth_limit;
    wire [2:0] rp_processing_mode;
    wire [7:0] rp_modulation_type;
    wire [7:0] rp_filter_bandwidth;
    wire rp_clock_gating_en;
    wire [7:0] rp_thermal_scaling;
    wire rp_resource_opt_en;
    wire [7:0] rp_power_profile;

    wire [2:0] active_processing_mode = rp_processing_mode;
    wire [7:0] active_modulation_type = rp_modulation_type;
    wire active_resource_opt_en = rp_resource_opt_en;

    wire mode_spectrum_en = (active_processing_mode == 3'd0);
    wire mode_iq_stream_en = (active_processing_mode == 3'd1);
    wire mode_audio_demod_en = (active_processing_mode == 3'd2);
    wire mode_invalid = (active_processing_mode > 3'd2);

    wire [31:0] adc_samples;
    wire adc_sample_valid;
    wire adc_overflow_detect;

    adc_interface u_adc_interface (
        .clk_adc(clk_105m_adc),
        .rst_n(reset_n),
        .adc_data(adc_data),
        .adc_valid(adc_valid),
        .adc_ovr(adc_ovr),
        .adc_samples(adc_samples),
        .sample_valid(adc_sample_valid),
        .overflow_detect(adc_overflow_detect)
    );

    wire [15:0] nco_sine;
    wire [15:0] nco_cosine;
    wire nco_valid;

    nco_generator u_nco_generator (
        .clk(clk_105m_adc),
        .rst_n(reset_n),
        .frequency_word(frequency_word),
        .enable(enable_control),
        .sine_out(nco_sine),
        .cosine_out(nco_cosine),
        .valid_out(nco_valid)
    );

    wire [31:0] ddc_i_data;
    wire [31:0] ddc_q_data;
    wire ddc_valid;

    digital_downconverter u_ddc (
        .clk(clk_105m_adc),
        .rst_n(reset_n),
        .adc_data(adc_samples),
        .data_valid(adc_sample_valid),
        .nco_sine(nco_sine),
        .nco_cosine(nco_cosine),
        .gain_control(gain_control),
        .i_component(ddc_i_data),
        .q_component(ddc_q_data),
        .ddc_valid(ddc_valid)
    );

    wire [23:0] windowed_i_data;
    wire [23:0] windowed_q_data;
    wire window_valid_i;
    wire window_valid_q;
    wire fft_input_valid = mode_spectrum_en && window_valid_i && window_valid_q;

    hamming_window #(
        .DATA_WIDTH(24),
        .FFT_SIZE(1024)
    ) u_hamming_window_i (
        .clk(clk_105m_adc),
        .rst_n(reset_n),
        .data_in(ddc_i_data[23:0]),
        .data_valid(ddc_valid),
        .data_out(windowed_i_data),
        .output_valid(window_valid_i)
    );

    hamming_window #(
        .DATA_WIDTH(24),
        .FFT_SIZE(1024)
    ) u_hamming_window_q (
        .clk(clk_105m_adc),
        .rst_n(reset_n),
        .data_in(ddc_q_data[23:0]),
        .data_valid(ddc_valid),
        .data_out(windowed_q_data),
        .output_valid(window_valid_q)
    );

    wire [23:0] fft_real_data;
    wire [23:0] fft_imag_data;
    wire fft_valid;
    wire [9:0] fft_index;
    wire [31:0] fft_frame_count;
    wire fft_overflow_flag;
    wire fft_processing_active;

    fft_processor #(
        .FFT_SIZE(1024),
        .DATA_WIDTH(24)
    ) u_fft_processor (
        .clk(clk_105m_adc),
        .rst_n(reset_n),
        .real_in(windowed_i_data),
        .imag_in(windowed_q_data),
        .data_valid(fft_input_valid),
        .real_out(fft_real_data),
        .imag_out(fft_imag_data),
        .fft_valid(fft_valid),
        .fft_index(fft_index),
        .frame_count(fft_frame_count),
        .overflow_flag(fft_overflow_flag),
        .processing_active(fft_processing_active)
    );

    wire [15:0] demodulated_audio;
    wire audio_valid = mode_audio_demod_en && ddc_valid;
    wire [31:0] am_envelope = $signed(ddc_i_data) * $signed(ddc_i_data) +
                              $signed(ddc_q_data) * $signed(ddc_q_data);
    wire [15:0] am_audio = am_envelope[30:15];
    reg [31:0] ddc_i_prev;
    reg [31:0] ddc_q_prev;
    reg [15:0] fsk_threshold;
    wire [31:0] fm_phase_diff = $signed(ddc_i_data) * $signed(ddc_q_prev) -
                                $signed(ddc_q_data) * $signed(ddc_i_prev);
    wire [15:0] fm_audio = fm_phase_diff[25:10];
    wire [15:0] fsk_audio = (am_envelope[30:15] > fsk_threshold) ? 16'h7FFF : 16'h0000;

    assign demodulated_audio = (active_modulation_type == 8'h01) ? am_audio :
                               (active_modulation_type == 8'h02) ? fm_audio :
                               (active_modulation_type == 8'h03) ? fsk_audio : 16'h0000;

    always @(posedge clk_105m_adc or negedge reset_n) begin
        if (!reset_n) begin
            ddc_i_prev <= 32'd0;
            ddc_q_prev <= 32'd0;
            fsk_threshold <= 16'h4000;
        end else if (ddc_valid) begin
            ddc_i_prev <= ddc_i_data;
            ddc_q_prev <= ddc_q_data;
            fsk_threshold <= (fsk_threshold * 7 + am_envelope[30:15]) >> 3;
        end
    end

    wire [31:0] fft_app_data;
    wire [15:0] fft_app_len;
    wire fft_app_valid;
    wire app_ready;
    wire fft_app_ready = mode_spectrum_en ? app_ready : 1'b0;
    wire fft_packet_fifo_full;
    wire fft_packet_fifo_empty;

    fft_packetizer #(
        .FFT_SIZE(1024),
        .BINS_PER_SUBFRAME(256),
        .DATA_WIDTH(24)
    ) u_fft_packetizer (
        .fft_clk(clk_105m_adc),
        .eth_clk(clk_125m_eth),
        .rst_n(reset_n),
        .fft_real(fft_real_data),
        .fft_imag(fft_imag_data),
        .fft_valid(fft_valid && mode_spectrum_en),
        .fft_index(fft_index),
        .fft_frame_count(fft_frame_count),
        .fft_overflow(fft_overflow_flag),
        .mode({5'd0, active_processing_mode}),
        .app_data(fft_app_data),
        .app_len(fft_app_len),
        .app_valid(fft_app_valid),
        .app_ready(fft_app_ready),
        .fifo_full(fft_packet_fifo_full),
        .fifo_empty(fft_packet_fifo_empty)
    );

    wire [31:0] iq_or_audio_data = mode_iq_stream_en ? {ddc_i_data[15:0], ddc_q_data[15:0]} :
                                   mode_audio_demod_en ? {demodulated_audio, 16'd0} : 32'd0;
    wire [15:0] iq_or_audio_len = mode_audio_demod_en ? 16'd2 : 16'd4;
    wire iq_or_audio_valid = (mode_iq_stream_en && ddc_valid) || audio_valid;
    wire [31:0] iq_audio_app_data;
    wire [15:0] iq_audio_app_len;
    wire iq_audio_app_valid;
    wire iq_audio_app_ready = mode_spectrum_en ? 1'b0 : app_ready;
    wire iq_audio_fifo_full;
    wire iq_audio_fifo_empty;

    app_stream_cdc u_iq_audio_stream_cdc (
        .wr_clk(clk_105m_adc),
        .rd_clk(clk_125m_eth),
        .rst_n(reset_n),
        .wr_data(iq_or_audio_data),
        .wr_len(iq_or_audio_len),
        .wr_valid(iq_or_audio_valid),
        .wr_full(iq_audio_fifo_full),
        .rd_data(iq_audio_app_data),
        .rd_len(iq_audio_app_len),
        .rd_valid(iq_audio_app_valid),
        .rd_ready(iq_audio_app_ready),
        .rd_empty(iq_audio_fifo_empty)
    );

    wire [31:0] selected_data = mode_spectrum_en ? fft_app_data : iq_audio_app_data;
    wire [15:0] selected_len = mode_spectrum_en ? fft_app_len : iq_audio_app_len;
    wire selected_valid = mode_spectrum_en ? fft_app_valid : iq_audio_app_valid;

    wire [31:0] eth_packet_data;
    wire [15:0] eth_packet_len;
    wire eth_packet_valid;
    wire eth_packet_ack;
    wire [15:0] system_status_int;

    udp_ip_stack u_udp_ip_stack (
        .clk(clk_125m_eth),
        .rst_n(reset_n && !mode_invalid),
        .app_data(selected_data),
        .app_len(selected_len),
        .app_valid(selected_valid),
        .app_ready(app_ready),
        .src_ip(32'hC0A80002),
        .dst_ip(32'hC0A80001),
        .src_port(16'd10000),
        .dst_port(16'd10001),
        .mac_data(eth_packet_data),
        .mac_len(eth_packet_len),
        .mac_valid(eth_packet_valid)
    );

    ethernet_mac u_ethernet_mac (
        .clk(clk_125m_eth),
        .rst_n(reset_n),
        .gmii_tx_d(gmii_tx_d),
        .gmii_tx_en(gmii_tx_en),
        .gmii_tx_er(gmii_tx_er),
        .gmii_rx_d(gmii_rx_d),
        .gmii_rx_dv(gmii_rx_dv),
        .gmii_rx_er(gmii_rx_er),
        .packet_data(eth_packet_data),
        .packet_len(eth_packet_len),
        .packet_valid(eth_packet_valid),
        .packet_ack(eth_packet_ack),
        .rx_packet_data(),
        .rx_packet_len(),
        .rx_packet_valid(),
        .rx_packet_ack(1'b1),
        .link_status(eth_link_status),
        .packet_counter(packet_counter)
    );

    rp2040_interface u_rp2040_interface (
        .spi_clk(spi_clk),
        .spi_mosi(spi_mosi),
        .spi_cs_n(spi_cs_n),
        .spi_miso(spi_miso),
        .frequency_word(frequency_word),
        .gain_control(gain_control),
        .filter_select(filter_select),
        .enable_control(enable_control),
        .streaming_mode(rp_streaming_mode),
        .bandwidth_limit(rp_bandwidth_limit),
        .processing_mode(rp_processing_mode),
        .modulation_type(rp_modulation_type),
        .filter_bandwidth(rp_filter_bandwidth),
        .clock_gating_en(rp_clock_gating_en),
        .thermal_scaling(rp_thermal_scaling),
        .resource_opt_en(rp_resource_opt_en),
        .power_profile(rp_power_profile),
        .status_reg(system_status_int),
        .pll_locked(pll_locked),
        .eth_link_status(eth_link_status)
    );

    assign system_status_int = {
        mode_invalid,
        rp_clock_gating_en,
        active_processing_mode[2:1],
        eth_link_status,
        pll_locked,
        adc_overflow_detect,
        selected_valid,
        ddc_valid,
        fft_packet_fifo_full,
        fft_packet_fifo_empty,
        active_processing_mode[0],
        enable_control,
        fft_processing_active,
        fft_overflow_flag,
        active_resource_opt_en
    };
    assign system_status = system_status_int;
    assign processing_mode_out = active_processing_mode;
    assign modulation_type_out = active_modulation_type;

    assign gmii_crs = 1'b0;
    assign gmii_col = 1'b0;
    assign gmii_tx_clk = clk_125m_eth;
    assign gmii_rx_clk = clk_125m_eth;

endmodule
