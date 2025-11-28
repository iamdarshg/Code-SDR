// ============================================================================
// NCO Generator Module
// ============================================================================
// Numerically Controlled Oscillator for digital downconversion
// ============================================================================

`timescale 1ns/1ps

module nco_generator #(
    parameter WIDTH = 32,
    parameter ROM_DEPTH = 256
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [31:0] frequency_word,  // NCO frequency word
    input  wire        enable,          // NCO enable
    output wire [15:0] sine_out,        // Sine output (16-bit)
    output wire [15:0] cosine_out,      // Cosine output (16-bit)
    output wire        valid_out        // Output valid flag
);

    // ========================================================================
    // ROM table for sine/cosine lookup
    // ========================================================================

    // Initialize ROM with sine and cosine values
    reg [15:0] sin_rom [0:ROM_DEPTH-1];
    reg [15:0] cos_rom [0:ROM_DEPTH-1];
    integer rom_init_i;

    // ROM initialization block
    initial begin
        // Quarter sine wave stored in ROM (0 to 90 degrees)
        // Then mirrored for full 360-degree coverage
        for (rom_init_i = 0; rom_init_i < ROM_DEPTH; rom_init_i = rom_init_i + 1) begin
            // Calculate angle in radians (0 to π/2) and scale sine/cosine
            sin_rom[rom_init_i] = $rtoi($sin((rom_init_i * 3.14159265358979323846) / (2.0 * ROM_DEPTH)) * 32767.0);
            cos_rom[rom_init_i] = $rtoi($cos((rom_init_i * 3.14159265358979323846) / (2.0 * ROM_DEPTH)) * 32767.0);
        end
    end
    
    // ========================================================================
    // Phase accumulator
    // ========================================================================
    
    reg [31:0] phase_accumulator;
    reg [31:0] phase_accumulator_next;
    
    // Phase increment based on frequency word
    wire [31:0] phase_increment;
    assign phase_increment = enable ? frequency_word : 32'd0;
    
    // Phase accumulator register
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            phase_accumulator <= 32'd0;
        end else begin
            phase_accumulator <= phase_accumulator_next;
        end
    end
    
    // Next phase calculation
    assign phase_accumulator_next = phase_accumulator + phase_increment;
    
    // ========================================================================
    // Phase to ROM address mapping
    // ========================================================================
    
    // Extract most significant bits for ROM addressing
    wire [7:0] phase_upper;
    assign phase_upper = phase_accumulator[31:24];
    
    // Determine quadrant for sine/cosine calculation
    wire [1:0] quadrant;
    assign quadrant = phase_accumulator[31:30];
    
    // ROM address (quarter sine wave table)
    wire [7:0] rom_address;
    assign rom_address = phase_accumulator[23:16];
    
    // ========================================================================
    // Sine and cosine calculation
    // ========================================================================
    
    reg [15:0] sin_raw;
    reg [15:0] cos_raw;
    
    // ROM lookup with quadrant correction
    always @(*) begin
        case (quadrant)
            2'b00: begin // 0 to 90 degrees
                sin_raw = sin_rom[rom_address];
                cos_raw = cos_rom[rom_address];
            end
            2'b01: begin // 90 to 180 degrees
                sin_raw = cos_rom[ROM_DEPTH - 1 - rom_address];
                cos_raw = -sin_rom[ROM_DEPTH - 1 - rom_address];
            end
            2'b10: begin // 180 to 270 degrees
                sin_raw = -sin_rom[rom_address];
                cos_raw = -cos_rom[rom_address];
            end
            2'b11: begin // 270 to 360 degrees
                sin_raw = -cos_rom[ROM_DEPTH - 1 - rom_address];
                cos_raw = sin_rom[ROM_DEPTH - 1 - rom_address];
            end
            default: begin
                sin_raw = 16'd0;
                cos_raw = 16'd0;
            end
        endcase
    end
    
    // ========================================================================
    // Output register and scaling
    // ========================================================================
    
    reg [15:0] sine_reg;
    reg [15:0] cosine_reg;
    reg        valid_reg;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sine_reg <= 16'd0;
            cosine_reg <= 16'd0;
            valid_reg <= 1'b0;
        end else begin
            if (enable) begin
                sine_reg <= sin_raw;
                cosine_reg <= cos_raw;
                valid_reg <= 1'b1;
            end else begin
                valid_reg <= 1'b0;
            end
        end
    end
    
    // Output assignment
    assign sine_out = sine_reg;
    assign cosine_out = cosine_reg;
    assign valid_out = valid_reg;
    
    // ========================================================================
    // Spurious response suppression (optional)
    // ========================================================================
    
    // Phase dithering to reduce spurious responses
    reg [31:0] dither_register;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            dither_register <= 32'h55555555; // Alternating pattern
        end else begin
            dither_register <= {dither_register[30:0], dither_register[31]}; // Rotate
        end
    end
    
    // Apply dithering to frequency word (optional feature)
    wire [31:0] dithered_frequency;
    assign dithered_frequency = frequency_word ^ dither_register[7:0];
    
    // Use dithered frequency word
    wire [31:0] effective_frequency;
    assign effective_frequency = dithered_frequency;

endmodule
