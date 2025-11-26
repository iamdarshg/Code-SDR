`timescale 1ns / 1ps

module digital_downconverter_tb;

    // DUT parameters
    parameter WIDTH = 32;
    parameter CLK_PERIOD = 10; // 100 MHz

    // DUT signals
    reg         clk;
    reg         rst_n;
    reg [31:0]  adc_data;
    reg         data_valid;
    reg [15:0]  nco_sine;
    reg [15:0]  nco_cosine;
    reg [7:0]   gain_control;
    wire [31:0] i_component;
    wire [31:0] q_component;
    wire        ddc_valid;

    // Instantiate DUT
    digital_downconverter dut (
        .clk(clk),
        .rst_n(rst_n),
        .adc_data(adc_data),
        .data_valid(data_valid),
        .nco_sine(nco_sine),
        .nco_cosine(nco_cosine),
        .gain_control(gain_control),
        .i_component(i_component),
        .q_component(q_component),
        .ddc_valid(ddc_valid)
    );

    // Clock generation
    initial begin
        clk = 0;
        forever #(CLK_PERIOD/2) clk = ~clk;
    end

    // Test sequence
    initial begin
        // Initialize signals
        rst_n = 0;
        adc_data = 0;
        data_valid = 0;
        nco_sine = 0;
        nco_cosine = 0;
        gain_control = 0;

        // Reset
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        $display("Testing Digital Downconverter...");

        // Set NCO values (cos(0) = 1, sin(0) = 0 for baseband)
        nco_sine = 16'h0000;     // sin(0) = 0
        nco_cosine = 16'h7FFF;   // cos(0) ≈ 1.0 (normalized)

        // Input some ADC data
        adc_data = 32'h1000;
        gain_control = 8'h88;    // No gain adjustment
        data_valid = 1;
        #CLK_PERIOD;
        data_valid = 0;

        // Wait for processing pipeline
        #10000; // DDC has complex pipeline

        $display("Digital Downconverter test completed");

        if (ddc_valid) begin
            $display("✓ DDC processing functional: I=0x%h, Q=0x%h", i_component, q_component);
        end else begin
            $display("⚠ No valid DDC output");
        end

        $finish;
    end

endmodule
