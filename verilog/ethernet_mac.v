// ============================================================================
// Ethernet MAC Layer Module
// ============================================================================
// GMII/MII interface for KSZ9031RNXCC Gigabit Ethernet PHY
// ============================================================================

`timescale 1ns/1ps

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
    
    // Data interface
    input  wire [DATA_WIDTH-1:0] packet_data,    // Packet data to transmit
    input  wire [15:0]  packet_len,               // Packet length
    input  wire        packet_valid,              // Packet valid flag
    output wire        packet_ack,                // Packet acknowledged
    
    // Status signals
    output wire        link_status,               // Ethernet link status
    output wire [31:0] packet_counter             // Packet counter
);

    // ========================================================================
    // MAC address configuration
    // ========================================================================
    
    // Local MAC address (48-bit): 02:00:00:00:00:01
    localparam [47:0] LOCAL_MAC = 48'h020000000001;
    
    // ========================================================================
    // TX State Machine
    // ========================================================================
    
    localparam TX_IDLE      = 4'd0;
    localparam TX_PREAMBLE  = 4'd1;
    localparam TX_SFD       = 4'd2;
    localparam TX_MAC_HDR   = 4'd3;
    localparam TX_DATA      = 4'd4;
    localparam TX_CRC       = 4'd5;
    localparam TX_GAP       = 4'd6;
    
    reg [3:0] tx_state;
    reg [7:0] tx_byte_counter;
    reg [31:0] crc32_reg;
    reg [7:0] tx_data_reg;
    reg       tx_en_reg;
    reg       tx_er_reg;
    reg [15:0] packet_len_reg;
    
    // TX data byte extraction
    wire [7:0] tx_data_byte;
    assign tx_data_byte = packet_data[tx_byte_counter[1:0] * 8 +: 8];
    
    // CRC32 calculation
    function [31:0] crc32_byte(input [31:0] crc, input [7:0] data);
        reg [31:0] crc_next;
        begin
            crc_next[0]  = crc[24] ^ data[0];
            crc_next[1]  = crc[25] ^ data[1] ^ crc[24];
            crc_next[2]  = crc[26] ^ data[2] ^ crc[25] ^ crc[24];
            crc_next[3]  = crc[27] ^ data[3] ^ crc[26] ^ crc[25] ^ crc[24];
            crc_next[4]  = crc[28] ^ data[4] ^ crc[27] ^ crc[26] ^ crc[25];
            crc_next[5]  = crc[29] ^ data[5] ^ crc[28] ^ crc[27] ^ crc[26];
            crc_next[6]  = crc[30] ^ data[6] ^ crc[29] ^ crc[28] ^ crc[27];
            crc_next[7]  = crc[31] ^ data[7] ^ crc[30] ^ crc[29] ^ crc[28];
            crc32_byte = {crc_next[7:0], crc[23:8]};
        end
    endfunction
    
    // TX state machine
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            tx_state <= TX_IDLE;
            tx_byte_counter <= 8'd0;
            crc32_reg <= 32'hFFFFFFFF;
            tx_data_reg <= 8'd0;
            tx_en_reg <= 1'b0;
            tx_er_reg <= 1'b0;
            packet_len_reg <= 16'd0;
        end else begin
            case (tx_state)
                TX_IDLE: begin
                    if (packet_valid) begin
                        packet_len_reg <= packet_len;
                        tx_byte_counter <= 8'd0;
                        crc32_reg <= 32'hFFFFFFFF;
                        tx_state <= TX_PREAMBLE;
                    end
                    tx_en_reg <= 1'b0;
                    tx_er_reg <= 1'b0;
                end
                
                TX_PREAMBLE: begin
                    // Transmit 7 bytes of 0x55 (preamble)
                    tx_data_reg <= 8'h55;
                    tx_en_reg <= 1'b1;
                    tx_er_reg <= 1'b0;
                    
                    if (tx_byte_counter < 8'd6) begin
                        tx_byte_counter <= tx_byte_counter + 1;
                        // Update CRC for preamble bytes
                        crc32_reg <= crc32_byte(crc32_reg, tx_data_reg);
                    end else begin
                        tx_state <= TX_SFD;
                        tx_byte_counter <= 8'd0;
                    end
                end
                
                TX_SFD: begin
                    // Transmit SFD (Start Frame Delimiter)
                    tx_data_reg <= 8'hD5;
                    if (tx_byte_counter == 8'd0) begin
                        // Update CRC for SFD byte
                        crc32_reg <= crc32_byte(crc32_reg, tx_data_reg);
                        tx_byte_counter <= tx_byte_counter + 1;
                    end else begin
                        tx_state <= TX_MAC_HDR;
                        tx_byte_counter <= 8'd0;
                    end
                end
                
                TX_MAC_HDR: begin
                    // Transmit MAC header (destination, source, type)
                    case (tx_byte_counter)
                        8'd0:  tx_data_reg <= 8'hFF;  // Destination MAC (broadcast)
                        8'd1:  tx_data_reg <= 8'hFF;
                        8'd2:  tx_data_reg <= 8'hFF;
                        8'd3:  tx_data_reg <= 8'hFF;
                        8'd4:  tx_data_reg <= 8'hFF;
                        8'd5:  tx_data_reg <= 8'hFF;
                        8'd6:  tx_data_reg <= LOCAL_MAC[47:40];  // Source MAC
                        8'd7:  tx_data_reg <= LOCAL_MAC[39:32];
                        8'd8:  tx_data_reg <= LOCAL_MAC[31:24];
                        8'd9:  tx_data_reg <= LOCAL_MAC[23:16];
                        8'd10: tx_data_reg <= LOCAL_MAC[15:8];
                        8'd11: tx_data_reg <= LOCAL_MAC[7:0];
                        8'd12: tx_data_reg <= 8'h08;  // Type: IP
                        8'd13: tx_data_reg <= 8'h00;
                        default: tx_data_reg <= 8'd0;
                    endcase
                    
                    if (tx_byte_counter < 8'd14) begin
                        // Update CRC for MAC header
                        crc32_reg <= crc32_byte(crc32_reg, tx_data_reg);
                        tx_byte_counter <= tx_byte_counter + 1;
                    end else begin
                        tx_state <= TX_DATA;
                        tx_byte_counter <= 8'd0;
                    end
                end
                
                TX_DATA: begin
                    // Transmit packet data
                    if (tx_byte_counter < packet_len_reg[15:0]) begin
                        // Extract current byte from packet data
                        case (tx_byte_counter[1:0])
                            2'b00: tx_data_reg <= packet_data[7:0];
                            2'b01: tx_data_reg <= packet_data[15:8];
                            2'b10: tx_data_reg <= packet_data[23:16];
                            2'b11: tx_data_reg <= packet_data[31:24];
                        endcase
                        
                        if (tx_byte_counter[1:0] == 2'b11) begin
                            // Update CRC for every 4 bytes
                            crc32_reg <= crc32_byte(crc32_reg, packet_data[7:0]);
                            crc32_reg <= crc32_byte(crc32_reg, packet_data[15:8]);
                            crc32_reg <= crc32_byte(crc32_reg, packet_data[23:16]);
                            crc32_reg <= crc32_byte(crc32_reg, packet_data[31:24]);
                        end
                        
                        tx_byte_counter <= tx_byte_counter + 1;
                    end else begin
                        tx_state <= TX_CRC;
                        tx_byte_counter <= 8'd0;
                    end
                end
                
                TX_CRC: begin
                    // Transmit CRC32
                    case (tx_byte_counter)
                        8'd0: tx_data_reg <= crc32_reg[7:0];
                        8'd1: tx_data_reg <= crc32_reg[15:8];
                        8'd2: tx_data_reg <= crc32_reg[23:16];
                        8'd3: tx_data_reg <= crc32_reg[31:24];
                        default: tx_data_reg <= 8'd0;
                    endcase
                    
                    if (tx_byte_counter < 8'd4) begin
                        tx_byte_counter <= tx_byte_counter + 1;
                    end else begin
                        tx_state <= TX_GAP;
                        tx_byte_counter <= 8'd0;
                    end
                end
                
                TX_GAP: begin
                    // Inter-frame gap (12 bytes idle)
                    tx_en_reg <= 1'b0;
                    tx_data_reg <= 8'd0;
                    tx_er_reg <= 1'b0;
                    
                    if (tx_byte_counter < 8'd11) begin
                        tx_byte_counter <= tx_byte_counter + 1;
                    end else begin
                        tx_state <= TX_IDLE;
                        tx_byte_counter <= 8'd0;
                    end
                end
                
                default: tx_state <= TX_IDLE;
            endcase
        end
    end
    
    // ========================================================================
    // RX State Machine (simplified)
    // ========================================================================
    
    reg [3:0] rx_state;
    reg [7:0] rx_byte_counter;
    reg [31:0] rx_crc_reg;
    reg link_status_reg;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rx_state <= TX_IDLE;
            rx_byte_counter <= 8'd0;
            rx_crc_reg <= 32'hFFFFFFFF;
            link_status_reg <= 1'b0;
        end else begin
            // Link status detection (simplified)
            link_status_reg <= gmii_rx_dv;
            
            // RX state machine (basic frame detection)
            case (rx_state)
                TX_IDLE: begin
                    if (gmii_rx_dv && gmii_rx_d == 8'h55) begin
                        rx_state <= TX_PREAMBLE;
                        rx_byte_counter <= 8'd0;
                    end
                end
                
                TX_PREAMBLE: begin
                    if (gmii_rx_dv) begin
                        rx_byte_counter <= rx_byte_counter + 1;
                        if (rx_byte_counter >= 8'd6 && gmii_rx_d == 8'hD5) begin
                            rx_state <= TX_MAC_HDR;
                            rx_byte_counter <= 8'd0;
                        end
                    end else begin
                        rx_state <= TX_IDLE;
                    end
                end
                
                TX_MAC_HDR: begin
                    if (gmii_rx_dv) begin
                        rx_byte_counter <= rx_byte_counter + 1;
                        if (rx_byte_counter >= 8'd13) begin
                            rx_state <= TX_DATA;
                            rx_byte_counter <= 8'd0;
                        end
                    end else begin
                        rx_state <= TX_IDLE;
                    end
                end
                
                TX_DATA: begin
                    if (gmii_rx_dv) begin
                        rx_byte_counter <= rx_byte_counter + 1;
                    end else begin
                        // Frame end
                        rx_state <= TX_IDLE;
                    end
                end
                
                default: rx_state <= TX_IDLE;
            endcase
        end
    end
    
    // ========================================================================
    // GMII output assignments
    // ========================================================================
    
    assign gmii_tx_d = tx_data_reg;
    assign gmii_tx_en = tx_en_reg;
    assign gmii_tx_er = tx_er_reg;
    
    // ========================================================================
    // Control and status signals
    // ========================================================================
    
    // Packet acknowledgment
    assign packet_ack = (tx_state == TX_IDLE);
    
    // Link status
    assign link_status = link_status_reg;
    
    // Packet counter
    reg [31:0] packet_counter_reg;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            packet_counter_reg <= 32'd0;
        end else begin
            if (tx_state == TX_GAP && tx_byte_counter == 8'd0) begin
                packet_counter_reg <= packet_counter_reg + 1;
            end
        end
    end
    
    assign packet_counter = packet_counter_reg;

endmodule
