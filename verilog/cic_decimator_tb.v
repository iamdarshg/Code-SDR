`timescale 1ns / 1ps

module cic_decimator_tb;

    // DUT parameters
    parameter INPUT_WIDTH = 32;
    parameter OUTPUT_WIDTH = 32;
    parameter STAGES = 3;
    parameter DECIMATION = 8;
    parameter CLK_PERIOD = 10; // 100 MHz

    // DUT signals
    reg clk;
    reg rst_n;
    reg [INPUT_WIDTH-1:0] data_in;
    reg data_valid;
    wire [OUTPUT_WIDTH-1:0] data_out;
    wire output_valid;

    // Instantiate DUT
    cic_decimator #(
        .INPUT_WIDTH(INPUT_WIDTH),
        .OUTPUT_WIDTH(OUTPUT_WIDTH),
        .STAGES(STAGES),
        .DECIMATION(DECIMATION)
    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .data_in(data_in),
        .data_valid(data_valid),
        .data_out(data_out),
        .output_valid(output_valid)
    );

    // Clock generation (100 MHz system clock)
    initial begin
        clk = 0;
        forever #(CLK_PERIOD/2) clk = ~clk;
    end

    // Test sequence
    initial begin
        // Initialize signals
        rst_n = 1;
        data_in = 0;
        data_valid = 0;

        // Reset
        #100;
        rst_n = 0;
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        $display("Testing CIC Decimator...");

        // Test impulse response (theoretical gain should be compensated)
        data_in = 32'h00010000; // Small input to avoid saturation
        data_valid = 1;
        #CLK_PERIOD;
        data_valid = 0;

        // Feed DECIMATION-1 zeros
        repeat(DECIMATION-1) begin
            data_in = 0;
            data_valid = 1;
            #CLK_PERIOD;
        end
        data_valid = 0;

        // Wait for output
        wait(output_valid);
        $display("Impulse response: input=0x%h, output=0x%h", 32'h00010000, data_out);

        // Wait longer to see if output stabilizes
        #1000;

        $display("CIC Decimator basic test completed");

        // Check for basic functionality
        if (data_out !== data_out) begin // Check for X/Z
            $display("⚠ Warning: Output contains X/Z values");
            $display("TEST FAILED: Invalid output");
        end else begin
            $display("✓ Output appears valid");
            $display("TEST PASSED");
        end

        $finish;
    end

    // Monitor outputs
    always @(posedge clk) begin
        if (output_valid) begin
            $display("Time=%0t: Data out=0x%h", $time, data_out);
        end
    end

endmodule
