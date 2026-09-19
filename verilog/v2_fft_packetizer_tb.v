// ============================================================================
// v2_fft_packetizer_tb - stages 256 bins, then checks the emitted payload:
// header fields, per-bin byte order, and packet count.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_fft_packetizer_tb;

    localparam integer BINS = 256, HDR = 16, W = 16;
    localparam integer PKT  = HDR + BINS*4;

    reg clk = 0, rst_n = 0;
    always #5 clk = ~clk;

    reg signed [W-1:0] fft_re = 0, fft_im = 0;
    reg                fft_valid = 0;
    reg  [9:0]         fft_index = 0;
    reg  [31:0]        fft_frame = 0;
    reg  [4:0]         fft_scale_exp = 0;
    reg                fft_overflow = 0;
    reg                upstream_busy = 0;

    wire [7:0]  p_data;
    wire        p_valid, send;
    wire [15:0] payload_len;
    wire [31:0] packet_seq;
    wire [31:0] packets_sent, bins_dropped;
    reg         p_ready;

    v2_fft_packetizer #(.BINS_PER_PKT(BINS), .HEADER_BYTES(HDR), .W(W)) dut (
        .clk(clk), .rst_n(rst_n),
        .fft_re(fft_re), .fft_im(fft_im), .fft_valid(fft_valid),
        .fft_index(fft_index), .fft_frame(fft_frame),
        .fft_scale_exp(fft_scale_exp), .fft_overflow(fft_overflow),
        .upstream_busy(upstream_busy),
        .p_data(p_data), .p_valid(p_valid), .p_ready(p_ready),
        .send(send), .payload_len(payload_len), .packet_seq(packet_seq),
        .packets_sent(packets_sent), .bins_dropped(bins_dropped)
    );

    integer errors = 0, i;
    reg [7:0] cap [0:PKT-1];
    integer   n = 0;

    always @(posedge clk) begin
        if (rst_n && p_valid && p_ready && n < PKT) begin
            cap[n] = p_data;
            n = n + 1;
        end
    end

    function [15:0] exp_re; input integer b; begin exp_re = 16'h1000 + b[15:0]; end endfunction
    function [15:0] exp_im; input integer b; begin exp_im = 16'h2000 + b[15:0]; end endfunction

    initial begin
        $display("=== v2_fft_packetizer_tb ===");
        repeat (4) @(negedge clk);
        rst_n = 1;
        repeat (4) @(negedge clk);

        // stage 256 bins
        fft_frame = 32'd7;
        fft_scale_exp = 5'd4;
        for (i = 0; i < BINS; i = i + 1) begin
            fft_valid = 1;
            fft_index = i[9:0];
            fft_re = exp_re(i);
            fft_im = exp_im(i);
            @(negedge clk);
        end
        fft_valid = 0;
        // wait (bounded) for the staged packet to be offered to the UDP layer
        i = 0;
        while (!send && i < 20) begin @(negedge clk); i = i + 1; end
        if (!send) begin
            $display("FAIL: packet not staged after %0d bins", BINS);
            errors = errors + 1;
        end

        // let the packet stream out
        p_ready = 1;
        while (packets_sent == 0) @(negedge clk);
        repeat (4) @(negedge clk);
        p_ready = 0;

        if (n != PKT) begin
            $display("FAIL: captured %0d bytes, expected %0d", n, PKT);
            errors = errors + 1;
        end else begin
            // header
            if ({cap[0],cap[1],cap[2],cap[3]} !== 32'd0) begin
                $display("FAIL: seq = %h", {cap[0],cap[1],cap[2],cap[3]}); errors = errors + 1;
            end
            if ({cap[4],cap[5],cap[6],cap[7]} !== 32'd7) begin
                $display("FAIL: frame = %h", {cap[4],cap[5],cap[6],cap[7]}); errors = errors + 1;
            end
            if ({cap[8],cap[9]} !== 16'd0) begin
                $display("FAIL: first_bin = %h", {cap[8],cap[9]}); errors = errors + 1;
            end
            if (cap[10] !== 8'd4) begin
                $display("FAIL: scale_exp = %0d", cap[10]); errors = errors + 1;
            end
            // flags byte: bit0 = overflow, bit1 = FFT-present. This used to be
            // emitted the other way round, so the host (which reads bit0 as
            // overflow) flagged every packet as overflowed. Asserting the whole
            // byte against the documented layout catches a swap either way.
            if (cap[11] !== {6'b0, 1'b1, fft_overflow}) begin
                $display("FAIL: flags = %h (expected bit0=overflow, bit1=present)",
                         cap[11]);
                errors = errors + 1;
            end
            // bins
            for (i = 0; i < BINS; i = i + 1) begin
                if ({cap[HDR+i*4], cap[HDR+i*4+1]} !== exp_re(i) ||
                    {cap[HDR+i*4+2], cap[HDR+i*4+3]} !== exp_im(i)) begin
                    if (errors < 6)
                        $display("  bin %0d: got %h %h expected %h %h", i,
                                 {cap[HDR+i*4],cap[HDR+i*4+1]},
                                 {cap[HDR+i*4+2],cap[HDR+i*4+3]},
                                 exp_re(i), exp_im(i));
                    errors = errors + 1;
                end
            end
            $display("  packet: %0d bytes, header + %0d bins verified", PKT, BINS);
        end

        if (bins_dropped !== 0) begin
            $display("FAIL: bins dropped = %0d", bins_dropped); errors = errors + 1;
        end
        if (errors !== 0) begin
            $display("FATAL: %0d errors", errors);
            $fatal(1);
        end
        $display("PASS: v2_fft_packetizer (header, bins, byte order)");
        $finish;
    end

    initial begin
        #2_000_000;
        $fatal(1, "v2_fft_packetizer_tb timeout");
    end

endmodule

`default_nettype wire
