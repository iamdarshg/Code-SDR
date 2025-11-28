// ============================================================================
// Adaptive Gain and Fixed-Point Scaler Module
// ============================================================================
// Applies digital gain and adaptive fixed-point scaling (bit-shifting)
// to digital samples based on RP2040 control.
// ============================================================================

`timescale 1ns/1ps

module adaptive_gain_scaler #(
    parameter DATA_WIDTH = 32
) (
    input  wire                  clk,             // Clock for the module
    input  wire                  rst_n,           // Reset (active low)
    input  wire [DATA_WIDTH-1:0] sample_in,       // Input digital sample
    input  wire                  sample_valid_in, // Input sample valid flag
    input  wire [7:0]            gain_control,    // 8-bit gain and shift control from RP2040
    output wire [DATA_WIDTH-1:0] sample_out,      // Output processed sample
    output wire                  sample_valid_out // Output sample valid flag
);

    // Decode gain_control:
    // Bits [7:4] for digital gain multiplier (e.g., 0-15)
    // Bits [3:0] for bit shift amount (e.g., 0-7 for right shift, 8-15 for left shift, 8 = no shift)
    wire [3:0] gain_mult_code;
    wire [3:0] shift_amount_code;

    assign gain_mult_code = gain_control[7:4];
    assign shift_amount_code = gain_control[3:0];

    // Digital Gain: A simple multiplier based on gain_mult_code
    // For simplicity, let's assume linear gain: 0 -> x1, 1 -> x2, 2 -> x4, etc.
    // Or, more realistically, use a lookup table or a direct multiplier value.
    // For this example, let's assume gain_mult_code directly represents a multiplier factor.
    // E.g., gain_mult_code = 0 => mult=1, 1 => mult=2, ..., 15 => mult=16.
    // Or, 2^gain_mult_code if gain is exponential.
    // Let's use a simple linear mapping for demonstration, max gain_mult_code of 15 means 15x, 0 means 1x.
    // This could be refined later.
    wire [DATA_WIDTH-1:0] digital_gain_factor;
    assign digital_gain_factor = gain_mult_code + 1; // Assuming gain_mult_code 0-15 maps to factor 1-16

    wire [2*DATA_WIDTH-1:0] gained_sample;
    assign gained_sample = $signed(sample_in) * $signed(digital_gain_factor);

    // Adaptive Fixed-Point Scaling (Bit Shifting)
    wire [DATA_WIDTH-1:0] shifted_sample;

    // Decode shift_amount_code and apply shift in one step:
    // If shift_amount_code < 8, it's a right shift (e.g., 0=0, 1=1, ..., 7=7)
    // If shift_amount_code == 8, it's no shift
    // If shift_amount_code > 8, it's a left shift (e.g., 9=1, 10=2, ..., 15=7)
    // Shift the gained sample (which is 2*DATA_WIDTH wide) to fit output
    assign shifted_sample = (shift_amount_code == 4'd8) ? gained_sample[DATA_WIDTH-1:0] :  // No shift
                           (shift_amount_code < 4'd8) ?   // Right shift
                             gained_sample >>> shift_amount_code :
                           (shift_amount_code > 4'd8) ?   // Left shift
                             gained_sample << (shift_amount_code - 4'd8) :
                             gained_sample[DATA_WIDTH-1:0]; // Default (shouldn't happen)

    assign sample_out = shifted_sample;

    // Register the valid signal to persist
    reg sample_valid_out_reg;
    assign sample_valid_out = sample_valid_out_reg;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sample_valid_out_reg <= 0;
        end else if (sample_valid_in) begin
            sample_valid_out_reg <= 1;
        end
        // Keep it high once set, until reset
    end

endmodule
