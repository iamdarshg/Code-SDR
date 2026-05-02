// ============================================================================
// LIF-MD6000-6UMG64I FPGA Processing Pipeline - Top Level Module
// ============================================================================
// Updated to support 1024-pt FFT with sustained 105 MSPS throughput.
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
    // Internal Clocks
    // ========================================================================
    wire clk_105m; // Unified clock for ADC, DDC, Window, and FFT
    wire clk_125m_eth;
    wire reset_n;
    wire pll_locked_int;

    clock_manager u_clock_manager (
        .clk_100m_in      (clk_100m_in),
        .rst_n            (rst_n),
        .clk_600m         (), // Unused
        .clk_1200m_fft    (), // Unused
        .clk_125m_eth     (clk_125m_eth),
        .clk_250m_eth     (),
        .clk_105m_adc     (clk_105m),
        .reset_n          (reset_n),
        .locked           (pll_locked_int)
    );

    // ========================================================================
    // Control Interface
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
        .eth_link_status  (eth_link_status)
    );

    // ========================================================================
    // Signal Processing Chain (All at 105 MHz)
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
    wire        nco_valid;

    nco_generator u_nco_generator (
        .clk              (clk_105m),
        .rst_n            (reset_n),
        .frequency_word   (frequency_word),
        .enable           (enable_control),
        .sine_out         (nco_sine),
        .cosine_out       (nco_cosine),
        .valid_out        (nco_valid)
    );

    wire [31:0] ddc_i_data, ddc_q_data;
    wire        ddc_valid;

    digital_downconverter u_ddc (
        .clk              (clk_105m),
        .rst_n            (reset_n),
        .adc_data         (adc_data_proc),
        .data_valid       (adc_valid_proc),
        .nco_sine         (nco_sine),
        .nco_cosine       (nco_cosine),
        .gain_control     (gain_control),
        .i_component      (ddc_i_data),
        .q_component      (ddc_q_data),
        .ddc_valid        (ddc_valid)
    );

    wire [31:0] window_i, window_q;
    wire        window_valid;

    hamming_window #(
        .WIDTH            (32),
        .FFT_SIZE         (FFT_SIZE)
    ) u_hamming_window_i (
        .clk              (clk_105m),
        .rst_n            (reset_n),
        .data_in          (ddc_i_data),
        .data_valid       (ddc_valid),
        .data_out         (window_i),
        .output_valid     (window_valid)
    );

    hamming_window #(
        .WIDTH            (32),
        .FFT_SIZE         (FFT_SIZE)
    ) u_hamming_window_q (
        .clk              (clk_105m),
        .rst_n            (reset_n),
        .data_in          (ddc_q_data),
        .data_valid       (ddc_valid),
        .data_out         (window_q),
        .output_valid     ()
    );

    wire [23:0] fft_real, fft_imag;
    wire        fft_valid;
    wire [ADDR_WIDTH-1:0] fft_idx;
    wire [31:0] fft_frame_count;
    wire        fft_overflow;

    fft_processor #(
        .FFT_SIZE         (FFT_SIZE),
        .DATA_WIDTH       (24)
    ) u_fft_processor (
        .clk              (clk_105m),
        .rst_n            (reset_n),
        .real_in          (window_i[23:0]),
        .imag_in          (window_q[23:0]),
        .data_valid       (window_valid),

        .real_out         (fft_real),
        .imag_out         (fft_imag),
        .fft_valid        (fft_valid),
        .fft_index        (fft_idx),
        .frame_count      (fft_frame_count),
        .overflow_flag    (fft_overflow),
        .processing_active ()
    );

    // ========================================================================
    // Packetization and Output
    // ========================================================================

    // Metadata-enriched data packing
    // [31:22] = Bin Index, [21:20] = Flags (Overflow, Mode), [19:0] = Data
    wire [31:0] packet_data = {fft_idx, fft_overflow, processing_mode[0], fft_real[23:4]};

    udp_ip_stack u_udp_ip_stack (
        .clk              (clk_125m_eth),
        .rst_n            (reset_n),
        .app_data         (packet_data),
        .app_len          (16'd4),
        .app_valid        (fft_valid),
        .app_ready        (),
        .src_ip           (32'hC0A80002),
        .dst_ip           (32'hC0A80001),
        .src_port         (16'd10000),
        .dst_port         (16'd10001),
        .mac_data         (),
        .mac_len          (),
        .mac_valid        ()
    );

    // Standard GMII Assignments
    assign pll_locked = pll_locked_int;
    assign gmii_crs = 1'b0;
    assign gmii_col = 1'b0;
    assign gmii_tx_clk = clk_125m_eth;
    assign gmii_rx_clk = clk_125m_eth;

endmodule
