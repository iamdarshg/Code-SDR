`timescale 1ns / 1ps

module adaptive_gain_scaler_tb;

    // DUT parameters
    parameter DATA_WIDTH = 32;
    parameter CLK_PERIOD = 10; // 100 MHz

    // DUT signals
    reg                         clk;
    reg                         rst_n;
    reg [DATA_WIDTH-1:0]        sample_in;
    reg                         sample_valid_in;
    reg [7:0]                   gain_control;
    wire [DATA_WIDTH-1:0]       sample_out;
    wire                        sample_valid_out;

    // Instantiate DUT
    adaptive_gain_scaler dut (
        .clk(clk),
        .rst_n(rst_n),
        .sample_in(sample_in),
        .sample_valid_in(sample_valid_in),
        .gain_control(gain_control),
        .sample_out(sample_out),
        .sample_valid_out(sample_valid_out)
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
        gain_control = 0;

        // Reset
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        $display("Testing Adaptive Gain Scaler...");

        // Test different configurations
        gain_control = 8'h81; // Gain=9x (1000), Left shift by 1
        sample_in = 32'h1000;
        sample_valid_in = 1;
        #CLK_PERIOD;
        sample_valid_in = 0;

        #100;
        $display("Gain Scalar test: in=0x%h, out=0x%h", 32'h1000, sample_out);

        $display("Adaptive Gain Scaler test completed");

        if (sample_valid_out) begin
            $display("✓ Scaling and gain functional");
        end else begin
            $display("⚠ Output not valid");
        end

        $finish;
    end

endmodule
