// ============================================================================
// Digital Downconverter (DDC) Module
// ============================================================================
// Implements complex mixing for frequency translation and filtering
// with compensation filtering and adaptive gain control
// ============================================================================

`timescale 1ns/1ps

module digital_downconverter #(
    parameter WIDTH = 32
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [31:0] adc_data,        // Input ADC data
    input  wire        data_valid,      // Input data valid
    input  wire [15:0] nco_sine,        // NCO sine component (16-bit)
    input  wire [15:0] nco_cosine,      // NCO cosine component (16-bit)
    input  wire [7:0]  gain_control,    // Gain control from RP2040
    output wire [31:0] i_component,     // I (in-phase) component
    output wire [31:0] q_component,     // Q (quadrature) component
    output wire        ddc_valid        // DDC output valid
);

    // ========================================================================
    // Data registration and scaling
    // ========================================================================
    
    reg [31:0] adc_data_reg;
    reg        data_valid_reg;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            adc_data_reg <= 32'd0;
            data_valid_reg <= 1'b0;
        end else begin
            adc_data_reg <= adc_data;
            data_valid_reg <= data_valid;
        end
    end
    
    // ========================================================================
    // Complex multiplication (ADC × NCO)
    // ========================================================================
    
    // Sign-extend NCO outputs to 32-bit for multiplication
    wire [31:0] nco_sine_ext;
    wire [31:0] nco_cosine_ext;
    
    assign nco_sine_ext = {{16{nco_sine[15]}}, nco_sine};
    assign nco_cosine_ext = {{16{nco_cosine[15]}}, nco_cosine};
    
    // Complex multiplication: (I + jQ) × (C + jS) = (IC - QS) + j(IS + QC)
    // Where ADC_data = I, NCO_sine = Q, NCO_cosine = C, NCO = jS
    
    // I component: ADC_data × NCO_cosine (real × real)
    wire [63:0] mult_i;
    assign mult_i = $signed(adc_data_reg) * $signed(nco_cosine_ext);
    
    // Q component: ADC_data × NCO_sine (real × real)
    wire [63:0] mult_q;
    assign mult_q = $signed(adc_data_reg) * $signed(nco_sine_ext);
    
    // ========================================================================
    // Multiplication results (take upper 32 bits for scaling)
    // ========================================================================

    reg [31:0] i_mult_result;
    reg [31:0] q_mult_result;

    always @(posedge clk) begin
        i_mult_result <= mult_i[55:24]; // Scale down by 2^24
        q_mult_result <= mult_q[55:24]; // Scale down by 2^24
    end

    // ========================================================================
    // Adaptive Gain Scaling (before CIC decimation)
    // ========================================================================

    wire [31:0] i_gain_scaled;
    wire [31:0] q_gain_scaled;
    wire        gain_valid;

    adaptive_gain_scaler u_i_gain_scaler (
        .clk              (clk),
        .rst_n            (rst_n),
        .sample_in        (i_mult_result),
        .sample_valid_in  (data_valid_reg),
        .gain_control     (gain_control),
        .sample_out       (i_gain_scaled),
        .sample_valid_out (gain_valid)
    );

    adaptive_gain_scaler u_q_gain_scaler (
        .clk              (clk),
        .rst_n            (rst_n),
        .sample_in        (q_mult_result),
        .sample_valid_in  (data_valid_reg),
        .gain_control     (gain_control),
        .sample_out       (q_gain_scaled),
        .sample_valid_out ()  // Not used since both should be valid together
    );

    // ========================================================================
    // CIC Decimation Filter
    // ========================================================================

    wire [31:0] i_decimated;
    wire [31:0] q_decimated;
    wire        dec_valid;

    cic_decimator #(
        .INPUT_WIDTH      (32),
        .OUTPUT_WIDTH     (32),
        .STAGES           (3),
        .DECIMATION       (12)
    ) u_cic_decimator_i (
        .clk              (clk),
        .rst_n            (rst_n),
        .data_in          (i_gain_scaled),
        .data_valid       (gain_valid),
        .data_out         (i_decimated),
        .output_valid     (dec_valid)
    );

    cic_decimator #(
        .INPUT_WIDTH      (32),
        .OUTPUT_WIDTH     (32),
        .STAGES           (3),
        .DECIMATION       (12)
    ) u_cic_decimator_q (
        .clk              (clk),
        .rst_n            (rst_n),
        .data_in          (q_gain_scaled),
        .data_valid       (gain_valid),
        .data_out         (q_decimated),
        .output_valid     ()  // Not used
    );

    // ========================================================================
    // Compensation Filter (Anti-imaging FIR)
    // ========================================================================

    wire [31:0] i_compensated;
    wire [31:0] q_compensated;
    wire        comp_valid;

    compensation_filter #(
        .WIDTH            (32),
        .COEFF_WIDTH      (16),
        .TAPS             (8),
        .DECIMATION       (12)
    ) u_compensation_filter_i (
        .clk              (clk),
        .rst_n            (rst_n),
        .data_in          (i_decimated),
        .data_valid       (dec_valid),
        .data_out         (i_compensated),
        .output_valid     (comp_valid)
    );

    compensation_filter #(
        .WIDTH            (32),
        .COEFF_WIDTH      (16),
        .TAPS             (8),
        .DECIMATION       (12)
    ) u_compensation_filter_q (
        .clk              (clk),
        .rst_n            (rst_n),
        .data_in          (q_decimated),
        .data_valid       (dec_valid),
        .data_out         (q_compensated),
        .output_valid     ()  // Not used
    );

    // ========================================================================
    // Output assignment
    // ========================================================================

    assign i_component = i_compensated;
    assign q_component = q_compensated;
    assign ddc_valid = comp_valid;
    
endmodule
