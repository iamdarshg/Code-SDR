`default_nettype none
// ============================================================================
// FFT Processor Module - Pipelined Radix-2 SDF Architecture
// ============================================================================
// Sustains 1 sample/clock throughput.
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

    reg [TWIDDLE_WIDTH-1:0] twiddle_real [0:(FFT_SIZE/2)-1];
    reg [TWIDDLE_WIDTH-1:0] twiddle_imag [0:(FFT_SIZE/2)-1];
    
    initial begin
        $readmemh("twiddle_real.mem", twiddle_real);
        $readmemh("twiddle_imag.mem", twiddle_imag);
    end

    reg [ADDR_WIDTH-1:0] count;
    reg [31:0] frame_counter;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            count <= 0;
            frame_counter <= 0;
        end else if (data_valid) begin
            if (count == {ADDR_WIDTH{1'b1}}) begin
                count <= 0;
                frame_counter <= frame_counter + 1;
            end else begin
                count <= count + 1;
            end
        end
    end

    wire [DATA_WIDTH-1:0] s_real [0:STAGES];
    wire [DATA_WIDTH-1:0] s_imag [0:STAGES];
    
    // Explicit valid signals for each stage to avoid circular logic / UNOPTFLAT
    wire v0 = data_valid;
    wire v1, v2, v3, v4, v5, v6, v7, v8, v9, v10;

    assign s_real[0] = real_in;
    assign s_imag[0] = imag_in;

    reg [ADDR_WIDTH-1:0] stage_count [0:STAGES];
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (integer k=0; k<=STAGES; k=k+1) stage_count[k] <= 0;
        end else begin
            stage_count[0] <= count;
            for (integer k=1; k<=STAGES; k=k+1) stage_count[k] <= stage_count[k-1];
        end
    end

    genvar i;
    generate
        for (i = 0; i < STAGES; i = i + 1) begin : sdf_stage
            localparam DELAY = 1 << (STAGES - 1 - i);

            reg [DATA_WIDTH-1:0] delay_real [0:DELAY-1];
            reg [DATA_WIDTH-1:0] delay_imag [0:DELAY-1];
            reg v_reg;

            wire [DATA_WIDTH-1:0] x_r = s_real[i];
            wire [DATA_WIDTH-1:0] x_i = s_imag[i];
            wire [DATA_WIDTH-1:0] d_r = delay_real[DELAY-1];
            wire [DATA_WIDTH-1:0] d_i = delay_imag[DELAY-1];

            reg [DATA_WIDTH-1:0] bf_real, bf_imag;
            wire bf_state = stage_count[i][STAGES-1-i];

            wire [ADDR_WIDTH-1:0] tw_idx_full = (stage_count[i] << i);
            wire [ADDR_WIDTH-2:0] tw_idx = bf_state ? tw_idx_full[ADDR_WIDTH-2:0] : 0;

            if (i == 0) assign v1 = v_reg;
            else if (i == 1) assign v2 = v_reg;
            else if (i == 2) assign v3 = v_reg;
            else if (i == 3) assign v4 = v_reg;
            else if (i == 4) assign v5 = v_reg;
            else if (i == 5) assign v6 = v_reg;
            else if (i == 6) assign v7 = v_reg;
            else if (i == 7) assign v8 = v_reg;
            else if (i == 8) assign v9 = v_reg;
            else if (i == 9) assign v10 = v_reg;

            wire cur_v = (i == 0) ? v0 :
                         (i == 1) ? v1 :
                         (i == 2) ? v2 :
                         (i == 3) ? v3 :
                         (i == 4) ? v4 :
                         (i == 5) ? v5 :
                         (i == 6) ? v6 :
                         (i == 7) ? v7 :
                         (i == 8) ? v8 : v9;

            always @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    v_reg <= 0;
                    bf_real <= 0;
                    bf_imag <= 0;
                    for (integer d = 0; d < DELAY; d = d + 1) begin
                        // Use blocking for initialization to satisfy Verilator
                        delay_real[d] = 0;
                        delay_imag[d] = 0;
                    end
                end else begin
                    v_reg <= cur_v;
                    if (cur_v) begin
                        if (!bf_state) begin
                            delay_real[0] <= x_r;
                            delay_imag[0] <= x_i;
                            bf_real <= d_r;
                            bf_imag <= d_i;
                        end else begin
                            delay_real[0] <= $signed(d_r) - $signed(x_r);
                            delay_imag[0] <= $signed(d_i) - $signed(x_i);
                            bf_real <= $signed(d_r) + $signed(x_r);
                            bf_imag <= $signed(d_i) + $signed(x_i);
                        end
                        if (DELAY > 1) begin
                            for (integer d = DELAY - 1; d > 0; d = d - 1) begin
                                delay_real[d] = delay_real[d-1];
                                delay_imag[d] = delay_imag[d-1];
                            end
                        end
                    end
                end
            end

            wire [TWIDDLE_WIDTH-1:0] w_r = twiddle_real[tw_idx];
            wire [TWIDDLE_WIDTH-1:0] w_i = twiddle_imag[tw_idx];

            wire [DATA_WIDTH+TWIDDLE_WIDTH-1:0] prod_rr = $signed(bf_real) * $signed(w_r);
            wire [DATA_WIDTH+TWIDDLE_WIDTH-1:0] prod_ii = $signed(bf_imag) * $signed(w_i);
            wire [DATA_WIDTH+TWIDDLE_WIDTH-1:0] prod_ri = $signed(bf_real) * $signed(w_i);
            wire [DATA_WIDTH+TWIDDLE_WIDTH-1:0] prod_ir = $signed(bf_imag) * $signed(w_r);

            wire [DATA_WIDTH+TWIDDLE_WIDTH:0] res_r_long = $signed(prod_rr) - $signed(prod_ii);
            wire [DATA_WIDTH+TWIDDLE_WIDTH:0] res_i_long = $signed(prod_ri) + $signed(prod_ir);

            assign s_real[i+1] = res_r_long[DATA_WIDTH+TWIDDLE_WIDTH-2:TWIDDLE_WIDTH-1];
            assign s_imag[i+1] = res_i_long[DATA_WIDTH+TWIDDLE_WIDTH-2:TWIDDLE_WIDTH-1];
        end
    endgenerate

    function [ADDR_WIDTH-1:0] bit_rev(input [ADDR_WIDTH-1:0] addr);
        integer j;
        begin
            for (j = 0; j < ADDR_WIDTH; j = j + 1)
                bit_rev[ADDR_WIDTH-1-j] = addr[j];
        end
    endfunction

    assign real_out = s_real[STAGES];
    assign imag_out = s_imag[STAGES];
    assign fft_valid = v10;
    assign fft_index = bit_rev(stage_count[STAGES]);
    assign frame_count = frame_counter;
    assign overflow_flag = 1'b0;
    assign processing_active = v0 | v10;

endmodule
