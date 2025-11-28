// ============================================================================
// ADC Interface Module
// ============================================================================
// Interfaces with AD9215BCPZ-105 ADC and performs data conditioning
// ============================================================================

`timescale 1ns/1ps

module adc_interface (
    input  wire        clk_adc,         // 105 MHz ADC clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [9:0]  adc_data,        // 10-bit parallel ADC data
    input  wire        adc_valid,       // ADC data valid flag
    input  wire        adc_ovr,         // ADC overflow flag
    output wire [31:0] adc_samples,     // Processed ADC samples
    output wire        sample_valid,    // Sample valid flag
    output wire        overflow_detect  // Overflow detected flag
);

    // ========================================================================
    // ADC data registration and conditioning
    // ========================================================================
    
    reg [9:0] adc_data_reg;
    reg       adc_valid_reg;
    reg       adc_ovr_reg;
    
    always @(posedge clk_adc or negedge rst_n) begin
        if (!rst_n) begin
            adc_data_reg <= 10'd0;
            adc_valid_reg <= 1'b0;
            adc_ovr_reg <= 1'b0;
        end else begin
            adc_data_reg <= adc_data;
            adc_valid_reg <= adc_valid;
            adc_ovr_reg <= adc_ovr;
        end
    end
    
    // ========================================================================
    // Overflow detection and error flagging
    // ========================================================================
    
    reg [3:0] overflow_count;
    reg       overflow_detect_reg;
    
    always @(posedge clk_adc or negedge rst_n) begin
        if (!rst_n) begin
            overflow_count <= 4'd0;
            overflow_detect_reg <= 1'b0;
        end else begin
            // Count consecutive overflows
            if (adc_ovr_reg) begin
                overflow_count <= (overflow_count < 4'd15) ? overflow_count + 1 : overflow_count;
                overflow_detect_reg <= ((overflow_count < 4'd15) ? overflow_count + 1 : overflow_count) >= 4'd3; // Trigger after 3 consecutive overflows
            end else begin
                overflow_count <= 4'd0;
                overflow_detect_reg <= overflow_detect_reg; // Keep flag set once triggered
            end
        end
    end
    
    // Register overflow flag for timing alignment
    reg adc_ovr_delay;
    always @(posedge clk_adc) begin
        adc_ovr_delay <= adc_ovr_reg;
    end
    
    assign overflow_detect = overflow_detect_reg;
    
    // ========================================================================
    // DC offset correction (optional)
    // ========================================================================
    
    // DC offset estimation using running average
    reg [31:0] dc_offset;
    reg [15:0] avg_count;
    
    always @(posedge clk_adc or negedge rst_n) begin
        if (!rst_n) begin
            dc_offset <= 32'd0;
            avg_count <= 16'd0;
        end else if (adc_valid_reg) begin
            // Simple DC offset estimation over 65536 samples
            if (avg_count < 16'd65535) begin
                avg_count <= avg_count + 1;
                dc_offset <= dc_offset + {{22{adc_data_reg[9]}}, adc_data_reg};
            end
        end
    end
    
    // Apply DC offset correction when stable
    wire [31:0] dc_corrected;
    assign dc_corrected = {{22{adc_data_reg[9]}}, adc_data_reg} - dc_offset;
    
    // ========================================================================
    // Sample rate conversion and synchronization
    // ========================================================================
    
    // Synchronize ADC data to processing domain
    reg [31:0] sample_sync;
    reg        sample_valid_sync;
    
    always @(posedge clk_adc or negedge rst_n) begin
        if (!rst_n) begin
            sample_sync <= 32'd0;
            sample_valid_sync <= 1'b0;
        end else begin
            if (adc_valid_reg) begin
                // Extend 10-bit ADC data to 32-bit signed
                sample_sync <= {{22{adc_data_reg[9]}}, adc_data_reg};
                sample_valid_sync <= 1'b1;
            end else begin
                sample_valid_sync <= 1'b0;
            end
        end
    end
    
    // ========================================================================
    // Output assignment
    // ========================================================================
    
    assign adc_samples = dc_corrected;
    assign sample_valid = sample_valid_sync;
    
endmodule
