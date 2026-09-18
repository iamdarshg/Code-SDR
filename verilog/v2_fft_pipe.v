// ============================================================================
// v2_fft_pipe - pipelined radix-2 single-path delay feedback (R2SDF) FFT.
//
// STATUS: THROUGHPUT VERIFIED, ARITHMETIC CORRECTNESS NOT YET CORRECT.
//
//   * Throughput: confirmed by v2_fft_pipe_tb - 128 consecutive output bins at
//     1 bin/clock, i.e. a genuine 100 MSPS consumer (the memory-based
//     v2_fft1024 tops out near 6-8 MSPS because it needs ~22k cycles/frame).
//   * Correctness: NOT achieved. An impulse produces the expected flat spectrum,
//     but that test is permutation-invariant (any ordering of a flat spectrum is
//     flat), so it proves nothing about ordering. A DC input and a single tone
//     both produce wrong spectra. Do NOT build this until the R2SDF schedule,
//     twiddle indexing and output ordering are reconciled against a reference.
//
// It is deliberately NOT instantiated by v2_top and not run in CI; the verified
// memory-based v2_fft1024 is still the FFT mode.
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
    reg signed [TW-1:0] twr [0:N/2-1];
    reg signed [TW-1:0] twi [0:N/2-1];

    initial begin
        $readmemh(RE_FILE, twr);
        $readmemh(IM_FILE, twi);
        if ((N & (N-1)) != 0)
            $fatal(1, "v2_fft_pipe N must be a power of two");
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

            wire [QB-1:0] q       = cnt[QB-1:0];
            wire          first   = (cnt < H);

            wire signed [DW-1:0] are = bre[q];
            wire signed [DW-1:0] aim = bim[q];

            // twiddle index for this stage: j << i, j = cnt - H in the 2nd half
            wire [LOGN-1:0] j     = cnt - H[LOGN-1:0];
            wire [LOGN-1:0] twidx = j << i;
            wire signed [TW-1:0] wr = twr[twidx];
            wire signed [TW-1:0] wi = twi[twidx];

            // butterfly
            wire signed [DW:0] sum_re = are + d_re;
            wire signed [DW:0] sum_im = aim + d_im;
            wire signed [DW:0] dif_re = are - d_re;
            wire signed [DW:0] dif_im = aim - d_im;

            // complex multiply dif * w, Karatsuba (3 multiplies)
            wire signed [2*DW+1:0] p1 = dif_re * wr;
            wire signed [2*DW+1:0] p2 = dif_im * wi;
            wire signed [2*DW+1:0] p3 = (dif_re + dif_im) * (wr + wi);
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
                end else begin
                    // The datapath must free-run, NOT be gated by the input
                    // valid: after a finite burst the delay lines still hold
                    // data that has to flush out. Gating the counter and buffer
                    // on s_vld freezes the pipeline and truncates every frame.
                    bre[q] <= wr_r;
                    bim[q] <= wr_i;
                    cnt <= (cnt == M[LOGN-1:0] - 1'b1) ? {LOGN{1'b0}} : (cnt + 1'b1);
                end
            end
        end
    endgenerate

    // --------------------------------------------------------------- output
    // Bins emerge bit-reversed; pair the stream with the natural bin index.
    //
    // Data latency: each stage's output depends on a delay-line read that is
    // H = M/2 samples old, so the chain delays by sum(N/2^i) = N-1 cycles. The
    // valid flag must be delayed by the SAME amount - propagating it
    // combinationally alongside the data puts it N-1 cycles early and every
    // frame is captured as zeros.
    localparam integer VLAT = N - 1;
    reg [VLAT-1:0] vsr;
    reg [LOGN-1:0] ocount;

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
            out_valid <= 1'b0;
            out_re    <= {DW{1'b0}};
            out_im    <= {DW{1'b0}};
            out_index <= {LOGN{1'b0}};
            ocount    <= {LOGN{1'b0}};
        end else begin
            vsr       <= {vsr[VLAT-2:0], in_valid};
            out_valid <= vsr[VLAT-1];
            out_re    <= s_re[LOGN];
            out_im    <= s_im[LOGN];
            out_index <= brev(ocount);
            if (vsr[VLAT-1])
                ocount <= ocount + 1'b1;
        end
    end

endmodule

`default_nettype wire
