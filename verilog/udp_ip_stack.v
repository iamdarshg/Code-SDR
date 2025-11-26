// ============================================================================
// UDP/IP Protocol Stack Module
// ============================================================================
// Lightweight UDP/IPv4 stack optimized for SDR data transmission
// ============================================================================

`timescale 1ns/1ps

module udp_ip_stack #(
    parameter DATA_WIDTH = 32
) (
    input  wire        clk,             // 125 MHz system clock
    input  wire        rst_n,           // Reset (active low)
    
    // Application data interface
    input  wire [DATA_WIDTH-1:0] app_data,   // Application data
    input  wire [15:0]  app_len,             // Application data length
    input  wire        app_valid,            // Application data valid
    output wire        app_ready,            // Application ready
    
    // UDP parameters
    input  wire [31:0] src_ip,               // Source IP address
    input  wire [31:0] dst_ip,               // Destination IP address
    input  wire [15:0] src_port,             // Source UDP port
    input  wire [15:0] dst_port,             // Destination UDP port
    
    // MAC interface
    output wire [DATA_WIDTH-1:0] mac_data,   // MAC layer data
    output wire [15:0]  mac_len,             // MAC packet length
    output wire        mac_valid             // MAC data valid
);

    // ========================================================================
    // UDP header construction
    // ========================================================================
    
    // UDP header fields
    wire [15:0] udp_length;
    wire [15:0] udp_checksum;
    
    // ============================================================================
    // High-throughput IQ streaming optimization - Reduce protocol overhead
    // ============================================================================

    // UDP length = UDP header (8 bytes) + data length
    // For IQ streaming, use jumbo packets to minimize overhead per sample
    reg [15:0] udp_length_reg;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            udp_length_reg <= 16'd8;
        end else if (app_valid) begin
            udp_length_reg <= 16'd8 + app_len;
        end
    end
    assign udp_length = udp_length_reg;

    // UDP checksum calculation (disabled for high throughput - checksum can be disabled for performance)
    // When FFT is disabled and IQ streaming is active, prioritize throughput over error detection
    assign udp_checksum = 16'd0;  // Simplified for 900+ Mbps throughput
    
    // ========================================================================
    // IPv4 header construction
    // ========================================================================
    
    // IPv4 header fields
    wire [7:0]  ipv4_version_ihl;           // Version (4) + IHL (5)
    wire [7:0]  ipv4_dscp_ecn;              // DSCP + ECN
    wire [15:0] ipv4_total_length;          // Total packet length
    wire [15:0] ipv4_identification;        // Identification
    wire [15:0] ipv4_flags_fragment;        // Flags + Fragment offset
    wire [7:0]  ipv4_ttl;                   // Time to live
    wire [7:0]  ipv4_protocol;              // Protocol (17 for UDP)
    wire [15:0] ipv4_header_checksum;       // Header checksum
    wire [31:0] ipv4_source_ip;             // Source IP address
    wire [31:0] ipv4_dest_ip;               // Destination IP address
    
    // IPv4 header values
    assign ipv4_version_ihl = 8'h45;        // Version 4, IHL 5 (20 bytes)
    assign ipv4_dscp_ecn = 8'h00;           // No DSCP/ECN
    assign ipv4_total_length = 20 + udp_length;  // IP header + UDP header + data
    assign ipv4_identification = 16'h0001;  // Simple identification
    assign ipv4_flags_fragment = 16'h4000;  // Don't fragment flag
    assign ipv4_ttl = 8'h40;                // TTL = 64
    assign ipv4_protocol = 8'h11;           // UDP protocol
    assign ipv4_header_checksum = 16'h0000; // Simplified - no checksum
    assign ipv4_source_ip = src_ip;
    assign ipv4_dest_ip = dst_ip;
    
    // ========================================================================
    // Frame assembly state machine
    // ========================================================================
    
    localparam IDLE     = 3'd0;
    localparam IP_HDR   = 3'd1;
    localparam UDP_HDR  = 3'd2;
    localparam DATA     = 3'd3;
    localparam DONE     = 3'd4;
    
    reg [2:0] frame_state;
    reg [4:0] byte_counter;  // Counter for bytes in current field
    reg [15:0] total_bytes;  // Total bytes transmitted
    reg [DATA_WIDTH-1:0] packet_data;
    
    // Frame assembly logic
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            frame_state <= IDLE;
            byte_counter <= 5'd0;
            total_bytes <= 16'd0;
            packet_data <= 32'd0;
        end else begin
            case (frame_state)
                IDLE: begin
                    if (app_valid && app_ready) begin
                        frame_state <= IP_HDR;
                        byte_counter <= 5'd0;
                        total_bytes <= 16'd0;
                    end
                end
                
                IP_HDR: begin
                    // Transmit IPv4 header
                    if (byte_counter < 5'd20) begin  // 20 bytes IP header
                        // Select current IP header byte
                        case (byte_counter)
                            5'd0:  packet_data <= {ipv4_version_ihl, ipv4_dscp_ecn, ipv4_total_length[15:8], ipv4_total_length[7:0]};
                            5'd1:  packet_data <= {ipv4_identification[15:8], ipv4_identification[7:0], ipv4_flags_fragment[15:8], ipv4_flags_fragment[7:0]};
                            5'd2:  packet_data <= {ipv4_ttl, ipv4_protocol, ipv4_header_checksum[15:8], ipv4_header_checksum[7:0]};
                            5'd3:  packet_data <= ipv4_source_ip;
                            5'd4:  packet_data <= ipv4_dest_ip;
                            default: packet_data <= 32'd0;
                        endcase
                        byte_counter <= byte_counter + 1;
                        total_bytes <= total_bytes + 4;
                        
                        if (byte_counter == 5'd19) begin
                            frame_state <= UDP_HDR;
                            byte_counter <= 5'd0;
                        end
                    end
                end
                
                UDP_HDR: begin
                    // Transmit UDP header
                    if (byte_counter < 5'd8) begin  // 8 bytes UDP header
                        case (byte_counter)
                            5'd0:  packet_data <= {src_port[15:8], src_port[7:0], dst_port[15:8], dst_port[7:0]};
                            5'd1:  packet_data <= {udp_length[15:8], udp_length[7:0], udp_checksum[15:8], udp_checksum[7:0]};
                            default: packet_data <= 32'd0;
                        endcase
                        byte_counter <= byte_counter + 1;
                        total_bytes <= total_bytes + 4;
                        
                        if (byte_counter == 5'd7) begin
                            frame_state <= DATA;
                            byte_counter <= 5'd0;
                        end
                    end
                end
                
                DATA: begin
                    // Transmit application data
                    if (total_bytes < (16'd20 + udp_length)) begin
                        packet_data <= app_data;
                        total_bytes <= total_bytes + 4;
                        
                        if (total_bytes >= (16'd20 + udp_length - 4)) begin
                            frame_state <= DONE;
                        end
                    end
                end
                
                DONE: begin
                    frame_state <= IDLE;
                end
                
                default: frame_state <= IDLE;
            endcase
        end
    end
    
    // ========================================================================
    // Control signals
    // ========================================================================
    
    // Application ready when in IDLE state
    assign app_ready = (frame_state == IDLE);
    
    // MAC interface signals
    assign mac_data = (frame_state != IDLE) ? packet_data : 32'd0;
    assign mac_len = total_bytes;
    assign mac_valid = (frame_state != IDLE && frame_state != DONE);

endmodule
