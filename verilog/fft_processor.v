// ============================================================================
// FFT Processor Module
// ============================================================================
// 4096-point Radix-2 FFT processor with optimized pipelined architecture
// Features: Proper bit-reversal input ordering, pipelined butterfly stages,
// improved fixed-point arithmetic with rounding and saturation, error detection
// ============================================================================

`timescale 1ns/1ps

module fft_processor #(
    parameter FFT_SIZE = 4096,
    parameter DATA_WIDTH = 24,
    parameter SCALE_FACTOR = 24  // Fixed-point scaling factor (2^24)
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [DATA_WIDTH-1:0] real_in,   // Real input data
    input  wire [DATA_WIDTH-1:0] imag_in,   // Imaginary input data
    input  wire        data_valid,      // Input data valid
    output wire [DATA_WIDTH-1:0] real_out,  // Real output data
    output wire [DATA_WIDTH-1:0] imag_out,  // Imaginary output data
    output wire        fft_valid,       // FFT output valid
    output wire [11:0] fft_index,       // FFT bin index
    output wire        overflow_flag,   // Overflow detection flag
    output wire        processing_active // FFT processing in progress
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
    reg [11:0] sample_counter;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sample_counter <= 12'd0;
        end else if (data_valid) begin
            sample_counter <= sample_counter + 1;
        end
    end
    
    // Bit-reversed address generation
    function [11:0] bit_reverse(input [11:0] addr);
        integer i;
        begin
            for (i = 0; i < 12; i = i + 1) begin
                bit_reverse[i] = addr[11-i];
            end
        end
    endfunction

    wire [11:0] bit_reversed_addr;
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
    // Pipeline stage storage -- Ping-pong buffers
    // ========================================================================

    // Define ping-pong buffers for pipeline stages
    reg [DATA_WIDTH-1:0] fft_buffer_real [0:1][0:FFT_SIZE-1];
    reg [DATA_WIDTH-1:0] fft_buffer_imag [0:1][0:FFT_SIZE-1];

    // Stage control signals
    reg [STAGES-1:0] stage_valid;
    reg [11:0] stage_counter [0:STAGES-1];

    // Overflow detection
    reg overflow_detected;
    reg processing_active_reg;

    // ========================================================================
    // Fixed-point arithmetic helper functions
    // ========================================================================

    // Function to perform saturated addition
    function [DATA_WIDTH-1:0] saturated_add(input [DATA_WIDTH-1:0] a, input [DATA_WIDTH-1:0] b);
        reg [DATA_WIDTH:0] sum;
        begin
            sum = {a[DATA_WIDTH-1], a} + {b[DATA_WIDTH-1], b};
            // Check for overflow (same sign inputs, different sign output)
            if (a[DATA_WIDTH-1] == b[DATA_WIDTH-1] && sum[DATA_WIDTH] != sum[DATA_WIDTH-1]) begin
                // Overflow occurred, saturate to maximum value
                saturated_add = {sum[DATA_WIDTH], {(DATA_WIDTH-1){!sum[DATA_WIDTH]}}};
            end else begin
                saturated_add = sum[DATA_WIDTH-1:0];
            end
        end
    endfunction

    // Function to perform saturated subtraction
    function [DATA_WIDTH-1:0] saturated_sub(input [DATA_WIDTH-1:0] a, input [DATA_WIDTH-1:0] b);
        begin
            saturated_sub = saturated_add(a, ~b + 1);
        end
    endfunction
    
    // ========================================================================
    // Stage 0: Input to first butterfly stage
    // ========================================================================
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            stage_valid[0] <= 1'b0;
            stage_counter[0] <= 12'd0;
        end else begin
            if (sample_counter == FFT_SIZE - 1) begin
                // Start processing when input buffer is full
                stage_valid[0] <= 1'b1;
                stage_counter[0] <= 12'd0;
            end else if (stage_valid[0]) begin
                stage_counter[0] <= stage_counter[0] + 1;
            end
        end
    end
    
    // Load stage 0 data
    always @(posedge clk) begin
        if (stage_valid[0]) begin
            fft_buffer_real[0][stage_counter[0]] <= input_real[stage_counter[0]];
            fft_buffer_imag[0][stage_counter[0]] <= input_imag[stage_counter[0]];
        end
    end
    
    // ========================================================================
    // Butterfly processing stages with improved algorithm
    // ========================================================================

    genvar stage;
    generate
        for (stage = 1; stage < STAGES; stage = stage + 1) begin : fft_stages

            // Registers for butterfly computation (synthesizable)
            reg [7:0] butterfly_size_reg;
            reg [11:0] butterfly_index_reg;
            reg [11:0] group_index_reg;
            reg [11:0] upper_index_reg;
            reg [DATA_WIDTH-1:0] a_r_reg, a_i_reg, b_r_reg, b_i_reg;
            reg [10:0] twiddle_index_reg;

            // Pipeline registers for arithmetic
            reg [DATA_WIDTH*2-1:0] mult_result_real_reg;
            reg [DATA_WIDTH*2-1:0] mult_result_imag_reg;
            reg [DATA_WIDTH-1:0] output_real_top_reg, output_imag_top_reg;
            reg [DATA_WIDTH-1:0] output_real_bottom_reg, output_imag_bottom_reg;

            // Stage control
            always @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    stage_valid[stage] <= 1'b0;
                    stage_counter[stage] <= 12'd0;
                    butterfly_size_reg <= 8'd0;
                    butterfly_index_reg <= 12'd0;
                    group_index_reg <= 12'd0;
                    upper_index_reg <= 12'd0;
                    twiddle_index_reg <= 11'd0;
                end else begin
                    if (stage_counter[stage-1] == FFT_SIZE - 1 && stage_valid[stage-1]) begin
                        stage_valid[stage] <= 1'b1;
                        stage_counter[stage] <= 12'd0;
                    end else if (stage_valid[stage]) begin
                        stage_counter[stage] <= stage_counter[stage] + 1;
                    end

                    // Pre-compute butterfly parameters
                    butterfly_size_reg <= 1 << stage;
                    butterfly_index_reg <= stage_counter[stage] % (1 << stage);
                    group_index_reg <= (stage_counter[stage] / (1 << stage)) * ((1 << stage) * 2);
                    upper_index_reg <= group_index_reg + butterfly_index_reg + (butterfly_size_reg >> 1);
                    // Correct twiddle indexing for Radix-2 FFT
                    twiddle_index_reg <= butterfly_index_reg * (FFT_SIZE >> (stage + 1));
                end
            end

            // Butterfly computation with proper twiddle indexing and rounding
            always @(posedge clk) begin
                if (stage_valid[stage] && butterfly_index_reg < (butterfly_size_reg >> 1)) begin
                    // Load input values (registered to avoid long combinational paths)
                    a_r_reg <= fft_buffer_real[(stage-1)%2][group_index_reg + butterfly_index_reg];
                    a_i_reg <= fft_buffer_imag[(stage-1)%2][group_index_reg + butterfly_index_reg];
                    b_r_reg <= fft_buffer_real[(stage-1)%2][upper_index_reg];
                    b_i_reg <= fft_buffer_imag[(stage-1)%2][upper_index_reg];
                end
            end

            // Arithmetic pipeline stage 1: Multiplications and initial additions
            always @(posedge clk) begin
                if (stage_valid[stage] && butterfly_index_reg < (butterfly_size_reg >> 1)) begin
                    // Load twiddle factors
                    reg [TWIDDLE_WIDTH-1:0] twiddle_r_val, twiddle_i_val;
                    twiddle_r_val = twiddle_real[twiddle_index_reg % (FFT_SIZE/2)];
                    twiddle_i_val = twiddle_imag[twiddle_index_reg % (FFT_SIZE/2)];

                    // Perform complex multiplication: b * W
                    // Truncate to prevent overflow, then scale back
                    reg [DATA_WIDTH-1:0] b_r_scaled, b_i_scaled;
                    b_r_scaled = b_r_reg >>> 4; // Scale down to prevent intermediate overflow
                    b_i_scaled = b_i_reg >>> 4;

                    // Real part: b_r * w_r - b_i * w_i
                    mult_result_real_reg <= $signed(b_r_scaled) * $signed(twiddle_r_val) -
                                           $signed(b_i_scaled) * $signed(twiddle_i_val);

                    // Imaginary part: b_r * w_i + b_i * w_r
                    mult_result_imag_reg <= $signed(b_r_scaled) * $signed(twiddle_i_val) +
                                           $signed(b_i_scaled) * $signed(twiddle_r_val);
                end
            end

            // Arithmetic pipeline stage 2: Rounding, saturation and final computation
            always @(posedge clk) begin
                if (stage_valid[stage] && butterfly_index_reg < (butterfly_size_reg >> 1)) begin
                    // Round back to original scale and round to nearest
                    reg [DATA_WIDTH-1:0] bw_real_rounded, bw_imag_rounded;

                    // Round: add 0.5 and truncate
                    bw_real_rounded = (mult_result_real_reg + (1 << (TWIDDLE_WIDTH + 3))) >>> (TWIDDLE_WIDTH + 4);
                    bw_imag_rounded = (mult_result_imag_reg + (1 << (TWIDDLE_WIDTH + 3))) >>> (TWIDDLE_WIDTH + 4);

                    // Compute butterfly outputs with saturation
                    // Top output: a + bw_rounded
                    output_real_top_reg <= saturated_add(a_r_reg, bw_real_rounded);
                    output_imag_top_reg <= saturated_add(a_i_reg, bw_imag_rounded);

                    // Bottom output: a - bw_rounded
                    output_real_bottom_reg <= saturated_sub(a_r_reg, bw_real_rounded);
                    output_imag_bottom_reg <= saturated_sub(a_i_reg, bw_imag_rounded);

                    // Check for overflow in multiplications
                    if (mult_result_real_reg[DATA_WIDTH*2-1:DATA_WIDTH*2-8] != {8{mult_result_real_reg[DATA_WIDTH*2-9]}} ||
                        mult_result_imag_reg[DATA_WIDTH*2-1:DATA_WIDTH*2-8] != {8{mult_result_imag_reg[DATA_WIDTH*2-9]}}) begin
                        overflow_detected <= 1'b1;
                    end
                end
            end

            // Store results back to buffer
            always @(posedge clk) begin
                if (stage_valid[stage] && butterfly_index_reg < (butterfly_size_reg >> 1)) begin
                    fft_buffer_real[stage%2][group_index_reg + butterfly_index_reg] <= output_real_top_reg;
                    fft_buffer_imag[stage%2][group_index_reg + butterfly_index_reg] <= output_imag_top_reg;
                    fft_buffer_real[stage%2][upper_index_reg] <= output_real_bottom_reg;
                    fft_buffer_imag[stage%2][upper_index_reg] <= output_imag_bottom_reg;
                end
            end
        end
    endgenerate
    
    // ========================================================================
    // Output stage
    // ========================================================================

    reg [11:0] output_counter;
    reg       output_valid_reg;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            output_counter <= 12'd0;
            output_valid_reg <= 1'b0;
        end else begin
            if (stage_counter[STAGES-1] == FFT_SIZE - 1 && stage_valid[STAGES-1]) begin
                output_valid_reg <= 1'b1;
                output_counter <= 12'd0;
            end else if (output_valid_reg) begin
                output_counter <= output_counter + 1;
                if (output_counter == FFT_SIZE - 1) begin
                    output_valid_reg <= 1'b0;
                end
            end
        end
    end
    
    // ========================================================================
    // Processing activity detection
    // ========================================================================

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            processing_active_reg <= 1'b0;
        end else begin
            // Active when input is valid or any stage is processing
            processing_active_reg <= data_valid | (|stage_valid);
        end
    end

    // ========================================================================
    // Output assignment and monitoring
    // ========================================================================

    // Bit-reversed output (natural order for FFT output)
    function [11:0] natural_order(input [11:0] addr);
        integer i;
        begin
            for (i = 0; i < 12; i = i + 1) begin
                natural_order[11-i] = addr[i];
            end
        end
    endfunction

    wire [11:0] natural_index;
    assign natural_index = natural_order(output_counter);

    assign real_out = output_valid_reg ? fft_buffer_real[(STAGES-1)%2][output_counter] : 24'd0;
    assign imag_out = output_valid_reg ? fft_buffer_imag[(STAGES-1)%2][output_counter] : 24'd0;
    assign fft_valid = output_valid_reg;
    assign fft_index = natural_index;  // Output in natural order

    // Status outputs
    assign overflow_flag = overflow_detected;
    assign processing_active = processing_active_reg;

endmodule
