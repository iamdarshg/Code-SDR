`default_nettype none
// ============================================================================
// UDP/IP Protocol Stack Module
// ============================================================================
// Lightweight transmit-only UDP/IPv4 framer for 32-bit application words.
// app_data and app_len are latched together when app_valid && app_ready.
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
    reg [2:0] word_counter;
    reg [DATA_WIDTH-1:0] packet_data;
    reg [DATA_WIDTH-1:0] app_data_latched;
    reg [15:0] app_len_latched;
    reg [15:0] mac_len_reg;

    wire [15:0] udp_length = 16'd8 + app_len_latched;
    wire [15:0] ipv4_total_length = 16'd20 + udp_length;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= IDLE;
            word_counter <= 3'd0;
            packet_data <= {DATA_WIDTH{1'b0}};
            app_data_latched <= {DATA_WIDTH{1'b0}};
            app_len_latched <= 16'd0;
            mac_len_reg <= 16'd0;
        end else begin
            case (state)
                IDLE: begin
                    word_counter <= 3'd0;
                    packet_data <= {DATA_WIDTH{1'b0}};
                    if (app_valid) begin
                        app_data_latched <= app_data;
                        app_len_latched <= app_len;
                        mac_len_reg <= 16'd28 + app_len;
                        state <= IP_HDR;
                    end
                end

                IP_HDR: begin
                    case (word_counter)
                        3'd0: packet_data <= {8'h45, 8'h00, ipv4_total_length};
                        3'd1: packet_data <= {16'h0001, 16'h4000};
                        3'd2: packet_data <= {8'h40, 8'h11, 16'h0000};
                        3'd3: packet_data <= src_ip;
                        3'd4: packet_data <= dst_ip;
                        default: packet_data <= {DATA_WIDTH{1'b0}};
                    endcase

                    if (word_counter == 3'd4) begin
                        word_counter <= 3'd0;
                        state <= UDP_HDR;
                    end else begin
                        word_counter <= word_counter + 1'b1;
                    end
                end

                UDP_HDR: begin
                    case (word_counter)
                        3'd0: packet_data <= {src_port, dst_port};
                        3'd1: packet_data <= {udp_length, 16'h0000};
                        default: packet_data <= {DATA_WIDTH{1'b0}};
                    endcase

                    if (word_counter == 3'd1) begin
                        word_counter <= 3'd0;
                        state <= DATA;
                    end else begin
                        word_counter <= word_counter + 1'b1;
                    end
                end

                DATA: begin
                    packet_data <= app_data_latched;
                    state <= DONE;
                end

                DONE: begin
                    packet_data <= {DATA_WIDTH{1'b0}};
                    state <= IDLE;
                end

                default: begin
                    state <= IDLE;
                end
            endcase
        end
    end

    assign app_ready = (state == IDLE);
    assign mac_data = packet_data;
    assign mac_len = mac_len_reg;
    assign mac_valid = (state == IP_HDR) || (state == UDP_HDR) || (state == DATA);

endmodule
