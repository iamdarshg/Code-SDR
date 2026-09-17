`default_nettype none
`timescale 1ns/1ps

module async_fifo_regression_tb;

    localparam WIDTH = 16;
    localparam DEPTH = 8;
    localparam ADDR_WIDTH = $clog2(DEPTH);

    reg wr_clk = 1'b0;
    reg rd_clk = 1'b0;

    always #3 wr_clk = ~wr_clk;
    always #4 rd_clk = ~rd_clk;

    reg                  wr_rst_n = 1'b0;
    reg                  rd_rst_n = 1'b0;
    reg  [WIDTH-1:0]     din      = '0;
    reg                  wr_en    = 1'b0;
    reg                  rd_en    = 1'b0;
    wire [WIDTH-1:0]     dout;
    wire                 full;
    wire                 empty;

    async_fifo #(
        .WIDTH(WIDTH),
        .DEPTH(DEPTH)
    ) dut (
        .wr_clk  (wr_clk),
        .rd_clk  (rd_clk),
        .wr_rst_n(wr_rst_n),
        .rd_rst_n(rd_rst_n),
        .din     (din),
        .wr_en   (wr_en),
        .rd_en   (rd_en),
        .dout    (dout),
        .full    (full),
        .empty   (empty)
    );

    integer errors = 0;
    integer tests_run = 0;
    reg test_done = 1'b0;

    task automatic check;
        input        cond;
        input [8*40-1:0] name;
        begin
            tests_run = tests_run + 1;
            if (cond !== 1'b1) begin
                errors = errors + 1;
                $display("[%0t] FAIL: %0s", $time, name);
            end
        end
    endtask

    task automatic wr_step;
        input [WIDTH-1:0] data;
        input             en;
        begin
            @(negedge wr_clk);
            din   = data;
            wr_en = en;
            @(posedge wr_clk);
            #1;
            wr_en = 1'b0;
            #1;
        end
    endtask

    task automatic rd_step;
        input en;
        begin
            @(negedge rd_clk);
            rd_en = en;
            @(posedge rd_clk);
            #1;
            rd_en = 1'b0;
            #1;
        end
    endtask

    task automatic sync_wait;
        input integer cycles;
        integer k;
        begin
            for (k = 0; k < cycles; k = k + 1) @(posedge rd_clk);
        end
    endtask

    task automatic wr_wait_ready;
        input [WIDTH-1:0] data;
        integer guard;
        begin
            guard = 0;
            while (full && guard < 100) begin
                @(negedge wr_clk);
                guard = guard + 1;
            end
            if (full) begin
                errors = errors + 1;
                $display("[%0t] FAIL: timeout waiting !full for write", $time);
            end
            wr_step(data, 1'b1);
        end
    endtask

    task automatic rd_wait_avail;
        input [WIDTH-1:0] exp;
        integer guard;
        begin
            guard = 0;
            while (empty && guard < 100) begin
                @(negedge rd_clk);
                guard = guard + 1;
            end
            if (empty) begin
                errors = errors + 1;
                $display("[%0t] FAIL: timeout waiting !empty for read", $time);
            end
            rd_step(1'b1);
            @(posedge rd_clk);
            #1;
            check(dout === exp, "dout data mismatch");
            @(negedge rd_clk);
        end
    endtask

    initial begin
        wr_rst_n = 1'b0;
        rd_rst_n = 1'b0;
        repeat (4) @(posedge wr_clk);
        repeat (4) @(posedge rd_clk);
        @(negedge rd_clk);
        wr_rst_n = 1'b1;
        rd_rst_n = 1'b1;
        repeat (4) @(posedge rd_clk);
        repeat (4) @(posedge wr_clk);
    end

    initial begin
        #10_000_000;
        if (!test_done) begin
            $display("[%0t] FATAL TIMEOUT: regression did not complete", $time);
            $fatal(1, "FIFO regression timeout");
        end
        #10;
        if (errors == 0) begin
            $display("TEST RESULT: PASS (%0d checks)", tests_run);
        end else begin
            $display("TEST RESULT: FAIL (%0d/%0d checks failed)", errors, tests_run);
        end
        $finish;
    end

    task automatic run_tests;
        integer i;
        integer n_wraps;
        begin
            tests_run = tests_run + 1;
            if (!(empty === 1'b1 && full === 1'b0)) begin
                errors = errors + 1;
                $display("[%0t] FAIL: reset state not empty/not !full", $time);
            end

            wr_step(16'hA5A5, 1'b1);
            wr_step(16'h5A5A, 1'b1);
            sync_wait(4);
            rd_step(1'b1);
            #1;
            check(dout === 16'hA5A5, "two-word drain word0");
            rd_step(1'b1);
            #1;
            check(dout === 16'h5A5A, "two-word drain word1");
            sync_wait(4);
            check(empty === 1'b1, "empty after two-word drain");

            for (i = 0; i < DEPTH; i = i + 1) begin
                wr_step(i[WIDTH-1:0], 1'b1);
            end
            sync_wait(6);
            check(full === 1'b1, "full after DEPTH writes");
            wr_step(16'hDEAD, 1'b1);
            sync_wait(2);
            rd_step(1'b1);
            #1;
            check(dout === 16'h0000, "overflow ignored, first word intact");
            for (i = 1; i < DEPTH; i = i + 1) begin
                rd_wait_avail(i[WIDTH-1:0]);
            end
            sync_wait(6);
            check(empty === 1'b1, "empty after full drain");

            n_wraps = 3;
            for (i = 0; i < DEPTH; i = i + 1) begin
                wr_wait_ready((i + 16'h1000));
            end
            for (i = 0; i < DEPTH; i = i + 1) begin
                rd_wait_avail((i + 16'h1000));
            end
            n_wraps = n_wraps - 1;
            while (n_wraps > 0) begin
                for (i = 0; i < DEPTH; i = i + 1) begin
                    wr_wait_ready(i + 1);
                end
                for (i = 0; i < DEPTH; i = i + 1) begin
                    rd_wait_avail(i + 1);
                end
                n_wraps = n_wraps - 1;
            end

            for (i = 0; i < 4; i = i + 1) begin
                fork
                    wr_step(i[WIDTH-1:0], 1'b1);
                    begin
                        @(negedge rd_clk);
                        rd_en = 1'b1;
                        @(posedge rd_clk);
                        #1;
                        rd_en = 1'b0;
                    end
                join
                #1;
                if (!empty) begin
                    check(dout === i[WIDTH-1:0], "simultaneous rw data");
                end
                sync_wait(3);
            end

            @(negedge wr_clk);
            wr_rst_n = 1'b0;
            @(negedge rd_clk);
            rd_rst_n = 1'b0;
            repeat (4) @(posedge wr_clk);
            @(negedge wr_clk);
            wr_rst_n = 1'b1;
            @(negedge rd_clk);
            rd_rst_n = 1'b1;
            repeat (6) @(posedge rd_clk);
            repeat (6) @(posedge wr_clk);
            check(empty === 1'b1, "empty after mid-run reset");
            check(full === 1'b0, "not full after mid-run reset");

            test_done = 1'b1;
        end
    endtask

    initial begin
        wait(wr_rst_n && rd_rst_n);
        repeat (8) @(negedge rd_clk);
        run_tests;
        if (errors != 0)
            $fatal(1, "FIFO regression failed: %0d errors", errors);
        $display("TEST RESULT: PASS (%0d checks)", tests_run);
        $finish;
    end

endmodule
