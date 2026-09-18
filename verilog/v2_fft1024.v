// ============================================================================
// v2_fft1024 - resource-frugal 1024-point complex FFT.
//
// Architecture: iterative radix-2 decimation-in-time, in-place, over one RAM,
// with ONE butterfly. Three real 16x16 multipliers implement the complex
// multiply (Karatsuba form, 3 multiplies instead of 4). This is the cheapest
// structure that produces a full 1024-bin spectrum; a pipelined/parallel FFT
// would need ~10 complex multipliers (~6000 LUT) and does not fit.
//
// Memory: re[1024] + im[1024] at 16-bit = 32 kbit -> ~4 EBR blocks.
// Twiddles: the existing 512-entry Q15 tables (cos and -sin), ROM.
//
// Cost model:
//   5120 butterflies x 4 cycles + 1024 load + 1024 out ~= 22500 cycles/frame.
//   At 100 MHz that is ~4.5 MSPS sustained input, so the CIC must decimate the
//   100 MSPS ADC by 32 before this block.
//
// Throughput note: with one shared butterfly the transform cannot consume the
// full ADC rate. Resolution (1024 bins) is traded for instantaneous bandwidth.
//
// Input is loaded at bit-reversed addresses so the output emerges in natural
// bin order and the packetizer can stream it straight out.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_fft1024 #(
    parameter integer N          = 1024,
    parameter integer LOGN       = 10,
    parameter integer W          = 16,
    parameter         TWID_FILE  = "verilog/twiddle_real.mem",
    parameter         TWID_FILEI = "verilog/twiddle_imag.mem",
    parameter         WINDOW_FILE= "verilog/window_coeff.mem",
    parameter integer WINDOW_EN  = 1
) (
    input  wire                clk,
    input  wire                rst_n,

    input  wire                start,        // pulse: begin a frame
    input  wire signed [W-1:0] in_re,
    input  wire signed [W-1:0] in_im,
    input  wire                in_valid,
    output wire                in_ready,

    output reg  signed [W-1:0] out_re,
    output reg  signed [W-1:0] out_im,
    output reg  [LOGN-1:0]     out_index,
    output reg                 out_valid,

    output reg                 busy,
    output reg                 frame_done,
    output reg  [31:0]         frame_count,
    output reg  [31:0]         overflow_count,
    // Block-floating-point exponent: the host multiplies bin magnitudes by
    // 2^scale_exp to normalise. `overflow` latches if any butterfly output ever
    // saturated, which is the error tolerance indicator for the caller.
    output reg  [4:0]          scale_exp,
    output reg                 overflow
);

    // ------------------------------------------------------------------ memory
    reg signed [W-1:0] mem_re [0:N-1];
    reg signed [W-1:0] mem_im [0:N-1];

    // ---------------------------------------------------------------- twiddles
    reg signed [W-1:0] twr [0:N/2-1];
    reg signed [W-1:0] twi [0:N/2-1];
    reg signed [W-1:0] win [0:N-1];

    initial begin
        $readmemh(TWID_FILE,  twr);
        $readmemh(TWID_FILEI, twi);
        if (WINDOW_EN) $readmemh(WINDOW_FILE, win);
    end

    // window applied at load, with rounding (Q15 coefficient)
    wire signed [2*W-1:0] win_prod = in_re * win[load_cnt];
    wire signed [W-1:0]   win_re   = (win_prod + (32'sd1 << 14)) >>> 15;

    // ------------------------------------------------------------------- state
    localparam [2:0] S_IDLE=3'd0, S_LOAD=3'd1, S_CALC=3'd2, S_OUT=3'd3, S_DONE=3'd4;
    reg [2:0]  state;
    reg [1:0]  sub;            // 0..3 micro-step inside a butterfly

    reg [LOGN-1:0] load_cnt;
    reg [LOGN-1:0] out_cnt;

    reg [4:0]  stage;          // 1..LOGN
    reg [LOGN:0] m;            // 1 << stage
    reg [LOGN-1:0] k, j;       // group base, position within half group

    // butterfly operand registers
    reg signed [W-1:0] ar, ai, br, bi;
    reg signed [W-1:0] wr, wi;
    reg signed [2*W-1:0] p1, p2, p3;
    reg signed [W:0]     t_re, t_im;

    // Block floating point: one shared exponent for the whole frame. After each
    // stage we note whether any output reached the guard threshold; the next
    // stage then shifts every read right by one (with rounding) and the exponent
    // increments. Scaling only when required keeps the numerical noise far below
    // the 10-bit ADC instead of losing a bit every stage unconditionally.
    reg        rd_scale;      // shift this stage's reads
    reg        need_scale;    // an output this stage reached the threshold
    initial begin
        rd_scale   = 1'b0;
        need_scale = 1'b0;
    end

    // m can be N (2^LOGN) at the last stage, so it needs LOGN+1 bits; half is
    // m/2 which fits in LOGN bits. Taking m[LOGN-1:0] here would truncate m=1024
    // to 0 and hang the sequencer.
    wire [LOGN-1:0] half = m[LOGN:1];
    wire [LOGN-1:0] idx_a = k + j;
    wire [LOGN-1:0] idx_b = k + j + half;

    // twiddle index = j * (N/m) = j << (LOGN - stage)
    wire [LOGN-1:0] tw_idx_full = j << (LOGN[4:0] - stage);
    wire [LOGN-2:0] tw_idx = tw_idx_full[LOGN-2:0];

    wire signed [W:0] sum_w = wr + wi;
    wire signed [W:0] sum_b = br + bi;

    // Q15 -> integer conversion of the three Karatsuba products, with rounding.
    localparam signed [2*W-1:0] RND = {{(2*W-1){1'b0}}, 1'b1} <<< 14;
    wire signed [W:0] q1 = ($signed({p1[2*W-1], p1}) + $signed({RND[2*W-1], RND})) >>> 15;
    wire signed [W:0] q2 = ($signed({p2[2*W-1], p2}) + $signed({RND[2*W-1], RND})) >>> 15;
    wire signed [W:0] q3 = ($signed({p3[2*W-1], p3}) + $signed({RND[2*W-1], RND})) >>> 15;

    // ------------------------------------------------- BFP read scaling (>>1)
    // Applied at the start of a stage only if the previous stage grew too far.
    wire signed [W:0] a_re_e = $signed({mem_re[idx_a][W-1], mem_re[idx_a]});
    wire signed [W:0] a_im_e = $signed({mem_im[idx_a][W-1], mem_im[idx_a]});
    wire signed [W:0] b_re_e = $signed({mem_re[idx_b][W-1], mem_re[idx_b]});
    wire signed [W:0] b_im_e = $signed({mem_im[idx_b][W-1], mem_im[idx_b]});
    wire signed [W-1:0] rd_ar = rd_scale ? ((a_re_e + 17'sd1) >>> 1) : mem_re[idx_a];
    wire signed [W-1:0] rd_ai = rd_scale ? ((a_im_e + 17'sd1) >>> 1) : mem_im[idx_a];
    wire signed [W-1:0] rd_br = rd_scale ? ((b_re_e + 17'sd1) >>> 1) : mem_re[idx_b];
    wire signed [W-1:0] rd_bi = rd_scale ? ((b_im_e + 17'sd1) >>> 1) : mem_im[idx_b];

    // ---------------------------------------------------- butterfly outputs
    // Widened to W+2 so a genuine carry out of the signed range is visible.
    wire signed [W+1:0] o1_re = $signed({ar[W-1],ar[W-1],ar}) + $signed({t_re[W],t_re});
    wire signed [W+1:0] o1_im = $signed({ai[W-1],ai[W-1],ai}) + $signed({t_im[W],t_im});
    wire signed [W+1:0] o2_re = $signed({ar[W-1],ar[W-1],ar}) - $signed({t_re[W],t_re});
    wire signed [W+1:0] o2_im = $signed({ai[W-1],ai[W-1],ai}) - $signed({t_im[W],t_im});

    // Guard at 2^(W-2)-1 so a following doubling can never exceed the signed range.
    localparam signed [W+1:0] GUARD = ({{(W+1){1'b0}}, 1'b1} << (W-2)) - 1;
    wire thresh_hit =
        (o1_re >= GUARD) || (o1_re <= -GUARD) ||
        (o1_im >= GUARD) || (o1_im <= -GUARD) ||
        (o2_re >= GUARD) || (o2_re <= -GUARD) ||
        (o2_im >= GUARD) || (o2_im <= -GUARD);

    // real signed overflow: the top two bits disagree
    wire sat_hit =
        (o1_re[W+1] ^ o1_re[W]) || (o1_im[W+1] ^ o1_im[W]) ||
        (o2_re[W+1] ^ o2_re[W]) || (o2_im[W+1] ^ o2_im[W]);

    assign in_ready = (state == S_LOAD) && (load_cnt < N);
    assign busy     = (state != S_IDLE) && (state != S_DONE);

    integer i;

    // bit-reverse a 10-bit index
    function [LOGN-1:0] brev;
        input [LOGN-1:0] v;
        integer b;
        begin
            for (b = 0; b < LOGN; b = b + 1)
                brev[b] = v[LOGN-1-b];
        end
    endfunction

    task calc_next;
        begin
            if (j == half - 1'b1) begin
                j <= {LOGN{1'b0}};
                if ((k + m) >= N) begin
                    // last group of this stage: fold in the BFP decision
                    rd_scale   <= need_scale;
                    need_scale <= 1'b0;
                    if (need_scale) scale_exp <= scale_exp + 5'd1;
                    if (stage == LOGN[4:0]) begin
                        state   <= S_OUT;
                        out_cnt <= {LOGN{1'b0}};
                        sub     <= 2'd0;
                    end else begin
                        stage <= stage + 5'd1;
                        m     <= m << 1;
                        k     <= {LOGN{1'b0}};
                        j     <= {LOGN{1'b0}};
                    end
                end else begin
                    k <= k + m[LOGN-1:0];
                end
            end else begin
                j <= j + 1'b1;
            end
        end
    endtask

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state       <= S_IDLE;
            sub         <= 2'd0;
            load_cnt    <= {LOGN{1'b0}};
            out_cnt     <= {LOGN{1'b0}};
            stage       <= 5'd1;
            m           <= {{(LOGN+1){1'b0}}};
            rd_scale    <= 1'b0;
            need_scale  <= 1'b0;
            scale_exp   <= 5'd0;
            overflow    <= 1'b0;
            k           <= {LOGN{1'b0}};
            j           <= {LOGN{1'b0}};
            out_re      <= {W{1'b0}};
            out_im      <= {W{1'b0}};
            out_index   <= {LOGN{1'b0}};
            out_valid   <= 1'b0;
            frame_done  <= 1'b0;
            frame_count <= 32'd0;
            overflow_count <= 32'd0;
        end else begin
            out_valid  <= 1'b0;
            frame_done <= 1'b0;

            case (state)
                // ------------------------------------------------------- load
                S_IDLE: begin
                    if (start) begin
                        load_cnt <= {LOGN{1'b0}};
                        state    <= S_LOAD;
                    end
                end

                S_LOAD: begin
                    if (in_valid) begin
                        mem_re[brev(load_cnt)] <= (WINDOW_EN != 0) ? win_re : in_re;
                        mem_im[brev(load_cnt)] <= in_im;
                        load_cnt <= load_cnt + 1'b1;
                        if (load_cnt == N - 1) begin
                            stage <= 5'd1;
                            m     <= {{(LOGN){1'b0}}, 1'b1} << 1;  // m = 2
                            k     <= {LOGN{1'b0}};
                            j     <= {LOGN{1'b0}};
                            sub   <= 2'd0;
                            // fresh block-floating-point state per frame
                            rd_scale   <= 1'b0;
                            need_scale <= 1'b0;
                            scale_exp  <= 5'd0;
                            overflow   <= 1'b0;
                            state <= S_CALC;
                        end
                    end
                end

                // ------------------------------------------------------ calc
                S_CALC: begin
                    case (sub)
                        2'd0: begin
                            ar <= rd_ar; ai <= rd_ai;
                            br <= rd_br; bi <= rd_bi;
                            wr <= twr[tw_idx];   wi <= twi[tw_idx];
                            sub <= 2'd1;
                        end
                        2'd1: begin
                            p1 <= wr * br;                 // wr*br
                            p2 <= wi * bi;                 // wi*bi
                            p3 <= sum_w * sum_b;           // (wr+wi)*(br+bi)
                            sub <= 2'd2;
                        end
                        2'd2: begin
                            // Twiddles are Q15 fractional, so each product must be
                            // shifted back down by 15. Round (add half an LSB)
                            // before shifting to keep the numerical noise low; the
                            // accumulated FFT noise stays far below the 10-bit ADC.
                            t_re <= q1 - q2;
                            t_im <= q3 - q1 - q2;
                            sub  <= 2'd3;
                        end
                        default: begin
                            mem_re[idx_a] <= o1_re[W-1:0];
                            mem_im[idx_a] <= o1_im[W-1:0];
                            mem_re[idx_b] <= o2_re[W-1:0];
                            mem_im[idx_b] <= o2_im[W-1:0];
                            if (thresh_hit) need_scale <= 1'b1;
                            if (sat_hit)
                                overflow <= 1'b1;      // latch: tolerance indicator
                            calc_next;
                            sub <= 2'd0;
                        end
                    endcase
                end

                // ------------------------------------------------------- out
                // Two cycles per bin: fetch into the output registers, then
                // assert valid on the following cycle so index and data are
                // registered together (no skew).
                S_OUT: begin
                    if (sub == 2'd0) begin
                        out_re    <= rd_scale ?
                                     (({mem_re[out_cnt][W-1], mem_re[out_cnt]} + 17'sd1) >>> 1) :
                                     mem_re[out_cnt];
                        out_im    <= rd_scale ?
                                     (({mem_im[out_cnt][W-1], mem_im[out_cnt]} + 17'sd1) >>> 1) :
                                     mem_im[out_cnt];
                        out_index <= out_cnt;
                        out_valid <= 1'b1;
                        sub       <= 2'd1;
                    end else begin
                        sub <= 2'd0;
                        if (out_cnt == N - 1) begin
                            frame_count <= frame_count + 32'd1;
                            frame_done  <= 1'b1;
                            state       <= S_DONE;
                        end else begin
                            out_cnt <= out_cnt + 1'b1;
                        end
                    end
                end

                S_DONE: state <= S_IDLE;
                default: state <= S_IDLE;
            endcase
        end
    end

endmodule

`default_nettype wire
