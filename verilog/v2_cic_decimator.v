// ============================================================================
// v2_cic_decimator - cascaded integrator-comb decimator (no multipliers).
//
// Sits between the 100 MSPS ADC and the FFT. Adders and registers only, which
// matters on a part with no DSP blocks.
//
//   integrators (fs)  ->  decimate by RATE  ->  combs (fs/RATE)
//
// RATE is programmable at run time through rate_cfg (0 = use the parameter), so
// the host can choose how much of the 100 MSPS stream to consume:
//
//   RATE=1   -> 100    MSPS  (CIC bypass, pass-through)
//   RATE=16  -> 6.25   MSPS  (fastest the 1024-point FFT can keep up with)
//   RATE=32  -> 3.125  MSPS
//   RATE=64  -> 1.5625 MSPS
//
// RATE must be a power of two. The output is shifted back down by the CIC gain
// (RATE^STAGES) with round-to-nearest so the magnitude is stable and the noise
// floor does not move with the ratio.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_cic_decimator #(
    parameter integer IN_WIDTH   = 10,
    parameter integer STAGES     = 3,
    parameter integer RATE       = 16,      // default decimation (power of two)
    parameter integer OUT_WIDTH  = 16,
    parameter integer ACC_WIDTH  = 36,      // must cover IN_WIDTH + STAGES*log2(MAX_RATE)
    parameter integer MAX_RATE   = 128      // largest selectable runtime rate
) (
    input  wire                       clk,
    input  wire                       rst_n,
    input  wire                       in_valid,
    input  wire signed [IN_WIDTH-1:0] din,
    input  wire [7:0]                 rate_cfg,   // 0 = use RATE parameter
    output reg                        out_valid,
    output reg  signed [OUT_WIDTH-1:0] dout,

    output reg  [31:0]                samples_in,
    output reg  [31:0]                samples_out
);

    localparam integer MAXBITS = $clog2(MAX_RATE);

    // ---------------------------------------------------- effective rate + shift
    // rate_eff is masked to a power of two up to MAX_RATE.
    wire [7:0] rate_sel = (rate_cfg == 8'd0) ? RATE[7:0] : rate_cfg;

    // Synthesizable log2 for the power-of-two rate (a variable-bound loop is not
    // accepted by synthesis tools).
    function [7:0] log2i;
        input [7:0] v;
        begin
            case (v)
                8'd1:    log2i = 8'd0;
                8'd2:    log2i = 8'd1;
                8'd4:    log2i = 8'd2;
                8'd8:    log2i = 8'd3;
                8'd16:   log2i = 8'd4;
                8'd32:   log2i = 8'd5;
                8'd64:   log2i = 8'd6;
                8'd128:  log2i = 8'd7;
                default: log2i = 8'd4;   // assume /16 if an invalid rate slips in
            endcase
        end
    endfunction

    // clamp rate_sel to [1, MAX_RATE]
    wire [7:0] rate_cl =
        (rate_sel == 8'd0)            ? 8'd1 :
        (rate_sel >  MAX_RATE[7:0])   ? MAX_RATE[7:0] : rate_sel;

    wire [7:0] shift_amt = STAGES[7:0] * log2i(rate_cl);

    initial begin
        if ((RATE & (RATE-1)) != 0)
            $fatal(1, "v2_cic_decimator RATE must be a power of two");
        if (ACC_WIDTH < IN_WIDTH + STAGES*MAXBITS + 4)
            $fatal(1, "v2_cic_decimator ACC_WIDTH too small for the gain");
    end

    wire signed [ACC_WIDTH-1:0] din_ext =
        {{(ACC_WIDTH-IN_WIDTH){din[IN_WIDTH-1]}}, din};

    // -------------------------------------------------- integrators (at fs)
    reg  signed [ACC_WIDTH-1:0] integ [0:STAGES-1];
    reg  signed [ACC_WIDTH-1:0] comb  [0:STAGES-1];
    reg  signed [ACC_WIDTH-1:0] cdel  [0:STAGES-1];

    reg  [MAXBITS-1:0] rcount;
    reg                dec_tick;

    integer i;
    reg signed [ACC_WIDTH-1:0] acc_i;
    reg signed [ACC_WIDTH-1:0] acc_c;

    // --------------------------------------------------- variable gain removal
    // (value + half) >>> shift, signed-safe via a widened intermediate.
    wire signed [ACC_WIDTH+1:0] comb_ext =
        $signed({comb[STAGES-1][ACC_WIDTH-1], comb[STAGES-1][ACC_WIDTH-1], comb[STAGES-1]});
    wire [ACC_WIDTH+1:0] round_bit =
        (shift_amt == 8'd0) ? {(ACC_WIDTH+2){1'b0}}
                            : ({{(ACC_WIDTH+1){1'b0}}, 1'b1} << (shift_amt - 8'd1));
    wire signed [ACC_WIDTH+1:0] shifted = (comb_ext + round_bit) >>> shift_amt;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (i = 0; i < STAGES; i = i + 1) begin
                integ[i] <= {ACC_WIDTH{1'b0}};
                comb[i]  <= {ACC_WIDTH{1'b0}};
                cdel[i]  <= {ACC_WIDTH{1'b0}};
            end
            rcount      <= {MAXBITS{1'b0}};
            dec_tick    <= 1'b0;
            out_valid   <= 1'b0;
            dout        <= {OUT_WIDTH{1'b0}};
            samples_in  <= 32'd0;
            samples_out <= 32'd0;
        end else begin
            out_valid <= 1'b0;
            dec_tick  <= 1'b0;

            if (in_valid) begin
                samples_in <= samples_in + 32'd1;

                acc_i = din_ext;
                for (i = 0; i < STAGES; i = i + 1) begin
                    integ[i] <= integ[i] + acc_i;
                    acc_i    = integ[i] + acc_i;
                end

                // rate_eff == 1 means pass-through: tick every sample
                if ((rate_cl == 8'd1) || (rcount == rate_cl[MAXBITS-1:0] - 1'b1)) begin
                    rcount   <= {MAXBITS{1'b0}};
                    dec_tick <= 1'b1;
                end else begin
                    rcount <= rcount + 1'b1;
                end
            end

            if (dec_tick) begin
                acc_c = integ[STAGES-1];
                for (i = 0; i < STAGES; i = i + 1) begin
                    comb[i] <= acc_c - cdel[i];
                    cdel[i] <= acc_c;
                    acc_c   = acc_c - cdel[i];
                end

                dout        <= shifted[OUT_WIDTH-1:0];
                out_valid   <= 1'b1;
                samples_out <= samples_out + 32'd1;
            end
        end
    end

endmodule

`default_nettype wire
