// ============================================================================
// v2_raw_path - raw ADC streaming path (the laptop-FFT mode).
//
//   100 MHz ADC domain                125 MHz Ethernet domain
//   ------------------                ----------------------
//   adc_data[9:0] --> pack --> v2_cdc_fifo --> packetizer --> UDP/IP --> MAC
//      (no data valid)  5-byte word         header + samples
//
// PACKING
//   The word is always 40 bits (5 bytes) so that both sample widths produce the
//   same words-per-datagram, and therefore the same FIFO depth, for a given MTU:
//
//     SAMPLE_BITS=8  -> 5 samples per 5-byte word (exact, no padding)
//     SAMPLE_BITS=10 -> 4 samples per 5-byte word (exact, no padding)
//
//   Byte extraction is identical for both (contiguous big-endian), so the
//   packetizer does not care which width is in use.
//
// RATE
//   100 MSPS x 8 bit  = 800 Mbps  -> fits, no drops
//   100 MSPS x 10 bit = 1002 Mbps -> exceeds the ~993 Mbps jumbo ceiling
//
//   The ADC runs at a fixed 100 MSPS, so the 10-bit stream cannot simply slow
//   down to fit: the next integer decimation would be 50 MSPS (500 Mbps, 52% of
//   the link). Instead of throwing away half the bandwidth, this path keeps the
//   full 100 MSPS / full 10 bits and drops WHOLE DATAGRAMS at a controlled
//   fractional rate, which costs ~0.9% of samples at jumbo MTU instead of 50%.
//   Every kept packet is contiguous and jitter-free.
//
//   The dropper is a first-order delta-sigma accumulator: it holds the average
//   drop ratio exactly, but spreads the individual drops aperiodically so the
//   gaps do not form a periodic pattern (a fixed "1 in N" cadence would put
//   discrete spurs in the host's FFT at the packet-rate/N spacing).
//
//   Dropped packets still advance `seq` and `base_sample`, so the header tells
//   the host exactly which absolute sample ranges are missing: it sees a gap in
//   seq and a matching jump in base_sample. Loss is never hidden.
//
//   A FIFO high-water override forces extra drops if the read side ever stalls,
//   which makes an overflow structurally impossible: `dropped_words` (FIFO
//   overrun) stays zero and all loss is deliberate, counted in packets_dropped.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_raw_path #(
    parameter integer FIFO_ADDR_WIDTH = 11,          // 2048 words: holds one jumbo datagram
    parameter integer SAMPLE_BITS     = 10,          // 8 or 10
    parameter integer DECIM           = 1,           // 1, 2, 4, 8, ...
    parameter integer MTU             = 9000,        // IP packet length in bytes
    parameter integer DROP_FRAC       = 0            // power-on drop fraction, Q16 (0 = none)
) (
    input  wire        clk_adc,       // 100 MHz
    input  wire        clk_eth,       // 125 MHz
    input  wire        rst_n,

    input  wire [9:0]  adc_data,

    input  wire        upstream_busy,
    input  wire        link_up,
    input  wire [7:0]  decim_cfg,     // runtime decimation; 0 = use parameter
    input  wire [15:0] drop_frac_cfg, // runtime Q16 drop fraction; 0 = use parameter
    output wire [7:0]  p_data,
    output wire        p_valid,
    input  wire        p_ready,
    output wire        send,
    output wire [15:0] payload_len,
    output wire [31:0] packet_seq,
    output wire [31:0] dropped_words,

    output reg  [31:0] overflow_count,
    output reg  [31:0] packets_sent,
    output reg  [31:0] packets_dropped
);

    localparam integer WORD_W      = 40;                  // fixed 5-byte word
    localparam integer BPW         = WORD_W/8;            // bytes per word = 5
    localparam integer SPW         = WORD_W/SAMPLE_BITS;  // samples per word: 8->5, 10->4
    localparam integer HEADER_BYTES = 16;
    localparam integer UDP_PAYLOAD = MTU - 28;
    localparam integer WORDS_PER_PACKET = (UDP_PAYLOAD - HEADER_BYTES) / BPW;
    localparam integer PKT_BYTES   = HEADER_BYTES + WORDS_PER_PACKET*BPW;
    localparam integer NSAMP       = WORDS_PER_PACKET*SPW;

    localparam [15:0] WPP  = WORDS_PER_PACKET[15:0];
    localparam [15:0] HB   = HEADER_BYTES[15:0];
    localparam [15:0] PKTB = PKT_BYTES[15:0];

    // The packetizer starts a datagram only when a whole one is buffered, so the
    // FIFO must hold at least WORDS_PER_PACKET words (the FIFO loses one slot to
    // its full-flag scheme).
    localparam integer HIGH_WATER = (1 << FIFO_ADDR_WIDTH) - 65;

    initial begin
        if (SAMPLE_BITS != 8 && SAMPLE_BITS != 10)
            $fatal(1, "SAMPLE_BITS must be 8 or 10");
        if ((WORD_W % SAMPLE_BITS) != 0)
            $fatal(1, "SAMPLE_BITS must divide the 40-bit word");
        if (DECIM < 1)
            $fatal(1, "DECIM must be >= 1");
        if ((1 << FIFO_ADDR_WIDTH) < (WORDS_PER_PACKET + 1))
            $fatal(1, "FIFO too shallow to hold one datagram");
    end

    wire [7:0] decim_eff = (decim_cfg == 8'd0) ? DECIM[7:0] : decim_cfg;

    // runtime drop fraction overrides the elaboration default
    wire [15:0] drop_frac_eff = (drop_frac_cfg == 16'd0) ? DROP_FRAC[15:0] : drop_frac_cfg;

    // ================================================== 100 MHz ADC domain
    wire [SAMPLE_BITS-1:0] sample = adc_data[9 -: SAMPLE_BITS];

    reg  [WORD_W-SAMPLE_BITS-1:0] acc;    // held samples (40 - SAMPLE_BITS bits)
    reg  [2:0]  cnt;                      // 0..SPW-1
    reg  [15:0] dcount;
    wire        sample_tick = (decim_eff == 8'd1) ? 1'b1 : (dcount >= decim_eff - 8'd1);
    wire [WORD_W-1:0]  fifo_din   = {acc, sample};
    wire               fifo_wr_en = sample_tick && (cnt == SPW[2:0] - 3'd1);

    wire [WORD_W-1:0]  fifo_dout;
    wire               fifo_full;
    wire               fifo_empty;
    wire [FIFO_ADDR_WIDTH:0] fifo_avail;
    reg                fifo_rd_en;

    always @(posedge clk_adc or negedge rst_n) begin
        if (!rst_n) begin
            acc    <= {(WORD_W-SAMPLE_BITS){1'b0}};
            cnt    <= 3'd0;
            dcount <= decim_eff - 8'd1;       // first ADC sample is kept
        end else begin
            dcount <= sample_tick ? 16'd0 : (dcount + 16'd1);
            if (sample_tick) begin
                acc <= {acc[WORD_W-2*SAMPLE_BITS-1:0], sample};
                cnt <= (cnt == SPW[2:0] - 3'd1) ? 3'd0 : (cnt + 3'd1);
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
    reg         discarding;
    reg  [15:0] disc_cnt;
    reg  [15:0] byte_i;
    reg  [2:0]  bidx;
    reg  [WORD_W-1:0] cur, nxt;
    reg         nxt_v;
    reg  [15:0] fetched;
    reg         fetch_pend, cap_d;

    // -------------------------------------------------- delta-sigma dropper
    // Average drop probability = drop_frac/65536, but the decision is spread
    // aperiodically: the accumulator carries the phase so the long-run rate is
    // exact while the drop pattern is not periodic.
    reg  [16:0] drop_acc;
    wire [16:0] acc_plus = {1'b0, drop_acc} + {1'b0, drop_frac_eff};
    wire        ds_drop  = (acc_plus >= 17'd65536);
    wire [16:0] acc_nxt  = ds_drop ? (acc_plus - 17'd65536) : acc_plus;

    // -------------------------------------------------- FIFO high-water guard
    // Safety net only: if the read side stalls, drop harder so the FIFO can
    // never overflow. Nominal operation never reaches this level.
    wire fifo_hw = (fifo_avail >= HIGH_WATER[FIFO_ADDR_WIDTH:0]);

    wire can_start = !active && !upstream_busy && (fifo_avail >= WPP);
    wire drop_this = ds_drop | fifo_hw;

    assign send        = can_start && !drop_this;
    assign payload_len = PKTB;
    assign packet_seq  = cur_seq;

    wire fetch = active && !discarding && !nxt_v && !fetch_pend && (fetched < WPP);

    // A discard pops exactly WORDS_PER_PACKET words as fast as one per clock, so
    // the FIFO drains at the production rate and never backs up. (Counting bytes
    // at one per clock would be slower than the 100 MSPS fill rate.)
    wire want_read = discarding ? (disc_cnt < WPP) : fetch;

    wire word_phase = (byte_i >= HB);
    wire rotate     = active && !discarding && p_ready &&
                      ((byte_i == 16'd0) || (word_phase && (bidx == BPW-1)));

    always @(posedge clk_eth or negedge rst_n) begin
        if (!rst_n) fifo_rd_en <= 1'b0;
        else        fifo_rd_en <= want_read;
    end

    always @(posedge clk_eth or negedge rst_n) begin
        if (!rst_n) begin
            seq             <= 32'd0;
            cur_seq         <= 32'd0;
            base_sample     <= 32'd0;
            cur_drops       <= 32'd0;
            active          <= 1'b0;
            discarding      <= 1'b0;
            disc_cnt        <= 16'd0;
            byte_i          <= 16'd0;
            bidx            <= 3'd0;
            cur             <= {WORD_W{1'b0}};
            nxt             <= {WORD_W{1'b0}};
            nxt_v           <= 1'b0;
            fetched         <= 16'd0;
            fetch_pend      <= 1'b0;
            cap_d           <= 1'b0;
            drop_acc        <= 17'd0;
            packets_sent    <= 32'd0;
            packets_dropped <= 32'd0;
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
            end else if (cap_d && !discarding) begin
                nxt   <= fifo_dout;
                nxt_v <= 1'b1;
            end

            if (!active) begin
                if (can_start) begin
                    active   <= 1'b1;
                    drop_acc <= acc_nxt;          // charge the delta-sigma once per decision
                    seq      <= seq + 32'd1;      // every produced packet advances seq
                    if (drop_this) begin
                        // Deliberate drop: consume the words, transmit nothing.
                        // seq advanced, so the next header's base_sample jumps.
                        discarding <= 1'b1;
                        disc_cnt   <= 16'd0;
                    end else begin
                        byte_i     <= 16'd0;
                        bidx       <= 3'd0;
                        fetched    <= 16'd0;
                        nxt_v      <= 1'b0;
                        fetch_pend <= 1'b0;
                        cur_seq    <= seq;
                        base_sample<= seq * NSAMP;
                        cur_drops  <= ovf_bin;
                    end
                end
            end else if (discarding) begin
                if (disc_cnt == WPP - 16'd1) begin
                    discarding      <= 1'b0;
                    active          <= 1'b0;
                    packets_dropped <= packets_dropped + 32'd1;
                end else begin
                    disc_cnt <= disc_cnt + 16'd1;
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
    assign p_valid = active && !discarding;

endmodule

`default_nettype wire
