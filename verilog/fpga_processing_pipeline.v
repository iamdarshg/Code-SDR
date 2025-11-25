// ============================================================================
// LIF-MD6000-6UMG64I FPGA Processing Pipeline - Top Level Module
// ============================================================================
// This module implements the complete SDR processing pipeline with
// ADC interface, digital downconversion, FFT processing, and Ethernet output
// ============================================================================

`timescale 1ns/1ps

module fpga_processing_pipeline (
    // System clocks and reset
    input  wire        clk_100m,        // Primary 100 MHz system clock
    input  wire        clk_105m_adc,    // 105 MHz ADC sampling clock
    input  wire        clk_125m_eth,    // 125 MHz Ethernet GMII clock
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
    input  wire [7:0]  gain_control,    // Gain control from RP2040 (added for DDC)
    
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
    // Internal signals and wires
    // ========================================================================
    
    // Clock domain crossing signals
    wire [31:0] adc_data_proc;          // ADC data in processing domain
    wire        adc_valid_proc;         // ADC valid signal in processing domain
    
    // Processing pipeline signals
    wire [31:0] ddc_i_data;             // DDC I component data
    wire [31:0] ddc_q_data;             // DDC Q component data
    wire        ddc_valid;              // DDC output valid
    
    wire [23:0] fft_real_data;          // FFT real component
    wire [23:0] fft_imag_data;          // FFT imaginary component
    wire        fft_valid;              // FFT output valid
    wire [11:0] fft_index;              // FFT bin index
    wire        fft_overflow_flag;      // FFT overflow detection
    wire        fft_processing_active;  // FFT processing activity
    
    // Control signals
    wire [31:0] frequency_word;         // NCO frequency word from RP2040
    wire [7:0]  gain_control;           // Gain control settings
    wire [3:0]  filter_select;          // Filter selection
    wire        enable_control;         // System enable control
    
    // Ethernet interface signals
    wire [31:0] eth_packet_data;        // Ethernet packet data
    wire [15:0] eth_packet_len;         // Ethernet packet length
    wire        eth_packet_valid;       // Ethernet packet valid
    wire        eth_packet_ack;         // Ethernet packet acknowledged
    
    // ========================================================================
    // Clock manager instance
    // ========================================================================
    clock_manager u_clock_manager (
        .clk_100m         (clk_100m),
        .clk_105m_adc     (clk_105m_adc),
        .clk_125m_eth     (clk_125m_eth),
        .rst_n            (rst_n),
        .clk_processing   (clk_processing),
        .clk_eth_tx       (clk_eth_tx),
        .clk_eth_rx       (clk_eth_rx),
        .clk_adc          (clk_adc),
        .reset_n          (reset_n)
    );
    
    // ========================================================================
    // ADC Interface and Data Conditioning
    // ========================================================================
    adc_interface u_adc_interface (
        .clk_adc          (clk_adc),
        .rst_n            (reset_n),
        .adc_data         (adc_data),
        .adc_valid        (adc_valid),
        .adc_ovr          (adc_ovr),
        .adc_samples      (adc_data_proc),
        .sample_valid     (adc_valid_proc),
        .overflow_detect  (overflow_detect)
    );
    
    // ========================================================================
    // Asynchronous FIFO for ADC to Processing domain crossing
    // ========================================================================
    async_fifo #(
        .WIDTH            (32),
        .DEPTH            (256)
    ) u_adc_async_fifo (
        .wr_clk           (clk_adc),
        .rd_clk           (clk_processing),
        .wr_rst_n         (reset_n),
        .rd_rst_n         (reset_n),
        .din              (adc_data_proc),
        .wr_en            (adc_valid_proc),
        .rd_en            (1'b1),       // Continuous read for streaming
        .dout             (adc_data_cross),
        .full             (adc_fifo_full),
        .empty            (adc_fifo_empty)
    );
    
    // ========================================================================
    // Digital Downconversion (DDC)
    // ========================================================================
    digital_downconverter u_ddc (
        .clk              (clk_processing),
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
    
    // ========================================================================
    // NCO Generator
    // ========================================================================
    nco_generator u_nco_generator (
        .clk              (clk_processing),
        .rst_n            (reset_n),
        .frequency_word   (frequency_word),
        .enable           (enable_control),
        .sine_out         (nco_sine),
        .cosine_out       (nco_cosine),
        .valid_out        (nco_valid)
    );
    
    // ========================================================================
    // Hamming Window
    // ========================================================================
    hamming_window #(
        .WIDTH            (32),
        .FFT_SIZE         (4096)
    ) u_hamming_window_i (
        .clk              (clk_processing),
        .rst_n            (reset_n),
        .data_in          (ddc_i_data),
        .data_valid       (ddc_valid),
        .data_out         (windowed_i_data),
        .output_valid     (window_valid)
    );

    hamming_window #(
        .WIDTH            (32),
        .FFT_SIZE         (4096)
    ) u_hamming_window_q (
        .clk              (clk_processing),
        .rst_n            (reset_n),
        .data_in          (ddc_q_data),
        .data_valid       (ddc_valid),
        .data_out         (windowed_q_data),
        .output_valid     (window_valid_q)
    );
    
    // ========================================================================
    // FFT Processor
    // ========================================================================
    fft_processor #(
        .FFT_SIZE         (4096),
        .DATA_WIDTH       (24)
    ) u_fft_processor (
        .clk              (clk_processing),
        .rst_n            (reset_n),
        .real_in          (windowed_i_data[23:0]),
        .imag_in          (windowed_q_data[23:0]),
        .data_valid       (window_valid && window_valid_q),
        .real_out         (fft_real_data),
        .imag_out         (fft_imag_data),
        .fft_valid        (fft_valid),
        .fft_index        (fft_index),
        .overflow_flag    (fft_overflow_flag),
        .processing_active (fft_processing_active)
    );
    
    // ========================================================================
    // UDP/IP Protocol Stack
    // ========================================================================
    udp_ip_stack u_udp_ip_stack (
        .clk              (clk_eth_tx),
        .rst_n            (reset_n),
        .app_data         ({fft_real_data, fft_imag_data}),
        .app_len          (6),          // 24-bit complex data = 6 bytes
        .app_valid        (fft_valid),
        .app_ready        (app_ready),
        .src_ip           (32'hC0A80002),  // 192.168.0.2
        .dst_ip           (32'hC0A80001),  // 192.168.0.1
        .src_port         (16'd10000),
        .dst_port         (16'd10001),
        .mac_data         (eth_packet_data),
        .mac_len          (eth_packet_len),
        .mac_valid        (eth_packet_valid)
    );
    
    // ========================================================================
    // Ethernet MAC Layer
    // ========================================================================
    ethernet_mac u_ethernet_mac (
        .clk              (clk_eth_tx),
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
        .link_status      (eth_link_status),
        .packet_counter   (packet_counter)
    );
    
    // ========================================================================
    // RP2040 Interface
    // ========================================================================
    rp2040_interface u_rp2040_interface (
        .spi_clk          (spi_clk),
        .spi_mosi         (spi_mosi),
        .spi_cs_n         (spi_cs_n),
        .spi_miso         (spi_miso),
        .frequency_word   (frequency_word),
        .gain_control     (gain_control),
        .filter_select    (filter_select),
        .enable_control   (enable_control),
        .status_reg       (system_status),
        .pll_locked       (pll_locked),
        .eth_link_status  (eth_link_status)
    );
    
    // ========================================================================
    // System Status Register
    // ========================================================================
    assign system_status = {
        fft_processing_active,   // Bit 15: FFT processing active
        fft_overflow_flag,       // Bit 14: FFT overflow flag
        2'b0,                    // Reserved (Bits 13-12)
        eth_link_status,         // Bit 11: Ethernet link
        pll_locked,              // Bit 10: PLL lock
        overflow_detect,         // Bit 9: ADC overflow
        fft_valid,               // Bit 8: FFT valid
        ddc_valid,               // Bit 7: DDC valid
        adc_fifo_full,           // Bit 6: ADC FIFO full
        adc_fifo_empty,          // Bit 5: ADC FIFO empty
        enable_control           // Bit 4: System enabled
    };
    
    // ========================================================================
    // GMII output assignments
    // ========================================================================
    assign gmii_crs = 1'b0;          // No carrier sense (full-duplex)
    assign gmii_col = 1'b0;          // No collision (full-duplex)
    assign gmii_tx_clk = clk_125m_eth;  // Transmit clock
    assign gmii_rx_clk = clk_125m_eth;  // Receive clock

endmodule
