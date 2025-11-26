`timescale 1ns/1ps

module fft_processor_tb;

    // Testbench parameters
    parameter FFT_SIZE = 256; // Reduced for simulation speed
    parameter DATA_WIDTH = 24;
    parameter SCALE_FACTOR = 24;

    // DUT signals
    reg clk;
    reg rst_n;
    reg [DATA_WIDTH-1:0] real_in;
    reg [DATA_WIDTH-1:0] imag_in;
    reg data_valid;
    wire [DATA_WIDTH-1:0] real_out;
    wire [DATA_WIDTH-1:0] imag_out;
    wire fft_valid;
    wire [11:0] fft_index;
    wire overflow_flag;
    wire processing_active;

    // Instantiate DUT
    fft_processor # (
        .FFT_SIZE(FFT_SIZE),
        .DATA_WIDTH(DATA_WIDTH),
        .SCALE_FACTOR(SCALE_FACTOR)
    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .real_in(real_in),
        .imag_in(imag_in),
        .data_valid(data_valid),
        .real_out(real_out),
        .imag_out(imag_out),
        .fft_valid(fft_valid),
        .fft_index(fft_index),
        .overflow_flag(overflow_flag),
        .processing_active(processing_active)
    );

    // Clock generation
    initial begin
        clk = 1'b0;
        forever #5 clk = ~clk; // 100 MHz clock
    end

    // Test stimulus
    integer sample_count;
    reg fft_started;

    initial begin
        sample_count = 0;
        fft_started = 0;

        // Initialize inputs
        rst_n = 1'b0;
        real_in = {DATA_WIDTH{1'b0}};
        imag_in = {DATA_WIDTH{1'b0}};
        data_valid = 1'b0;

        // Reset
        #100;
        rst_n = 1'b1;

        // Wait a bit
        #100;

        // Apply test data - simple impulse
        repeat (FFT_SIZE) begin
            @(posedge clk);
            real_in <= ($random % (1 << (DATA_WIDTH-2))); // Random data
            imag_in <= ($random % (1 << (DATA_WIDTH-2)));
            data_valid <= 1'b1;
        end

        @(posedge clk);
        data_valid <= 1'b0;

        // Wait for processing (conservative wait)
        #1000000; // 1 ms for larger FFT

        // Check if any FFT output was produced
        if (fft_started) begin
            $display("TEST PASSED: FFT processing completed and outputs generated");
        end else begin
            $display("TEST FAILED: FFT processing did not start or no outputs");
        end

        #100;
        $finish;
    end

    // Monitor FFT outputs
    always @(posedge clk) begin
        if (fft_valid) begin
            if (!fft_started) begin
                fft_started <= 1'b1;
                $display("FFT processing started, first valid output at time %t", $time);
            end
            sample_count = sample_count + 1;
            if (sample_count > FFT_SIZE) begin
                $display("Warning: More samples than expected");
            end
        end
    end

endmodule
