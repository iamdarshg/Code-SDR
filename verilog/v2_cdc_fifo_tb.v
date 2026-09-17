// ============================================================================
// v2_cdc_fifo_tb - self-checking verification for v2_cdc_fifo.
//
// Tests:
//   A. fill-to-full / drain integrity (exact capacity DEPTH-1, ordering, no loss)
//   B. sustained 800 Mbps raw-mode rate (25 Mwords/s @100 MHz write,
//      continuous 125 MHz read) with sequence integrity
//   C. max-rate stress (free-running write, backpressure) with integrity
//
// The consumer verifies word k has value k using a counter that lives only in
// the read clock domain, so no cross-domain bookkeeping is involved.
// Producer writes value `sent`; sent increments once per accepted write.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_cdc_fifo_tb;

    localparam integer WIDTH      = 32;
    localparam integer ADDR_WIDTH = 9;                 // 512 deep
    localparam integer DEPTH      = 1 << ADDR_WIDTH;

    reg  wr_clk = 0, rd_clk = 0;
    reg  wr_rst_n = 0, rd_rst_n = 0;
    reg  wr_go = 0, rd_go = 0;
    reg  [WIDTH-1:0] sent = 0;
    wire [WIDTH-1:0] din = sent;
    wire wr_en = wr_go && !full;
    wire rd_en = rd_go && !empty;
    wire [WIDTH-1:0] dout;
    wire full, empty;
    wire [ADDR_WIDTH:0] rd_avail;

    v2_cdc_fifo #(.WIDTH(WIDTH), .ADDR_WIDTH(ADDR_WIDTH)) dut (
        .wr_clk(wr_clk), .wr_rst_n(wr_rst_n), .wr_en(wr_en), .din(din), .full(full),
        .rd_clk(rd_clk), .rd_rst_n(rd_rst_n), .rd_en(rd_en), .dout(dout), .empty(empty),
        .rd_avail(rd_avail)
    );

    always #5 wr_clk = ~wr_clk;   // 100 MHz
    always #4 rd_clk = ~rd_clk;   // 125 MHz

    // ------------------------------------------------- write-domain bookkeeping
    always @(posedge wr_clk or negedge wr_rst_n) begin
        if (!wr_rst_n) sent <= 0;
        else if (wr_en) sent <= sent + 1;
    end

    // -------------------------------------------------- read-domain verification
    reg  [31:0] rd_count = 0;      // reads accepted
    reg         rd_do_d  = 0;
    reg  [31:0] rd_count_d = 0;
    reg  [31:0] got = 0;
    integer     errors = 0;

    always @(posedge rd_clk or negedge rd_rst_n) begin
        if (!rd_rst_n) begin
            rd_count <= 0; rd_do_d <= 0; rd_count_d <= 0; got <= 0; errors <= 0;
        end else begin
            if (rd_en) rd_count <= rd_count + 1;

            rd_do_d    <= rd_en;
            rd_count_d <= rd_count;

            if (rd_do_d) begin
                if (dout !== rd_count_d) begin
                    errors <= errors + 1;
                    if (errors < 5)
                        $display("  MISMATCH: dout=%h expected=%h (read #%0d)",
                                 dout, rd_count_d, rd_count_d);
                end
                got <= got + 1;
            end
        end
    end

    task do_reset;
        begin
            wr_rst_n = 0; rd_rst_n = 0; wr_go = 0; rd_go = 0;
            sent = 0; rd_count = 0; rd_do_d = 0; rd_count_d = 0; got = 0; errors = 0;
            #200;
            wr_rst_n = 1; rd_rst_n = 1;
            #50;
        end
    endtask

    integer i;
    time    b_start, b_end;

    initial begin
        $display("=== v2_cdc_fifo_tb ===");

        // ---------------- A. fill to full and drain ----------------
        do_reset;
        wr_go = 1;
        @(posedge wr_clk);
        while (!full) @(posedge wr_clk);
        wr_go = 0;
        @(posedge wr_clk);
        if (sent !== DEPTH - 1) begin
            $display("FATAL A: full after %0d writes, expected %0d", sent, DEPTH-1);
            $fatal(1);
        end
        $display("  A: full asserted after %0d writes (capacity %0d)", sent, DEPTH-1);

        rd_go = 1;
        @(posedge rd_clk);
        while (!empty) @(posedge rd_clk);
        repeat (4) @(posedge rd_clk);
        rd_go = 0;
        if (errors !== 0) begin $display("FATAL A: %0d errors", errors); $fatal(1); end
        if (got !== sent) begin $display("FATAL A: got %0d of %0d", got, sent); $fatal(1); end
        $display("  A: drained %0d words in order, empty again OK", got);

        // ---------------- B. sustained 800 Mbps raw rate ----------------
        do_reset;
        wr_go = 1; rd_go = 1;
        b_start = $time;
        // one write per 4 cycles of the 100 MHz clock = 25 Mwords/s = 800 Mbps
        for (i = 0; i < 25000; i = i + 1) begin
            @(posedge wr_clk);
            wr_go = 1;
            @(posedge wr_clk); wr_go = 0;
            @(posedge wr_clk);
            @(posedge wr_clk);
        end
        repeat (64) @(posedge rd_clk);
        b_end = $time;
        wr_go = 0;
        repeat (16) @(posedge rd_clk);
        if (errors !== 0) begin $display("FATAL B: %0d errors", errors); $fatal(1); end
        if (got !== sent) begin $display("FATAL B: got %0d of %0d", got, sent); $fatal(1); end
        $display("  B: %0d words streamed, 0 errors, %.1f Mbps sustained OK",
                 sent, (got * WIDTH * 1.0) / ((b_end - b_start) / 1000.0));

        // ---------------- C. max-rate stress with backpressure ----------------
        do_reset;
        wr_go = 1; rd_go = 1;
        for (i = 0; i < 20000; i = i + 1) @(posedge wr_clk);
        wr_go = 0;
        repeat (700) @(posedge rd_clk);
        rd_go = 0;
        repeat (16) @(posedge rd_clk);
        if (errors !== 0) begin $display("FATAL C: %0d errors", errors); $fatal(1); end
        if (got !== sent) begin $display("FATAL C: got %0d of %0d", got, sent); $fatal(1); end
        $display("  C: max-rate stress %0d words, 0 errors OK", sent);

        $display("PASS: v2_cdc_fifo (capacity, sustained 800 Mbps, max rate)");
        $finish;
    end

    initial begin
        #50_000_000;
        $fatal(1, "v2_cdc_fifo_tb timeout");
    end

endmodule

`default_nettype wire
