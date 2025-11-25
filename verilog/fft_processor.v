// ============================================================================
// FFT Processor Module
// ============================================================================
// 1024-point Radix-2 FFT processor with pipelined architecture
// ============================================================================

`timescale 1ns/1ps

module fft_processor #(
    parameter FFT_SIZE = 1024,
    parameter DATA_WIDTH = 24
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [DATA_WIDTH-1:0] real_in,   // Real input data
    input  wire [DATA_WIDTH-1:0] imag_in,   // Imaginary input data
    input  wire        data_valid,      // Input data valid
    output wire [DATA_WIDTH-1:0] real_out,  // Real output data
    output wire [DATA_WIDTH-1:0] imag_out,  // Imaginary output data
    output wire        fft_valid,       // FFT output valid
    output wire [10:0] fft_index        // FFT bin index
);

    // ========================================================================
    // Parameter calculations
    // ========================================================================
    
    localparam STAGES = $clog2(FFT_SIZE);
    localparam TWIDDLE_WIDTH = 16;
    
    // ========================================================================
    // Input buffer and bit-reversal addressing
    // ========================================================================
    
    // Input sample counter
    reg [9:0] sample_counter;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sample_counter <= 10'd0;
        end else if (data_valid) begin
            sample_counter <= sample_counter + 1;
        end
    end
    
    // Bit-reversed address generation
    function [9:0] bit_reverse(input [9:0] addr);
        integer i;
        begin
            for (i = 0; i < 10; i = i + 1) begin
                bit_reverse[i] = addr[9-i];
            end
        end
    endfunction
    
    wire [9:0] bit_reversed_addr;
    assign bit_reversed_addr = bit_reverse(sample_counter);
    
    // Input memory (bit-reversed storage)
    reg [DATA_WIDTH-1:0] input_real [0:FFT_SIZE-1];
    reg [DATA_WIDTH-1:0] input_imag [0:FFT_SIZE-1];
    
    always @(posedge clk) begin
        if (data_valid) begin
            input_real[bit_reversed_addr] <= real_in;
            input_imag[bit_reversed_addr] <= imag_in;
        end
    end
    
    // ========================================================================
    // Twiddle factor ROM
    // ========================================================================
    
    reg [TWIDDLE_WIDTH-1:0] twiddle_real [0:(FFT_SIZE/2)-1];
    reg [TWIDDLE_WIDTH-1:0] twiddle_imag [0:(FFT_SIZE/2)-1];
    
    // Initialize twiddle factors
    initial begin
        integer k, n;
        real angle, cos_val, sin_val;
        for (k = 0; k < FFT_SIZE/2; k = k + 1) begin
            // Twiddle factor: W_N^k = e^(-j*2*pi*k/N)
            angle = -2.0 * 3.141592653589793 * k / FFT_SIZE;
            cos_val = $cos(angle);
            sin_val = $sin(angle);
            
            // Convert to fixed-point
            twiddle_real[k] = $rtoi(cos_val * (2.0 ** (TWIDDLE_WIDTH - 1) - 1));
            twiddle_imag[k] = $rtoi(sin_val * (2.0 ** (TWIDDLE_WIDTH - 1) - 1));
        end
    end
    
    // ========================================================================
    // Pipeline stage storage
    // ========================================================================
    
    // Define storage for each pipeline stage
    reg [DATA_WIDTH-1:0] stage_real [0:STAGES][0:FFT_SIZE-1];
    reg [DATA_WIDTH-1:0] stage_imag [0:STAGES][0:FFT_SIZE-1];
    
    // Stage control signals
    reg [STAGES-1:0] stage_valid;
    reg [9:0] stage_counter [0:STAGES-1];
    
    // ========================================================================
    // Stage 0: Input to first butterfly stage
    // ========================================================================
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            stage_valid[0] <= 1'b0;
            stage_counter[0] <= 10'd0;
        end else begin
            if (sample_counter == FFT_SIZE - 1) begin
                // Start processing when input buffer is full
                stage_valid[0] <= 1'b1;
                stage_counter[0] <= 10'd0;
            end else if (stage_valid[0]) begin
                stage_counter[0] <= stage_counter[0] + 1;
            end
        end
    end
    
    // Load stage 0 data
    always @(posedge clk) begin
        if (stage_valid[0]) begin
            stage_real[0][stage_counter[0]] <= input_real[stage_counter[0]];
            stage_imag[0][stage_counter[0]] <= input_imag[stage_counter[0]];
        end
    end
    
    // ========================================================================
    // Butterfly processing stages
    // ========================================================================
    
    genvar stage, index;
    generate
        for (stage = 1; stage < STAGES; stage = stage + 1) begin : fft_stages
            
            // Stage control
            always @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    stage_valid[stage] <= 1'b0;
                    stage_counter[stage] <= 10'd0;
                end else begin
                    if (stage_counter[stage-1] == FFT_SIZE - 1 && stage_valid[stage-1]) begin
                        stage_valid[stage] <= 1'b1;
                        stage_counter[stage] <= 10'd0;
                    end else if (stage_valid[stage]) begin
                        stage_counter[stage] <= stage_counter[stage] + 1;
                    end
                end
            end
            
            // Butterfly computation for this stage
            always @(posedge clk) begin
                if (stage_valid[stage]) begin
                    // Calculate butterfly indices
                    // For stage s, butterflies are of size 2^s, with stride 2^s
                    integer butterfly_size;
                    integer butterfly_index;
                    integer group_index;
                    
                    butterfly_size = 1 << stage;
                    butterfly_index = stage_counter[stage] % butterfly_size;
                    group_index = (stage_counter[stage] / butterfly_size) * (butterfly_size * 2);
                    
                    // Compute butterfly pairs
                    if (butterfly_index < (butterfly_size / 2)) begin
                        integer upper_index;
                        upper_index = group_index + butterfly_index + (butterfly_size / 2);
                        
                        // Butterfly computation
                        // W = twiddle_factor for this stage
                        wire [TWIDDLE_WIDTH-1:0] twiddle_r;
                        wire [TWIDDLE_WIDTH-1:0] twiddle_i;
                        assign twiddle_r = twiddle_real[(1 << (stage-1)) * butterfly_index];
                        assign twiddle_i = twiddle_imag[(1 << (stage-1)) * butterfly_index];
                        
                        // Load input values
                        reg [DATA_WIDTH-1:0] a_r, a_i, b_r, b_i;
                        a_r = stage_real[stage-1][group_index + butterfly_index];
                        a_i = stage_imag[stage-1][group_index + butterfly_index];
                        b_r = stage_real[stage-1][upper_index];
                        b_i = stage_imag[stage-1][upper_index];
                        
                        // Twiddle multiplication: b * W
                        wire [63:0] mult_br, mult_bi, mult_bi_r, mult_br_r;
                        wire [63:0] mult_br_i, mult_bi_i;
                        
                        // b_r * w_r - b_i * w_i
                        assign mult_br = $signed(b_r) * $signed({{(DATA_WIDTH-TWIDDLE_WIDTH){twiddle_r[TWIDDLE_WIDTH-1]}}, twiddle_r});
                        assign mult_bi = $signed(b_i) * $signed({{(DATA_WIDTH-TWIDDLE_WIDTH){twiddle_i[TWIDDLE_WIDTH-1]}}, twiddle_i});
                        assign mult_br_i = mult_br - mult_bi;
                        
                        // b_r * w_i + b_i * w_r
                        assign mult_bi_r = $signed(b_i) * $signed({{(DATA_WIDTH-TWIDDLE_WIDTH){twiddle_r[TWIDDLE_WIDTH-1]}}, twiddle_r});
                        assign mult_br_r = $signed(b_r) * $signed({{(DATA_WIDTH-TWIDDLE_WIDTH){twiddle_i[TWIDDLE_WIDTH-1]}}, twiddle_i});
                        assign mult_bi_i = mult_bi_r + mult_br_r;
                        
                        // Butterfly outputs
                        // Top output: a + b*W
                        // Bottom output: a - b*W
                        stage_real[stage][group_index + butterfly_index] <= 
                            mult_br_i[(DATA_WIDTH + TWIDDLE_WIDTH - 2) : (TWIDDLE_WIDTH - 1)] + a_r;
                        stage_imag[stage][group_index + butterfly_index] <= 
                            mult_bi_i[(DATA_WIDTH + TWIDDLE_WIDTH - 2) : (TWIDDLE_WIDTH - 1)] + a_i;
                        stage_real[stage][upper_index] <= 
                            mult_br_i[(DATA_WIDTH + TWIDDLE_WIDTH - 2) : (TWIDDLE_WIDTH - 1)] - a_r;
                        stage_imag[stage][upper_index] <= 
                            mult_bi_i[(DATA_WIDTH + TWIDDLE_WIDTH - 2) : (TWIDDLE_WIDTH - 1)] - a_i;
                    end
                end
            end
        end
    endgenerate
    
    // ========================================================================
    // Output stage
    // ========================================================================
    
    reg [9:0] output_counter;
    reg       output_valid_reg;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            output_counter <= 10'd0;
            output_valid_reg <= 1'b0;
        end else begin
            if (stage_counter[STAGES-1] == FFT_SIZE - 1 && stage_valid[STAGES-1]) begin
                output_valid_reg <= 1'b1;
                output_counter <= 10'd0;
            end else if (output_valid_reg) begin
                output_counter <= output_counter + 1;
                if (output_counter == FFT_SIZE - 1) begin
                    output_valid_reg <= 1'b0;
                end
            end
        end
    end
    
    // ========================================================================
    // Output assignment
    // ========================================================================
    
    assign real_out = output_valid_reg ? stage_real[STAGES-1][output_counter] : 24'd0;
    assign imag_out = output_valid_reg ? stage_imag[STAGES-1][output_counter] : 24'd0;
    assign fft_valid = output_valid_reg;
    assign fft_index = output_counter;

endmodule
