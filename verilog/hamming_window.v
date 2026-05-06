`default_nettype none
// ============================================================================
// Hamming Window Module - Synthesizable ROM Implementation
// ============================================================================
// Optimized for sample-accurate streaming and synthesis efficiency.
// ============================================================================

`timescale 1ns/1ps

module hamming_window #(
    parameter DATA_WIDTH = 24,
    parameter FFT_SIZE = 1024,
    parameter COEFF_WIDTH = 16
) (
    input  wire        clk,
    input  wire        rst_n,
    input  wire [DATA_WIDTH-1:0] data_in,
    input  wire        data_valid,
    output wire [DATA_WIDTH-1:0] data_out,
    output wire        output_valid
);

    localparam ADDR_WIDTH = $clog2(FFT_SIZE);

    // Window Coefficient ROM (Loaded from .mem)
    reg [COEFF_WIDTH-1:0] window_rom [0:FFT_SIZE-1];
    initial begin
        $readmemh("window_coeff.mem", window_rom);
    end

    // Address Counter - Resets on frame boundaries
    reg [ADDR_WIDTH-1:0] addr;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) addr <= 0;
        else if (data_valid) addr <= addr + 1;
        else addr <= 0; // Reset for next frame burst
    end

    // Pipeline Registers
    reg [COEFF_WIDTH-1:0] coeff_reg;
    reg [DATA_WIDTH-1:0] data_reg;
    reg v1, v2;

    // Fixed-Point Multiplier
    // Q15 Coefficient * DATA_WIDTH Data
    wire [DATA_WIDTH+COEFF_WIDTH-1:0] product = $signed(data_reg) * $signed({1'b0, coeff_reg});

    reg [DATA_WIDTH-1:0] result;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            coeff_reg <= 0; data_reg <= 0; v1 <= 0; v2 <= 0; result <= 0;
        end else begin
            v1 <= data_valid;
            if (data_valid) begin
                coeff_reg <= window_rom[addr];
                data_reg <= data_in;
            end

            v2 <= v1;
            // Shift by 15 (normalization for Q15 coefficients)
            result <= product[DATA_WIDTH+COEFF_WIDTH-2:COEFF_WIDTH-1];
        end
    end

    assign data_out = result;
    assign output_valid = v2;

endmodule
