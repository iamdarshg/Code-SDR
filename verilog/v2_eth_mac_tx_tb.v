// ============================================================================
// v2_eth_mac_tx_tb - self-checking transmit MAC testbench.
//
//  * reconstructs each frame from the GMII byte stream
//  * checks preamble/SFD/DA/SA/EtherType/payload/FCS byte-for-byte
//  * checks the CRC algorithm against the canonical vector crc32("123456789")
//  * checks the inter-frame gap
//  * measures achieved wire and payload throughput over back-to-back 1500-byte
//    IP packets (the raw-mode operating point)
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_eth_mac_tx_tb;

    localparam [47:0] TEST_DA = 48'h02_00_00_00_00_01;
    localparam [47:0] TEST_SA = 48'h02_00_00_00_00_02;
    localparam integer LEN     = 1500;             // IP packet length (bytes)
    localparam integer FRAMES  = 200;
    localparam [7:0]  ETH_TYPE_HI = 8'h08, ETH_TYPE_LO = 8'h00;

    reg clk = 0, rst_n = 0;
    always #4 clk = ~clk;                          // 125 MHz

    wire        s_ready;
    wire [7:0]  gmii_tx_d;
    wire        gmii_tx_en;

    reg [31:0] consumed = 0;
    reg [31:0] frames_done = 0;

    function [7:0] payload_byte;
        input [31:0] k;
        begin
            payload_byte = (k * 32'd7 + 32'd3) & 32'hFF;
        end
    endfunction

    wire [7:0] s_data  = payload_byte(consumed);
    wire       s_valid = 1'b1;

    v2_eth_mac_tx #(
        .DEFAULT_DST_MAC(TEST_DA),
        .DEFAULT_SRC_MAC(TEST_SA)
    ) dut (
        .clk(clk), .rst_n(rst_n),
        .dst_mac(48'h0), .src_mac(48'h0),
        .s_data(s_data), .s_valid(s_valid), .s_len(LEN[15:0]), .s_ready(s_ready),
        .gmii_tx_d(gmii_tx_d), .gmii_tx_en(gmii_tx_en),
        .packet_count(), .frame_count()
    );

    // Payload byte consumption handshake.
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) consumed <= 0;
        else if (s_valid && s_ready) begin
            if (consumed == LEN - 1) consumed <= 0;
            else consumed <= consumed + 1;
        end
    end

    // ------------------------------------------------------- wire frame capture
    reg [7:0] frame [0:2047];
    integer   bidx = 0;
    integer   gap  = 0;
    reg       seen_frame = 0;
    reg       prev_en = 0;
    integer   errors = 0;
    integer   checked_frames = 0;
    time      first_en = 0, last_en = 0;
    integer   wire_bytes = 0;

    integer j, f;
    reg [31:0] crc_ref;

    `include "verilog/tb_utils.vh"

    task check_frame;
        input integer n;
        integer k;
        reg [31:0] c;
        reg [31:0] inv;
        begin
            if (n !== 26 + LEN) begin
                $display("  FRAME %0d: bad length %0d expected %0d", checked_frames, n, 26+LEN);
                errors = errors + 1;
            end else begin
                for (k = 0; k < 7; k = k + 1)
                    if (frame[k] !== 8'h55) begin
                        $display("  FRAME %0d: preamble[%0d]=%h", checked_frames, k, frame[k]);
                        errors = errors + 1;
                    end
                if (frame[7] !== 8'hD5) begin
                    $display("  FRAME %0d: SFD=%h", checked_frames, frame[7]);
                    errors = errors + 1;
                end
                for (k = 0; k < 6; k = k + 1)
                    if (frame[8+k] !== TEST_DA[47-k*8 -: 8]) begin
                        $display("  FRAME %0d: DA[%0d]=%h", checked_frames, k, frame[8+k]);
                        errors = errors + 1;
                    end
                for (k = 0; k < 6; k = k + 1)
                    if (frame[14+k] !== TEST_SA[47-k*8 -: 8]) begin
                        $display("  FRAME %0d: SA[%0d]=%h", checked_frames, k, frame[14+k]);
                        errors = errors + 1;
                    end
                if (frame[20] !== ETH_TYPE_HI || frame[21] !== ETH_TYPE_LO) begin
                    $display("  FRAME %0d: ethertype=%h%h", checked_frames, frame[20], frame[21]);
                    errors = errors + 1;
                end
                for (k = 0; k < LEN; k = k + 1)
                    if (frame[22+k] !== payload_byte(k)) begin
                        if (errors < 6)
                            $display("  FRAME %0d: payload[%0d]=%h expected %h",
                                     checked_frames, k, frame[22+k], payload_byte(k));
                        errors = errors + 1;
                    end

                c = 32'hFFFFFFFF;
                for (k = 0; k < 14 + LEN; k = k + 1)   // DA..payload (14+LEN bytes)
                    c = crc32_byte(c, frame[8+k]);
                inv = ~c;
                for (k = 0; k < 4; k = k + 1)
                    if (frame[22+LEN+k] !== inv[8*k +: 8]) begin
                        $display("  FRAME %0d: FCS[%0d]=%h expected %h",
                                 checked_frames, k, frame[22+LEN+k], inv[8*k +: 8]);
                        errors = errors + 1;
                    end
            end
            checked_frames = checked_frames + 1;
        end
    endtask

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            bidx <= 0; gap <= 0; seen_frame <= 0; wire_bytes <= 0;
            first_en <= 0; last_en <= 0; prev_en <= 0;
        end else if (gmii_tx_en) begin
            if (!prev_en && seen_frame && gap < 12) begin
                $display("  IFG too short: %0d cycles before frame %0d", gap, checked_frames);
                errors = errors + 1;
            end
            prev_en <= 1;
            gap <= 0;
            if (bidx < 2048) frame[bidx] <= gmii_tx_d;
            bidx <= bidx + 1;
            wire_bytes <= wire_bytes + 1;
            if (first_en == 0) first_en <= $time;
            last_en <= $time;
        end else begin
            prev_en <= 0;
            gap <= gap + 1;
            if (bidx > 0) begin
                check_frame(bidx);
                bidx <= 0;
                seen_frame <= 1;
            end
        end
    end

    // ------------------------------------------------------------ CRC vector test
    initial begin
        reg [31:0] c;
        c = 32'hFFFFFFFF;
        c = crc32_byte(c, 8'h31); c = crc32_byte(c, 8'h32); c = crc32_byte(c, 8'h33);
        c = crc32_byte(c, 8'h34); c = crc32_byte(c, 8'h35); c = crc32_byte(c, 8'h36);
        c = crc32_byte(c, 8'h37); c = crc32_byte(c, 8'h38); c = crc32_byte(c, 8'h39);
        if ((~c) !== 32'hCBF43926) begin
            $display("FATAL: CRC vector wrong: %h expected CBF43926", ~c);
            $fatal(1);
        end
        $display("  CRC-32 canonical vector (\"123456789\") = CBF43926 OK");
    end

    // ---------------------------------------------------------------- main test
    initial begin
        $display("=== v2_eth_mac_tx_tb ===");
        repeat (4) @(posedge clk);
        rst_n = 1;

        // run until FRAMES frames have been checked
        while (checked_frames < FRAMES) @(posedge clk);

        repeat (4) @(posedge clk);

        if (errors !== 0) begin
            $display("FATAL: %0d frame errors", errors);
            $fatal(1);
        end

        if (wire_bytes < 1000) begin
            $display("FATAL: no packets transmitted (wire_bytes=%0d)", wire_bytes);
            $fatal(1);
        end

        $display("  %0d frames verified, %0d wire bytes, 0 errors",
                 checked_frames, wire_bytes);
        $display("  wire throughput   = %.1f Mbps",
                 (wire_bytes * 8.0 * 1000.0) / (last_en - first_en));
        $display("  payload throughput= %.1f Mbps  (LEN=%0d)",
                 ((checked_frames * LEN) * 8.0 * 1000.0) / (last_en - first_en), LEN);
        $display("PASS: v2_eth_mac_tx (framing, FCS, IFG, throughput)");
        $finish;
    end

    initial begin
        #5_000_000;
        $fatal(1, "v2_eth_mac_tx_tb timeout");
    end

endmodule

`default_nettype wire
