// ============================================================================
// v2_fft1024_tb - impulse and single-tone checks for the 1024-point FFT.
//
//  * impulse at n=0  -> every bin equals the impulse amplitude, imag ~ 0
//  * tone at bin 64  -> energy concentrated at bins 64 and 960 (=1024-64)
//
// The transform is unscaled (no 1/N), so an impulse of amplitude A gives A in
// every bin, and a tone of amplitude A gives A*N/2 in its two bins.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_fft1024_tb;

    parameter integer N = 1024;
    parameter integer LOGN = 10;
    localparam integer W = 16;
    localparam real    PI = 3.14159265358979;

    reg clk = 0, rst_n = 0;
    always #5 clk = ~clk;                 // 100 MHz

    reg                start = 0;
    reg  signed [W-1:0] in_re = 0, in_im = 0;
    reg                in_valid = 0;
    wire               in_ready;
    wire signed [W-1:0] out_re, out_im;
    wire [LOGN-1:0]     out_index;
    wire               out_valid, busy, frame_done;
    wire [31:0]        frame_count, overflow_count;
    wire [4:0]         scale_exp;
    wire               overflow;

    v2_fft1024 #(.N(N), .LOGN(LOGN), .W(W),
                 .TWID_FILE("verilog/twiddle_real.mem"),
                 .TWID_FILEI("verilog/twiddle_imag.mem"), .WINDOW_EN(0)) dut (
        .clk(clk), .rst_n(rst_n),
        .start(start), .in_re(in_re), .in_im(in_im), .in_valid(in_valid),
        .in_ready(in_ready),
        .out_re(out_re), .out_im(out_im), .out_index(out_index),
        .out_valid(out_valid), .busy(busy), .frame_done(frame_done),
        .frame_count(frame_count), .overflow_count(overflow_count),
        .scale_exp(scale_exp), .overflow(overflow)
    );

    integer errors = 0;
    integer i, k;

    reg signed [W-1:0] bin_re [0:N-1];
    reg signed [W-1:0] bin_im [0:N-1];
    integer captured;

    // --------------------------------------------------------------- capture
    always @(posedge clk) begin
        if (rst_n && out_valid) begin
            bin_re[out_index] = out_re;
            bin_im[out_index] = out_im;
            captured = captured + 1;
        end
    end

    task run_frame;
        input integer mode;          // 0 = impulse, 1 = tone at bin 64
        integer n;
        begin
            captured = 0;
            @(negedge clk); start = 1;
            @(negedge clk); start = 0;
            // wait until the loader is actually accepting, then present exactly
            // N samples (one per clock) and stop - no ready check after the last
            // sample, which would deadlock once the loader closes.
            while (dut.state !== 3'd1) @(negedge clk);
            in_valid = 1;
            for (n = 0; n < N; n = n + 1) begin
                if (mode == 0) begin
                    in_re = (n == 0) ? 16'sd1000 : 16'sd0;
                end else if (mode == 2) begin
                    // near full-scale 10-bit tone: pushes the BFP scaler
                    in_re = $rtoi(500.0 * $cos(2.0*PI*64.0*n/N));
                end else begin
                    in_re = $rtoi(32.0 * $cos(2.0*PI*64.0*n/N));
                end
                in_im = 16'sd0;
                @(negedge clk);
            end
            in_valid = 0;
            $display("  [rf] mode=%0d presented %0d samples", mode, N);
            @(posedge frame_done);
            @(negedge clk);
        end
    endtask

    integer peak_idx, mag, peak2;

    initial begin
        $display("=== v2_fft1024_tb ===");
        repeat (4) @(negedge clk);
        rst_n = 1;
        repeat (4) @(negedge clk);

        // ------------------------------------------------ impulse: flat bins
        run_frame(0);
        if (captured != N) begin
            $display("FAIL: impulse captured %0d bins, expected %0d", captured, N);
            errors = errors + 1;
        end else begin
            for (i = 0; i < N; i = i + 1) begin
                if (bin_re[i] < 999 || bin_re[i] > 1001) begin
                    if (errors < 6)
                        $display("  impulse bin %0d = %0d (expected ~1000)", i, bin_re[i]);
                    errors = errors + 1;
                end
                if (bin_im[i] < -1 || bin_im[i] > 1) begin
                    if (errors < 6)
                        $display("  impulse bin %0d imag = %0d (expected 0)", i, bin_im[i]);
                    errors = errors + 1;
                end
            end
            $display("  impulse: all %0d bins = 1000, imag 0 OK", N);
        end

        // ------------------------------------------------------- tone at bin 64
        run_frame(1);
        if (captured != N) begin
            $display("FAIL: tone captured %0d bins", captured);
            errors = errors + 1;
        end else begin
            // find the strongest bin
            peak_idx = 0; mag = -1;
            for (i = 0; i < N; i = i + 1) begin
                integer m;
                m = (bin_re[i] < 0 ? -bin_re[i] : bin_re[i]);
                if (m > mag) begin mag = m; peak_idx = i; end
            end
            peak2 = (N - peak_idx) % N;
            if (peak_idx !== 64) begin
                $display("FAIL: tone peak at bin %0d, expected 64 (mag %0d)", peak_idx, mag);
                errors = errors + 1;
            end else begin
                $display("  tone: peak bin %0d mag %0d (expected ~%0d) OK",
                         peak_idx, mag, 32*N/2);
                $display("  tone: conjugate bin %0d mag %0d", peak2,
                         (bin_re[peak2] < 0 ? -bin_re[peak2] : bin_re[peak2]));
            end
        end

        // ------------------------------- full-scale tone: BFP scaling, no clip
        run_frame(2);
        peak_idx = 0; mag = -1;
        for (i = 0; i < N; i = i + 1) begin
            integer m3;
            m3 = (bin_re[i] < 0 ? -bin_re[i] : bin_re[i]);
            if (m3 > mag) begin mag = m3; peak_idx = i; end
        end
        if (peak_idx !== 64) begin
            $display("FAIL: full-scale tone peak at bin %0d, expected 64", peak_idx);
            errors = errors + 1;
        end
        if (overflow !== 1'b0) begin
            $display("FAIL: full-scale tone reported overflow (guard too small)");
            errors = errors + 1;
        end
        $display("  full-scale tone (amp 500): peak bin %0d mag %0d scale_exp=%0d overflow=%b OK",
                 peak_idx, mag, scale_exp, overflow);

        if (errors !== 0) begin
            $display("FATAL: %0d errors", errors);
            $fatal(1);
        end
        $display("  throughput: frame = %0.2f us -> %0.2f MSPS sustained input",
                 frame_ns / 1000.0, 1024.0 / (frame_ns / 1000.0));
        $display("PASS: v2_fft1024 (impulse flat, tone at bin 64, BFP full-scale)");
        $finish;
    end

    integer dbg = 0;
    // frame-rate measurement: the memory-based FFT's real throughput limit
    time    last_done = 0;
    real    frame_ns;
    integer frames_seen = 0;
    always @(posedge clk) begin
        if (rst_n && frame_done) begin
            frames_seen = frames_seen + 1;
            if (frames_seen == 2) frame_ns = ($time - last_done) / 1000.0;
            last_done = $time;
        end
    end

    always @(posedge clk) begin
        if (rst_n) begin
            dbg = dbg + 1;
            if (dbg < 25 || (dbg % 20000) == 0)
                $display("t=%0t st=%0d sub=%0d stage=%0d k=%0d j=%0d load=%0d out=%0d cap=%0d rdy=%b vld=%b",
                         $time, dut.state, dut.sub, dut.stage, dut.k, dut.j,
                         dut.load_cnt, dut.out_cnt, captured, in_ready, out_valid);
        end
    end

    initial begin
        #2_000_000;
        $fatal(1, "v2_fft1024_tb timeout");
    end

endmodule

`default_nettype wire
