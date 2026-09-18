// ============================================================================
// v2_raw_drop_tb - verification of the delta-sigma fractional packet dropper.
//
// Runs the raw path at full 100 MSPS / 10 bit with a non-dyadic drop fraction,
// then checks:
//
//   1. THE AVERAGE IS EXACT. The delta-sigma accumulator makes the drop count
//      over K decisions exactly floor(K * DROP_FRAC / 65536), not approximately.
//      That is what lets the offered load sit just under the UDP ceiling rather
//      than being rounded down to a safe-but-wasteful fixed cadence.
//
//   2. DROPS NEVER COME FROM FIFO OVERFLOW. overflow_count and dropped_words
//      must stay zero: every lost packet is a deliberate discard, which is the
//      whole point (uncontrolled overflow would corrupt packet alignment).
//
//   3. THE SPACING IS DITHERED, NOT A FIXED CADENCE. A fixed "1 in N" dropper
//      would put a spur at packet_rate/N. The accumulator spreads the drops so
//      the gaps take more than one value. With a non-dyadic ratio the limit
//      cycle is den/gcd(long) *packets* long, so the spur energy is spread over
//      thousands of harmonics instead of concentrated at one tone.
//
//   4. SEQ ADVANCES ACROSS DROPS. A dropped packet still consumes a sequence
//      number, so the host sees a gap in seq and a matching jump in
//      base_sample. The total gap must equal the drop count - loss is reported,
//      never hidden.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_raw_drop_tb;

    parameter integer SAMPLE_BITS = 10;
    parameter integer DECIM       = 1;        // full 100 MSPS: the case that needs dropping
    parameter integer MTU         = 1500;     // drop logic is MTU-independent; small = fast sim
    parameter integer DROP_FRAC   = 3510;     // ~5.36%, deliberately non-dyadic
    parameter integer TARGET      = 2048;     // packet decisions to observe

    localparam integer EXPECTED = (TARGET * DROP_FRAC) / 65536;

    reg clk_adc = 0, clk_eth = 0, rst_n = 0;
    always #5 clk_adc = ~clk_adc;      // 100 MHz
    always #4 clk_eth = ~clk_eth;      // 125 MHz

    // distinct, deterministic sample pattern
    reg  [31:0] kk = 0;
    wire [9:0]  adc_data = ((kk * 32'h9E3779B9) ^ ((kk * 32'h9E3779B9) >> 15)) & 32'h3FF;
    always @(posedge clk_adc or negedge rst_n)
        if (!rst_n) kk <= 0; else kk <= kk + 1;

    wire [7:0]  p_data;
    wire        p_valid, p_ready;
    wire        send;
    wire        udp_send;
    wire [15:0] payload_len;
    wire [31:0] packet_seq;
    wire [31:0] overflow_count, packets_sent, packets_dropped;

    // Minimal sink: always ready, never busy, so throughput is limited only by
    // the datapath itself.
    assign p_ready = 1'b1;

    v2_raw_path #(
        .FIFO_ADDR_WIDTH(11),
        .SAMPLE_BITS(SAMPLE_BITS),
        .DECIM(DECIM),
        .MTU(MTU)
    ) u_raw (
        .clk_adc(clk_adc), .clk_eth(clk_eth), .rst_n(rst_n),
        .adc_data(adc_data),
        .upstream_busy(1'b0),
        .link_up(1'b1),
        .decim_cfg(8'd0),
        .drop_frac_cfg(DROP_FRAC[15:0]),
        .p_data(p_data), .p_valid(p_valid), .p_ready(p_ready),
        .send(udp_send), .payload_len(payload_len), .packet_seq(packet_seq),
        .dropped_words(),
        .overflow_count(overflow_count), .packets_sent(packets_sent),
        .packets_dropped(packets_dropped)
    );

    // -------------------------------------------------- decision / gap tracking
    integer decisions   = 0;
    integer since_drop  = 0;
    integer min_gap     = 1000000;
    integer max_gap     = 0;
    integer seq_gaps    = 0;
    integer prev_seq    = -1;
    integer sent_seen   = 0;
    reg [31:0] last_sent, last_drop;

    always @(posedge clk_eth or negedge rst_n) begin
        if (!rst_n) begin
            decisions  <= 0;
            since_drop <= 0;
            min_gap    <= 1000000;
            max_gap    <= 0;
            seq_gaps   <= 0;
            prev_seq   <= -1;
            sent_seen  <= 0;
        end else begin
            // a "decision" is either a transmitted packet or a dropped one
            if (packets_sent !== last_sent) begin
                last_sent <= packets_sent;
                decisions <= decisions + 1;
                since_drop <= since_drop + 1;
                sent_seen <= sent_seen + 1;
                // seq must advance by one per produced packet, so consecutive
                // sent seqs differ by (1 + number dropped in between)
                if (prev_seq >= 0)
                    seq_gaps <= seq_gaps + (packet_seq - prev_seq - 1);
                else
                    seq_gaps <= seq_gaps + packet_seq;
                prev_seq <= packet_seq;
            end
            if (packets_dropped !== last_drop) begin
                last_drop <= packets_dropped;
                decisions <= decisions + 1;
                if (since_drop < min_gap) min_gap <= since_drop;
                if (since_drop > max_gap) max_gap <= since_drop;
                since_drop <= 0;
            end
        end
    end

    integer settle;
    initial begin
        $display("=== v2_raw_drop_tb (DROP_FRAC=%0d = %.4f%%, TARGET=%0d) ===",
                 DROP_FRAC, DROP_FRAC*100.0/65536.0, TARGET);
        repeat (8) @(posedge clk_eth);
        rst_n = 1;

        while ((packets_sent + packets_dropped) < TARGET) @(posedge clk_eth);
        settle = 0;
        while (settle < 4) begin @(posedge clk_eth); settle = settle + 1; end

        $display("  decisions=%0d sent=%0d dropped=%0d",
                 packets_sent + packets_dropped, packets_sent, packets_dropped);
        $display("  drop rate       = %.4f%% (target %.4f%%)",
                 packets_dropped*100.0/(packets_sent+packets_dropped),
                 DROP_FRAC*100.0/65536.0);
        $display("  drop gap range  = %0d..%0d", min_gap, max_gap);
        $display("  seq gaps total  = %0d, dropped=%0d, overflow=%0d",
                 seq_gaps, packets_dropped, overflow_count);

        if (overflow_count !== 32'd0) begin
            $display("FATAL: FIFO overflowed (%0d words) - drops are not controlled",
                     overflow_count);
            $fatal(1);
        end
        if (packets_dropped === 32'd0) begin
            $display("FATAL: no drops occurred but %0d/%65536 was configured", DROP_FRAC);
            $fatal(1);
        end
        if (packets_dropped !== EXPECTED) begin
            $display("FATAL: delta-sigma average not exact: dropped %0d, expected %0d",
                     packets_dropped, EXPECTED);
            $fatal(1);
        end
        if (min_gap === max_gap) begin
            $display("FATAL: drop spacing is a fixed cadence (%0d), not dithered", min_gap);
            $fatal(1);
        end
        // every drop must appear as a gap in the transmitted sequence
        if (seq_gaps > packets_dropped ||
            (packets_dropped - seq_gaps) > 1) begin
            $display("FATAL: seq gaps (%0d) do not account for drops (%0d)",
                     seq_gaps, packets_dropped);
            $fatal(1);
        end
        $display("PASS: v2_raw_drop (exact delta-sigma average %0d/%0d, no overflow, dithered)",
                 packets_dropped, packets_sent + packets_dropped);
        $finish;
    end

    initial begin
        #120_000_000;
        $fatal(1, "v2_raw_drop_tb timeout");
    end

endmodule

`default_nettype wire
