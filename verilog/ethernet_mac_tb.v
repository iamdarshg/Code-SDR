`timescale 1ns / 1ps

module ethernet_mac_tb;

    // DUT parameters
    parameter DATA_WIDTH = 32;
    parameter CLK_PERIOD = 8; // 125 MHz

    // DUT signals
    reg clk;
    reg rst_n;

    // GMII interface
    wire [7:0] gmii_tx_d;
    wire       gmii_tx_en;
    wire       gmii_tx_er;
    reg [7:0]  gmii_rx_d;
    reg        gmii_rx_dv;
    reg        gmii_rx_er;

    // Data interface
    reg [DATA_WIDTH-1:0] packet_data;
    reg [15:0]           packet_len;
    reg                  packet_valid;
    wire                 packet_ack;

    // Status signals
    wire                 link_status;
    wire [31:0]          packet_counter;

    // Instantiate DUT
    ethernet_mac dut (
        .clk(clk),
        .rst_n(rst_n),
        .gmii_tx_d(gmii_tx_d),
        .gmii_tx_en(gmii_tx_en),
        .gmii_tx_er(gmii_tx_er),
        .gmii_rx_d(gmii_rx_d),
        .gmii_rx_dv(gmii_rx_dv),
        .gmii_rx_er(gmii_rx_er),
        .packet_data(packet_data),
        .packet_len(packet_len),
        .packet_valid(packet_valid),
        .packet_ack(packet_ack),
        .link_status(link_status),
        .packet_counter(packet_counter)
    );

    // Clock generation (125 MHz GMII clock)
    initial begin
        clk = 0;
        forever #(CLK_PERIOD/2) clk = ~clk;
    end

    // Test sequence
    initial begin
        // Initialize signals
        rst_n = 0;
        packet_data = 0;
        packet_len = 0;
        packet_valid = 0;
        gmii_rx_d = 0;
        gmii_rx_dv = 0;
        gmii_rx_er = 0;

        // Reset
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        $display("Testing Ethernet MAC...");

        // Test basic packet transmission
        packet_data = 32'hDEADBEEF;
        packet_len = 16'd4;  // Simple test packet
        packet_valid = 1;

        // Wait for packet acknowledgment
        wait(packet_ack);
        packet_valid = 0;

        $display("Packet sent, waiting for transmission...");

        // Wait for transmission to complete (monitor TX enable)
        #10000; // Long enough for full packet transmission

        $display("Packet counter: %0d", packet_counter);

        // Test link status
        gmii_rx_dv = 1;
        gmii_rx_d = 8'h55;  // Simulate incoming preamble
        #CLK_PERIOD;
        gmii_rx_dv = 0;

        $display("Link status: %b", link_status);

        #1000;

        $display("Ethernet MAC test completed");

        // Basic checks
        if (packet_counter > 0) begin
            $display("✓ Packet transmission detected");
        end else begin
            $display("⚠ No packets transmitted");
        end

        $finish;
    end

    // Monitor TX activity
    always @(posedge clk) begin
        if (gmii_tx_en) begin
            $display("Time=%0t: TX Data=0x%h", $time, gmii_tx_d);
        end
    end

endmodule
