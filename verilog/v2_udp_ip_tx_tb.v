// ============================================================================
// v2_udp_ip_tx_tb - end-to-end UDP/IP -> MAC -> GMII wire verification.
//
//  * captures the GMII byte stream
//  * verifies Ethernet framing + EtherType
//  * verifies every IPv4 header field AND that the header checksum is exact
//    (one's-complement sum over the header including the checksum == 0xFFFF)
//  * verifies UDP ports/length/checksum(0)
//  * verifies payload contents and FCS
//  * reports achieved throughput at the 1500-byte MTU operating point
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_udp_ip_tx_tb;

    localparam [47:0] TEST_DA = 48'h02_00_00_00_00_01;
    localparam [47:0] TEST_SA = 48'h02_00_00_00_00_02;
    localparam integer PAYLOAD = 1472;               // UDP data bytes
    localparam integer IP_LEN  = PAYLOAD + 28;       // IP packet length = 1500
    localparam integer FRAME_LEN = 54 + PAYLOAD;     // wire bytes = 1526
    localparam integer FRAMES  = 200;

    localparam [31:0] SRC_IP = 32'hC0A80002;         // 192.168.0.2
    localparam [31:0] DST_IP = 32'hC0A80001;         // 192.168.0.1
    localparam [15:0] SRC_PORT = 16'd4660;
    localparam [15:0] DST_PORT = 16'd10000;

    reg clk = 0, rst_n = 0;
    always #4 clk = ~clk;                            // 125 MHz

    // payload source
    reg [31:0] consumed = 0;
    function [7:0] payload_byte;
        input [31:0] k;
        begin
            payload_byte = (k * 32'd7 + 32'd3) & 32'hFF;
        end
    endfunction
    wire [7:0] p_data  = payload_byte(consumed);
    wire       p_valid = 1'b1;
    wire       p_ready;

    // UDP/IP layer
    wire [7:0]  m_data;
    wire        m_valid, m_ready;
    wire [15:0] m_len;
    wire        busy;

    v2_udp_ip_tx dut_ip (
        .clk(clk), .rst_n(rst_n),
        .src_ip(SRC_IP), .dst_ip(DST_IP), .src_port(SRC_PORT), .dst_port(DST_PORT),
        .send(1'b1), .payload_len(PAYLOAD[15:0]), .busy(busy),
        .p_data(p_data), .p_valid(p_valid), .p_ready(p_ready),
        .m_data(m_data), .m_valid(m_valid), .m_ready(m_ready), .m_len(m_len)
    );

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) consumed <= 0;
        else if (p_ready && p_valid) begin
            if (consumed == PAYLOAD - 1) consumed <= 0;
            else consumed <= consumed + 1;
        end
    end

    // MAC
    wire [7:0] gmii_tx_d;
    wire       gmii_tx_en;

    v2_eth_mac_tx #(
        .DEFAULT_DST_MAC(TEST_DA),
        .DEFAULT_SRC_MAC(TEST_SA)
    ) dut_mac (
        .clk(clk), .rst_n(rst_n),
        .dst_mac(48'h0), .src_mac(48'h0),
        .s_data(m_data), .s_valid(m_valid), .s_len(m_len), .s_ready(m_ready),
        .gmii_tx_d(gmii_tx_d), .gmii_tx_en(gmii_tx_en),
        .packet_count(), .frame_count()
    );

    // ------------------------------------------------------------ wire capture
    reg [7:0] frame [0:2047];
    integer   bidx = 0, gap = 0, errors = 0, checked = 0;
    integer   wire_bytes = 0;
    reg       seen_frame = 0, prev_en = 0;
    time      first_en = 0, last_en = 0;

    integer k;
    reg [31:0] sum, inv, crc;
    reg [31:0] c;

    function [31:0] crc32_byte;
        input [31:0] c_in;
        input [7:0]  d;
        integer i;
        reg [31:0] x;
        begin
            x = c_in ^ {24'h0, d};
            for (i = 0; i < 8; i = i + 1)
                x = x[0] ? ((x >> 1) ^ 32'hEDB88320) : (x >> 1);
            crc32_byte = x;
        end
    endfunction

    function [15:0] rd16;
        input integer i;
        begin
            rd16 = {frame[i], frame[i+1]};
        end
    endfunction

    task check_frame;
        input integer n;
        integer i;
        begin
            if (n !== FRAME_LEN) begin
                $display("  FRAME %0d: length %0d expected %0d", checked, n, FRAME_LEN);
                errors = errors + 1;
                checked = checked + 1;
                disable check_frame;
            end

            for (i = 0; i < 7; i = i + 1)
                if (frame[i] !== 8'h55) begin errors=errors+1; end
            if (frame[7] !== 8'hD5) begin errors=errors+1; end
            if (rd16(20) !== 16'h0800) begin
                $display("  FRAME %0d: ethertype %h", checked, rd16(20)); errors=errors+1;
            end

            // IPv4 header at frame[22]
            if (frame[22] !== 8'h45)   begin $display("  IP ver/ihl %h", frame[22]); errors=errors+1; end
            if (rd16(24) !== IP_LEN[15:0]) begin $display("  IP total len %h", rd16(24)); errors=errors+1; end
            if (rd16(26) !== 16'h0000) begin $display("  IP id %h", rd16(26)); errors=errors+1; end
            if (rd16(28) !== 16'h4000) begin $display("  IP frag %h", rd16(28)); errors=errors+1; end
            if (frame[30] !== 8'd64)   begin $display("  IP ttl %h", frame[30]); errors=errors+1; end
            if (frame[31] !== 8'd17)   begin $display("  IP proto %h", frame[31]); errors=errors+1; end
            if (rd16(34) !== SRC_IP[31:16] || rd16(36) !== SRC_IP[15:0]) begin
                $display("  IP src %h%h", rd16(34), rd16(36)); errors=errors+1;
            end
            if (rd16(38) !== DST_IP[31:16] || rd16(40) !== DST_IP[15:0]) begin
                $display("  IP dst %h%h", rd16(38), rd16(40)); errors=errors+1;
            end

            // exact header checksum: one's complement sum over the 20 bytes == FFFF
            sum = 0;
            for (i = 0; i < 10; i = i + 1)
                sum = sum + rd16(22 + 2*i);
            sum = (sum & 32'hFFFF) + (sum >> 16);
            sum = (sum & 32'hFFFF) + (sum >> 16);
            if (sum[15:0] !== 16'hFFFF) begin
                $display("  FRAME %0d: IP checksum invalid (sum=%h)", checked, sum[15:0]);
                errors = errors + 1;
            end

            // UDP header at frame[42]
            if (rd16(42) !== SRC_PORT) begin $display("  UDP sport %h", rd16(42)); errors=errors+1; end
            if (rd16(44) !== DST_PORT) begin $display("  UDP dport %h", rd16(44)); errors=errors+1; end
            if (rd16(46) !== (PAYLOAD + 8)) begin $display("  UDP len %h", rd16(46)); errors=errors+1; end
            if (rd16(48) !== 16'h0000) begin $display("  UDP csum %h", rd16(48)); errors=errors+1; end

            // payload at frame[50]
            for (i = 0; i < PAYLOAD; i = i + 1)
                if (frame[50+i] !== payload_byte(i)) begin
                    if (errors < 6)
                        $display("  FRAME %0d: payload[%0d]=%h expected %h",
                                 checked, i, frame[50+i], payload_byte(i));
                    errors = errors + 1;
                end

            // FCS over frame[8 .. 41+PAYLOAD] (DA..payload)
            c = 32'hFFFFFFFF;
            for (i = 0; i < 14 + 28 + PAYLOAD; i = i + 1)
                c = crc32_byte(c, frame[8+i]);
            inv = ~c;
            for (i = 0; i < 4; i = i + 1)
                if (frame[50+PAYLOAD+i] !== inv[8*i +: 8]) begin
                    $display("  FRAME %0d: FCS[%0d]=%h expected %h",
                             checked, i, frame[50+PAYLOAD+i], inv[8*i +: 8]);
                    errors = errors + 1;
                end

            checked = checked + 1;
        end
    endtask

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            bidx <= 0; gap <= 0; wire_bytes <= 0; prev_en <= 0; seen_frame <= 0;
            first_en <= 0; last_en <= 0;
        end else if (gmii_tx_en) begin
            if (!prev_en && seen_frame && gap < 12) begin
                $display("  IFG too short: %0d", gap); errors = errors + 1;
            end
            prev_en <= 1;
            gap <= 0;
            if (bidx < 2048) frame[bidx] <= gmii_tx_d;
            bidx <= bidx + 1;
            wire_bytes <= wire_bytes + 1;
            if (first_en == 0) first_en <= $time;
            last_en <= $time;
        end else begin
            prev_en <= 0;
            gap <= gap + 1;
            if (bidx > 0) begin
                check_frame(bidx);
                bidx <= 0;
                seen_frame <= 1;
            end
        end
    end

    initial begin
        $display("=== v2_udp_ip_tx_tb ===");
        repeat (4) @(posedge clk);
        rst_n = 1;
        while (checked < FRAMES) @(posedge clk);
        repeat (4) @(posedge clk);
        if (errors !== 0) begin
            $display("FATAL: %0d errors", errors);
            $fatal(1);
        end
        $display("  %0d frames verified (IP hdr + exact checksum + UDP + payload + FCS)",
                 checked);
        $display("  wire throughput    = %.1f Mbps",
                 (wire_bytes * 8.0 * 1000.0) / (last_en - first_en));
        $display("  UDP payload rate   = %.1f Mbps",
                 ((checked * PAYLOAD) * 8.0 * 1000.0) / (last_en - first_en));
        $display("PASS: v2_udp_ip_tx (IPv4/UDP framing, checksum, throughput)");
        $finish;
    end

    initial begin
        #5_000_000;
        $fatal(1, "v2_udp_ip_tx_tb timeout");
    end

endmodule

`default_nettype wire
