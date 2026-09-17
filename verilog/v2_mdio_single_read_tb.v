`timescale 1ns/1ps
`default_nettype none
module v2_mdio_single_read_tb;
    reg clk = 0;
    always #5 clk = ~clk;
    reg rst_n = 0;
    reg start = 0;
    wire [15:0] read_data;
    wire busy, done, error, mdc, mdio_o, mdio_oe;
    tri1 mdio;
    reg phy_oe = 0;
    reg phy_data = 1;
    integer sampled_bits = 0;
    reg [63:0] frame = 0;
    localparam [15:0] RESPONSE = 16'h1234;
    assign mdio = mdio_oe ? mdio_o : 1'bz;
    assign mdio = phy_oe ? phy_data : 1'bz;
    v2_mdio_master dut (
        .clk(clk), .rst_n(rst_n), .start(start), .read_op(1'b1),
        .phy_addr(5'd3), .reg_addr(5'd2), .write_data(16'd0),
        .read_data(read_data), .busy(busy), .done(done), .error(error),
        .mdc(mdc), .mdio_i(mdio), .mdio_o(mdio_o), .mdio_oe(mdio_oe)
    );
    always @(posedge mdc) begin
        frame = {frame[62:0], mdio};
        if (sampled_bits >= 46 && mdio_oe !== 1'b0)
            $fatal(1, "Master drove read turnaround or data");
        sampled_bits = sampled_bits + 1;
    end
    always @(negedge mdc) begin
        phy_oe = 0;
        if (sampled_bits >= 47 && sampled_bits < 64) begin
            phy_oe = 1;
            phy_data = sampled_bits == 47 ? 1'b0 : RESPONSE[63-sampled_bits];
        end
    end
    initial begin
        $dumpfile("outputs/mdio_single_read.vcd");
        $dumpvars(0, v2_mdio_single_read_tb);
        repeat (4) @(negedge clk);
        rst_n = 1;
        @(negedge clk); start = 1;
        @(negedge clk); start = 0;
        wait(done === 1'b1);
        #1;
        if (busy !== 1'b0 || mdc !== 1'b0 || mdio_oe !== 1'b0)
            $fatal(1, "Bus not idle on completion");
        if (sampled_bits != 64 || error !== 1'b0 || read_data !== RESPONSE)
            $fatal(1, "Read failed: bits=%0d error=%b data=%h", sampled_bits, error, read_data);
        if (frame[63:18] !== {32'hffffffff, 2'b01, 2'b10, 5'd3, 5'd2})
            $fatal(1, "Read header mismatch");
        if (frame[16:0] !== {1'b0, RESPONSE})
            $fatal(1, "Read turnaround/data mismatch");
        @(posedge clk); #1;
        if (done !== 1'b0) $fatal(1, "Done was not one cycle");
        $display("PASS: isolated MDIO read 0x1234, 64 bits, one-cycle done");
        $finish;
    end
    initial begin
        #100000;
        $fatal(1, "Single MDIO read exceeded 100 us");
    end
endmodule
`default_nettype wire
