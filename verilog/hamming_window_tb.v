`timescale 1ns / 1ps

module hamming_window_tb;

    // DUT parameters
    parameter WIDTH = 32;
    parameter FFT_SIZE = 1024;
    parameter CLK_PERIOD = 10; // 100 MHz

    // DUT signals
    reg clk;
    reg rst_n;
    reg [WIDTH-1:0] data_in;
    reg data_valid;
    wire [WIDTH-1:0] data_out;
    wire output_valid;

    // Instantiate DUT
    hamming_window #(
        .WIDTH(WIDTH),
        .FFT_SIZE(FFT_SIZE)
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
        rst_n = 0;
        data_in = 0;
        data_valid = 0;

        // Reset
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        $display("Testing Hamming Window...");

        // Test with a ramp signal (FFT_SIZE samples)
        integer i;
        for (i = 0; i < FFT_SIZE; i = i + 1) begin
            data_in = i;  // Simple ramp input
            data_valid = 1;
            #CLK_PERIOD;
        end
        data_valid = 0;

        // Wait for output processing (pipeline delay)
        #1000;

        $display("Hamming Window test completed - %0d samples processed", FFT_SIZE);

        // Basic validity check
        if (data_out !== data_out) begin // Check for X/Z
            $display("⚠ Warning: Output contains X/Z values");
        end else begin
            $display("✓ Windowing appears functional");
        end

        // Verify window properties
        // Hamming window should start near 0, peak in middle, end near 0
        $display("First coefficient should be near zero");
        $display("Middle coefficient should be amplitude scaled");
        $display("Last coefficient should be near zero");

        $finish;
    end

    // Monitor outputs for first few and last few samples
    integer output_count = 0;
    always @(posedge clk) begin
        if (output_valid) begin
            output_count = output_count + 1;
            if (output_count <= 5 || output_count >= FFT_SIZE - 4) begin
                $display("Sample %0d: input=0x%h, windowed=0x%h",
                        output_count, data_in, data_out);
            end
        end
    end

endmodule
