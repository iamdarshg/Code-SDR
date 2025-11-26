`timescale 1ns / 1ps

module adc_interface_tb;

    // DUT parameters
    parameter ADC_WIDTH = 10;
    parameter CLK_PERIOD = 10; // 100 MHz

    // DUT signals
    reg clk;
    reg rst_n;
    reg [ADC_WIDTH-1:0] adc_data;
    reg adc_clk;

    wire [ADC_WIDTH-1:0] data_out;
    wire data_valid;
    wire overflow_flag;

    // Instantiate DUT
    adc_interface dut (
        .clk(clk),
        .rst_n(rst_n),
        .adc_data(adc_data),
        .adc_clk(adc_clk),
        .data_out(data_out),
        .data_valid(data_valid),
        .overflow_flag(overflow_flag)
    );

    // Clock generation
    initial begin
        clk = 0;
        forever #(CLK_PERIOD/2) clk = ~clk;
    end

    initial begin
        adc_clk = 0;
        forever #9.52 adc_clk = ~adc_clk; // ~105 MHz
    end

    // Test sequence
    initial begin
        // Initialize signals
        rst_n = 0;
        adc_data = 0;

        // Reset
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        // Test normal operation
        $display("Testing ADC interface...");

        // Test various ADC values
        adc_data = 10'h1FF; #20; // Max positive
        adc_data = 10'h000; #20; // Zero
        adc_data = 10'h200; #20; // Negative
        adc_data = 10'h100; #20; // Mid-scale

        // Test overflow condition
        adc_data = 10'h400; #20; // Overflow value

        // Wait for processing
        #500;

        $display("ADC interface test completed");

        // Check results
        if (overflow_flag) begin
            $display("✓ Overflow detection working");
        end else begin
            $display("✗ Overflow detection failed");
        end

        $finish;
    end

    // Monitor
    always @(posedge clk) begin
        if (data_valid) begin
            $display("Time=%0t: ADC data=%h, Output data=%h",
                    $time, adc_data, data_out);
        end
    end

endmodule
