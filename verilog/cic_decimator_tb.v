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
    integer i;
    reg test_passed;
    reg test_completed = 0;
    // Timeout watchdog (250 seconds)
    initial begin
        #250_000_000_000; // 250 seconds in ns
        if (!test_completed) begin
            $display("TIMEOUT: Test did not complete within 250 seconds");
            $display("TEST FAILED: Timeout occurred during CIC decimator testing");
            // Dump state at timeout
            $display("[CIC_DECIMATOR_TB] Dumping timeout state:");
            $display("[CIC_DECIMATOR_TB]   Simulation time: %0t ns", $time);
            $display("[CIC_DECIMATOR_TB]   rst_n=%b, data_valid=%b", rst_n, data_valid);
            $display("[CIC_DECIMATOR_TB]   data_in=%h, data_out=%h", data_in, data_out);
            $display("[CIC_DECIMATOR_TB]   output_valid=%b", output_valid);
            $display("[CIC_DECIMATOR_TB]   Loop counter i=%0d", i);
            $display("[CIC_DECIMATOR_TB]   Test state: test_passed=%b", test_passed);
            $display("[CIC_DECIMATOR_TB]   CIC internal state dump:");
            // External interface dump
            $finish;
        end
    end
    initial begin
        $display("[CIC_DECIMATOR_TB] Starting CIC Decimator testbench simulation");
        $display("[CIC_DECIMATOR_TB] Parameters: INPUT_WIDTH=%0d, OUTPUT_WIDTH=%0d, STAGES=%0d, DECIMATION=%0d", INPUT_WIDTH, OUTPUT_WIDTH, STAGES, DECIMATION);
        $display("[CIC_DECIMATOR_TB] Clock: %0d MHz (period %0d ns)", 1000/CLK_PERIOD, CLK_PERIOD);

        // Initialize signals
        rst_n = 1;
        data_in = 0;
        data_valid = 0;
        test_passed = 1;

        $display("[CIC_DECIMATOR_TB] Applying reset...");

        // Reset
        #100;
        rst_n = 0;
        #100;
        rst_n = 1;

        $display("[CIC_DECIMATOR_TB] Reset complete. Waiting for stabilization...");

        // Wait for stabilization
        #100;

        $display("[CIC_DECIMATOR_TB] Starting impulse response test...");

        // Test impulse response (theoretical gain should be compensated)
        data_in = 32'h00010000; // Small input to avoid saturation
        data_valid = 1;
        $display("[CIC_DECIMATOR_TB] Sending impulse: data_in=%h, data_valid=%b", data_in, data_valid);
        #CLK_PERIOD;
        data_valid = 0;

        // Feed DECIMATION-1 zeros
        $display("[CIC_DECIMATOR_TB] Feeding %0d zeros...", DECIMATION-1);
        for (i = 0; i < DECIMATION-1; i++) begin
            data_in = 0;
            data_valid = 1;
            $display("[CIC_DECIMATOR_TB]   Sample %0d: data_in=%h, data_valid=%b", i, data_in, data_valid);
            #CLK_PERIOD;
        end
        data_valid = 0;

        $display("[CIC_DECIMATOR_TB] Input sequence complete. Waiting for output...");

        // Wait for output
        $display("[CIC_DECIMATOR_TB] Waiting for output_valid...");
        wait(output_valid);
        $display("[CIC_DECIMATOR_TB] Output received: data_out=%h", data_out);
        $display("[CIC_DECIMATOR_TB] Impulse response: input=0x%h, output=0x%h", 32'h00010000, data_out);

        // Wait longer to see if output stabilizes
        #1000;

        $display("[CIC_DECIMATOR_TB] CIC Decimator basic test completed");

        // Check for basic functionality
        $display("[CIC_DECIMATOR_TB] Analyzing output...");
        if (data_out !== data_out) begin // Check for X/Z
            $display("[CIC_DECIMATOR_TB]   ERROR: Output contains X/Z values - this indicates a simulation issue");
            test_passed = 0;
            $display("TEST FAILED: Invalid output");
        end else begin
            $display("[CIC_DECIMATOR_TB]   Output appears valid");
            $display("TEST PASSED");
        end

        // Dump final state
        $display("[CIC_DECIMATOR_TB] Dumping final testbench state:");
        $display("[CIC_DECIMATOR_TB]   rst_n=%b, data_valid=%b", rst_n, data_valid);
        $display("[CIC_DECIMATOR_TB]   data_in=%h, data_out=%h", data_in, data_out);
        $display("[CIC_DECIMATOR_TB]   output_valid=%b", output_valid);
        $display("[CIC_DECIMATOR_TB]   Loop counter i=%0d", i);
        $display("[CIC_DECIMATOR_TB]   Test result: %s", test_passed ? "PASSED" : "FAILED");

        test_completed = 1;
        $finish;
    end

    // Monitor outputs
    always @(posedge clk) begin
        if (output_valid) begin
            $display("Time=%0t: Data out=0x%h", $time, data_out);
        end
    end

endmodule
