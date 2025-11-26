// ============================================================================
// CIC Decimation Filter Module
// ============================================================================
// Cascaded Integrator-Comb filter for decimation and anti-imaging filtering
// ============================================================================

`timescale 1ns/1ps

module cic_decimator #(
    parameter INPUT_WIDTH = 32,
    parameter OUTPUT_WIDTH = 32,
    parameter STAGES = 3,
    parameter DECIMATION = 8
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [INPUT_WIDTH-1:0] data_in,    // Input data
    input  wire        data_valid,      // Input data valid
    output wire [OUTPUT_WIDTH-1:0] data_out,  // Decimated output
    output wire        output_valid     // Output data valid
);

    // ========================================================================
    // Parameter calculations
    // ========================================================================

    // Bit growth calculation: each integrator stage adds M*log2(R) bits
    // For M=1 (comb delay), growth = STAGES * log2(DECIMATION)
    localparam bit_growth = STAGES * $clog2(DECIMATION);
    localparam STAGE_WIDTH = INPUT_WIDTH + bit_growth;
    localparam COMB_DELAY = 1;  // Comb filter delay
    
    // ========================================================================
    // Integrator stages
    // ========================================================================
    
    genvar i;
    reg [STAGE_WIDTH-1:0] integrator [0:STAGES-1];
    reg [STAGE_WIDTH-1:0] integrator_next [0:STAGES-1];
    
    // Integrator stage registers
    generate
        for (i = 0; i < STAGES; i = i + 1) begin : integrator_stages
            always @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    integrator[i] <= 'd0;
                end else begin
                    integrator[i] <= integrator_next[i];
                end
            end
        end
    endgenerate
    
    // Integrator calculation (cumulative sum)
    always @(*) begin
        integer i;
        if (data_valid) begin
            // First integrator stage: integrate input data
            integrator_next[0] = integrator[0] + data_in;
            // Subsequent integrator stages: integrate previous output
            for (i = 1; i < STAGES; i = i + 1) begin
                integrator_next[i] = integrator[i] + integrator[i-1];
            end
        end else begin
            // Hold values when no data valid
            for (i = 0; i < STAGES; i = i + 1) begin
                integrator_next[i] = integrator[i];
            end
        end
    end
    
    // ========================================================================
    // Decimation logic
    // ========================================================================
    
    reg decimate_enable;
    reg [$clog2(DECIMATION):0] decimate_counter;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            decimate_counter <= 'd0;
            decimate_enable <= 1'b0;
        end else begin
            if (data_valid) begin
                if (decimate_counter == DECIMATION - 1) begin
                    decimate_counter <= 'd0;
                    decimate_enable <= 1'b1;
                end else begin
                    decimate_counter <= decimate_counter + 1;
                    decimate_enable <= 1'b0;
                end
            end else begin
                decimate_enable <= 1'b0;
            end
        end
    end
    
    // ========================================================================
    // Comb filter stages (after decimation)
    // ========================================================================
    
    reg [STAGE_WIDTH-1:0] comb [0:STAGES-1];
    reg [STAGE_WIDTH-1:0] comb_delay [0:STAGES-1][0:COMB_DELAY];
    
    generate
        for (i = 0; i < STAGES; i = i + 1) begin : comb_stages
            integer j;
            
            // Comb filter delay line
            always @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    for (j = 0; j <= COMB_DELAY; j = j + 1) begin
                        comb_delay[i][j] <= 'd0;
                    end
                end else if (decimate_enable) begin
                    // Shift delay line
                    for (j = COMB_DELAY; j > 0; j = j + 1) begin
                        comb_delay[i][j] <= comb_delay[i][j-1];
                    end
                    comb_delay[i][0] <= comb[i];
                end
            end
            
            // Comb filter output (difference)
            always @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    comb[i] <= 'd0;
                end else if (decimate_enable) begin
                    comb[i] <= comb_delay[i][0] - comb_delay[i][COMB_DELAY];
                end
            end
        end
    endgenerate
    
    // ========================================================================
    // Gain compensation and output scaling
    // ========================================================================

    // Gain of CIC filter: (R*M)^N where R=decimation, M=delay, N=stages
    // For our case: (8*1)^3 = 512
    localparam GAIN = DECIMATION ** STAGES;
    localparam GAIN_SHIFT = $clog2(GAIN);

    // Apply gain compensation by dividing output by gain (with rounding)
    wire [STAGE_WIDTH-1:0] scaled_output;
    assign scaled_output = (comb[STAGES-1] + (1 << (GAIN_SHIFT-1))) >> GAIN_SHIFT;
    
    // Output register with saturation
    reg [OUTPUT_WIDTH-1:0] output_register;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            output_register <= 'd0;
        end else if (decimate_enable) begin
            // Saturation logic to prevent overflow
            if (scaled_output[STAGE_WIDTH - 1]) begin
                // Negative saturation
                output_register <= {1'b1, {(OUTPUT_WIDTH-1){1'b0}}};
            end else if (scaled_output[STAGE_WIDTH - 1:OUTPUT_WIDTH] != 'd0) begin
                // Positive saturation
                output_register <= {1'b0, {(OUTPUT_WIDTH-1){1'b1}}};
            end else begin
                // Normal operation
                output_register <= scaled_output[OUTPUT_WIDTH-1:0];
            end
        end
    end
    
    // ========================================================================
    // Output assignment
    // ========================================================================
    
    assign data_out = output_register;
    assign output_valid = decimate_enable;

endmodule
