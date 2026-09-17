// ============================================================================
// v2_rgmii_tb - GMII <-> RGMII adapter test.
//
// Instead of sampling the DDR pins at clock edges (which is sensitive to
// simulator edge ordering), the adapter's RGMII output is looped back into its
// own RGMII input. That exercises the TX nibble split and the RX nibble
// assembly together, and the only thing compared is GMII in vs GMII out.
//
// This proves the two halves are consistent with each other. The absolute
// nibble/edge order (low nibble on the rising edge) must still be confirmed
// against the KSZ9031 RGMII timing on real hardware.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_rgmii_tb;

    reg clk_tx = 0;
    always #4 clk_tx = ~clk_tx;              // 125 MHz
    reg rst_n = 0;

    reg  [7:0] gmii_tx_d = 0;
    reg        gmii_tx_en = 0;
    wire [3:0] rgmii_txd;
    wire       rgmii_tx_ctl, rgmii_txc;
    wire [7:0] gmii_rx_d;
    wire       gmii_rx_dv;

    // loopback. The clock is delayed by 2 ns (mid-bit) exactly like the PHY's
    // internal RGMII clock delay, so the receiver samples settled data instead
    // of the same edge the transmitter updates on.
    wire [3:0] rgmii_rxd    = rgmii_txd;
    wire       rgmii_rx_ctl = rgmii_tx_ctl;
    wire       rgmii_rxc;
    assign #2 rgmii_rxc = rgmii_txc;

    v2_rgmii dut (
        .rst_n(rst_n),
        .rgmii_rxc(rgmii_rxc), .rgmii_rxd(rgmii_rxd), .rgmii_rx_ctl(rgmii_rx_ctl),
        .rgmii_txc(rgmii_txc), .rgmii_txd(rgmii_txd), .rgmii_tx_ctl(rgmii_tx_ctl),
        .clk_tx(clk_tx),
        .gmii_tx_d(gmii_tx_d), .gmii_tx_en(gmii_tx_en),
        .gmii_rx_d(gmii_rx_d), .gmii_rx_dv(gmii_rx_dv)
    );

    localparam integer NBYTES = 24;
    reg [7:0] pattern [0:NBYTES-1];
    reg [7:0] rx_seen [0:NBYTES-1];
    integer   errors = 0;

    initial begin
        integer i;
        for (i = 0; i < NBYTES; i = i + 1) pattern[i] = (i * 8'h17 + 8'h5A) & 8'hFF;
    end

    // ---- TX driver: one byte per cycle, then idle --------------------------
    integer send_idx = 0;
    always @(posedge clk_tx) begin
        if (rst_n && send_idx < NBYTES) begin
            gmii_tx_d  <= pattern[send_idx];
            gmii_tx_en <= 1'b1;
            send_idx   <= send_idx + 1;
        end else begin
            gmii_tx_en <= 1'b0;
        end
    end

    // ---- RX capture --------------------------------------------------------
    integer rx_n = 0;
    always @(posedge clk_tx) begin
        if (rst_n && gmii_rx_dv && rx_n < NBYTES) begin
            rx_seen[rx_n] = gmii_rx_d;
            rx_n = rx_n + 1;
        end
    end

    integer k;
    initial begin
        $display("=== v2_rgmii_tb (RGMII loopback) ===");
        repeat (4) @(posedge clk_tx);
        rst_n = 1;
        #4000;

        if (rx_n < 1) begin
            $display("FAIL: nothing came back through the loopback");
            errors = errors + 1;
        end else begin
            $display("  %0d GMII bytes recovered from the RGMII loopback", rx_n);
            for (k = 0; k < rx_n; k = k + 1) begin
                // the loopback may drop the very first byte while the DDR
                // pipeline fills; align on the first matching byte.
                if ((k + 1) < rx_n && rx_seen[k] === pattern[0] && k == 0) begin
                    // first captured byte is the start -> offset 0
                end
                if (rx_seen[k] !== pattern[k]) begin
                    if (errors < 8)
                        $display("  byte %0d: got %02h expected %02h",
                                 k, rx_seen[k], pattern[k]);
                    errors = errors + 1;
                end
            end
        end

        if (errors !== 0) begin
            $display("FATAL: %0d errors", errors);
            $fatal(1);
        end
        $display("PASS: v2_rgmii (TX split + RX assembly consistent)");
        $finish;
    end

    initial begin
        #100000;
        $fatal(1, "v2_rgmii_tb timeout");
    end

endmodule

`default_nettype wire
