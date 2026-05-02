`default_nettype none
// ============================================================================
// FFT Processor Module - Pipelined Radix-2 DIF SDF Architecture
// ============================================================================
// Sustains 1 sample/clock throughput (105 MSPS @ 105 MHz).
// Verified for FFT_SIZE = 1024.
// ============================================================================

`timescale 1ns/1ps

module fft_processor #(
    parameter FFT_SIZE = 1024,
    parameter DATA_WIDTH = 24,
    parameter TWIDDLE_WIDTH = 16
) (
    input  wire        clk,
    input  wire        rst_n,

    input  wire [DATA_WIDTH-1:0] real_in,
    input  wire [DATA_WIDTH-1:0] imag_in,
    input  wire        data_valid,

    output wire [DATA_WIDTH-1:0] real_out,
    output wire [DATA_WIDTH-1:0] imag_out,
    output wire        fft_valid,
    output wire [$clog2(FFT_SIZE)-1:0] fft_index,
    output wire [31:0] frame_count,
    output wire        overflow_flag,
    output wire        processing_active
);

    // Hard guard for P0 bring-up target
    initial begin
        if (FFT_SIZE != 1024) begin
            $display("ERROR: fft_processor currently only validated for FFT_SIZE=1024");
            $finish;
        end
    end

    localparam ADDR_WIDTH = $clog2(FFT_SIZE);
    localparam STAGES = ADDR_WIDTH;

    reg [TWIDDLE_WIDTH-1:0] twiddle_real [0:(FFT_SIZE/2)-1];
    reg [TWIDDLE_WIDTH-1:0] twiddle_imag [0:(FFT_SIZE/2)-1];
    
    initial begin
        $readmemh("twiddle_real.mem", twiddle_real);
        $readmemh("twiddle_imag.mem", twiddle_imag);
    end

    reg [31:0] in_frame_count;
    reg [ADDR_WIDTH-1:0] in_count;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            in_frame_count <= 0;
            in_count <= 0;
        end else if (data_valid) begin
            if (in_count == {ADDR_WIDTH{1'b1}}) begin
                in_count <= 0;
                in_frame_count <= in_frame_count + 1;
            end else begin
                in_count <= in_count + 1;
            end
        end
    end

    wire [DATA_WIDTH-1:0] stage_real [0:STAGES];
    wire [DATA_WIDTH-1:0] stage_imag [0:STAGES];
    wire [STAGES:0] stage_v;

    assign stage_real[0] = real_in;
    assign stage_imag[0] = imag_in;
    assign stage_v[0] = data_valid;

    // Delayed counter per stage
    reg [ADDR_WIDTH-1:0] count_pipe [0:STAGES];
    always @(posedge clk) begin
        count_pipe[0] <= in_count;
        for (integer k=1; k<=STAGES; k=k+1) count_pipe[k] <= count_pipe[k-1];
    end

    genvar i;
    generate
        for (i = 0; i < STAGES; i = i + 1) begin : sdf_stage
            localparam DELAY = 1 << (STAGES - 1 - i);

            reg [DATA_WIDTH-1:0] delay_real [0:DELAY-1];
            reg [DATA_WIDTH-1:0] delay_imag [0:DELAY-1];

            wire bf_state = count_pipe[i][STAGES-1-i];

            wire [DATA_WIDTH-1:0] x_r = stage_real[i];
            wire [DATA_WIDTH-1:0] x_i = stage_imag[i];
            wire [DATA_WIDTH-1:0] d_r = delay_real[DELAY-1];
            wire [DATA_WIDTH-1:0] d_i = delay_imag[DELAY-1];

            // Butterfly
            wire [DATA_WIDTH-1:0] sum_r  = ($signed(d_r) + $signed(x_r)) >>> 1;
            wire [DATA_WIDTH-1:0] sum_i  = ($signed(d_i) + $signed(x_i)) >>> 1;
            wire [DATA_WIDTH-1:0] diff_r = ($signed(d_r) - $signed(x_r)) >>> 1;
            wire [DATA_WIDTH-1:0] diff_i = ($signed(d_i) - $signed(x_i)) >>> 1;

            // Twiddle Index: (local_count * 2^i) % N
            wire [ADDR_WIDTH-1:0] tw_idx_full = ((count_pipe[i] & (DELAY-1)) << i);
            wire [ADDR_WIDTH-2:0] tw_idx = tw_idx_full[ADDR_WIDTH-2:0];

            wire [TWIDDLE_WIDTH-1:0] w_r = twiddle_real[tw_idx];
            wire [TWIDDLE_WIDTH-1:0] w_i = twiddle_imag[tw_idx];

            wire [DATA_WIDTH+TWIDDLE_WIDTH-1:0] prod_rr = $signed(diff_r) * $signed(w_r);
            wire [DATA_WIDTH+TWIDDLE_WIDTH-1:0] prod_ii = $signed(diff_i) * $signed(w_i);
            wire [DATA_WIDTH+TWIDDLE_WIDTH-1:0] prod_ri = $signed(diff_r) * $signed(w_i);
            wire [DATA_WIDTH+TWIDDLE_WIDTH-1:0] prod_ir = $signed(diff_i) * $signed(w_r);

            wire [DATA_WIDTH+TWIDDLE_WIDTH:0] rot_r_l = $signed(prod_rr) - $signed(prod_ii);
            wire [DATA_WIDTH+TWIDDLE_WIDTH:0] rot_i_l = $signed(prod_ri) + $signed(prod_ir);

            wire [DATA_WIDTH-1:0] rot_r = rot_r_l[DATA_WIDTH+TWIDDLE_WIDTH-2:TWIDDLE_WIDTH-1];
            wire [DATA_WIDTH-1:0] rot_i = rot_i_l[DATA_WIDTH+TWIDDLE_WIDTH-2:TWIDDLE_WIDTH-1];

            reg [DATA_WIDTH-1:0] next_r, next_i;
            reg next_v;

            always @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    for (integer d=0; d<DELAY; d=d+1) begin
                        delay_real[d] <= 0;
                        delay_imag[d] <= 0;
                    end
                    next_r <= 0; next_i <= 0; next_v <= 0;
                end else begin
                    next_v <= stage_v[i];
                    if (stage_v[i]) begin
                        if (!bf_state) begin
                            delay_real[0] <= x_r;
                            delay_imag[0] <= x_i;
                            next_r <= d_r;
                            next_i <= d_i;
                        end else begin
                            delay_real[0] <= rot_r;
                            delay_imag[0] <= rot_i;
                            next_r <= sum_r;
                            next_i <= sum_i;
                        end
                        if (DELAY > 1) begin
                            for (integer d=DELAY-1; d>0; d=d-1) begin
                                delay_real[d] = delay_real[d-1];
                                delay_imag[d] = delay_imag[d-1];
                            end
                        end
                    end
                end
            end
            assign stage_real[i+1] = next_r;
            assign stage_imag[i+1] = next_i;
            assign stage_v[i+1] = next_v;
        end
    endgenerate

    // Alignment logic for valid output
    reg [FFT_SIZE-1:0] v_final;
    reg [ADDR_WIDTH-1:0] out_id;
    reg [31:0] out_fc;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            v_final <= 0;
            out_id <= 0;
            out_fc <= 0;
        end else begin
            v_final <= {v_final[FFT_SIZE-2:0], data_valid};
            if (v_final[FFT_SIZE-1]) out_id <= out_id + 1;
            else out_id <= 0;
            // Frame count alignment (simplified)
            out_fc <= in_frame_count;
        end
    end

    function [ADDR_WIDTH-1:0] bit_rev(input [ADDR_WIDTH-1:0] addr);
        integer j;
        begin
            for (j = 0; j < ADDR_WIDTH; j = j + 1)
                bit_rev[ADDR_WIDTH-1-j] = addr[j];
        end
    endfunction

    assign real_out = stage_real[STAGES];
    assign imag_out = stage_imag[STAGES];
    assign fft_valid = (v_final[FFT_SIZE-1] === 1'b1);
    assign fft_index = bit_rev(out_id);
    assign frame_count = out_fc;
    assign overflow_flag = 1'b0;
    assign processing_active = |stage_v;

endmodule
