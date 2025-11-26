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

    initial begin
        // Initialize inputs
        wr_rst_n = 1'b0;
        rd_rst_n = 1'b0;
        din = 0;
        wr_en = 1'b0;
        rd_en = 1'b0;
        test_passed = 1'b1;

        // Reset
        #100;
        wr_rst_n = 1'b1;
        rd_rst_n = 1'b1;

        // Wait a bit
        #100;

        // Write some data
        for (i = 0; i < 8; i++) begin
            @(posedge wr_clk);
            if (!full) begin
                din <= i[WIDTH-1:0];
                wr_en <= 1'b1;
            end
            @(posedge wr_clk);
            wr_en <= 1'b0;
        end

        // Wait a bit for cross-domain sync
        #200;

        // Read data and verify
        expected_data = 0;
        for (i = 0; i < 8; i++) begin
            @(posedge rd_clk);
            if (!empty) begin
                rd_en <= 1'b1;
                @(posedge rd_clk);
                rd_en <= 1'b0;
                #1; // Settle time
                if (dout !== expected_data) begin
                    $display("DATA MISMATCH: expected %h, got %h", expected_data, dout);
                    test_passed = 1'b0;
                end
                expected_data = expected_data + 1;
            end
        end

        // Check if test passes
        #100;
        if (test_passed) begin
            $display("TEST PASSED: FIFO read/write test successful");
        end else begin
            $display("TEST FAILED: FIFO test failed");
        end

        #100;
        $finish;
    end

endmodule
