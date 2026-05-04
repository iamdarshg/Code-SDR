// ============================================================================
// LIF-MD6000-6UMG64I FPGA Processing Pipeline - Top Level Module
// ============================================================================
// Optimized for 105 MSPS FFT with 1G Ethernet-compliant UDP packetization.
// Implements frame segmentation (4 packets/FFT frame) to respect 1500B MTU.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

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

    localparam FFT_SIZE = 1024;
    localparam ADDR_WIDTH = $clog2(FFT_SIZE);

    // ========================================================================
    // Clocks & Reset
    // ========================================================================
    wire clk_105m, clk_125m_eth, reset_n, pll_locked_int;

    clock_manager u_clock_manager (
        .clk_100m_in(clk_100m_in), .rst_n(rst_n),
        .clk_125m_eth(clk_125m_eth), .clk_105m_adc(clk_105m),
        .reset_n(reset_n), .locked(pll_locked_int)
    );

    reg [31:0] timestamp_cnt;
    always @(posedge clk_105m or negedge reset_n) begin
        if (!reset_n) timestamp_cnt <= 0;
        else timestamp_cnt <= timestamp_cnt + 1;
    end

    // ========================================================================
    // Configuration & RP2040
    // ========================================================================
    wire [31:0] frequency_word;
    wire [7:0] gain_control, modulation_type;
    wire [3:0] filter_select;
    wire [2:0] processing_mode;
    wire enable_control, eth_link_status_int;

    rp2040_interface u_rp2040_interface (
        .spi_clk(spi_clk), .spi_mosi(spi_mosi), .spi_cs_n(spi_cs_n), .spi_miso(spi_miso),
        .frequency_word(frequency_word), .gain_control(gain_control), .filter_select(filter_select),
        .enable_control(enable_control), .processing_mode(processing_mode), .modulation_type(modulation_type),
        .status_reg(system_status), .pll_locked(pll_locked_int), .rst_n(reset_n), .eth_link_status(eth_link_status_int)
    );

    // ========================================================================
    // DSP: ADC -> DDC -> Window -> FFT
    // ========================================================================
    wire [31:0] adc_samples;
    wire adc_v;
    adc_interface u_adc_int (.clk_adc(clk_105m), .rst_n(reset_n), .adc_data(adc_data), .adc_valid(adc_valid), .adc_ovr(adc_ovr), .adc_samples(adc_samples), .sample_valid(adc_v));

    wire [15:0] nco_sin, nco_cos;
    nco_generator u_nco (.clk(clk_105m), .rst_n(reset_n), .frequency_word(frequency_word), .enable(enable_control), .sine_out(nco_sin), .cosine_out(nco_cos), .valid_out());

    wire [31:0] ddc_i, ddc_q;
    wire ddc_v;
    digital_downconverter u_ddc (.clk(clk_105m), .rst_n(reset_n), .adc_data(adc_samples), .data_valid(adc_v), .nco_sine(nco_sin), .nco_cosine(nco_cos), .gain_control(gain_control), .i_component(ddc_i), .q_component(ddc_q), .ddc_valid(ddc_v));

    wire [23:0] win_i, win_q;
    wire win_v;
    hamming_window #(.DATA_WIDTH(24)) u_win_i (.clk(clk_105m), .rst_n(reset_n), .data_in(ddc_i[23:0]), .data_valid(ddc_v), .data_out(win_i), .output_valid(win_v));
    hamming_window #(.DATA_WIDTH(24)) u_win_q (.clk(clk_105m), .rst_n(reset_n), .data_in(ddc_q[23:0]), .data_valid(ddc_v), .data_out(win_q), .output_valid());

    wire [23:0] fft_r, fft_im;
    wire fft_v, fft_ov;
    wire [ADDR_WIDTH-1:0] fft_idx;
    wire [31:0] fft_fc;

    fft_processor #(.DATA_WIDTH(24)) u_fft (
        .clk(clk_105m), .rst_n(reset_n), .real_in(win_i), .imag_in(win_q), .data_valid(win_v),
        .real_out(fft_r), .imag_out(fft_im), .fft_valid(fft_v), .fft_index(fft_idx),
        .frame_count(fft_fc), .overflow_flag(fft_ov), .processing_active()
    );

    // ========================================================================
    // Multi-Packet UDP Packetizer (MTU compliant)
    // Segments 1024 bins into 4 packets of 256 bins each.
    // Egress: 105 MSPS -> 1:4 Decimation = ~26 FPS * 1024 bins * 4B = ~107 Mbps.
    // ========================================================================
    reg [31:0] pkt_word;
    reg pkt_v;
    reg [15:0] app_len_sig;
    reg [ADDR_WIDTH:0] pkt_cnt; // 0..1023
    reg [2:0] pkt_state;
    wire frame_selected = (fft_fc[1:0] == 2'b00); // 1:4 decimation for bandwidth

    localparam ST_IDLE = 3'd0, ST_DATA = 3'd1, ST_META = 3'd2;
    localparam WORDS_PER_PACKET = 256;
    localparam PACKET_DATA_BYTES = WORDS_PER_PACKET * 4;

    always @(posedge clk_105m or negedge reset_n) begin
        if (!reset_n) begin
            pkt_state <= ST_IDLE; pkt_cnt <= 0; pkt_word <= 0; pkt_v <= 0; app_len_sig <= 0;
        end else begin
            pkt_v <= 0;
            case (pkt_state)
                ST_IDLE: if (fft_v && fft_idx == 0 && frame_selected) begin
                    pkt_word <= {fft_r[23:8], fft_im[23:8]}; // First SC16 word
                    pkt_v <= 1;
                    pkt_cnt <= 1;
                    app_len_sig <= PACKET_DATA_BYTES + 16; // 1024B data + 16B metadata
                    pkt_state <= ST_DATA;
                end
                ST_DATA: if (fft_v) begin
                    pkt_word <= {fft_r[23:8], fft_im[23:8]};
                    pkt_v <= 1;
                    pkt_cnt <= pkt_cnt + 1;
                    // Trigger metadata footer every 256 bins
                    if (pkt_cnt[7:0] == 8'hFF) pkt_state <= ST_META;
                end
                ST_META: begin
                    // 4-word metadata footer (16 bytes)
                    case (pkt_cnt[1:0])
                        2'b00: pkt_word <= 32'hFEEDFEED; // Sub-packet Sync
                        2'b01: pkt_word <= fft_fc;
                        2'b10: pkt_word <= timestamp_cnt;
                        2'b11: begin
                            pkt_word <= {16'h0, 15'h0, fft_ov};
                            if (pkt_cnt == 1024+3) pkt_state <= ST_IDLE; // End of frame
                            else pkt_state <= ST_DATA;
                        end
                    endcase
                    pkt_v <= 1;
                    pkt_cnt <= pkt_cnt + 1;
                end
            endcase
        end
    end

    // ========================================================================
    // CDC & UDP (125 MHz)
    // ========================================================================
    wire [31:0] cdc_dout;
    wire cdc_empty, stack_ready;
    async_fifo #(.WIDTH(32), .DEPTH(2048)) u_cdc (
        .wr_clk(clk_105m), .rd_clk(clk_125m_eth), .wr_rst_n(reset_n), .rd_rst_n(reset_n),
        .din(pkt_word), .wr_en(pkt_v), .rd_en(!cdc_empty && stack_ready),
        .dout(cdc_dout), .full(), .empty(cdc_empty)
    );

    wire [31:0] eth_data;
    wire [15:0] eth_len;
    wire eth_v, eth_ack;
    udp_ip_stack u_udp (
        .clk(clk_125m_eth), .rst_n(reset_n), .app_data(cdc_dout), .app_len(app_len_sig),
        .app_valid(!cdc_empty), .app_ready(stack_ready),
        .src_ip(32'hC0A80002), .dst_ip(32'hC0A80001), .src_port(16'd10000), .dst_port(16'd10001),
        .mac_data(eth_data), .mac_len(eth_len), .mac_valid(eth_v)
    );

    ethernet_mac u_mac (
        .clk(clk_125m_eth), .rst_n(reset_n),
        .gmii_tx_d(gmii_tx_d), .gmii_tx_en(gmii_tx_en), .gmii_tx_er(gmii_tx_er),
        .gmii_rx_d(gmii_rx_d), .gmii_rx_dv(gmii_rx_dv), .gmii_rx_er(gmii_rx_er),
        .packet_data(eth_data), .packet_len(eth_len), .packet_valid(eth_v), .packet_ack(eth_ack),
        .link_status(eth_link_status_int), .packet_counter(packet_counter)
    );

    assign pll_locked = pll_locked_int;
    assign eth_link_status = eth_link_status_int;
    assign processing_mode_out = processing_mode;
    assign modulation_type_out = modulation_type;
    assign gmii_crs = 1'b0; assign gmii_col = 1'b0;
    assign gmii_tx_clk = clk_125m_eth; assign gmii_rx_clk = clk_125m_eth;

endmodule
