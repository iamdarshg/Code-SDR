// ============================================================================
// Hamming Window Module
// ============================================================================
// Hamming window implementation for spectral analysis
// ============================================================================

`timescale 1ns/1ps

module hamming_window #(
    parameter WIDTH = 32,
    parameter FFT_SIZE = 1024
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [WIDTH-1:0] data_in,     // Input data
    input  wire        data_valid,      // Input data valid
    output wire [WIDTH-1:0] data_out,    // Windowed output
    output wire        output_valid     // Output valid flag
);

    // ========================================================================
    // Window coefficients calculation
    // ========================================================================
    
    localparam WINDOW_WIDTH = 16;  // Window coefficient precision
    
    reg [WINDOW_WIDTH-1:0] window_coeff [0:FFT_SIZE-1];
    
    // Initialize Hamming window coefficients
    initial begin
        integer i;
        real coefficient;
        for (i = 0; i < FFT_SIZE; i = i + 1) begin
            // Hamming window: w(n) = 0.54 - 0.46*cos(2*pi*n/(N-1))
            coefficient = 0.54 - 0.46 * $cos(2.0 * 3.141592653589793 * i / (FFT_SIZE - 1));
            // Scale to fixed-point representation
            window_coeff[i] = $rtoi(coefficient * (2.0 ** (WINDOW_WIDTH - 1) - 1));
        end
    end
    
    // ========================================================================
    // Windowing buffer and address generation
    // ========================================================================
    
    reg [9:0] sample_counter;  // 10-bit counter for 1024 samples
    reg       buffer_full;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sample_counter <= 10'd0;
            buffer_full <= 1'b0;
        end else begin
            if (data_valid) begin
                if (sample_counter == FFT_SIZE - 1) begin
                    sample_counter <= 10'd0;
                    buffer_full <= 1'b1;  // Signal that we have a full window
                end else begin
                    sample_counter <= sample_counter + 1;
                    buffer_full <= 1'b0;
                end
            end
        end
    end
    
    // ========================================================================
    // Circular buffer for windowed samples
    // ========================================================================
    
    reg [WIDTH-1:0] window_buffer [0:FFT_SIZE-1];
    reg [WIDTH-1:0] window_buffer_next [0:FFT_SIZE-1];
    
    // Write to circular buffer
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            // Initialize buffer to zero
            integer i;
            for (i = 0; i < FFT_SIZE; i = i + 1) begin
                window_buffer[i] <= 'd0;
            end
        end else if (data_valid) begin
            window_buffer[sample_counter] <= data_in;
        end
    end
    
    // ========================================================================
    // Windowing operation
    // ========================================================================
    
    // Get current window coefficient
    wire [WINDOW_WIDTH-1:0] current_coeff;
    assign current_coeff = window_coeff[sample_counter];
    
    // Multiply input data by window coefficient
    wire [63:0] window_mult;
    assign window_mult = $signed(data_in) * $signed({{(WIDTH-WINDOW_WIDTH){current_coeff[WINDOW_WIDTH-1]}}, current_coeff});
    
    // Register multiplication result
    reg [WIDTH-1:0] windowed_data;
    
    always @(posedge clk) begin
        // Scale down by 2^(WINDOW_WIDTH-1) to normalize
        windowed_data <= window_mult[WIDTH + WINDOW_WIDTH - 2 : WINDOW_WIDTH - 1];
    end
    
    // ========================================================================
    // Output control
    // ========================================================================
    
    reg output_valid_reg;
    reg [2:0] output_delay;  // Pipeline delay compensation
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            output_valid_reg <= 1'b0;
            output_delay <= 3'd0;
        end else begin
            // Pipeline delay for windowing operation
            output_delay <= {output_delay[1:0], buffer_full};
            output_valid_reg <= output_delay[2];
        end
    end
    
    // ========================================================================
    // Output assignment
    // ========================================================================
    
    assign data_out = windowed_data;
    assign output_valid = output_valid_reg;
    
endmodule
