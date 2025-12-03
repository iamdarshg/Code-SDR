`timescale 1ns/1ps

module async_fifo_tb;

    // Testbench parameters
    parameter WIDTH = 32;
    parameter DEPTH = 16; // Smaller for simulation

    // DUT signals
    reg wr_clk;
    reg rd_clk;
    reg wr_rst_n;
    reg rd_rst_n;
    reg [WIDTH-1:0] din;
    reg wr_en;
    reg rd_en;
    wire [WIDTH-1:0] dout;
    wire full;
    wire empty;

    // Instantiate DUT
    async_fifo # (
        .WIDTH(WIDTH),
        .DEPTH(DEPTH)
    ) dut (
        .wr_clk(wr_clk),
        .rd_clk(rd_clk),
        .wr_rst_n(wr_rst_n),
        .rd_rst_n(rd_rst_n),
        .din(din),
        .wr_en(wr_en),
        .rd_en(rd_en),
        .dout(dout),
        .full(full),
        .empty(empty)
    );

    // Clock generation
    initial begin
        wr_clk = 1'b0;
        forever #6 wr_clk = ~wr_clk; // ~83.33 MHz clock
    end

    initial begin
        rd_clk = 1'b0;
        forever #5 rd_clk = ~rd_clk; // 100 MHz clock
    end

    // Test stimulus
    integer i;
    reg [WIDTH-1:0] expected_data;
    reg test_passed;
    reg test_completed = 0;
    // Timeout watchdog (250 seconds)
    initial begin
        #250_000_000_000; // 250 seconds in ns
        if (!test_completed) begin
            $display("TIMEOUT: Test did not complete within 250 seconds");
            $display("TEST FAILED: Timeout occurred during FIFO testing");
            // Dump state at timeout
            $display("[ASYNC_FIFO_TB] Dumping timeout state:");
            $display("[ASYNC_FIFO_TB]   Simulation time: %0t ns", $time);
            $display("[ASYNC_FIFO_TB]   wr_rst_n=%b, rd_rst_n=%b", wr_rst_n, rd_rst_n);
            $display("[ASYNC_FIFO_TB]   wr_en=%b, rd_en=%b", wr_en, rd_en);
            $display("[ASYNC_FIFO_TB]   din=%h, dout=%h", din, dout);
            $display("[ASYNC_FIFO_TB]   full=%b, empty=%b", full, empty);
            $display("[ASYNC_FIFO_TB]   Loop counter i=%0d", i);
            $display("[ASYNC_FIFO_TB]   Expected data=%h", expected_data);
            $display("[ASYNC_FIFO_TB]   Test state: test_passed=%b", test_passed);
            $display("[ASYNC_FIFO_TB]   FIFO internal state dump:");
            // Add any internal signals if accessible, but since not, the external will suffice
            $finish;
        end
    end

    initial begin
        $display("[ASYNC_FIFO_TB] Starting FIFO testbench simulation");
        $display("[ASYNC_FIFO_TB] Test parameters: WIDTH=%0d, DEPTH=%0d", WIDTH, DEPTH);
        $display("[ASYNC_FIFO_TB] Write clock: ~83.33 MHz, Read clock: 100 MHz");

        // Initialize inputs
        wr_rst_n = 1'b0;
        rd_rst_n = 1'b0;
        din = 0;
        wr_en = 1'b0;
        rd_en = 1'b0;
        test_passed = 1'b1;

        $display("[ASYNC_FIFO_TB] Applying reset...");

        // Reset
        #100;
        wr_rst_n = 1'b1;
        rd_rst_n = 1'b1;

        $display("[ASYNC_FIFO_TB] Reset complete. Waiting for stabilization...");

        // Wait a bit
        #100;

        $display("[ASYNC_FIFO_TB] Starting write operations...");

        // Write some data
        for (i = 0; i < 8; i++) begin
            @(posedge wr_clk);
            $display("[ASYNC_FIFO_TB] Write %0d: full=%b, din=%h", i, full, i[WIDTH-1:0]);
            if (!full) begin
                din = i[WIDTH-1:0];
                wr_en = 1'b1;
                $display("[ASYNC_FIFO_TB]   Writing data %h", i[WIDTH-1:0]);
            end else begin
                $display("[ASYNC_FIFO_TB]   FIFO full, cannot write");
                i = i - 1; // retry
            end
            @(posedge wr_clk);
            wr_en = 1'b0;
        end

        $display("[ASYNC_FIFO_TB] Write operations complete. Waiting for cross-domain sync...");

        // Wait a bit for cross-domain sync
        #200;

        $display("[ASYNC_FIFO_TB] Starting read operations and verification...");

        // Read data and verify
        expected_data = 0;
        for (i = 0; i < 8; i++) begin
            @(posedge rd_clk);
            $display("[ASYNC_FIFO_TB] Read %0d: empty=%b, expected=%h", i, empty, expected_data);
            if (!empty) begin
                rd_en = 1'b1;
                @(posedge rd_clk);
                rd_en = 1'b0;
                #1; // Settle time
                $display("[ASYNC_FIFO_TB]   Read data: %h", dout);
                if (dout !== expected_data) begin
                    $display("[ASYNC_FIFO_TB]   ERROR: DATA MISMATCH - expected %h, got %h", expected_data, dout);
                    test_passed = 1'b0;
                end else begin
                    $display("[ASYNC_FIFO_TB]   Data verified successfully");
                end
                expected_data = expected_data + 1;
            end else begin
                $display("[ASYNC_FIFO_TB]   FIFO empty, cannot read");
                i = i - 1; // retry
            end
        end

        $display("[ASYNC_FIFO_TB] Read and verification complete.");
        $display("[ASYNC_FIFO_TB] Final test state: full=%b, empty=%b", full, empty);

        // Check if test passes
        #100;
        $display("[ASYNC_FIFO_TB] Test result: %s", test_passed ? "PASSED" : "FAILED");
        if (test_passed) begin
            $display("TEST PASSED: FIFO read/write test successful");
        end else begin
            $display("TEST FAILED: FIFO test failed");
        end

        // Dump final state
        $display("[ASYNC_FIFO_TB] Dumping final testbench state:");
        $display("[ASYNC_FIFO_TB]   wr_rst_n=%b, rd_rst_n=%b", wr_rst_n, rd_rst_n);
        $display("[ASYNC_FIFO_TB]   wr_en=%b, rd_en=%b", wr_en, rd_en);
        $display("[ASYNC_FIFO_TB]   din=%h, dout=%h", din, dout);
        $display("[ASYNC_FIFO_TB]   full=%b, empty=%b", full, empty);
        $display("[ASYNC_FIFO_TB]   Loop counter i=%0d", i);
        $display("[ASYNC_FIFO_TB]   Expected data=%h", expected_data);

        test_completed = 1;
        #100;
        $finish;
    end

endmodule
