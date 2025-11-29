// ============================================================================
// Compensation Filter Module - Anti-imaging FIR Filter
// ============================================================================
// Finite Impulse Response (FIR) filter to compensate for CIC decimation
// artifacts and provide clean spectrum response
// ============================================================================

`timescale 1ns/1ps

module compensation_filter #(
    parameter WIDTH = 32,
    parameter COEFF_WIDTH = 16,
    parameter TAPS = 8,
    parameter DECIMATION = 8
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [WIDTH-1:0] data_in,    // Input data from CIC decimator
    input  wire        data_valid,      // Input data valid
    output wire [WIDTH-1:0] data_out,   // Filtered output data
    output wire        output_valid     // Output data valid
);

    // ========================================================================
    // FIR Filter Coefficients for CIC Compensation
    // ========================================================================
    // These coefficients compensate for the droop in CIC filter passband
    // Designed for specified decimation rate

    reg [COEFF_WIDTH-1:0] coeff [0:TAPS-1];

    // Initialize coefficients based on decimation rate
    initial begin
        integer i;
        for (i = 0; i < TAPS; i = i + 1) begin
            // Simple raised cosine compensation - placeholder
            // Real coefficients should be calculated for specific CIC parameters
            case (DECIMATION)
                8: begin
                    // For DECIMATION = 8
                    coeff[0] = 16'h1000; coeff[1] = 16'h0F00; coeff[2] = 16'h0E00;
                    coeff[3] = 16'h0D00; coeff[4] = 16'h0D00; coeff[5] = 16'h0E00;
                    coeff[6] = 16'h0F00; coeff[7] = 16'h1000;
                end
                16: begin
                    // For DECIMATION = 16
                    coeff[0] = 16'h1200; coeff[1] = 16'h1000; coeff[2] = 16'h0E00;
                    coeff[3] = 16'h0C00; coeff[4] = 16'h0C00; coeff[5] = 16'h0E00;
                    coeff[6] = 16'h1000; coeff[7] = 16'h1200;
                end
                default: begin
                    // Default coefficients
                    coeff[0] = 16'h1000; coeff[1] = 16'h0F00; coeff[2] = 16'h0E00;
                    coeff[3] = 16'h0D00; coeff[4] = 16'h0D00; coeff[5] = 16'h0E00;
                    coeff[6] = 16'h0F00; coeff[7] = 16'h1000;
                end
            endcase
        end
    end

    // ========================================================================
    // Delay Line for FIR Tap Storage
    // ========================================================================

    reg [WIDTH-1:0] delay_line [0:TAPS-1];
    reg [TAPS-1:0] data_valid_shift;

    always @(posedge clk or negedge rst_n) begin
        integer i; // Declare i at the beginning of the always block
        if (!rst_n) begin
            for (i = 0; i < TAPS; i = i + 1) begin
                delay_line[i] <= {WIDTH{1'b0}};
            end
            data_valid_shift <= {TAPS{1'b0}};
        end else begin
            if (data_valid) begin
                // Shift delay line
                for (i = TAPS-1; i > 0; i = i - 1) begin
                    delay_line[i] <= delay_line[i-1];
                end
                delay_line[0] <= data_in;

                // Shift valid signal
                data_valid_shift <= {data_valid_shift[TAPS-2:0], data_valid};
            end
        end
    end

    // ========================================================================
    // FIR Filter Accumulation
    // ========================================================================

    reg [WIDTH+COEFF_WIDTH-1:0] accumulator;
    reg accumulator_valid;

    always @(posedge clk or negedge rst_n) begin
        integer i; // Declare i at the beginning of the always block
        if (!rst_n) begin
            accumulator <= {WIDTH+COEFF_WIDTH{1'b0}};
            accumulator_valid <= 1'b0;
        end else begin
            if (data_valid_shift[TAPS-1]) begin
                // Compute FIR sum
                accumulator = 0;
                for (i = 0; i < TAPS; i = i + 1) begin
                    // Sign-extend coefficients and multiply
                    accumulator = accumulator + ($signed(delay_line[i]) *
                              $signed({{(WIDTH-COEFF_WIDTH){coeff[i][COEFF_WIDTH-1]}}, coeff[i]}));
                end
                accumulator_valid <= 1'b1;
            end else begin
                accumulator_valid <= 1'b0;
            end
        end
    end

    // ========================================================================
    // Output Scaling and Saturation
    // ========================================================================

    wire [WIDTH+COEFF_WIDTH:0] scaled_output;
    assign scaled_output = accumulator >>> (COEFF_WIDTH - 1);  // Scale back down

    reg [WIDTH-1:0] output_register;
    reg output_valid_reg;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            output_register <= {WIDTH{1'b0}};
            output_valid_reg <= 1'b0;
        end else begin
            if (accumulator_valid) begin
                // Saturation logic to prevent overflow
                if (scaled_output[WIDTH+COEFF_WIDTH]) begin
                    // Negative saturation
                    output_register <= {1'b1, {(WIDTH-1){1'b0}}};
                end else if (scaled_output[WIDTH+COEFF_WIDTH-1:WIDTH] != 'd0) begin
                    // Positive saturation
                    output_register <= {1'b0, {(WIDTH-1){1'b1}}};
                end else begin
                    // Normal operation
                    output_register <= scaled_output[WIDTH-1:0];
                end
                output_valid_reg <= 1'b1;
            end else begin
                output_valid_reg <= 1'b0;
            end
        end
    end

    // ========================================================================
    // Output Assignment
    // ========================================================================

    assign data_out = output_register;
    assign output_valid = output_valid_reg;

endmodule
