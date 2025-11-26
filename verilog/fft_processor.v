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

    // Stage control signals (moved to module level)
    reg [STAGES-1:0] stage_valid;
    reg [11:0] stage_counter [0:STAGES-1];

    // Butterfly computation registers (moved to module level arrays)
    reg [7:0] butterfly_size_reg [0:STAGES-1];
    reg [11:0] butterfly_index_reg [0:STAGES-1];
    reg [11:0] group_index_reg [0:STAGES-1];
    reg [11:0] upper_index_reg [0:STAGES-1];
    reg [10:0] twiddle_index_reg [0:STAGES-1];

    // Butterfly input/output registers (per stage)
    reg [DATA_WIDTH-1:0] a_r_reg [0:STAGES-1];
    reg [DATA_WIDTH-1:0] a_i_reg [0:STAGES-1];
    reg [DATA_WIDTH-1:0] b_r_reg [0:STAGES-1];
    reg [DATA_WIDTH-1:0] b_i_reg [0:STAGES-1];

    // Arithmetic pipeline registers (per stage)
    reg [DATA_WIDTH*2-1:0] mult_result_real_reg [0:STAGES-1];
    reg [DATA_WIDTH*2-1:0] mult_result_imag_reg [0:STAGES-1];
    reg [DATA_WIDTH-1:0] output_real_top_reg [0:STAGES-1];
    reg [DATA_WIDTH-1:0] output_imag_top_reg [0:STAGES-1];
    reg [DATA_WIDTH-1:0] output_real_bottom_reg [0:STAGES-1];
    reg [DATA_WIDTH-1:0] output_imag_bottom_reg [0:STAGES-1];

    // Combinatorial butterfly parameter calculations
    wire [7:0] butterfly_size [0:STAGES-1];
    wire [11:0] butterfly_index [0:STAGES-1];
    wire [11:0] group_index [0:STAGES-1];
    wire [11:0] upper_index [0:STAGES-1];
    wire [10:0] twiddle_index [0:STAGES-1];

    // Generate combinatorial calculations for each stage
    genvar calc_stage;
    generate
        for (calc_stage = 1; calc_stage < STAGES; calc_stage = calc_stage + 1) begin : butterfly_calc
            assign butterfly_size[calc_stage] = 1 << calc_stage;
            assign butterfly_index[calc_stage] = stage_counter[calc_stage] % (1 << calc_stage);
            assign group_index[calc_stage] = (stage_counter[calc_stage] / (1 << calc_stage)) * ((1 << calc_stage) * 2);
            assign upper_index[calc_stage] = group_index[calc_stage] + butterfly_index[calc_stage] + (butterfly_size[calc_stage] >> 1);
            assign twiddle_index[calc_stage] = butterfly_index[calc_stage] * (FFT_SIZE >> (calc_stage + 1));
        end
    endgenerate

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

    // Generate combinatorial assignments for butterfly parameters (synthesis-safe)
    assign butterfly_size_reg[0] = 8'd0; // Unused for stage 0
    assign butterfly_index_reg[0] = 12'd0;
    assign group_index_reg[0] = 12'd0;
    assign upper_index_reg[0] = 12'd0;
    assign twiddle_index_reg[0] = 11'd0;

    genvar comb_stage;
    generate
        for (comb_stage = 1; comb_stage < STAGES; comb_stage = comb_stage + 1) begin : comb_assign
            assign butterfly_size_reg[comb_stage] = 1 << comb_stage;
            assign butterfly_index_reg[comb_stage] = butterfly_index[comb_stage];
            assign group_index_reg[comb_stage] = group_index[comb_stage];
            assign upper_index_reg[comb_stage] = upper_index[comb_stage];
            assign twiddle_index_reg[comb_stage] = twiddle_index[comb_stage];
        end
    endgenerate

    // Stage control logic for all stages (now outside generate block)
    genvar ctrl_stage;
    generate
        for (ctrl_stage = 1; ctrl_stage < STAGES; ctrl_stage = ctrl_stage + 1) begin : stage_control
            always @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    stage_valid[ctrl_stage] <= 1'b0;
                    stage_counter[ctrl_stage] <= 12'd0;
                    // Initialize pipeline registers
                    a_r_reg[ctrl_stage] <= {DATA_WIDTH{1'b0}};
                    a_i_reg[ctrl_stage] <= {DATA_WIDTH{1'b0}};
                    b_r_reg[ctrl_stage] <= {DATA_WIDTH{1'b0}};
                    b_i_reg[ctrl_stage] <= {DATA_WIDTH{1'b0}};
                    mult_result_real_reg[ctrl_stage] <= {DATA_WIDTH*2{1'b0}};
                    mult_result_imag_reg[ctrl_stage] <= {DATA_WIDTH*2{1'b0}};
                    output_real_top_reg[ctrl_stage] <= {DATA_WIDTH{1'b0}};
                    output_imag_top_reg[ctrl_stage] <= {DATA_WIDTH{1'b0}};
                    output_real_bottom_reg[ctrl_stage] <= {DATA_WIDTH{1'b0}};
                    output_imag_bottom_reg[ctrl_stage] <= {DATA_WIDTH{1'b0}};
                end else begin
                    // Stage activation logic
                    if (stage_counter[ctrl_stage-1] == FFT_SIZE - 1 && stage_valid[ctrl_stage-1]) begin
                        stage_valid[ctrl_stage] <= 1'b1;
                        stage_counter[ctrl_stage] <= 12'd0;
                    end else if (stage_valid[ctrl_stage]) begin
                        stage_counter[ctrl_stage] <= stage_counter[ctrl_stage] + 1;

                        // Butterfly computation when enabled
                        if (butterfly_index_reg[ctrl_stage] < (butterfly_size_reg[ctrl_stage] >> 1)) begin
                            // Load input values (registered to avoid long combinational paths)
                            a_r_reg[ctrl_stage] <= fft_buffer_real[(ctrl_stage-1)%2][group_index_reg[ctrl_stage] + butterfly_index_reg[ctrl_stage]];
                            a_i_reg[ctrl_stage] <= fft_buffer_imag[(ctrl_stage-1)%2][group_index_reg[ctrl_stage] + butterfly_index_reg[ctrl_stage]];
                            b_r_reg[ctrl_stage] <= fft_buffer_real[(ctrl_stage-1)%2][upper_index_reg[ctrl_stage]];
                            b_i_reg[ctrl_stage] <= fft_buffer_imag[(ctrl_stage-1)%2][upper_index_reg[ctrl_stage]];

                            // Arithmetic pipeline stage 1: Multiplications
                            begin
                                reg [TWIDDLE_WIDTH-1:0] twiddle_r_val, twiddle_i_val;
                                reg [DATA_WIDTH-1:0] b_r_scaled, b_i_scaled;

                                twiddle_r_val = twiddle_real[twiddle_index_reg[ctrl_stage] % (FFT_SIZE/2)];
                                twiddle_i_val = twiddle_imag[twiddle_index_reg[ctrl_stage] % (FFT_SIZE/2)];

                                // Perform complex multiplication: b * W
                                b_r_scaled = b_r_reg[ctrl_stage] >>> 4;
                                b_i_scaled = b_i_reg[ctrl_stage] >>> 4;

                                mult_result_real_reg[ctrl_stage] <= $signed(b_r_scaled) * $signed(twiddle_r_val) -
                                                                   $signed(b_i_scaled) * $signed(twiddle_i_val);
                                mult_result_imag_reg[ctrl_stage] <= $signed(b_r_scaled) * $signed(twiddle_i_val) +
                                                                   $signed(b_i_scaled) * $signed(twiddle_r_val);
                            end

                            // Arithmetic pipeline stage 2: Rounding, saturation and final computation
                            begin
                                reg [DATA_WIDTH-1:0] bw_real_rounded, bw_imag_rounded;

                                // Round back to original scale and round to nearest
                                bw_real_rounded = (mult_result_real_reg[ctrl_stage] + (1 << (TWIDDLE_WIDTH + 3))) >>> (TWIDDLE_WIDTH + 4);
                                bw_imag_rounded = (mult_result_imag_reg[ctrl_stage] + (1 << (TWIDDLE_WIDTH + 3))) >>> (TWIDDLE_WIDTH + 4);

                                // Compute butterfly outputs with saturation
                                output_real_top_reg[ctrl_stage] <= saturated_add(a_r_reg[ctrl_stage], bw_real_rounded);
                                output_imag_top_reg[ctrl_stage] <= saturated_add(a_i_reg[ctrl_stage], bw_imag_rounded);
                                output_real_bottom_reg[ctrl_stage] <= saturated_sub(a_r_reg[ctrl_stage], bw_real_rounded);
                                output_imag_bottom_reg[ctrl_stage] <= saturated_sub(a_i_reg[ctrl_stage], bw_imag_rounded);

                                // Store results back to buffer
                                fft_buffer_real[ctrl_stage%2][group_index_reg[ctrl_stage] + butterfly_index_reg[ctrl_stage]] <= output_real_top_reg[ctrl_stage];
                                fft_buffer_imag[ctrl_stage%2][group_index_reg[ctrl_stage] + butterfly_index_reg[ctrl_stage]] <= output_imag_top_reg[ctrl_stage];
                                fft_buffer_real[ctrl_stage%2][upper_index_reg[ctrl_stage]] <= output_real_bottom_reg[ctrl_stage];
                                fft_buffer_imag[ctrl_stage%2][upper_index_reg[ctrl_stage]] <= output_imag_bottom_reg[ctrl_stage];
                            end

                            // Check for overflow in multiplications
                            if (mult_result_real_reg[ctrl_stage][DATA_WIDTH*2-1:DATA_WIDTH*2-8] != {8{mult_result_real_reg[ctrl_stage][DATA_WIDTH*2-9]}} ||
                                mult_result_imag_reg[ctrl_stage][DATA_WIDTH*2-1:DATA_WIDTH*2-8] != {8{mult_result_imag_reg[ctrl_stage][DATA_WIDTH*2-9]}}) begin
                                overflow_detected <= 1'b1;
                            end
                        end
                    end
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
