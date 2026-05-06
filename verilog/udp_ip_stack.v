`default_nettype none
// ============================================================================
// UDP/IP Protocol Stack Module
// ============================================================================
// Transmit-only UDP/IPv4 framer for 32-bit application packet streams.
//
// The first app word accepted in IDLE starts a UDP packet and latches app_len.
// mac_ready is a per-word ready signal from the MAC: it is asserted for the
// first word of a packet and for each later 32-bit word boundary. The stack
// holds mac_valid/mac_data stable until that ready/valid handshake completes.
// ============================================================================

`timescale 1ns/1ps

module udp_ip_stack #(
    parameter DATA_WIDTH = 32
) (
    input  wire                  clk,
    input  wire                  rst_n,

    input  wire [DATA_WIDTH-1:0] app_data,
    input  wire [15:0]           app_len,
    input  wire                  app_valid,
    output wire                  app_ready,

    input  wire [31:0]           src_ip,
    input  wire [31:0]           dst_ip,
    input  wire [15:0]           src_port,
    input  wire [15:0]           dst_port,

    output wire [DATA_WIDTH-1:0] mac_data,
    output wire [15:0]           mac_len,
    output wire                  mac_valid,
    input  wire                  mac_ready
);

    localparam IDLE    = 3'd0;
    localparam IP_HDR  = 3'd1;
    localparam UDP_HDR = 3'd2;
    localparam DATA    = 3'd3;

    reg [2:0] state;
    reg [2:0] header_word;
    reg [15:0] payload_bytes_remaining;
    reg [15:0] app_len_latched;
    reg [31:0] first_payload_word;
    reg        first_payload_pending;
    reg [DATA_WIDTH-1:0] packet_data;
    reg                  packet_valid_reg;
    reg [15:0]           mac_len_reg;

    wire [15:0] udp_length = 16'd8 + app_len_latched;
    wire [15:0] ipv4_total_length = 16'd20 + udp_length;
    wire [15:0] ipv4_header_checksum =
        ipv4_checksum(ipv4_total_length, src_ip, dst_ip);
    wire output_can_advance = !packet_valid_reg || mac_ready;

    assign app_ready = ((state == IDLE) && output_can_advance) ||
                       ((state == DATA) && !first_payload_pending && output_can_advance);
    assign mac_data = packet_data;
    assign mac_len = mac_len_reg;
    assign mac_valid = packet_valid_reg;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= IDLE;
            header_word <= 3'd0;
            payload_bytes_remaining <= 16'd0;
            app_len_latched <= 16'd0;
            first_payload_word <= 32'd0;
            first_payload_pending <= 1'b0;
            packet_data <= {DATA_WIDTH{1'b0}};
            packet_valid_reg <= 1'b0;
            mac_len_reg <= 16'd0;
        end else if (output_can_advance) begin
            packet_valid_reg <= 1'b0;

            case (state)
                IDLE: begin
                    header_word <= 3'd1;

                    if (app_valid) begin
                        app_len_latched <= app_len;
                        payload_bytes_remaining <= app_len;
                        mac_len_reg <= 16'd28 + app_len;
                        first_payload_word <= app_data;
                        first_payload_pending <= 1'b1;
                        packet_data <= {8'h45, 8'h00, 16'd28 + app_len};
                        packet_valid_reg <= 1'b1;
                        state <= IP_HDR;
                    end
                end

                IP_HDR: begin
                    packet_valid_reg <= 1'b1;

                    case (header_word)
                        3'd1: packet_data <= {16'h0001, 16'h4000};
                        3'd2: packet_data <= {8'h40, 8'h11, ipv4_header_checksum};
                        3'd3: packet_data <= src_ip;
                        3'd4: packet_data <= dst_ip;
                        default: packet_data <= 32'd0;
                    endcase

                    if (header_word == 3'd4) begin
                        header_word <= 3'd0;
                        state <= UDP_HDR;
                    end else begin
                        header_word <= header_word + 1'b1;
                    end
                end

                UDP_HDR: begin
                    packet_valid_reg <= 1'b1;

                    case (header_word)
                        3'd0: packet_data <= {src_port, dst_port};
                        3'd1: packet_data <= {udp_length, 16'h0000};
                        default: packet_data <= 32'd0;
                    endcase

                    if (header_word == 3'd1) begin
                        header_word <= 3'd0;
                        if (payload_bytes_remaining == 16'd0) begin
                            state <= IDLE;
                            first_payload_pending <= 1'b0;
                        end else begin
                            state <= DATA;
                        end
                    end else begin
                        header_word <= header_word + 1'b1;
                    end
                end

                DATA: begin
                    if (first_payload_pending) begin
                        packet_data <= first_payload_word;
                        packet_valid_reg <= 1'b1;
                        first_payload_pending <= 1'b0;

                        if (payload_bytes_remaining <= 16'd4) begin
                            payload_bytes_remaining <= 16'd0;
                            state <= IDLE;
                        end else begin
                            payload_bytes_remaining <= payload_bytes_remaining - 16'd4;
                        end
                    end else if (app_valid) begin
                        packet_data <= app_data;
                        packet_valid_reg <= 1'b1;

                        if (payload_bytes_remaining <= 16'd4) begin
                            payload_bytes_remaining <= 16'd0;
                            state <= IDLE;
                        end else begin
                            payload_bytes_remaining <= payload_bytes_remaining - 16'd4;
                        end
                    end
                end

                default: begin
                    state <= IDLE;
                    header_word <= 3'd0;
                    payload_bytes_remaining <= 16'd0;
                    first_payload_pending <= 1'b0;
                end
            endcase
        end
    end

    function [15:0] ipv4_checksum;
        input [15:0] total_length;
        input [31:0] src_addr;
        input [31:0] dst_addr;
        reg [19:0] sum;
        reg [19:0] folded;
        begin
            sum = 20'd0;
            sum = sum + 16'h4500;
            sum = sum + total_length;
            sum = sum + 16'h0001;
            sum = sum + 16'h4000;
            sum = sum + 16'h4011;
            sum = sum + src_addr[31:16];
            sum = sum + src_addr[15:0];
            sum = sum + dst_addr[31:16];
            sum = sum + dst_addr[15:0];
            folded = sum[15:0] + sum[19:16];
            folded = folded[15:0] + folded[19:16];
            ipv4_checksum = ~folded[15:0];
        end
    endfunction

endmodule
`default_nettype wire
