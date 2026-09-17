// ============================================================================
// app_stream_cdc_tb - verifies each word is presented exactly once, including
// with the consumer holding rd_ready continuously (the issue #58 case).
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module app_stream_cdc_tb;

    reg wr_clk = 0, rd_clk = 0, rst_n = 0;
    always #5  wr_clk = ~wr_clk;      // 100 MHz
    always #4  rd_clk = ~rd_clk;      // 125 MHz

    reg  [31:0] wr_data = 0;
    reg  [15:0] wr_len  = 0;
    reg         wr_valid = 0;
    wire        wr_full;
    wire [31:0] rd_data;
    wire [15:0] rd_len;
    wire        rd_valid, rd_empty;
    reg         rd_ready = 0;

    app_stream_cdc dut (
        .wr_clk(wr_clk), .rd_clk(rd_clk), .rst_n(rst_n),
        .wr_data(wr_data), .wr_len(wr_len), .wr_valid(wr_valid), .wr_full(wr_full),
        .rd_data(rd_data), .rd_len(rd_len), .rd_valid(rd_valid),
        .rd_ready(rd_ready), .rd_empty(rd_empty)
    );

    localparam integer NW = 40;
    integer errors = 0;
    integer got = 0;
    integer exp_word = 0;

    // tally accepted words; each data value must appear exactly once, in order
    always @(posedge rd_clk) begin
        if (rst_n && rd_valid && rd_ready) begin
            if (rd_data !== exp_word[31:0]) begin
                if (errors < 8)
                    $display("  MISMATCH at word %0d: got %0d (%h) expected %0d",
                             got, rd_data, rd_data, exp_word);
                errors = errors + 1;
            end
            got = got + 1;
            exp_word = exp_word + 1;
        end
    end

    integer i;
    initial begin
        $display("=== app_stream_cdc_tb ===");
        repeat (4) @(negedge wr_clk);
        rst_n = 1;
        repeat (4) @(negedge wr_clk);

        // producer: val = index, len = 32+index
        fork
            begin
                for (i = 0; i < NW; i = i + 1) begin
                    @(negedge wr_clk);
                    wr_data = i;
                    wr_len  = 16'd32 + i[15:0];
                    wr_valid = 1'b1;
                    while (wr_full) @(negedge wr_clk);
                end
                @(negedge wr_clk);
                wr_valid = 1'b0;
            end
            begin
                // consumer holds rd_ready HIGH the whole time (worst case for
                // duplicate transfers)
                repeat (6) @(negedge rd_clk);
                rd_ready = 1'b1;
                wait (got == NW);
                @(negedge rd_clk);
                rd_ready = 1'b0;
            end
        join

        repeat (8) @(negedge rd_clk);
        if (got != NW) begin
            $display("FAIL: received %0d of %0d words", got, NW);
            errors = errors + 1;
        end
        if (errors !== 0) begin
            $display("FATAL: %0d errors (duplicate or out-of-order transfers)", errors);
            $fatal(1);
        end
        $display("  %0d words, each presented exactly once, in order OK", got);
        $display("PASS: app_stream_cdc (no duplicate transfers under continuous rd_ready)");
        $finish;
    end

    initial begin
        #2_000_000;
        $fatal(1, "app_stream_cdc_tb timeout (got=%0d)", got);
    end

endmodule

`default_nettype wire
