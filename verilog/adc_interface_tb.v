`timescale 1ns / 1ps

module adc_interface_tb;

    // DUT parameters
    parameter ADC_WIDTH = 10;
    parameter CLK_PERIOD = 10; // 100 MHz

    // DUT signals
    reg clk_adc;
    reg rst_n;
    reg [ADC_WIDTH-1:0] adc_data;
    reg adc_valid;
    reg adc_ovr;

    wire [31:0] adc_samples;
    wire sample_valid;
    wire overflow_detect;

    // Instantiate DUT
    adc_interface dut (
        .clk_adc(clk_adc),
        .rst_n(rst_n),
        .adc_data(adc_data),
        .adc_valid(adc_valid),
        .adc_ovr(adc_ovr),
        .adc_samples(adc_samples[ADC_WIDTH-1:0]), // Use lower bits for test
        .sample_valid(sample_valid),
        .overflow_detect(overflow_detect)
    );

    // Clock generation
    initial begin
        clk_adc = 0;
        forever #9.52 clk_adc = ~clk_adc; // ~105 MHz
    end

    // Test sequence
    initial begin
        // Initialize signals
        rst_n = 0;
        adc_data = 0;
        adc_valid = 0;
        adc_ovr = 0;

        // Reset
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        // Test normal operation
        $display("Testing ADC interface...");

        // Test various ADC values
        @(posedge clk_adc); adc_data = 10'h1FF; adc_valid = 1; #9.52; adc_valid = 0;
        @(posedge clk_adc); adc_data = 10'h000; adc_valid = 1; #9.52; adc_valid = 0;
        @(posedge clk_adc); adc_data = 10'h200; adc_valid = 1; #9.52; adc_valid = 0;
        @(posedge clk_adc); adc_data = 10'h100; adc_valid = 1; #9.52; adc_valid = 0;

        // Test overflow condition
        @(posedge clk_adc); adc_data = 10'h400; adc_valid = 1; adc_ovr = 1; #9.52; adc_valid = 0; adc_ovr = 0;
        @(posedge clk_adc); adc_data = 10'h400; adc_valid = 1; adc_ovr = 1; #9.52; adc_valid = 0; adc_ovr = 0;
        @(posedge clk_adc); adc_data = 10'h400; adc_valid = 1; adc_ovr = 1; #9.52; adc_valid = 0; adc_ovr = 0;

        // Wait for processing
        #500;

        $display("ADC interface test completed");

        // Check results
        if (overflow_detect) begin
            $display("✓ Overflow detection working");
            $display("TEST PASSED");
        end else begin
            $display("✗ Overflow detection failed");
            $display("TEST FAILED");
        end

        $finish;
    end

    // Monitor
    always @(posedge clk_adc) begin
        if (sample_valid) begin
            $display("Time=%0t: ADC data=%h, Output data=%h",
                    $time, adc_data, adc_samples[9:0]);
        end
    end

endmodule
