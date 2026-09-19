`timescale 1ns/1ps
`default_nettype none
module v2_phy_manager_tb;
    reg clk = 0;
    always #5 clk = ~clk;
    reg rst_n = 0;
    wire mdc, mdio_o, mdio_oe, link_up, ready, error;
    reg phy_int_n = 1'b1;
    reg mdio_i = 1'b1;
    reg phy_oe = 0;
    reg phy_data = 1;
    wire mdio;
    assign mdio = mdio_oe ? mdio_o : 1'bz;
    assign mdio = phy_oe ? phy_data : 1'bz;
    v2_phy_manager dut (
        .clk(clk), .rst_n(rst_n), .phy_int_n(phy_int_n), .mdio_i(mdio),
        .mdc(mdc), .mdio_o(mdio_o), .mdio_oe(mdio_oe),
        .link_up(link_up), .ready(ready), .error(error)
    );
    reg [63:0] captured = 0;
    reg [63:0] previous = 0;
    integer frames = 0;
    reg [4:0] last_addr = 0;
    reg [15:0] last_data = 0;
    reg [15:0] regfile [0:31];
    reg [4:0] addr_latch = 0;
    reg [15:0] id2_value = 16'h1620;
    reg rst_pending = 0;
    reg [15:0] bmsr_value = 16'h0024;
    integer poll_count = 0;
    reg link_established = 0;
    always @(posedge clk) begin
        if (dut.start) begin
            frames = 0;
            captured = 0;
            addr_latch = 0;
        end
    end
    always @(posedge mdc) begin
        captured = {captured[62:0], mdio};
        frames = frames + 1;
    end
    always @(negedge mdc) begin
        phy_oe = 0;
        if (frames == 46) addr_latch = captured[4:0];
        if (frames >= 47 && frames < 64) begin
            if (frames == 47) phy_data = 0;
            else phy_data = regfile[addr_latch][63-frames];
            phy_oe = 1;
        end
    end
    initial begin
        regfile[2] = 16'h0022;
        regfile[3] = 16'h1620;
        regfile[0] = 16'h0000;
        regfile[1] = 16'h0024;
        regfile[9] = 16'h0300;
        regfile[13] = 16'h0000;
        regfile[14] = 16'h0000;
        #30;
        @(negedge clk);
        rst_n = 1;
        $display("TB: reset released");
    end
    always @(posedge clk) begin
        if (mdio_oe) begin
            if (captured[63:18] === {32'hffffffff, 2'b01, 2'b01, 5'd0, 5'd0, 2'b10} && !rst_pending) begin
                regfile[0] <= 16'h8000;
                rst_pending <= 1;
            end else if (captured[63:18] === {32'hffffffff, 2'b01, 2'b01, 5'd0, 5'd0, 2'b10} && rst_pending && captured[15] == 1'b0) begin
                regfile[0] <= 16'h0000;
                rst_pending <= 0;
                poll_count <= 0;
            end
        end
    end
    always @(negedge mdc) begin
        if (rst_pending && frames == 64) begin
            regfile[0] = 16'h0000;
            rst_pending = 0;
            poll_count = poll_count + 1;
        end
    end
    initial begin
        #40000000;
        $fatal(1, "Timeout: state=%0d start=%b busy=%b done=%b read=%b reg=%0d data=%h wire_bits=%0d", dut.op, dut.start, dut.md_busy, dut.md_done, dut.read_op, dut.md_reg_addr, dut.read_data, frames);
    end
    initial begin
        wait(ready);
        #100;
        if (link_up !== 1'b1)
            $fatal(1, "link_up not asserted after bring-up");
        $display("TB: bring-up complete, link_up=1");

        // Cable unplug: BMSR's link bit clears and the periodic re-poll must
        // drop link_up without any bring-up retrigger.
        regfile[1] = 16'h0020;
        wait (!link_up);
        $display("TB: link_up dropped after BMSR link cleared");

        // Replug: the same poll must re-assert it.
        regfile[1] = 16'h0024;
        wait (link_up);
        $display("TB: link_up re-asserted after BMSR link returned");

        // phy_int_n must force an immediate re-read, not wait for the timer.
        regfile[1] = 16'h0020;
        if (link_up !== 1'b1) $fatal(1, "link_up changed before interrupt test");
        phy_int_n = 1'b0;
        repeat (4) @(posedge clk);
        phy_int_n = 1'b1;
        wait (!link_up);
        $display("TB: link_up dropped on PHY interrupt");

        $display("PASS: manager ready=%b link_up=%b error=%b", ready, link_up, error);
        $finish;
    end
endmodule
`default_nettype wire
