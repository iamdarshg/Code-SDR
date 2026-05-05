`default_nettype none
// ============================================================================
// UDP/IP Protocol Stack Module
// ============================================================================
// Transmit-only UDP/IPv4 framer for 32-bit application packet streams.
// The first app word accepted in IDLE starts a UDP packet and latches app_len.
// DATA then consumes app_len bytes from the app stream, one 32-bit word at a
// time, so 256-bin FFT subframes are emitted as real UDP payloads.
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
    output wire                  mac_valid
);

    localparam IDLE    = 3'd0;
    localparam IP_HDR  = 3'd1;
    localparam UDP_HDR = 3'd2;
    localparam DATA    = 3'd3;
    localparam DONE    = 3'd4;

    reg [2:0] state;
    reg [2:0] header_word;
    reg [15:0] payload_bytes_remaining;
    reg [15:0] app_len_latched;
    reg [31:0] first_payload_word;
    reg        first_payload_pending;
    reg [DATA_WIDTH-1:0] packet_data;
    reg packet_valid_reg;
    reg [15:0] mac_len_reg;

    wire [15:0] udp_length = 16'd8 + app_len_latched;
    wire [15:0] ipv4_total_length = 16'd20 + udp_length;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= IDLE;
            header_word <= 3'd0;
            payload_bytes_remaining <= 16'd0;
            app_len_latched <= 16'd0;
            first_payload_word <= 32'd0;
            first_payload_pending <= 1'b0;
            packet_data <= 32'd0;
            packet_valid_reg <= 1'b0;
            mac_len_reg <= 16'd0;
        end else begin
            packet_valid_reg <= 1'b0;

            case (state)
                IDLE: begin
                    header_word <= 3'd0;
                    if (app_valid) begin
                        app_len_latched <= app_len;
                        payload_bytes_remaining <= app_len;
                        mac_len_reg <= 16'd28 + app_len;
                        first_payload_word <= app_data;
                        first_payload_pending <= 1'b1;
                        state <= IP_HDR;
                    end
                end

                IP_HDR: begin
                    packet_valid_reg <= 1'b1;
                    case (header_word)
                        3'd0: packet_data <= {8'h45, 8'h00, ipv4_total_length};
                        3'd1: packet_data <= {16'h0001, 16'h4000};
                        3'd2: packet_data <= {8'h40, 8'h11, 16'h0000};
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
                        state <= DATA;
                    end else begin
                        header_word <= header_word + 1'b1;
                    end
                end

                DATA: begin
                    if (first_payload_pending) begin
                        packet_data <= first_payload_word;
                        packet_valid_reg <= 1'b1;
                        first_payload_pending <= 1'b0;
                        payload_bytes_remaining <= (payload_bytes_remaining > 16'd4) ?
                                                   payload_bytes_remaining - 16'd4 : 16'd0;
                        if (payload_bytes_remaining <= 16'd4) begin
                            state <= DONE;
                        end
                    end else if (app_valid) begin
                        packet_data <= app_data;
                        packet_valid_reg <= 1'b1;
                        payload_bytes_remaining <= (payload_bytes_remaining > 16'd4) ?
                                                   payload_bytes_remaining - 16'd4 : 16'd0;
                        if (payload_bytes_remaining <= 16'd4) begin
                            state <= DONE;
                        end
                    end
                end

                DONE: begin
                    state <= IDLE;
                end

                default: begin
                    state <= IDLE;
                end
            endcase
        end
    end

    assign app_ready = (state == IDLE) || ((state == DATA) && !first_payload_pending);
    assign mac_data = packet_data;
    assign mac_len = mac_len_reg;
    assign mac_valid = packet_valid_reg;

endmodule
`default_nettype wire
