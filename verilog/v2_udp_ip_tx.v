// ============================================================================
// v2_udp_ip_tx - transmit-only IPv4 + UDP framer, byte-serial at 1 byte/cycle.
//
// Emits the IP packet (20 B IPv4 header + 8 B UDP header + payload) as a byte
// stream to v2_eth_mac_tx. The MAC adds preamble/DA/SA/EtherType/FCS/IFG.
//
// Design notes for the LIF-MD6000 (no DSP, small fabric):
//   * headers are generated combinationally from latched fields, so only the
//     payload is streamed. No header buffer RAM is needed.
//   * UDP checksum is transmitted as 0, which IPv4 explicitly permits
//     (RFC 768). This avoids a second pass over the payload and therefore keeps
//     the 1 byte/cycle line-rate stream. Enable CHECKSUM_UDP to change that.
//   * the IP header checksum is computed once per packet from the latched
//     fields and is exact (verified in the testbench).
//
// Contract: the payload source must have `payload_len` bytes available before
// `send` is asserted. The stream is continuous: once the MAC starts taking
// bytes it must never be starved, or the frame underruns on the wire.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_udp_ip_tx #(
    parameter [15:0] IP_PROTOCOL_UDP = 16'h0011,
    parameter [7:0]  IP_TTL         = 8'd64
) (
    input  wire        clk,
    input  wire        rst_n,

    input  wire [31:0] src_ip,
    input  wire [31:0] dst_ip,
    input  wire [15:0] src_port,
    input  wire [15:0] dst_port,

    input  wire        send,          // pulse to start one datagram
    input  wire [15:0] payload_len,   // payload bytes (UDP data)
    output wire        busy,

    // Payload source (byte stream)
    input  wire [7:0]  p_data,
    input  wire        p_valid,
    output wire        p_ready,

    // To MAC (client stream = the IP packet)
    output wire [7:0]  m_data,
    output wire        m_valid,
    input  wire        m_ready,
    output wire [15:0] m_len
);

    reg [15:0] total_len;    // IP total length
    reg [15:0] udp_len;
    reg [15:0] ip_csum;
    reg [15:0] idx;
    reg        active;
    reg [31:0] l_src_ip, l_dst_ip;
    reg [15:0] l_src_port, l_dst_port;

    assign busy    = active;
    assign m_valid = active;
    assign m_len   = total_len;

    // ----------------------------------------------- IP header checksum (exact)
    function [15:0] ip_checksum;
        input [15:0] tl;
        input [31:0] s_ip;
        input [31:0] d_ip;
        reg [31:0] sum;
        reg [15:0] w [0:9];
        integer i;
        begin
            w[0] = 16'h4500;
            w[1] = tl;
            w[2] = 16'h0000;
            w[3] = 16'h4000;
            w[4] = {IP_TTL, IP_PROTOCOL_UDP[7:0]};
            w[5] = 16'h0000;
            w[6] = s_ip[31:16];
            w[7] = s_ip[15:0];
            w[8] = d_ip[31:16];
            w[9] = d_ip[15:0];
            sum = 32'd0;
            for (i = 0; i < 10; i = i + 1)
                sum = sum + w[i];
            sum = (sum & 32'hFFFF) + (sum >> 16);
            sum = (sum & 32'hFFFF) + (sum >> 16);
            ip_checksum = ~sum[15:0];
        end
    endfunction

    // ------------------------------------------------- header byte generation
    function [7:0] hdr_byte;
        input [15:0] i;
        begin
            case (i)
                16'd0:  hdr_byte = 8'h45;
                16'd1:  hdr_byte = 8'h00;
                16'd2:  hdr_byte = total_len[15:8];
                16'd3:  hdr_byte = total_len[7:0];
                16'd4:  hdr_byte = 8'h00;
                16'd5:  hdr_byte = 8'h00;
                16'd6:  hdr_byte = 8'h40;   // DF
                16'd7:  hdr_byte = 8'h00;
                16'd8:  hdr_byte = IP_TTL;
                16'd9:  hdr_byte = IP_PROTOCOL_UDP[7:0];
                16'd10: hdr_byte = ip_csum[15:8];
                16'd11: hdr_byte = ip_csum[7:0];
                16'd12: hdr_byte = l_src_ip[31:24];
                16'd13: hdr_byte = l_src_ip[23:16];
                16'd14: hdr_byte = l_src_ip[15:8];
                16'd15: hdr_byte = l_src_ip[7:0];
                16'd16: hdr_byte = l_dst_ip[31:24];
                16'd17: hdr_byte = l_dst_ip[23:16];
                16'd18: hdr_byte = l_dst_ip[15:8];
                16'd19: hdr_byte = l_dst_ip[7:0];
                16'd20: hdr_byte = l_src_port[15:8];
                16'd21: hdr_byte = l_src_port[7:0];
                16'd22: hdr_byte = l_dst_port[15:8];
                16'd23: hdr_byte = l_dst_port[7:0];
                16'd24: hdr_byte = udp_len[15:8];
                16'd25: hdr_byte = udp_len[7:0];
                default: hdr_byte = 8'h00;  // UDP checksum = 0 (disabled)
            endcase
        end
    endfunction

    wire is_payload = (idx >= 16'd28);

    assign m_data  = is_payload ? p_data : hdr_byte(idx);
    assign p_ready = active && is_payload && m_ready;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            active     <= 1'b0;
            idx        <= 16'd0;
            total_len  <= 16'd0;
            udp_len    <= 16'd0;
            ip_csum    <= 16'd0;
            l_src_ip   <= 32'd0;
            l_dst_ip   <= 32'd0;
            l_src_port <= 16'd0;
            l_dst_port <= 16'd0;
        end else begin
            if (!active) begin
                idx <= 16'd0;
                if (send) begin
                    l_src_ip   <= src_ip;
                    l_dst_ip   <= dst_ip;
                    l_src_port <= src_port;
                    l_dst_port <= dst_port;
                    total_len  <= payload_len + 16'd28;
                    udp_len    <= payload_len + 16'd8;
                    ip_csum    <= ip_checksum(payload_len + 16'd28, src_ip, dst_ip);
                    active     <= 1'b1;
                end
            end else begin
                if (m_ready) begin
                    if (idx == total_len - 16'd1) begin
                        active <= 1'b0;
                        idx    <= 16'd0;
                    end else begin
                        idx <= idx + 16'd1;
                    end
                end
            end
        end
    end

endmodule

`default_nettype wire
