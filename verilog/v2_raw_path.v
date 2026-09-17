// ============================================================================
// v2_raw_path - raw ADC streaming path (the laptop-FFT mode).
//
//   100 MHz ADC domain                125 MHz Ethernet domain
//   ------------------                ----------------------
//   adc_data[9:0] --> pack --> v2_cdc_fifo --> packetizer --> UDP/IP --> MAC
//      (no data valid)  4 samples/word        header + samples
//
// SAMPLE_BITS selects how many ADC bits are transmitted, DECIM selects the
// sample-rate divider. Both are packed exactly, with no wasted bits.
//
//   SAMPLE_BITS=10, DECIM=1 -> 1000 Mbps  FAILS: exceeds the 957 Mbps UDP
//                                          ceiling. Not selectable in hardware.
//   SAMPLE_BITS=10, DECIM=2 ->  500 Mbps  all 10 bits, 50 MSPS
//   SAMPLE_BITS= 8, DECIM=1 ->  800 Mbps  full 100 MSPS, 8 bits
//   SAMPLE_BITS= 8, DECIM=2 ->  400 Mbps  full 8 bits, 50 MSPS
//
// Gigabit Ethernet carries at most 957 Mbps of UDP payload at 1500 MTU
// (1472 B payload / 1538 B wire) so 100 MSPS x 10 bit = 1000 Mbps of samples
// cannot be sent, at any packing. To keep all 10 bits the sample rate must drop;
// DECIM does that here. Use SAMPLE_BITS=8/DECIM=1 when bandwidth matters more
// than the bottom 2 bits.
//
// A datagram is only started when its whole payload is already in the CDC FIFO,
// so it never underruns mid-frame. 10-bit samples are packed 4-per-5-bytes;
// 8-bit samples 4-per-4-bytes.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_raw_path #(
    parameter integer FIFO_ADDR_WIDTH = 10,          // 1024 words
    parameter integer SAMPLE_BITS     = 10,          // 8 or 10
    parameter integer DECIM           = 2,           // 1, 2, 4, 8, ...
    parameter integer MTU             = 1500         // IP packet length in bytes
) (
    input  wire        clk_adc,       // 100 MHz
    input  wire        clk_eth,       // 125 MHz
    input  wire        rst_n,

    input  wire [9:0]  adc_data,

    input  wire        upstream_busy,
    input  wire        link_up,
    input  wire [7:0]  decim_cfg,     // runtime decimation; 0 = use parameter
    output wire [7:0]  p_data,
    output wire        p_valid,
    input  wire        p_ready,
    output wire        send,
    output wire [15:0] payload_len,
    output wire [31:0] packet_seq,
    output wire [31:0] dropped_words,

    output reg  [31:0] overflow_count,
    output reg  [31:0] packets_sent
);

    localparam integer WORD_W      = 4*SAMPLE_BITS;
    localparam integer BPW         = (WORD_W + 7)/8;      // bytes per 4 samples
    localparam integer HEADER_BYTES = 16;
    localparam integer UDP_PAYLOAD = MTU - 28;
    localparam integer WORDS_PER_PACKET = (UDP_PAYLOAD - HEADER_BYTES) / BPW;
    localparam integer PKT_BYTES   = HEADER_BYTES + WORDS_PER_PACKET*BPW;
    localparam integer NSAMP       = WORDS_PER_PACKET*4;

    localparam [15:0] WPP  = WORDS_PER_PACKET[15:0];
    localparam [15:0] HB   = HEADER_BYTES[15:0];
    localparam [15:0] PKTB = PKT_BYTES[15:0];

    initial begin
        if (SAMPLE_BITS != 8 && SAMPLE_BITS != 10)
            $fatal(1, "SAMPLE_BITS must be 8 or 10");
        if (DECIM < 1)
            $fatal(1, "DECIM must be >= 1");
    end

    wire [7:0] decim_eff = (decim_cfg == 8'd0) ? DECIM[7:0] : decim_cfg;

    // ================================================== 100 MHz ADC domain
    wire [SAMPLE_BITS-1:0] sample = adc_data[9 -: SAMPLE_BITS];

    reg  [3*SAMPLE_BITS-1:0] acc;      // three held samples
    reg  [1:0]  cnt;
    reg  [15:0] dcount;
    wire        sample_tick = (decim_eff == 8'd1) ? 1'b1 : (dcount >= decim_eff - 8'd1);
    wire [WORD_W-1:0]  fifo_din   = {acc, sample};
    wire               fifo_wr_en = sample_tick && (cnt == 2'd3);

    wire [WORD_W-1:0]  fifo_dout;
    wire               fifo_full;
    wire               fifo_empty;
    wire [FIFO_ADDR_WIDTH:0] fifo_avail;
    reg                fifo_rd_en;

    always @(posedge clk_adc or negedge rst_n) begin
        if (!rst_n) begin
            acc    <= {3*SAMPLE_BITS{1'b0}};
            cnt    <= 2'd0;
            dcount <= decim_eff - 8'd1;       // first ADC sample is kept
        end else begin
            dcount <= sample_tick ? 16'd0 : (dcount + 16'd1);
            if (sample_tick) begin
                acc <= {acc[2*SAMPLE_BITS-1:0], sample};
                cnt <= cnt + 2'd1;
            end
        end
    end

    wire wr_dropped = fifo_wr_en && fifo_full;

    // Drop counter lives in the ADC domain. It is published to the Ethernet
    // domain as gray code (a +1 counter changes exactly one gray bit) through a
    // two-flop synchroniser, then decoded with a 31-XOR prefix chain. That gives
    // a coherent snapshot in the header without a multi-bit CDC hazard.
    reg [31:0] ovf_gray;
    always @(posedge clk_adc or negedge rst_n) begin
        if (!rst_n) begin
            overflow_count <= 32'd0;
            ovf_gray       <= 32'd0;
        end else if (wr_dropped) begin
            overflow_count <= overflow_count + 32'd1;
            ovf_gray       <= (overflow_count + 32'd1) ^ ((overflow_count + 32'd1) >> 1);
        end
    end

    reg  [31:0] ovf_g1, ovf_g2;
    always @(posedge clk_eth or negedge rst_n) begin
        if (!rst_n) begin
            ovf_g1 <= 32'd0;
            ovf_g2 <= 32'd0;
        end else begin
            ovf_g1 <= ovf_gray;
            ovf_g2 <= ovf_g1;
        end
    end

    // gray -> binary (prefix XOR chain)
    reg [31:0] ovf_bin;
    integer gi;
    always @* begin
        ovf_bin[31] = ovf_g2[31];
        for (gi = 30; gi >= 0; gi = gi - 1)
            ovf_bin[gi] = ovf_bin[gi+1] ^ ovf_g2[gi];
    end

    assign dropped_words = ovf_bin;

    // ================================================== clock-domain bridge
    v2_cdc_fifo #(.WIDTH(WORD_W), .ADDR_WIDTH(FIFO_ADDR_WIDTH)) u_fifo (
        .wr_clk(clk_adc), .wr_rst_n(rst_n), .wr_en(fifo_wr_en),
        .din(fifo_din), .full(fifo_full),
        .rd_clk(clk_eth), .rd_rst_n(rst_n), .rd_en(fifo_rd_en),
        .dout(fifo_dout), .empty(fifo_empty), .rd_avail(fifo_avail)
    );

    // ================================================= 125 MHz packetizer
    reg  [31:0] seq, cur_seq, base_sample, cur_drops;
    reg         active;
    reg  [15:0] byte_i;
    reg  [2:0]  bidx;
    reg  [WORD_W-1:0] cur, nxt;
    reg         nxt_v;
    reg  [15:0] fetched;
    reg         fetch_pend, cap_d;

    wire can_start = !active && !upstream_busy && (fifo_avail >= WPP);

    assign send        = can_start;
    assign payload_len = PKTB;
    assign packet_seq  = cur_seq;

    wire fetch = active && !nxt_v && !fetch_pend && (fetched < WPP);

    wire word_phase = (byte_i >= HB);
    wire rotate     = active && p_ready &&
                      ((byte_i == 16'd0) || (word_phase && (bidx == BPW-1)));

    always @(posedge clk_eth or negedge rst_n) begin
        if (!rst_n) fifo_rd_en <= 1'b0;
        else        fifo_rd_en <= fetch;
    end

    always @(posedge clk_eth or negedge rst_n) begin
        if (!rst_n) begin
            seq          <= 32'd0;
            cur_seq      <= 32'd0;
            base_sample  <= 32'd0;
            cur_drops    <= 32'd0;
            active       <= 1'b0;
            byte_i       <= 16'd0;
            bidx         <= 3'd0;
            cur          <= {WORD_W{1'b0}};
            nxt          <= {WORD_W{1'b0}};
            nxt_v        <= 1'b0;
            fetched      <= 16'd0;
            fetch_pend   <= 1'b0;
            cap_d        <= 1'b0;
            packets_sent <= 32'd0;
        end else begin
            cap_d <= fifo_rd_en;

            if (fetch) begin
                fetched    <= fetched + 16'd1;
                fetch_pend <= 1'b1;
            end
            if (cap_d) fetch_pend <= 1'b0;

            if (rotate) begin
                cur   <= nxt_v ? nxt : cur;
                nxt_v <= 1'b0;
                if (cap_d) begin
                    nxt   <= fifo_dout;
                    nxt_v <= 1'b1;
                end
            end else if (cap_d) begin
                nxt   <= fifo_dout;
                nxt_v <= 1'b1;
            end

            if (!active) begin
                if (can_start) begin
                    active      <= 1'b1;
                    byte_i      <= 16'd0;
                    bidx        <= 3'd0;
                    fetched     <= 16'd0;
                    nxt_v       <= 1'b0;
                    fetch_pend  <= 1'b0;
                    cur_seq     <= seq;
                    base_sample <= seq * NSAMP;
                    cur_drops   <= ovf_bin;
                    seq         <= seq + 32'd1;
                end
            end else if (p_ready) begin
                if (byte_i == PKTB - 16'd1) begin
                    active       <= 1'b0;
                    packets_sent <= packets_sent + 32'd1;
                end
                byte_i <= byte_i + 16'd1;
                if (word_phase) begin
                    if (bidx == BPW-1) bidx <= 3'd0;
                    else               bidx <= bidx + 3'd1;
                end
            end
        end
    end

    // ----------------------------------------------------- payload byte mux
    // header: seq(4) | base_sample(4) | dropped_words(4) | 0xA5, bits, decim, flags
    wire overflow_seen = (ovf_bin != 32'd0);
    wire [7:0] hdr_byte =
        (byte_i == 16'd0)  ? cur_seq[31:24] :
        (byte_i == 16'd1)  ? cur_seq[23:16] :
        (byte_i == 16'd2)  ? cur_seq[15:8]  :
        (byte_i == 16'd3)  ? cur_seq[7:0]   :
        (byte_i == 16'd4)  ? base_sample[31:24] :
        (byte_i == 16'd5)  ? base_sample[23:16] :
        (byte_i == 16'd6)  ? base_sample[15:8]  :
        (byte_i == 16'd7)  ? base_sample[7:0]   :
        (byte_i == 16'd8)  ? cur_drops[31:24] :
        (byte_i == 16'd9)  ? cur_drops[23:16] :
        (byte_i == 16'd10) ? cur_drops[15:8]  :
        (byte_i == 16'd11) ? cur_drops[7:0]   :
        (byte_i == 16'd12) ? 8'hA5 :
        (byte_i == 16'd13) ? SAMPLE_BITS[7:0] :
        (byte_i == 16'd14) ? decim_eff :
                             {6'b000000, overflow_seen, link_up};

    // Big-endian byte extraction from the packed word.
    wire [7:0] samp_byte = cur[WORD_W-1 - 8*bidx -: 8];

    assign p_data  = (byte_i < HB) ? hdr_byte : samp_byte;
    assign p_valid = active;

endmodule

`default_nettype wire
