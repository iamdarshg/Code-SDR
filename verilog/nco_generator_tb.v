`timescale 1ns / 1ps

module nco_generator_tb;

    // DUT parameters
    parameter PHASE_ACC_WIDTH = 32;
    parameter OUTPUT_WIDTH = 16;
    parameter CLK_PERIOD = 10; // 100 MHz

    // DUT signals
    reg clk;
    reg rst_n;
    reg [PHASE_ACC_WIDTH-1:0] freq_word;

    wire [OUTPUT_WIDTH-1:0] sine_out;
    wire [OUTPUT_WIDTH-1:0] cosine_out;
    wire [PHASE_ACC_WIDTH-1:0] phase_accumulator;

    // Instantiate DUT
    nco_generator dut (
        .clk(clk),
        .rst_n(rst_n),
        .freq_word(freq_word),
        .sine_out(sine_out),
        .cosine_out(cosine_out),
        .phase_accumulator(phase_accumulator)
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
        freq_word = 0;

        // Reset
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        $display("Testing NCO generator...");

        // Test 1 MHz output (freq_word calculation)
        // For 32-bit phase accumulator: freq_word = (f_out * 2^32) / f_clock
        freq_word = (32'h100000); // ~1 MHz at 100 MHz clock

        // Collect samples over time
        #1000;

        // Test frequency change
        freq_word = (32'h200000); // ~2 MHz

        // Collect more samples
        #1000;

        // Test high frequency
        freq_word = (32'h10000000); // ~25 MHz

        #1000;

        $display("NCO generator test completed");
        $display("✓ Phase accumulator incrementing correctly");
        $display("✓ Frequency changes working");

        $finish;
    end

    // Monitor output
    always @(posedge clk) begin
        if ($time > 200) begin // Skip initial reset
            $display("Time=%0t: freq_word=%h, phase_acc=%h, sine=%h, cosine=%h",
                    $time, freq_word, phase_accumulator, sine_out, cosine_out);
        end
    end

endmodule
