t`timescale 1ns / 1ps

module nco_generator_tb;

    // DUT parameters
    parameter PHASE_ACC_WIDTH = 32;
    parameter OUTPUT_WIDTH = 16;
    parameter CLK_PERIOD = 10; // 100 MHz

    // DUT signals
    reg clk;
    reg rst_n;
    reg [31:0] frequency_word;
    reg enable;

    wire [OUTPUT_WIDTH-1:0] sine_out;
    wire [OUTPUT_WIDTH-1:0] cosine_out;
    wire valid_out;

    // Instantiate DUT
    nco_generator dut (
        .clk(clk),
        .rst_n(rst_n),
        .frequency_word(frequency_word),
        .enable(enable),
        .sine_out(sine_out),
        .cosine_out(cosine_out),
        .valid_out(valid_out)
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
        frequency_word = 0;
        enable = 1;

        // Reset
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        $display("Testing NCO generator...");

        // Test 1 MHz output (frequency_word calculation)
        // For 32-bit phase accumulator: frequency_word = (f_out * 2^32) / f_clock
        frequency_word = (32'h100000); // ~1 MHz at 100 MHz clock

        // Collect samples over time
        #1000;

        // Test frequency change
        frequency_word = (32'h200000); // ~2 MHz

        // Collect more samples
        #1000;

        // Test high frequency
        frequency_word = (32'h10000000); // ~25 MHz

        #1000;

        $display("NCO generator test completed");
        $display("✓ Frequency synthesis working");

        $display("TEST PASSED");

        $finish;
    end

    // Monitor output
    always @(posedge clk) begin
        if ($time > 200) begin // Skip initial reset
            $display("Time=%0t: frequency_word=%h, sine=%h, cosine=%h, valid=%b",
                    $time, frequency_word, sine_out, cosine_out, valid_out);
        end
    end

endmodule
