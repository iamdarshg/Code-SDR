`default_nettype none
// ============================================================================
// FFT Processor Module - Pipelined Radix-2 DIF SDF Architecture
// Optimized for Synthesis and Hardware Safety with Parameter-Safe Latency.
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

    localparam ADDR_WIDTH = $clog2(FFT_SIZE);
    localparam STAGES = ADDR_WIDTH;
    // Total Latency = (N-1) structural delay + STAGES pipeline delay
    localparam PIPELINE_LATENCY = (FFT_SIZE - 1) + STAGES;

    // Twiddle ROMs
    reg [TWIDDLE_WIDTH-1:0] twiddle_real [0:(FFT_SIZE/2)-1];
    reg [TWIDDLE_WIDTH-1:0] twiddle_imag [0:(FFT_SIZE/2)-1];
    initial begin
        $readmemh("twiddle_real.mem", twiddle_real);
        $readmemh("twiddle_imag.mem", twiddle_imag);
    end

    // Persistent Counter Logic
    reg [ADDR_WIDTH-1:0] in_count;
    reg [31:0] frame_counter;
    reg active_frame;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            in_count <= 0;
            frame_counter <= 0;
            active_frame <= 0;
        end else begin
            if (data_valid) active_frame <= 1'b1;

            if (active_frame || data_valid) begin
                if (in_count == {ADDR_WIDTH{1'b1}}) begin
                    in_count <= 0;
                    frame_counter <= frame_counter + 1;
                    if (!data_valid) active_frame <= 1'b0;
                end else begin
                    in_count <= in_count + 1;
                end
            end
        end
    end

    // Data and Control Pipelines
    wire [DATA_WIDTH-1:0] stage_r [0:STAGES];
    wire [DATA_WIDTH-1:0] stage_i [0:STAGES];

    assign stage_r[0] = data_valid ? real_in : {DATA_WIDTH{1'b0}};
    assign stage_i[0] = data_valid ? imag_in : {DATA_WIDTH{1'b0}};

    wire [ADDR_WIDTH-1:0] count_pipe [0:STAGES];
    reg  [ADDR_WIDTH-1:0] count_reg [0:STAGES-1];
    reg  [31:0] frame_count_pipe [0:STAGES];

    assign count_pipe[0] = in_count;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (integer k=0; k<STAGES; k=k+1) count_reg[k] <= 0;
            for (integer k=0; k<=STAGES; k=k+1) frame_count_pipe[k] <= 0;
        end else begin
            frame_count_pipe[0] <= frame_counter;
            for (integer k=0; k<STAGES; k=k+1) begin
                count_reg[k] <= count_pipe[k];
                frame_count_pipe[k+1] <= frame_count_pipe[k];
            end
        end
    end

    genvar k;
    generate
        for (k=0; k<STAGES; k=k+1) begin : count_assign
            assign count_pipe[k+1] = count_reg[k];
        end
    endgenerate

    // Overflow Detection
    reg sticky_overflow;
    wire [STAGES-1:0] stage_overflow;

    // SDF Stages
    genvar i;
    generate
        for (i = 0; i < STAGES; i = i + 1) begin : sdf_stage
            localparam DELAY = 1 << (STAGES - 1 - i);
            reg [DATA_WIDTH-1:0] delay_real [0:DELAY-1];
            reg [DATA_WIDTH-1:0] delay_imag [0:DELAY-1];

            wire bf_state = count_pipe[i][STAGES-1-i];

            wire [DATA_WIDTH-1:0] x_r = stage_r[i];
            wire [DATA_WIDTH-1:0] x_i = stage_i[i];
            wire [DATA_WIDTH-1:0] d_r = delay_real[DELAY-1];
            wire [DATA_WIDTH-1:0] d_i = delay_imag[DELAY-1];

            // Butterfly
            wire [DATA_WIDTH:0] full_s_r = $signed(d_r) + $signed(x_r);
            wire [DATA_WIDTH:0] full_s_i = $signed(d_i) + $signed(x_i);
            wire [DATA_WIDTH:0] full_d_r = $signed(d_r) - $signed(x_r);
            wire [DATA_WIDTH:0] full_d_i = $signed(d_i) - $signed(x_i);

            wire sum_ov_r = (full_s_r[DATA_WIDTH] != full_s_r[DATA_WIDTH-1]);
            wire sum_ov_i = (full_s_i[DATA_WIDTH] != full_s_i[DATA_WIDTH-1]);
            wire diff_ov_r = (full_d_r[DATA_WIDTH] != full_d_r[DATA_WIDTH-1]);
            wire diff_ov_i = (full_d_i[DATA_WIDTH] != full_d_i[DATA_WIDTH-1]);

            // Scaled outputs
            wire [DATA_WIDTH-1:0] sum_r  = full_s_r[DATA_WIDTH:1];
            wire [DATA_WIDTH-1:0] sum_i  = full_s_i[DATA_WIDTH:1];
            wire [DATA_WIDTH-1:0] diff_r = full_d_r[DATA_WIDTH:1];
            wire [DATA_WIDTH-1:0] diff_i = full_d_i[DATA_WIDTH:1];

            // Twiddle
            wire [ADDR_WIDTH-1:0] tw_idx_full = (count_pipe[i] << i);
            wire [ADDR_WIDTH-2:0] tw_idx = tw_idx_full[ADDR_WIDTH-2:0];
            wire [TWIDDLE_WIDTH-1:0] w_r = twiddle_real[tw_idx];
            wire [TWIDDLE_WIDTH-1:0] w_i = twiddle_imag[tw_idx];

            // Complex Rotation
            wire [39:0] p_rr = $signed(diff_r) * $signed(w_r);
            wire [39:0] p_ii = $signed(diff_i) * $signed(w_i);
            wire [39:0] p_ri = $signed(diff_r) * $signed(w_i);
            wire [39:0] p_ir = $signed(diff_i) * $signed(w_r);

            wire [40:0] r_r_l = $signed(p_rr) - $signed(p_ii);
            wire [40:0] r_i_l = $signed(p_ri) + $signed(p_ir);

            wire [40:0] rot_r_full = $signed(r_r_l) >>> 15;
            wire [40:0] rot_i_full = $signed(r_i_l) >>> 15;
            wire [DATA_WIDTH-1:0] rot_r = rot_r_full[DATA_WIDTH-1:0];
            wire [DATA_WIDTH-1:0] rot_i = rot_i_full[DATA_WIDTH-1:0];

            wire rot_ov_r = (rot_r_full[40:DATA_WIDTH-1] != { (41-DATA_WIDTH+1){rot_r_full[DATA_WIDTH-1]} });
            wire rot_ov_i = (rot_i_full[40:DATA_WIDTH-1] != { (41-DATA_WIDTH+1){rot_i_full[DATA_WIDTH-1]} });

            assign stage_overflow[i] = sum_ov_r | sum_ov_i | diff_ov_r | diff_ov_i | rot_ov_r | rot_ov_i;

            reg [DATA_WIDTH-1:0] next_r, next_i;

            always @(posedge clk) begin
                if (!bf_state) begin
                    delay_real[0] <= x_r;
                    delay_imag[0] <= x_i;
                end else begin
                    delay_real[0] <= rot_r;
                    delay_imag[0] <= rot_i;
                end
                for (integer d=1; d<DELAY; d=d+1) begin
                    delay_real[d] <= delay_real[d-1];
                    delay_imag[d] <= delay_imag[d-1];
                end
            end

            always @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    next_r <= 0; next_i <= 0;
                end else begin
                    if (!bf_state) begin
                        next_r <= d_r;
                        next_i <= d_i;
                    end else begin
                        next_r <= sum_r;
                        next_i <= sum_i;
                    end
                end
            end
            assign stage_r[i+1] = next_r;
            assign stage_i[i+1] = next_i;
        end
    endgenerate

    // Sticky Overflow Logic
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) sticky_overflow <= 1'b0;
        else if (|stage_overflow) sticky_overflow <= 1'b1;
    end

    // Parameter-Safe Latency Pipeline
    reg [PIPELINE_LATENCY-1:0] v_pipe;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) v_pipe <= 0;
        else v_pipe <= {v_pipe[PIPELINE_LATENCY-2:0], data_valid};
    end

    function [ADDR_WIDTH-1:0] bit_rev(input [ADDR_WIDTH-1:0] addr);
        integer j;
        begin
            for (j = 0; j < ADDR_WIDTH; j = j + 1)
                bit_rev[ADDR_WIDTH-1-j] = addr[j];
        end
    endfunction

    reg [ADDR_WIDTH-1:0] out_cnt;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) out_cnt <= 0;
        else if (v_pipe[PIPELINE_LATENCY-1]) out_cnt <= out_cnt + 1;
        else out_cnt <= 0;
    end

    assign real_out = stage_r[STAGES];
    assign imag_out = stage_i[STAGES];
    assign fft_valid = v_pipe[PIPELINE_LATENCY-1];
    assign fft_index = bit_rev(out_cnt);
    assign frame_count = frame_count_pipe[STAGES];
    assign overflow_flag = sticky_overflow;
    assign processing_active = |v_pipe;

endmodule
