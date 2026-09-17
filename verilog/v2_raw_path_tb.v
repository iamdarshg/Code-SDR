// ============================================================================
// v2_raw_path_tb - full raw-mode chain verification:
//   ADC(100 MHz) -> pack(SAMPLE_BITS) -> CDC FIFO -> packetizer -> UDP/IP -> MAC
//
// Verifies the packed sample stream bit-exactly by UNPACKING the on-wire bytes
// back into samples and comparing against the ADC stimulus:
//   SAMPLE_BITS=10: 4 samples packed into 5 bytes
//   SAMPLE_BITS= 8: 4 samples packed into 4 bytes
// Also checks the per-packet header (sequence, base sample index, config),
// IPv4/UDP headers, exact IP checksum, and FCS.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_raw_path_tb;

    parameter integer SAMPLE_BITS = 10;
    parameter integer DECIM       = 2;
    parameter integer MTU         = 1500;

    localparam [47:0] TEST_DA = 48'h02_00_00_00_00_01;
    localparam [47:0] TEST_SA = 48'h02_00_00_00_00_02;

    localparam integer WORD_W  = 4*SAMPLE_BITS;
    localparam integer BPW     = (WORD_W + 7)/8;
    localparam integer HEADER  = 16;
    localparam integer UDP_PAY = MTU - 28;
    localparam integer WORDS   = (UDP_PAY - HEADER) / BPW;
    localparam integer PKT_BYTES = HEADER + WORDS*BPW;
    localparam integer IP_LEN  = PKT_BYTES + 28;
    localparam integer NSAMP   = WORDS*4;
    localparam integer FRAME_LEN = 54 + PKT_BYTES;
    localparam integer SAMP_OFF  = 22 + 28 + HEADER;   // first sample byte
    localparam integer FRAMES  = 40;

    localparam [31:0] SRC_IP = 32'hC0A80002;
    localparam [31:0] DST_IP = 32'hC0A80001;
    localparam [15:0] SRC_PORT = 16'd4660;
    localparam [15:0] DST_PORT = 16'd10000;

    reg clk_adc = 0, clk_eth = 0, rst_n = 0;
    always #5 clk_adc = ~clk_adc;      // 100 MHz
    always #4 clk_eth = ~clk_eth;      // 125 MHz

    // ---------------------------------------------------------- ADC stimulus
    // Pseudo-random 10-bit pattern (a hash of the sample index) so every sample
    // is distinct - a stuck bit or an address slip cannot alias to a passing
    // value. The checker recomputes the same function from the header's base
    // sample index, so corruption anywhere in the chain is caught.
    function [9:0] adc_val;
        input [31:0] k;
        reg [31:0] h;
        begin
            h = k * 32'h9E3779B9;
            adc_val = (h ^ (h >> 15)) & 32'h3FF;
        end
    endfunction

    reg  [31:0] kk = 0;
    wire [9:0]  adc_data = adc_val(kk);
    always @(posedge clk_adc or negedge rst_n)
        if (!rst_n) kk <= 0; else kk <= kk + 1;

    // ------------------------------------------------ random upstream backpressure
    // Randomly asserts the raw path's start-gate so packet starts are jittered.
    reg [15:0] fuzz_lfsr = 16'hACE1;
    always @(posedge clk_eth or negedge rst_n)
        if (!rst_n) fuzz_lfsr <= 16'hACE1;
        else         fuzz_lfsr <= {fuzz_lfsr[14:0],
                                   fuzz_lfsr[15] ^ fuzz_lfsr[13] ^ fuzz_lfsr[12] ^ fuzz_lfsr[10]};
    wire udp_busy_fuzz = udp_busy | (fuzz_lfsr[3] & fuzz_lfsr[0]);

    // ------------------------------------------------------------- datapath
    wire [7:0]  p_data;
    wire        p_valid, p_ready;
    wire        udp_send;
    wire [15:0] udp_plen;
    wire [31:0] udp_seq;
    wire        udp_busy;
    wire [7:0]  m_data;
    wire        m_valid, m_ready;
    wire [15:0] m_len;
    wire [7:0]  gmii_tx_d;
    wire        gmii_tx_en;
    wire [31:0] overflow_count, packets_sent;

    v2_raw_path #(
        .FIFO_ADDR_WIDTH(10),
        .SAMPLE_BITS(SAMPLE_BITS),
        .DECIM(DECIM),
        .MTU(MTU)
    ) u_raw (
        .clk_adc(clk_adc), .clk_eth(clk_eth), .rst_n(rst_n),
        .adc_data(adc_data),
        .upstream_busy(udp_busy_fuzz),
        .link_up(1'b1),
        .decim_cfg(8'd0),
        .p_data(p_data), .p_valid(p_valid), .p_ready(p_ready),
        .send(udp_send), .payload_len(udp_plen), .packet_seq(udp_seq),
        .dropped_words(),
        .overflow_count(overflow_count), .packets_sent(packets_sent)
    );

    v2_udp_ip_tx u_udp (
        .clk(clk_eth), .rst_n(rst_n),
        .src_ip(SRC_IP), .dst_ip(DST_IP), .src_port(SRC_PORT), .dst_port(DST_PORT),
        .send(udp_send), .payload_len(udp_plen), .busy(udp_busy),
        .p_data(p_data), .p_valid(p_valid), .p_ready(p_ready),
        .m_data(m_data), .m_valid(m_valid), .m_ready(m_ready), .m_len(m_len)
    );

    v2_eth_mac_tx #(
        .DEFAULT_DST_MAC(TEST_DA), .DEFAULT_SRC_MAC(TEST_SA)
    ) u_mac (
        .clk(clk_eth), .rst_n(rst_n),
        .dst_mac(48'h0), .src_mac(48'h0),
        .s_data(m_data), .s_valid(m_valid), .s_len(m_len), .s_ready(m_ready),
        .gmii_tx_d(gmii_tx_d), .gmii_tx_en(gmii_tx_en),
        .packet_count(), .frame_count()
    );

    // ------------------------------------------------------------ wire capture
    reg [7:0] frame [0:4095];
    integer   bidx = 0, gap = 0, errors = 0, checked = 0;
    integer   wire_bytes = 0;
    reg       seen_frame = 0, prev_en = 0;
    time      first_en = 0, last_en = 0;
    integer   i, j, g, b;
    reg [31:0] sum, crc, inv;
    reg [31:0] base_w;
    reg [9:0]  s0, s1, s2, s3;
    reg [7:0]  exp_byte;

    function [31:0] crc32_byte;
        input [31:0] c_in;
        input [7:0]  d;
        integer i2;
        reg [31:0] x;
        begin
            x = c_in ^ {24'h0, d};
            for (i2 = 0; i2 < 8; i2 = i2 + 1)
                x = x[0] ? ((x >> 1) ^ 32'hEDB88320) : (x >> 1);
            crc32_byte = x;
        end
    endfunction

    function [15:0] rd16;
        input integer idx;
        begin
            rd16 = {frame[idx], frame[idx+1]};
        end
    endfunction

    // Expected packed byte b of the group of four 10-bit samples.
    function [7:0] exp10;
        input [9:0] a0, a1, a2, a3;
        input integer bsel;
        begin
            case (bsel)
                0: exp10 = a0[9:2];
                1: exp10 = {a0[1:0], a1[9:4]};
                2: exp10 = {a1[3:0], a2[9:6]};
                3: exp10 = {a2[5:0], a3[9:8]};
                default: exp10 = a3[7:0];
            endcase
        end
    endfunction

    task check_frame;
        input integer n;
        reg [31:0] seq_w;
        integer groups, rem;
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
                $display("  ethertype %h", rd16(20)); errors=errors+1;
            end

            if (rd16(24) !== IP_LEN[15:0]) begin
                $display("  ip len %h expected %h", rd16(24), IP_LEN[15:0]); errors=errors+1;
            end
            sum = 0;
            for (i = 0; i < 10; i = i + 1) sum = sum + rd16(22 + 2*i);
            sum = (sum & 32'hFFFF) + (sum >> 16);
            sum = (sum & 32'hFFFF) + (sum >> 16);
            if (sum[15:0] !== 16'hFFFF) begin
                $display("  ip checksum"); errors = errors + 1;
            end

            if (rd16(42) !== SRC_PORT) begin errors=errors+1; end
            if (rd16(44) !== DST_PORT) begin errors=errors+1; end
            if (rd16(46) !== (PKT_BYTES + 8)) begin
                $display("  udp len %h", rd16(46)); errors=errors+1;
            end
            if (rd16(48) !== 16'h0000) begin errors=errors+1; end

            seq_w  = {frame[50], frame[51], frame[52], frame[53]};
            base_w = {frame[54], frame[55], frame[56], frame[57]};
            if (seq_w !== checked) begin
                $display("  seq %0d expected %0d", seq_w, checked); errors = errors + 1;
            end
            if (base_w !== checked*NSAMP) begin
                $display("  base %0d expected %0d", base_w, checked*NSAMP); errors = errors + 1;
            end
            if (frame[62] !== 8'hA5 || frame[63] !== SAMPLE_BITS || frame[64] !== DECIM) begin
                $display("  config magic %h %0d %0d", frame[62], frame[63], frame[64]);
                errors = errors + 1;
            end
            if (frame[65] !== 8'h01) begin   // link_up=1, no overflow
                $display("  flags %h expected 01", frame[65]);
                errors = errors + 1;
            end
            if ({frame[58],frame[59],frame[60],frame[61]} !== 32'd0) begin
                $display("  drops %h expected 0",
                         {frame[58],frame[59],frame[60],frame[61]});
                errors = errors + 1;
            end

            // unpack and compare every sample
            groups = WORDS;
            for (g = 0; g < groups; g = g + 1) begin
                s0 = adc_val((base_w + g*4 + 0) * DECIM);
                s1 = adc_val((base_w + g*4 + 1) * DECIM);
                s2 = adc_val((base_w + g*4 + 2) * DECIM);
                s3 = adc_val((base_w + g*4 + 3) * DECIM);
                for (b = 0; b < BPW; b = b + 1) begin
                    if (SAMPLE_BITS == 10)
                        exp_byte = exp10(s0, s1, s2, s3, b);
                    else
                        exp_byte = (b == 0) ? s0[9:2] : (b == 1) ? s1[9:2] :
                                   (b == 2) ? s2[9:2] : s3[9:2];
                    if (frame[SAMP_OFF + g*BPW + b] !== exp_byte) begin
                        if (errors < 6)
                            $display("  FRAME %0d grp %0d byte %0d: got %h expected %h",
                                     checked, g, b, frame[SAMP_OFF + g*BPW + b], exp_byte);
                        errors = errors + 1;
                    end
                end
            end

            crc = 32'hFFFFFFFF;
            for (i = 0; i < 14 + IP_LEN; i = i + 1)
                crc = crc32_byte(crc, frame[8+i]);
            inv = ~crc;
            for (i = 0; i < 4; i = i + 1)
                if (frame[SAMP_OFF + WORDS*BPW + i] !== inv[8*i +: 8]) begin
                    $display("  FCS"); errors = errors + 1;
                end

            checked = checked + 1;
        end
    endtask

    always @(posedge clk_eth or negedge rst_n) begin
        if (!rst_n) begin
            bidx <= 0; gap <= 0; wire_bytes <= 0; prev_en <= 0; seen_frame <= 0;
            first_en <= 0; last_en <= 0;
        end else if (gmii_tx_en) begin
            if (!prev_en && seen_frame && gap < 12) begin
                $display("  IFG too short: %0d", gap); errors = errors + 1;
            end
            prev_en <= 1;
            gap <= 0;
            if (bidx < 4095) frame[bidx] <= gmii_tx_d;
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
        $display("=== v2_raw_path_tb (SAMPLE_BITS=%0d DECIM=%0d) ===", SAMPLE_BITS, DECIM);
        repeat (8) @(posedge clk_eth);
        rst_n = 1;
        while (checked < FRAMES) @(posedge clk_eth);
        repeat (4) @(posedge clk_eth);
        if (errors !== 0) begin
            $display("FATAL: %0d errors", errors);
            $fatal(1);
        end
        $display("  %0d frames, %0d packed sample bytes verified, overflows=%0d",
                 checked, checked*WORDS*BPW, overflow_count);
        $display("  sample payload   = %.1f Mbps (100/%0d MSPS x %0d bit)",
                 (checked*NSAMP*SAMPLE_BITS*1.0*1000.0)/(last_en-first_en), DECIM, SAMPLE_BITS);
        $display("  wire throughput  = %.1f Mbps",
                 (wire_bytes * 8.0 * 1000.0) / (last_en - first_en));
        $display("PASS: v2_raw_path (%0d-bit packed, DECIM=%0d)", SAMPLE_BITS, DECIM);
        $finish;
    end

    initial begin
        #20_000_000;
        $fatal(1, "v2_raw_path_tb timeout");
    end

endmodule

`default_nettype wire
