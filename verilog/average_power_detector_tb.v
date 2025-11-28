`timescale 1ns / 1ps

module average_power_detector_tb;

    // DUT parameters
    parameter DATA_WIDTH = 32;
    parameter AVG_WINDOW_BITS = 10; // 1024 samples
    parameter CLK_PERIOD = 10; // 100 MHz

    // DUT signals
    reg                         clk;
    reg                         rst_n;
    reg [DATA_WIDTH-1:0]        sample_in;
    reg                         sample_valid_in;
    wire [DATA_WIDTH-1:0]       avg_power_out;
    wire                        avg_power_valid;
    reg [31:0]                  i;

    // Instantiate DUT
    average_power_detector dut (
        .clk(clk),
        .rst_n(rst_n),
        .sample_in(sample_in),
        .sample_valid_in(sample_valid_in),
        .avg_power_out(avg_power_out),
        .avg_power_valid(avg_power_valid)
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
        sample_in = 0;
        sample_valid_in = 0;

        // Reset
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        $display("Testing Average Power Detector...");

        // Input a sequence of samples
        for (i = 0; i < 2048; i = i + 1) begin  // Two full windows
            sample_in = i % 256;  // Pseudo-random samples
            sample_valid_in = 1;
            #CLK_PERIOD;
        end
        sample_valid_in = 0;

        // Wait for processing
        #1000;

        $display("Average Power Detector test completed");

        // Basic checks
        if (avg_power_valid) begin
            $display("(OK) Average power calculation functional: 0x%h", avg_power_out);
            $display("TEST PASSED");
        end else begin
            $display("⚠ No valid average power output");
            $display("TEST FAILED");
        end

        $finish;
    end

endmodule
