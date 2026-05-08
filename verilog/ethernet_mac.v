// ============================================================================
// Ethernet MAC Layer Module
// ============================================================================
// GMII/MII interface for KSZ9031RNXCC Gigabit Ethernet PHY
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module ethernet_mac #(
    parameter DATA_WIDTH = 32
) (
    // System interface
    input  wire        clk,             // 125 MHz system clock
    input  wire        rst_n,           // Reset (active low)
    
    // GMII interface
    output wire [7:0]  gmii_tx_d,       // GMII transmit data
    output wire        gmii_tx_en,      // GMII transmit enable
    output wire        gmii_tx_er,      // GMII transmit error
    input  wire [7:0]  gmii_rx_d,       // GMII receive data
    input  wire        gmii_rx_dv,      // GMII receive data valid
    input  wire        gmii_rx_er,      // GMII receive error
    
    // Transmit configuration
    input  wire [47:0] src_mac,
    input  wire [47:0] dst_mac,

    // Streaming transmit interface. packet_ack is the per-word ready signal:
    // a transfer occurs when packet_valid and packet_ack are both high.
    input  wire [DATA_WIDTH-1:0] packet_data,    // Packet word to transmit
    input  wire [15:0]  packet_len,               // Total packet length in bytes
    input  wire        packet_valid,              // Packet word valid flag
    output wire        packet_ack,                // Packet word accepted

    // Receive data interface
    output wire [DATA_WIDTH-1:0] rx_packet_data,  // Received packet data
    output wire [15:0]  rx_packet_len,            // Received packet length
    output reg         rx_packet_valid,           // Received packet valid
    input  wire        rx_packet_ack,             // Received packet acknowledged

    // Status signals
    output wire        link_status,               // Ethernet link status
    output wire [31:0] tx_packet_counter,         // TX Packet counter
    output wire [31:0] rx_packet_counter          // RX Packet counter
);

    // ========================================================================
    // TX State Machine
    // ========================================================================
    
    localparam TX_IDLE      = 3'd0;
    localparam TX_PREAMBLE  = 3'd1;
    localparam TX_SFD       = 3'd2;
    localparam TX_MAC_HDR   = 3'd3;
    localparam TX_DATA      = 4'd4;
    localparam TX_PADDING   = 4'd5;
    localparam TX_CRC       = 4'd6;
    localparam TX_GAP       = 4'd7;
    
    reg [2:0] tx_state;
    reg [15:0] tx_byte_counter;
    reg [31:0] crc32_reg;
    reg [7:0] tx_data_reg;
    reg       tx_en_reg;
    reg [15:0] packet_len_reg;
    reg [31:0] tx_pkt_count_reg;
    reg [DATA_WIDTH-1:0] tx_word_reg;
    
    // CRC32 calculation for Ethernet (0xEDB88320 reversed polynomial)
    function [31:0] next_crc32(input [31:0] crc, input [7:0] data);
        reg [31:0] c;
        integer i;
        begin
            c = crc ^ {24'h0, data};
            for (i = 0; i < 8; i = i + 1) begin
                if (c[0])
                    c = (c >> 1) ^ 32'hEDB88320;
                else
                    c = c >> 1;
            end
            next_crc32 = c;
        end
    endfunction

    wire tx_word_ready = (tx_state == TX_IDLE) ||
                         (tx_state == TX_DATA &&
                          tx_byte_counter < packet_len_reg &&
                          tx_byte_counter[1:0] == 2'b00);
    wire tx_word_fire = tx_word_ready && packet_valid;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            tx_state <= TX_IDLE;
            tx_byte_counter <= 16'd0;
            crc32_reg <= 32'hFFFFFFFF;
            tx_data_reg <= 8'd0;
            tx_en_reg <= 1'b0;
            packet_len_reg <= 16'd0;
            tx_pkt_count_reg <= 32'd0;
            tx_word_reg <= {DATA_WIDTH{1'b0}};
        end else begin
            case (tx_state)
                TX_IDLE: begin
                    tx_en_reg <= 1'b0;
                    if (tx_word_fire) begin
                        packet_len_reg <= packet_len;
                        tx_word_reg <= packet_data;
                        tx_byte_counter <= 16'd0;
                        crc32_reg <= 32'hFFFFFFFF;
                        tx_state <= TX_PREAMBLE;
                    end
                end
                
                TX_PREAMBLE: begin
                    tx_data_reg <= 8'h55;
                    tx_en_reg <= 1'b1;
                    if (tx_byte_counter < 16'd6) begin
                        tx_byte_counter <= tx_byte_counter + 1;
                    end else begin
                        tx_state <= TX_SFD;
                        tx_byte_counter <= 16'd0;
                    end
                end
                
                TX_SFD: begin
                    tx_data_reg <= 8'hD5;
                    tx_state <= TX_MAC_HDR;
                    tx_byte_counter <= 16'd0;
                end
                
                TX_MAC_HDR: begin
                    case (tx_byte_counter)
                        16'd0:  tx_data_reg <= dst_mac[47:40];
                        16'd1:  tx_data_reg <= dst_mac[39:32];
                        16'd2:  tx_data_reg <= dst_mac[31:24];
                        16'd3:  tx_data_reg <= dst_mac[23:16];
                        16'd4:  tx_data_reg <= dst_mac[15:8];
                        16'd5:  tx_data_reg <= dst_mac[7:0];
                        16'd6:  tx_data_reg <= src_mac[47:40];
                        16'd7:  tx_data_reg <= src_mac[39:32];
                        16'd8:  tx_data_reg <= src_mac[31:24];
                        16'd9:  tx_data_reg <= src_mac[23:16];
                        16'd10: tx_data_reg <= src_mac[15:8];
                        16'd11: tx_data_reg <= src_mac[7:0];
                        16'd12: tx_data_reg <= 8'h08; // EtherType IPv4
                        16'd13: tx_data_reg <= 8'h00;
                        default: tx_data_reg <= 8'd0;
                    endcase
                    
                    crc32_reg <= next_crc32(crc32_reg, tx_data_reg);

                    if (tx_byte_counter < 16'd13) begin
                        tx_byte_counter <= tx_byte_counter + 1;
                    end else begin
                        tx_state <= TX_DATA;
                        tx_byte_counter <= 16'd0;
                    end
                end
                
                TX_DATA: begin
                    if (tx_word_fire) begin
                        tx_word_reg <= packet_data;
                    end

                    case (tx_byte_counter[1:0])
                        2'b00: tx_data_reg <= tx_word_fire ? packet_data[7:0] : tx_word_reg[7:0];
                        2'b01: tx_data_reg <= tx_word_reg[15:8];
                        2'b10: tx_data_reg <= tx_word_reg[23:16];
                        2'b11: tx_data_reg <= tx_word_reg[31:24];
                    endcase

                    crc32_reg <= next_crc32(crc32_reg, tx_data_reg);
                    tx_byte_counter <= tx_byte_counter + 1;

                    if (tx_byte_counter == packet_len_reg - 1) begin
                        if (14 + packet_len_reg < 60) begin
                            tx_state <= TX_PADDING;
                        end else begin
                            tx_state <= TX_CRC;
                            tx_byte_counter <= 16'd0;
                        end
                    end
                end

                TX_PADDING: begin
                    tx_data_reg <= 8'h00;
                    crc32_reg <= next_crc32(crc32_reg, 8'h00);
                    tx_byte_counter <= tx_byte_counter + 1;
                    if (14 + tx_byte_counter == 59) begin
                        tx_state <= TX_CRC;
                        tx_byte_counter <= 16'd0;
                    end
                end
                
                TX_CRC: begin
                    case (tx_byte_counter[1:0])
                        2'b00: tx_data_reg <= ~crc32_reg[0  +: 8];
                        2'b01: tx_data_reg <= ~crc32_reg[8  +: 8];
                        2'b10: tx_data_reg <= ~crc32_reg[16 +: 8];
                        2'b11: tx_data_reg <= ~crc32_reg[24 +: 8];
                    endcase
                    
                    if (tx_byte_counter < 16'd3) begin
                        tx_byte_counter <= tx_byte_counter + 1;
                    end else begin
                        tx_state <= TX_GAP;
                        tx_byte_counter <= 16'd0;
                    end
                end
                
                TX_GAP: begin
                    tx_en_reg <= 1'b0;
                    if (tx_byte_counter < 16'd11) begin
                        tx_byte_counter <= tx_byte_counter + 1;
                    end else begin
                        tx_pkt_count_reg <= tx_pkt_count_reg + 1;
                        tx_state <= TX_IDLE;
                    end
                end
                
                default: tx_state <= TX_IDLE;
            endcase
        end
    end
    
    // ========================================================================
    // RX State Machine
    // ========================================================================

    localparam RX_IDLE      = 4'd0;
    localparam RX_PREAMBLE  = 4'd1;
    localparam RX_SFD       = 4'd2;
    localparam RX_MAC_HDR   = 4'd3;
    localparam RX_DATA      = 4'd4;
    localparam RX_CRC       = 4'd5;
    localparam RX_DONE      = 4'd6;
    localparam RX_ERROR     = 4'd7;

    reg [3:0] rx_state;
    reg [15:0] rx_byte_count;
    reg [31:0] rx_pkt_count_reg;
    reg link_status_reg;

    reg [DATA_WIDTH-1:0] rx_data_buffer [0:511];
    reg [15:0] rx_buffer_write_ptr;
    reg [15:0] rx_frame_length;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rx_state <= RX_IDLE;
            rx_byte_count <= 16'd0;
            link_status_reg <= 1'b0;
            rx_buffer_write_ptr <= 16'd0;
            rx_frame_length <= 16'd0;
            rx_packet_valid <= 1'b0;
            rx_pkt_count_reg <= 32'd0;
        end else begin
            case (rx_state)
                RX_IDLE: begin
                    rx_packet_valid <= 1'b0;
                    rx_buffer_write_ptr <= 16'd0;
                    if (gmii_rx_dv && gmii_rx_d == 8'h55) begin
                        rx_state <= RX_PREAMBLE;
                        link_status_reg <= 1'b1;
                    end
                end

                RX_PREAMBLE: begin
                    if (gmii_rx_dv) begin
                        if (gmii_rx_d == 8'hD5) begin
                            rx_state <= RX_MAC_HDR;
                            rx_byte_count <= 16'd0;
                        end
                    end else rx_state <= RX_IDLE;
                end

                RX_MAC_HDR: begin
                    if (gmii_rx_dv) begin
                        if (rx_byte_count == 16'd13) begin
                            rx_state <= RX_DATA;
                            rx_byte_count <= 16'd0;
                        end else rx_byte_count <= rx_byte_count + 1;
                    end else rx_state <= RX_IDLE;
                end

                RX_DATA: begin
                    if (gmii_rx_dv) begin
                        if (rx_byte_count[1:0] == 2'b00) rx_data_buffer[rx_byte_count[10:2]][7:0] <= gmii_rx_d;
                        else if (rx_byte_count[1:0] == 2'b01) rx_data_buffer[rx_byte_count[10:2]][15:8] <= gmii_rx_d;
                        else if (rx_byte_count[1:0] == 2'b10) rx_data_buffer[rx_byte_count[10:2]][23:16] <= gmii_rx_d;
                        else rx_data_buffer[rx_byte_count[10:2]][31:24] <= gmii_rx_d;

                        rx_byte_count <= rx_byte_count + 1;
                    end else begin
                        rx_frame_length <= rx_byte_count;
                        rx_pkt_count_reg <= rx_pkt_count_reg + 1;
                        rx_state <= RX_DONE;
                        rx_packet_valid <= 1'b1;
                    end
                end

                RX_DONE: rx_state <= RX_IDLE;
                default: rx_state <= RX_IDLE;
            endcase
        end
    end

    reg [15:0] rx_read_ptr;
    reg [15:0] rx_words_available;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rx_read_ptr <= 16'd0;
            rx_words_available <= 16'd0;
        end else begin
            if (rx_packet_valid && rx_packet_ack) begin
                rx_words_available <= (rx_frame_length + 3) >> 2;
                rx_read_ptr <= 16'd0;
            end else if (rx_words_available > 0) begin
                rx_read_ptr <= rx_read_ptr + 1;
                if (rx_read_ptr >= rx_words_available - 1) begin
                    rx_words_available <= 16'd0;
                end
            end
        end
    end

    assign gmii_tx_d = tx_data_reg;
    assign gmii_tx_en = tx_en_reg;
    assign gmii_tx_er = 1'b0;
    assign packet_ack = tx_word_ready;
    assign link_status = link_status_reg;
    assign tx_packet_counter = tx_pkt_count_reg;
    assign rx_packet_counter = rx_pkt_count_reg;
    assign rx_packet_data = (rx_words_available > 0) ? rx_data_buffer[rx_read_ptr[8:0]] : 32'd0;
    assign rx_packet_len = rx_frame_length;

endmodule
`default_nettype wire
