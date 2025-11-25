// ============================================================================
// Average Power Detector Module
// ============================================================================
// Calculates the average power of incoming digital samples
// ============================================================================

`timescale 1ns/1ps

module average_power_detector #(
    parameter DATA_WIDTH = 32,
    parameter AVG_WINDOW_BITS = 10 // Averaging window size = 2^AVG_WINDOW_BITS
) (
    input  wire                  clk,             // Clock for the module
    input  wire                  rst_n,           // Reset (active low)
    input  wire [DATA_WIDTH-1:0] sample_in,       // Input digital sample
    input  wire                  sample_valid_in, // Input sample valid flag
    output wire [DATA_WIDTH-1:0] avg_power_out,   // Output average power
    output wire                  avg_power_valid  // Output average power valid flag
);

    // Squared sample (power is proportional to magnitude squared)
    wire [2*DATA_WIDTH-1:0] sample_squared;
    assign sample_squared = $signed(sample_in) * $signed(sample_in);

    // Accumulator for sum of squared samples
    reg [2*DATA_WIDTH-1 + AVG_WINDOW_BITS:0] sum_of_squares;
    reg [AVG_WINDOW_BITS-1:0] sample_count;
    reg                       output_valid_reg;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sum_of_squares <= 'd0;
            sample_count <= 'd0;
            output_valid_reg <= 1'b0;
        end else begin
            output_valid_reg <= 1'b0; // Default to invalid
            if (sample_valid_in) begin
                if (sample_count == (2**AVG_WINDOW_BITS - 1)) begin
                    // Averaging window full, calculate average and reset
                    sum_of_squares <= sample_squared; // Start new sum
                    sample_count <= 'd0;
                    output_valid_reg <= 1'b1; // New average power is ready
                end else begin
                    sum_of_squares <= sum_of_squares + sample_squared;
                    sample_count <= sample_count + 1;
                end
            end
        end
    end

    // Average power calculation (division by 2^AVG_WINDOW_BITS is a right shift)
    assign avg_power_out = sum_of_squares >>> AVG_WINDOW_BITS;
    assign avg_power_valid = output_valid_reg;

endmodule
