`timescale 1ns/1ps
`default_nettype none
module v2_mdio_tb;
    reg clk = 0;
    always #5 clk = ~clk;
    reg rst_n = 0;
    reg start = 0;
    reg read_op = 0;
    reg [4:0] phy_addr = 5'd3;
    reg [4:0] reg_addr = 5'd2;
    reg [15:0] write_data = 16'ha531;
    wire [15:0] read_data;
    wire busy, done, error, mdc, mdio_o, mdio_oe;
    tri1 mdio;
    reg phy_oe = 0;
    reg phy_data = 1;
    reg respond = 1;
    reg [15:0] response = 16'h9625;
    reg [63:0] captured = 0;
    integer count = 0;
    integer transactions = 0;
    integer mmd_frames = 0;
    reg check_mmd = 0;
    reg [4:0] expected_reg [0:15];
    reg [15:0] expected_data [0:15];
    integer init_index;
    time previous_rise = 0;
    assign mdio = mdio_oe ? mdio_o : 1'bz;
    assign mdio = phy_oe ? phy_data : 1'bz;
    v2_mdio_master dut (
        .clk(clk), .rst_n(rst_n), .start(start), .read_op(read_op),
        .phy_addr(phy_addr), .reg_addr(reg_addr), .write_data(write_data),
        .read_data(read_data), .busy(busy), .done(done), .error(error),
        .mdc(mdc), .mdio_i(mdio), .mdio_o(mdio_o), .mdio_oe(mdio_oe)
    );
    always @(posedge mdc) begin
        if (count != 0 && $time - previous_rise != 400)
            $fatal(1, "Incorrect MDC period");
        previous_rise = $time;
        if (read_op && count >= 46 && mdio_oe)
            $fatal(1, "Master drove read turnaround/data");
        captured = {captured[62:0], mdio};
        count = count + 1;
        if (check_mmd && count == 64) begin
            if (mmd_frames >= 16) $fatal(1, "Unexpected extra MMD frame");
            if (captured[31:0] !== {2'b01, 2'b01, 5'd3,
                    expected_reg[mmd_frames], 2'b10, expected_data[mmd_frames]})
                $fatal(1, "MMD wire frame %0d mismatch: %h", mmd_frames, captured[31:0]);
            mmd_frames = mmd_frames + 1;
        end
    end
    always @(negedge mdc) begin
        phy_oe = 0;
        if (read_op && respond && count >= 47 && count < 64) begin
            phy_oe = 1;
            if (count == 47) phy_data = 0;
            else phy_data = response[63-count];
        end
    end
    task transaction;
        input reading;
        input answering;
        begin
            @(negedge clk);
            read_op = reading;
            respond = answering;
            count = 0;
            captured = 0;
            start = 1;
            @(negedge clk);
            start = 0;
            wait(done);
            @(posedge clk);
            if (busy || mdc || mdio_oe || count != 64)
                $fatal(1, "Invalid transaction completion");
            if (captured[63:18] !== {32'hffffffff, 2'b01,
                    reading ? 2'b10 : 2'b01, phy_addr, reg_addr})
                $fatal(1, "Incorrect MDIO header %h", captured);
            if (!reading && captured[17:0] !== {2'b10, write_data})
                $fatal(1, "Incorrect MDIO write");
            if (reading && answering && read_data !== response)
                $fatal(1, "Incorrect MDIO read %h", read_data);
            if (error !== (reading && !answering))
                $fatal(1, "Incorrect turnaround error status");
            transactions = transactions + 1;
            @(posedge clk);
            if (done) $fatal(1, "Done not a one-cycle pulse");
        end
    endtask
    task mmd_write;
        input [15:0] address;
        input [15:0] value;
        begin
            reg_addr = 13; write_data = 16'h0002; transaction(0, 1);
            if (captured[22:18] !== 5'd13 || captured[15:0] !== 16'h0002)
                $fatal(1, "MMD address mode not selected");
            reg_addr = 14; write_data = address; transaction(0, 1);
            if (captured[22:18] !== 5'd14 || captured[15:0] !== address)
                $fatal(1, "MMD target address mismatch");
            reg_addr = 13; write_data = 16'h4002; transaction(0, 1);
            if (captured[22:18] !== 5'd13 || captured[15:0] !== 16'h4002)
                $fatal(1, "MMD data mode not selected");
            reg_addr = 14; write_data = value; transaction(0, 1);
            if (captured[22:18] !== 5'd14 || captured[15:0] !== value)
                $fatal(1, "MMD data mismatch");
        end
    endtask
    initial begin
        #30;
        @(negedge clk); rst_n = 1;
        transaction(0, 1);
        transaction(1, 1);
        transaction(1, 0);
        response = 16'h0022;
        transaction(1, 1);
        for (init_index = 0; init_index < 16; init_index = init_index + 4) begin
            expected_reg[init_index] = 13;
            expected_data[init_index] = 16'h0002;
            expected_reg[init_index+1] = 14;
            expected_reg[init_index+2] = 13;
            expected_data[init_index+2] = 16'h4002;
            expected_reg[init_index+3] = 14;
        end
        expected_data[1] = 16'h0004; expected_data[3] = 16'h0077;
        expected_data[5] = 16'h0005; expected_data[7] = 16'h7777;
        expected_data[9] = 16'h0006; expected_data[11] = 16'h7777;
        expected_data[13] = 16'h0008; expected_data[15] = 16'h01ef;
        check_mmd = 1;
        mmd_write(16'h0004, 16'h0077);
        mmd_write(16'h0005, 16'h7777);
        mmd_write(16'h0006, 16'h7777);
        mmd_write(16'h0008, 16'h01ef);
        if (mmd_frames != 16) $fatal(1, "Missing MMD frames");
        $display("PASS: MDIO %0d transactions, including 16 checked MMD wire frames", transactions);
        $finish;
    end
    initial begin
        #1000000;
        $fatal(1, "MDIO test timeout");
    end
endmodule
`default_nettype wire
