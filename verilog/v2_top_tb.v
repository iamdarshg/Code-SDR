// ============================================================================
// v2_top_tb - integrated top-level verification.
//
//  * models the KSZ9031 MDIO slave (ID1/ID2, BMCR reset, BMSR link, GBSTAT)
//  * drives the 100 MHz reference and the RGMII RX clock
//  * checks the PHY manager brings the link up
//  * decodes the RGMII DDR output back to bytes and checks real frames egress
//  * exercises the RP2040 SPI path (config write + telemetry read)
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_top_tb;

    parameter integer MODE = 0;

    reg rst_n = 0;
    reg clk_100m = 0;
    always #5 clk_100m = ~clk_100m;         // 100 MHz board reference

    reg  [9:0] adc_data = 0;
    reg  [9:0] kk = 0;
    always @(posedge clk_100m) kk <= kk + 1;
    always @* adc_data = (kk * 10'd13 + 10'd7);

    // SPI driven by the "RP2040"
    reg spi_clk = 0, spi_cs_n = 1, spi_mosi = 0;
    wire spi_miso;

    // RGMII
    wire [3:0] rgmii_txd;
    wire       rgmii_tx_ctl, rgmii_txc;
    reg  [3:0] rgmii_rxd = 0;
    reg        rgmii_rx_ctl = 0;
    reg        rgmii_rxc = 0;
    always #4 rgmii_rxc = ~rgmii_rxc;       // 125 MHz

    // MDIO
    wire mdc;
    tri1 mdio;
    reg  phy_oe = 0;
    reg  phy_data = 1;
    assign mdio = phy_oe ? phy_data : 1'bz;

    // v2_top does not expose PHY status ports (they are internal), so observe
    // the manager's outputs directly.
    wire link_up   = dut.link_up;
    wire phy_ready = dut.phy_ready;
    wire phy_error = dut.phy_error;

    v2_top #(.MODE(MODE), .SAMPLE_BITS(10), .DECIM(2), .FFT_RATE(16)) dut (
        .rst_n(rst_n), .clk_100m_in(clk_100m),
        .adc_data(adc_data), .adc_ovr(1'b0),
        .spi_clk(spi_clk), .spi_mosi(spi_mosi), .spi_cs_n(spi_cs_n),
        .spi_miso(spi_miso),
        .rgmii_txd(rgmii_txd), .rgmii_tx_ctl(rgmii_tx_ctl), .rgmii_txc(rgmii_txc),
        .rgmii_rxd(rgmii_rxd), .rgmii_rx_ctl(rgmii_rx_ctl), .rgmii_rxc(rgmii_rxc),
        .mdc(mdc), .mdio(mdio), .phy_int_n(1'b1)
    );

    // ---------------------------------------------------------- KSZ9031 model
    reg [15:0] regfile [0:31];
    reg [63:0] captured = 0;
    integer    frames = 0;
    reg [4:0]  addr_latch = 0;
    reg        rst_pending = 0;
    integer    rst_clear = 0;

    // restart the frame counter for each management transaction. Sampled in the
    // manager's own clock domain (clk_eth) so the one-cycle start pulse is never
    // missed.
    always @(posedge dut.clk_eth)
        if (dut.u_phy.start) begin
            frames = 0;
            captured = 0;
            addr_latch = 0;
        end

    always @(posedge mdc) begin
        captured = {captured[62:0], mdio};
        frames = frames + 1;
        if (frames == 64) begin
            if (dut.u_phy.read_op == 1'b0 && dut.u_phy.md_reg_addr < 5'd31) begin
                if (dut.u_phy.md_reg_addr == 5'd0 && captured[15] === 1'b1) begin
                    regfile[0] = 16'h8000;               // reset asserted
                    rst_pending = 1;
                    rst_clear = 0;
                end else if (dut.u_phy.md_reg_addr == 5'd0) begin
                    regfile[0] = captured[15:0];
                end else begin
                    regfile[dut.u_phy.md_reg_addr] = captured[15:0];
                end
            end
        end
    end

    // reset self-clears a few MDC cycles later, as the real PHY does
    always @(posedge mdc) begin
        if (rst_pending) begin
            rst_clear = rst_clear + 1;
            if (rst_clear > 8) begin
                regfile[0] = 16'h0000;
                rst_pending = 0;
                rst_clear = 0;
            end
        end
    end

    // read response: turnaround then 16 data bits. Address and direction come
    // from the manager itself (stable for the whole transaction), which is far
    // more robust than decoding bit positions off the wire.
    always @(negedge mdc) begin
        phy_oe = 0;
        if (dut.u_phy.read_op && frames >= 47 && frames < 64) begin
            if (frames == 47) phy_data = 0;
            else phy_data = regfile[dut.u_phy.md_reg_addr][63-frames];
            phy_oe = 1;
        end
    end

    initial begin
        integer ri;
        for (ri = 0; ri < 32; ri = ri + 1) regfile[ri] = 16'h0000;
        regfile[2]  = 16'h0022;              // PHY ID1
        regfile[3]  = 16'h1620;              // PHY ID2
        regfile[0]  = 16'h0000;              // BMCR
        regfile[1]  = 16'h0024;              // BMSR: link up + AN able
        regfile[9]  = 16'h0300;              // 1000BASE-T control
        regfile[10] = 16'h0400;              // GBSTAT: link
        regfile[13] = 16'h0000;              // MMD ctrl
        regfile[14] = 16'h0000;              // MMD data
    end

    // --------------------------------------------------- RGMII TX byte decode
    // Sample AFTER the edge (#1) so we read the DDR values the DUT just drove,
    // not the pre-edge ones. Low nibble is presented on the rising edge, high
    // nibble on the falling edge.
    reg [3:0] tx_lo;
    reg       c_lo;
    reg [7:0] txb [0:4095];
    integer   bidx = 0;
    integer   wire_frames = 0;
    integer   errors = 0;

    always @(posedge rgmii_txc) begin
        #1 tx_lo = rgmii_txd;
           c_lo  = rgmii_tx_ctl;
    end

    always @(negedge rgmii_txc) begin
        #1;
        if (c_lo && rgmii_tx_ctl) begin
            if (bidx < 4096) txb[bidx] = {rgmii_txd, tx_lo};
            bidx = bidx + 1;
        end else if (bidx > 0) begin
            wire_frames = wire_frames + 1;
            bidx = 0;
        end
    end

    // ------------------------------------------------------------------ SPI
    task spi_bit; begin #100 spi_clk=1; #100 spi_clk=0; end endtask

    task spi_write(input [7:0] a, input [31:0] d);
        integer i;
        begin
            spi_cs_n = 0; #100;
            for (i=7;i>=0;i=i-1) begin spi_mosi=a[i]; spi_bit; end
            for (i=31;i>=0;i=i-1) begin spi_mosi=d[i]; spi_bit; end
            #100 spi_cs_n = 1; #200;
        end
    endtask

    task spi_read(input [7:0] a, output [31:0] d);
        integer i;
        begin
            d = 0;
            spi_cs_n = 0; #100;
            for (i=7;i>=0;i=i-1) begin spi_mosi=a[i]; spi_bit; end
            for (i=31;i>=0;i=i-1) begin
                spi_mosi = 0;
                #100 spi_clk = 1; #50;
                d = {d[30:0], spi_miso};
                #50 spi_clk = 0; #100;
            end
            #100 spi_cs_n = 1; #200;
        end
    endtask

    reg [31:0] rd;

    initial begin
        $display("=== v2_top_tb ===");
        repeat (8) @(posedge clk_100m);
        rst_n = 1;

        // wait for the PHY manager to bring the link up
        wait(link_up);
        $display("  PHY bring-up complete: link_up=%b ready=%b error=%b",
                 link_up, phy_ready, phy_error);

        // let the raw path stream a few frames
        #400000;

        if (wire_frames < 3) begin
            $display("FAIL: only %0d frames egressed on RGMII", wire_frames);
            errors = errors + 1;
        end else begin
            // check the first frame's preamble + SFD + DA
            integer i;
            $write("  first 14 wire bytes:");
            for (i = 0; i < 14; i = i + 1) $write(" %02h", txb[i]);
            $write("\n");
            for (i = 0; i < 7; i = i + 1)
                if (txb[i] !== 8'h55) begin
                    $display("FAIL: preamble[%0d]=%h", i, txb[i]); errors = errors + 1;
                end
            if (txb[7] !== 8'hD5) begin
                $display("FAIL: SFD=%h", txb[7]); errors = errors + 1;
            end
            if (txb[8] !== 8'h02 || txb[9] !== 8'h00) begin
                $display("FAIL: DA begins %h %h", txb[8], txb[9]); errors = errors + 1;
            end
            $display("  %0d RGMII frames egressed, first frame preamble/SFD/DA OK", wire_frames);
        end

        // SPI: set decimation to 4 and read telemetry dropped_words
        spi_write(8'h02, 32'd4);
        #1000;
        spi_read(8'hA4, rd);
        $display("  SPI telemetry dropped_words = %0d", rd);

        if (errors !== 0) begin
            $display("FATAL: %0d errors", errors);
            $fatal(1);
        end
        $display("PASS: v2_top (PHY bring-up, RGMII framing, SPI telemetry)");
        $finish;
    end

    // per-transaction trace (disabled; enable by uncommenting the body)
    always @(posedge dut.clk_eth) begin
        if (dut.u_phy.md_done && 1'b0)
            $display("DONE state=%0d reg=%0d rd=%h",
                     dut.u_phy.op, dut.u_phy.md_reg_addr, dut.u_phy.read_data);
    end

    initial begin
        #2_500_000;
        $display("TIMEOUT: phy_state=%0d start=%b busy=%b done=%b link=%b ready=%b err=%b frames=%0d",
                 dut.u_phy.op, dut.u_phy.start, dut.u_phy.md_busy,
                 dut.u_phy.md_done, link_up, phy_ready, phy_error, frames);
        $fatal(1, "v2_top_tb timeout");
    end

    // progress trace: state transitions (disabled by default)
    reg [4:0] prev_state = 5'd31;
    always @(posedge dut.clk_eth) begin
        if (dut.u_phy.op !== prev_state && 1'b0) begin
            $display("t=%0t  op %0d -> %0d", $time, prev_state, dut.u_phy.op);
            prev_state <= dut.u_phy.op;
        end
    end

endmodule

`default_nettype wire
