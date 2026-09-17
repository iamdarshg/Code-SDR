// ============================================================================
// v2_eth_mac_tx - transmit-only Ethernet MAC (GMII, 1 byte/cycle @ 125 MHz).
//
// Byte-serial by design: GMII is 8-bit at 125 MHz, i.e. exactly 1000 Mbps, so
// the cheapest structure that can also sustain line rate is a 1-byte/cycle
// pipeline. This matters on the LIF-MD6000 (5,936 LUT4, no DSP blocks).
//
// The MAC owns everything the upper layer should not care about:
//   preamble (7x 0x55) + SFD (0xD5) + DA + SA + EtherType + payload + FCS + IFG
//
// Client contract (streaming, not packet-buffered):
//   * assert s_valid with s_len = IP packet length in bytes, then hold s_valid
//     high and present one byte per cycle on s_data until s_len bytes are taken.
//   * a byte is consumed on a cycle where s_valid and s_ready are both high.
//   * s_ready is high only during the header/payload; it is low during
//     preamble, FCS and IFG.
//
// FCS is the standard Ethernet CRC-32 (reflected 0xEDB88320, init all ones,
// final complement), transmitted least-significant byte first.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_eth_mac_tx #(
    parameter [47:0] DEFAULT_DST_MAC = 48'h02_00_00_00_00_01,
    parameter [47:0] DEFAULT_SRC_MAC = 48'h02_00_00_00_00_02,
    parameter [15:0] ETHERTYPE_IP    = 16'h0800
) (
    input  wire        clk,          // 125 MHz GMII clock
    input  wire        rst_n,

    input  wire [47:0] dst_mac,
    input  wire [47:0] src_mac,

    input  wire [7:0]  s_data,
    input  wire        s_valid,
    input  wire [15:0] s_len,        // length of the IP packet (bytes)
    output wire        s_ready,

    output reg  [7:0]  gmii_tx_d,
    output reg         gmii_tx_en,

    output reg  [31:0] packet_count,
    output reg  [31:0] frame_count
);

    localparam [2:0] S_IDLE = 3'd0, S_PRE = 3'd1, S_HDR = 3'd2,
                     S_PAY  = 3'd3, S_FCS = 3'd4, S_IFG = 3'd5;

    reg [2:0]  state;
    reg [2:0]  pre_cnt;     // 0..7 -> 0x55 x7 then 0xD5
    reg [3:0]  hdr_idx;     // 0..13
    reg [2:0]  fcs_idx;     // 0..3
    reg [3:0]  ifg_cnt;     // 0..11
    reg [15:0] pay_left;
    reg [31:0] crc;

    wire [47:0] da = (dst_mac == 48'h0) ? DEFAULT_DST_MAC : dst_mac;
    wire [47:0] sa = (src_mac == 48'h0) ? DEFAULT_SRC_MAC : src_mac;

    // ------------------------------------------------------------- CRC-32 byte
    function [31:0] crc32_byte;
        input [31:0] c;
        input [7:0]  d;
        integer i;
        reg [31:0] x;
        begin
            x = c ^ {24'h0, d};
            for (i = 0; i < 8; i = i + 1)
                x = x[0] ? ((x >> 1) ^ 32'hEDB88320) : (x >> 1);
            crc32_byte = x;
        end
    endfunction

    function [7:0] hdr_byte;
        input [3:0] i;
        begin
            if (i < 6)
                hdr_byte = da[(5 - i) * 8 +: 8];
            else if (i < 12)
                hdr_byte = sa[(11 - i) * 8 +: 8];
            else if (i == 12)
                hdr_byte = ETHERTYPE_IP[15:8];
            else
                hdr_byte = ETHERTYPE_IP[7:0];
        end
    endfunction

    // FCS payload byte: complement then LSB-first.
    function [7:0] fcs_byte;
        input [31:0] c;
        input [2:0]  i;
        begin
            case (i)
                3'd0: fcs_byte = ~c[7:0];
                3'd1: fcs_byte = ~c[15:8];
                3'd2: fcs_byte = ~c[23:16];
                default: fcs_byte = ~c[31:24];
            endcase
        end
    endfunction

    reg tx_en_r;
    reg [7:0] tx_byte_r;
    reg s_ready_r;

    assign s_ready = s_ready_r;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state      <= S_IDLE;
            pre_cnt    <= 3'd0;
            hdr_idx    <= 4'd0;
            fcs_idx    <= 3'd0;
            ifg_cnt    <= 4'd0;
            pay_left   <= 16'd0;
            crc        <= 32'hffffffff;
            tx_en_r    <= 1'b0;
            tx_byte_r  <= 8'h00;
            s_ready_r  <= 1'b0;
            packet_count <= 32'd0;
            frame_count  <= 32'd0;
        end else begin
            case (state)
                S_IDLE: begin
                    tx_en_r <= 1'b0;
                    s_ready_r <= 1'b0;
                    if (s_valid) begin
                        crc       <= 32'hffffffff;
                        pre_cnt   <= 3'd0;
                        pay_left  <= s_len;
                        state     <= S_PRE;
                    end
                end

                S_PRE: begin
                    tx_en_r   <= 1'b1;
                    tx_byte_r <= (pre_cnt < 3'd7) ? 8'h55 : 8'hD5;
                    if (pre_cnt == 3'd7) begin
                        hdr_idx <= 4'd0;
                        state   <= S_HDR;
                    end else begin
                        pre_cnt <= pre_cnt + 3'd1;
                    end
                end

                S_HDR: begin
                    tx_en_r   <= 1'b1;
                    tx_byte_r <= hdr_byte(hdr_idx);
                    crc       <= crc32_byte(crc, hdr_byte(hdr_idx));
                    if (hdr_idx == 4'd13) begin
                        if (s_len == 16'd0) begin
                            fcs_idx <= 3'd0;
                            state   <= S_FCS;
                        end else begin
                            s_ready_r <= 1'b1;
                            state     <= S_PAY;
                        end
                    end else begin
                        hdr_idx <= hdr_idx + 4'd1;
                    end
                end

                S_PAY: begin
                    tx_en_r   <= 1'b1;
                    tx_byte_r <= s_data;
                    s_ready_r <= 1'b1;
                    crc       <= crc32_byte(crc, s_data);
                    pay_left  <= pay_left - 16'd1;
                    if (pay_left == 16'd1) begin
                        s_ready_r <= 1'b0;
                        fcs_idx   <= 3'd0;
                        state     <= S_FCS;
                    end
                end

                S_FCS: begin
                    tx_en_r   <= 1'b1;
                    tx_byte_r <= fcs_byte(crc, fcs_idx);
                    if (fcs_idx == 3'd3) begin
                        ifg_cnt <= 4'd0;
                        state   <= S_IFG;
                        frame_count <= frame_count + 32'd1;
                    end else begin
                        fcs_idx <= fcs_idx + 3'd1;
                    end
                end

                S_IFG: begin
                    tx_en_r <= 1'b0;
                    if (ifg_cnt == 4'd11) begin
                        packet_count <= packet_count + 32'd1;
                        state        <= S_IDLE;
                    end else begin
                        ifg_cnt <= ifg_cnt + 4'd1;
                    end
                end

                default: state <= S_IDLE;
            endcase
        end
    end

    always @(posedge clk) begin
        gmii_tx_d  <= tx_byte_r;
        gmii_tx_en <= tx_en_r;
    end

endmodule

`default_nettype wire
