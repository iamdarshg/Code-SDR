// ============================================================================
// v2_fft_pipe - pipelined radix-2 single-path delay feedback (R2SDF) FFT.
//
// STATUS: VERIFIED - throughput AND arithmetic.
//
//   * Throughput: 128 consecutive output bins at 1 bin/clock, a genuine
//     100 MSPS consumer (the memory-based v2_fft1024 tops out near 6-8 MSPS
//     because it needs ~22k cycles/frame).
//   * Arithmetic: v2_fft_pipe_tb checks the output against a DFT computed in
//     the testbench itself (not hardcoded) for an impulse, DC, and tones at
//     bins 1, 8, 17 and 31. All 64 bins match within +/-4 LSB. tone@17 is
//     deliberate: it is one of the twiddles that exposed the Karatsuba bug
//     described below.
//
// Three real bugs were found and fixed getting here, all of them invisible to
// a DC-only or impulse-only test:
//
//   1. DELAY-LINE INDEX. q was taken as the low QB bits of cnt, but the
//      reference is q = cnt % H. For the last stage H=1 that made q = cnt[0],
//      indexing bre[1] out of bounds and returning X on every second clock,
//      which poisoned the whole combinational chain.
//   2. KARATSUBA OPERAND SUMS. Verilog sizes `a + b` self-determined, so
//      `(dif_re + dif_im) * (wr + wi)` evaluated wr+wi at TW bits. |cos|+|sin|
//      reaches 1.41*2^(TW-1), overflowing a Q(TW-1) word for 15 of the 32
//      twiddles. This corrupts only the difference/twiddle path, so a DC input
//      (where every difference is zero) passes while everything else fails.
//   3. PIPELINE PHASE. The stage counters free-ran from reset, so the frame
//      boundary depended on how many idle cycles elapsed before in_valid. Two
//      testbenches differing by two idle cycles produced different spectra.
//      `run` now holds the pipeline in reset until the first valid sample.
//
// Transform length: N is free (any power of two up to TBL_N) with NO per-size
// table. Stage i of an N-point transform needs W_N^(j<<i), and
//     W_N^(j<<i) == W_TBL^((j<<i) * (TBL_N/N))
// so a smaller transform simply indexes the same shared ROM with a shift. One
// 1024-point table therefore serves N = 64, 256 and 1024 (all verified in
// v2_fft_pipe_tb). Regenerate it with:
//     python tools/gen_twiddles.py --n 1024 --width 12 --out verilog
//
// Why it exists: a pipelined FFT consumes 1 sample/cycle (the 100 MSPS path) but
// needs one complex multiplier PER STAGE (log2(N)), so it trades FFT size for
// throughput. N=64 -> 6 stages ~2.7k LUTs; N=1024 would not fit.
// ============================================================================
// Structure (per stage i, block M = N >> i, delay H = M/2):
//   first half  : out = delay_rd;      delay_wr = in
//   second half : out = delay_rd + in; delay_wr = (delay_rd - in) * W
// which is the standard R2SDF butterfly: the sum goes forward, the twiddled
// difference is fed back through the delay line and emerges in the next block.
//
// Output emerges in bit-reversed order; out_index carries the natural bin index
// so the packetizer can stream it directly.
//
// Scaling: a >>1 (with rounding) every other stage keeps the growth inside the
// datapath. With DW=14 and 3 scalings, a +-512 (10-bit) input maps to +-4096.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_fft_pipe #(
    parameter integer N     = 64,
    parameter integer LOGN  = 6,
    parameter integer DW    = 14,          // stored data width (signed)
    parameter integer TW    = 12,          // twiddle width (signed, Q(TW-1))
    // One shared twiddle ROM serves every transform length: stage i of an
    // N-point transform needs W_N^(j<<i), and W_N^(j<<i) == W_TBL^((j<<i)*(TBL_N/N)),
    // so an N < TBL_N transform simply indexes the same table with a shift.
    // TBL_N must be a power of two and a multiple of N.
    parameter integer TBL_N = 1024,
    parameter         RE_FILE = "verilog/tw12_real.mem",
    parameter         IM_FILE = "verilog/tw12_imag.mem"
) (
    input  wire                  clk,
    input  wire                  rst_n,
    input  wire                  in_valid,
    input  wire signed [DW-1:0]  in_re,
    input  wire signed [DW-1:0]  in_im,

    output reg                   out_valid,
    output reg  signed [DW-1:0]  out_re,
    output reg  signed [DW-1:0]  out_im,
    output reg  [LOGN-1:0]       out_index
);

    // ---------------------------------------------------------------- twiddles
    localparam integer TBW       = $clog2(TBL_N);        // twiddle index width
    localparam integer TBL_SHIFT = $clog2(TBL_N / N);    // shared-table index scale

    reg signed [TW-1:0] twr [0:TBL_N/2-1];
    reg signed [TW-1:0] twi [0:TBL_N/2-1];

    initial begin
        $readmemh(RE_FILE, twr);
        $readmemh(IM_FILE, twi);
        if ((N & (N-1)) != 0)
            $fatal(1, "v2_fft_pipe N must be a power of two");
        if (TBL_N < N || (TBL_N & (TBL_N-1)) != 0 || (TBL_N % N) != 0)
            $fatal(1, "v2_fft_pipe TBL_N must be a power of two and a multiple of N");
    end

    localparam signed [DW+1:0] RND = ({{(DW+1){1'b0}}, 1'b1} <<< (TW-2));

    // ------------------------------------------------------------ stage chain
    // The data between stages.
    wire signed [DW-1:0] s_re [0:LOGN];      // s[0] = input, s[LOGN] = output
    wire signed [DW-1:0] s_im [0:LOGN];
    wire                 s_vld [0:LOGN];

    assign s_re[0]  = in_re;
    assign s_im[0]  = in_im;
    assign s_vld[0] = in_valid;

    // ------------------------------------------------------------ phase align
    // The stage counters and delay lines must be PHASE-ALIGNED to the first
    // valid sample. If they free-run from reset, the frame boundary depends on
    // how many idle cycles elapsed between reset and in_valid, which shifts the
    // entire output stream and scrambles the spectrum. Measured directly: two
    // testbenches differing only by two idle cycles produced different spectra.
    //
    // `run` holds the whole pipeline in its reset state until the first sample
    // arrives, then lets it run on (case OR-ed), so a finite burst still
    // flushes its delay lines instead of being frozen mid-frame.
    reg  started;
    wire run = started | in_valid;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)        started <= 1'b0;
        else if (in_valid) started <= 1'b1;
    end

    genvar i;
    generate
        for (i = 0; i < LOGN; i = i + 1) begin : g_stage
            localparam integer M = N >> i;          // block length
            localparam integer H = M >> 1;          // delay depth
            localparam integer QB = (H > 1) ? $clog2(H) : 1;

            reg [LOGN-1:0] cnt;
            reg signed [DW-1:0] bre [0:H-1];
            reg signed [DW-1:0] bim [0:H-1];

            // scale every other stage to bound growth
            wire scale_in = (i % 2) == 1;
            wire signed [DW-1:0] d_re = scale_in ? ((s_re[i] + 1) >>> 1) : s_re[i];
            wire signed [DW-1:0] d_im = scale_in ? ((s_im[i] + 1) >>> 1) : s_im[i];

            // Delay-line index is cnt % H (the reference model's `q = cnt % H`).
            // For H = 1 that is always 0: taking the low QB bits would give
            // cnt[0], which indexes bre[1] out of bounds and returns X on every
            // second clock - which then poisons the whole combinational chain.
            wire [QB-1:0] q       = (H > 1) ? cnt[QB-1:0] : {QB{1'b0}};
            wire          first   = (cnt < H);

            wire signed [DW-1:0] are = bre[q];
            wire signed [DW-1:0] aim = bim[q];

            // twiddle index for this stage: j << i, j = cnt - H in the 2nd half
            wire [LOGN-1:0] j     = cnt - H[LOGN-1:0];
            wire [TBW-1:0]  twidx = j << (i + TBL_SHIFT);
            wire signed [TW-1:0] wr = twr[twidx];
            wire signed [TW-1:0] wi = twi[twidx];

            // butterfly
            wire signed [DW:0] sum_re = are + d_re;
            wire signed [DW:0] sum_im = aim + d_im;
            wire signed [DW:0] dif_re = are - d_re;
            wire signed [DW:0] dif_im = aim - d_im;

            // complex multiply dif * w, Karatsuba (3 multiplies)
            //
            // The operand sums MUST be widened. Verilog sizes `a + b` self-
            // determined, so writing `(dif_re + dif_im) * (wr + wi)` evaluates
            // the twiddle sum at TW bits: |cos| + |sin| reaches 1.41 * 2^(TW-1),
            // which overflows a Q(TW-1) word for 15 of the 32 twiddles. That
            // silently corrupts only the difference/twiddle path - which a DC
            // input never touches, so DC passes while every other input fails.
            wire signed [DW+1:0]   dif_sum = dif_re + dif_im;
            wire signed [TW:0]     w_sum   = wr + wi;
            wire signed [2*DW+1:0] p1 = dif_re * wr;
            wire signed [2*DW+1:0] p2 = dif_im * wi;
            wire signed [2*DW+1:0] p3 = dif_sum * w_sum;
            // Q(TW-1): pull the product back down by TW-1 with rounding
            wire signed [DW+1:0] mr = (p1 - p2 + RND) >>> (TW-1);
            wire signed [DW+1:0] mi = (p3 - p1 - p2 + RND) >>> (TW-1);

            wire signed [DW-1:0] out_r = first ? are : sum_re[DW-1:0];
            wire signed [DW-1:0] out_i = first ? aim : sum_im[DW-1:0];
            wire signed [DW-1:0] wr_r  = first ? d_re : mr[DW-1:0];
            wire signed [DW-1:0] wr_i  = first ? d_im : mi[DW-1:0];

            assign s_re[i+1]  = out_r;
            assign s_im[i+1]  = out_i;
            assign s_vld[i+1] = s_vld[i];

            integer k;
            always @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    cnt <= {LOGN{1'b0}};
                    for (k = 0; k < H; k = k + 1) begin
                        bre[k] <= {DW{1'b0}};
                        bim[k] <= {DW{1'b0}};
                    end
                end else if (run) begin
                    // Free-run once started (do NOT gate on s_vld): after a
                    // finite burst the delay lines still hold data that has to
                    // flush out. `run` only holds the pipeline before the very
                    // first valid sample, to fix the block phase.
                    bre[q] <= wr_r;
                    bim[q] <= wr_i;
                    cnt <= (cnt == M[LOGN-1:0] - 1'b1) ? {LOGN{1'b0}} : (cnt + 1'b1);
                end
            end
        end
    endgenerate

    // --------------------------------------------------------------- output
    // Bins emerge bit-reversed. Two separate things have to line up here:
    //
    //  1. ALIGNMENT. The last stage's value is captured into a register, so in
    //     cycle c the presented data is the last stage's value from c-1, while
    //     the valid flag is in_valid delayed by N-1. Measured against the
    //     reference oracle that leaves the whole stream exactly one output
    //     early (observed dut[p] == ref[p+1]). A second output register puts
    //     the data and the valid flag back in step, so the frame lands at
    //     dut[N-1 .. 2N-2].
    //
    //  2. INDEX. The transform occupies stream positions [N-1, 2N-1): the first
    //     N-1 outputs are just the delay lines flushing. So the sample at
    //     stream position p is bin bitrev((p - (N-1)) mod N) = bitrev(p+1).
    //     With ocount counting outputs from reset, out_index = brev(ocount+1).
    localparam integer VLAT = N - 1;
    reg [VLAT-1:0] vsr;
    reg [LOGN-1:0] ocount;

    reg signed [DW-1:0] dre, dim;   // extra data register (see note 1 above)

    function [LOGN-1:0] brev;
        input [LOGN-1:0] v;
        integer b;
        begin
            for (b = 0; b < LOGN; b = b + 1)
                brev[b] = v[LOGN-1-b];
        end
    endfunction

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            vsr       <= {VLAT{1'b0}};
            ocount    <= {LOGN{1'b0}};
            dre       <= {DW{1'b0}};
            dim       <= {DW{1'b0}};
            out_valid <= 1'b0;
            out_re    <= {DW{1'b0}};
            out_im    <= {DW{1'b0}};
            out_index <= {LOGN{1'b0}};
        end else if (run) begin
            vsr       <= {vsr[VLAT-2:0], in_valid};
            out_valid <= vsr[VLAT-1];
            out_index <= brev(ocount - 1'b1);

            // Data takes one MORE register than the valid flag. Registering both
            // together shifts them by the same cycle and changes nothing; only a
            // relative shift moves the stream by one output.
            dre       <= s_re[LOGN];
            dim       <= s_im[LOGN];
            out_re    <= dre;
            out_im    <= dim;

            if (vsr[VLAT-1])
                ocount <= ocount + 1'b1;
        end
    end

endmodule

`default_nettype wire
