// ============================================================================
// FFT Processor Module
// ============================================================================
// Synthesizable radix-2 DIT FFT for the 1024-point receive path.
//
// Samples are written into bit-reversed addresses and processed in place, so
// output bins are emitted in natural order. Each butterfly scales by 1 bit to
// keep the 24-bit fixed-point path deterministic. overflow_flag is frame-local:
// it is cleared when a new frame starts and reported with that frame's bins.
// ============================================================================

`timescale 1ns/1ps

module fft_processor #(
    parameter FFT_SIZE = 1024,
    parameter DATA_WIDTH = 24,
    parameter SCALE_FACTOR = 24
) (
    input  wire                    clk,
    input  wire                    rst_n,
    input  wire [DATA_WIDTH-1:0]   real_in,
    input  wire [DATA_WIDTH-1:0]   imag_in,
    input  wire                    data_valid,
    output wire [DATA_WIDTH-1:0]   real_out,
    output wire [DATA_WIDTH-1:0]   imag_out,
    output wire                    fft_valid,
    output wire [11:0]             fft_index,
    output wire                    overflow_flag,
    output wire                    processing_active
);

    localparam INDEX_WIDTH = $clog2(FFT_SIZE);
    localparam STAGES = $clog2(FFT_SIZE);
    localparam TWIDDLE_WIDTH = 16;
    localparam signed [DATA_WIDTH-1:0] MAX_POS = {1'b0, {(DATA_WIDTH-1){1'b1}}};
    localparam signed [DATA_WIDTH-1:0] MIN_NEG = {1'b1, {(DATA_WIDTH-1){1'b0}}};

    localparam ST_LOAD    = 2'd0;
    localparam ST_PROCESS = 2'd1;
    localparam ST_OUTPUT  = 2'd2;

    reg [1:0] state;
    reg [INDEX_WIDTH-1:0] sample_counter;
    reg [INDEX_WIDTH-1:0] output_counter;
    reg [INDEX_WIDTH-1:0] group_base;
    reg [INDEX_WIDTH-1:0] butterfly_index;
    reg [3:0] stage;

    reg signed [DATA_WIDTH-1:0] fft_real [0:FFT_SIZE-1];
    reg signed [DATA_WIDTH-1:0] fft_imag [0:FFT_SIZE-1];
    reg signed [TWIDDLE_WIDTH-1:0] twiddle_real [0:(FFT_SIZE/2)-1];
    reg signed [TWIDDLE_WIDTH-1:0] twiddle_imag [0:(FFT_SIZE/2)-1];

    reg signed [DATA_WIDTH-1:0] real_out_reg;
    reg signed [DATA_WIDTH-1:0] imag_out_reg;
    reg fft_valid_reg;
    reg frame_overflow;
    reg output_overflow;

    integer init_k;
    real init_angle;
    initial begin
        for (init_k = 0; init_k < FFT_SIZE/2; init_k = init_k + 1) begin
            init_angle = -2.0 * 3.14159265358979323846 * init_k / FFT_SIZE;
            twiddle_real[init_k] = $rtoi($cos(init_angle) * ((1 << (TWIDDLE_WIDTH-1)) - 1));
            twiddle_imag[init_k] = $rtoi($sin(init_angle) * ((1 << (TWIDDLE_WIDTH-1)) - 1));
        end
    end

    function [INDEX_WIDTH-1:0] bit_reverse;
        input [INDEX_WIDTH-1:0] value;
        integer i;
        begin
            for (i = 0; i < INDEX_WIDTH; i = i + 1) begin
                bit_reverse[i] = value[INDEX_WIDTH-1-i];
            end
        end
    endfunction

    function signed [DATA_WIDTH-1:0] saturate;
        input signed [DATA_WIDTH+8:0] value;
        begin
            if (value > $signed(MAX_POS)) begin
                saturate = MAX_POS;
            end else if (value < $signed(MIN_NEG)) begin
                saturate = MIN_NEG;
            end else begin
                saturate = value[DATA_WIDTH-1:0];
            end
        end
    endfunction

    function did_saturate;
        input signed [DATA_WIDTH+8:0] value;
        begin
            did_saturate = (value > $signed(MAX_POS)) || (value < $signed(MIN_NEG));
        end
    endfunction

    wire [INDEX_WIDTH-1:0] half_span = {{(INDEX_WIDTH-1){1'b0}}, 1'b1} << stage;
    wire [INDEX_WIDTH-1:0] span = half_span << 1;
    wire [INDEX_WIDTH-1:0] index_a = group_base + butterfly_index;
    wire [INDEX_WIDTH-1:0] index_b = index_a + half_span;
    wire [INDEX_WIDTH-1:0] twiddle_step = FFT_SIZE >> (stage + 1);
    wire [INDEX_WIDTH-1:0] twiddle_index = butterfly_index * twiddle_step;

    reg signed [DATA_WIDTH-1:0] a_real;
    reg signed [DATA_WIDTH-1:0] a_imag;
    reg signed [DATA_WIDTH-1:0] b_real;
    reg signed [DATA_WIDTH-1:0] b_imag;
    reg signed [TWIDDLE_WIDTH-1:0] w_real;
    reg signed [TWIDDLE_WIDTH-1:0] w_imag;
    reg signed [DATA_WIDTH+TWIDDLE_WIDTH:0] mult_real;
    reg signed [DATA_WIDTH+TWIDDLE_WIDTH:0] mult_imag;
    reg signed [DATA_WIDTH+8:0] t_real;
    reg signed [DATA_WIDTH+8:0] t_imag;
    reg signed [DATA_WIDTH+8:0] top_real;
    reg signed [DATA_WIDTH+8:0] top_imag;
    reg signed [DATA_WIDTH+8:0] bot_real;
    reg signed [DATA_WIDTH+8:0] bot_imag;
    reg butterfly_overflow;

    always @* begin
        a_real = fft_real[index_a];
        a_imag = fft_imag[index_a];
        b_real = fft_real[index_b];
        b_imag = fft_imag[index_b];
        w_real = twiddle_real[twiddle_index[INDEX_WIDTH-2:0]];
        w_imag = twiddle_imag[twiddle_index[INDEX_WIDTH-2:0]];

        mult_real = ($signed(b_real) * $signed(w_real)) - ($signed(b_imag) * $signed(w_imag));
        mult_imag = ($signed(b_real) * $signed(w_imag)) + ($signed(b_imag) * $signed(w_real));

        t_real = (mult_real + (1 <<< (TWIDDLE_WIDTH-2))) >>> (TWIDDLE_WIDTH-1);
        t_imag = (mult_imag + (1 <<< (TWIDDLE_WIDTH-2))) >>> (TWIDDLE_WIDTH-1);

        top_real = ($signed(a_real) + t_real) >>> 1;
        top_imag = ($signed(a_imag) + t_imag) >>> 1;
        bot_real = ($signed(a_real) - t_real) >>> 1;
        bot_imag = ($signed(a_imag) - t_imag) >>> 1;

        butterfly_overflow = did_saturate(top_real) || did_saturate(top_imag) ||
                             did_saturate(bot_real) || did_saturate(bot_imag);
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= ST_LOAD;
            sample_counter <= {INDEX_WIDTH{1'b0}};
            output_counter <= {INDEX_WIDTH{1'b0}};
            group_base <= {INDEX_WIDTH{1'b0}};
            butterfly_index <= {INDEX_WIDTH{1'b0}};
            stage <= 4'd0;
            real_out_reg <= {DATA_WIDTH{1'b0}};
            imag_out_reg <= {DATA_WIDTH{1'b0}};
            fft_valid_reg <= 1'b0;
            frame_overflow <= 1'b0;
            output_overflow <= 1'b0;
        end else begin
            fft_valid_reg <= 1'b0;

            case (state)
                ST_LOAD: begin
                    if (data_valid) begin
                        fft_real[bit_reverse(sample_counter)] <= real_in;
                        fft_imag[bit_reverse(sample_counter)] <= imag_in;

                        if (sample_counter == FFT_SIZE-1) begin
                            sample_counter <= {INDEX_WIDTH{1'b0}};
                            group_base <= {INDEX_WIDTH{1'b0}};
                            butterfly_index <= {INDEX_WIDTH{1'b0}};
                            stage <= 4'd0;
                            frame_overflow <= 1'b0;
                            state <= ST_PROCESS;
                        end else begin
                            sample_counter <= sample_counter + 1'b1;
                        end
                    end
                end

                ST_PROCESS: begin
                    fft_real[index_a] <= saturate(top_real);
                    fft_imag[index_a] <= saturate(top_imag);
                    fft_real[index_b] <= saturate(bot_real);
                    fft_imag[index_b] <= saturate(bot_imag);
                    frame_overflow <= frame_overflow | butterfly_overflow;

                    if (butterfly_index == half_span - 1'b1) begin
                        butterfly_index <= {INDEX_WIDTH{1'b0}};
                        if (group_base == FFT_SIZE - span) begin
                            group_base <= {INDEX_WIDTH{1'b0}};
                            if (stage == STAGES - 1) begin
                                output_counter <= {INDEX_WIDTH{1'b0}};
                                output_overflow <= frame_overflow | butterfly_overflow;
                                state <= ST_OUTPUT;
                            end else begin
                                stage <= stage + 1'b1;
                            end
                        end else begin
                            group_base <= group_base + span;
                        end
                    end else begin
                        butterfly_index <= butterfly_index + 1'b1;
                    end
                end

                ST_OUTPUT: begin
                    real_out_reg <= fft_real[output_counter];
                    imag_out_reg <= fft_imag[output_counter];
                    fft_valid_reg <= 1'b1;

                    if (output_counter == FFT_SIZE-1) begin
                        output_counter <= {INDEX_WIDTH{1'b0}};
                        state <= ST_LOAD;
                    end else begin
                        output_counter <= output_counter + 1'b1;
                    end
                end

                default: begin
                    state <= ST_LOAD;
                end
            endcase
        end
    end

    assign real_out = real_out_reg;
    assign imag_out = imag_out_reg;
    assign fft_valid = fft_valid_reg;
    assign fft_index = {{(12-INDEX_WIDTH){1'b0}}, output_counter};
    assign overflow_flag = output_overflow;
    assign processing_active = (state != ST_LOAD) || data_valid;

endmodule
