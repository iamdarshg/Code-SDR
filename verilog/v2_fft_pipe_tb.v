// ============================================================================
// v2_fft_pipe_tb - impulse and single-tone checks for the pipelined R2SDF FFT.
//
//  * impulse at n=0 -> every bin equals A/2^S where S is the number of scaling
//    stages (i odd), i.e. the flat spectrum an impulse must produce.
//  * tone at bin 8 -> energy concentrated at bins 8 and N-8.
//
// The impulse test exercises every stage and twiddle path, so a flat result
// validates the R2SDF schedule, the delay feedback and the bit-reversed output
// index together.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_fft_pipe_tb;

    localparam integer N    = 64;
    localparam integer LOGN = 6;
    localparam integer DW   = 14;
    localparam real    PI   = 3.14159265358979;

    reg clk = 0, rst_n = 0, in_valid = 0;
    reg signed [DW-1:0] in_re = 0, in_im = 0;
    wire out_valid;
    wire signed [DW-1:0] out_re, out_im;
    wire [LOGN-1:0] out_index;

    always #5 clk = ~clk;

    v2_fft_pipe #(.N(N), .LOGN(LOGN), .DW(DW), .TW(12),
                  .RE_FILE("verilog/tw12_real.mem"),
                  .IM_FILE("verilog/tw12_imag.mem")) dut (
        .clk(clk), .rst_n(rst_n), .in_valid(in_valid),
        .in_re(in_re), .in_im(in_im),
        .out_valid(out_valid), .out_re(out_re), .out_im(out_im),
        .out_index(out_index)
    );

    integer errors = 0, i;
    reg signed [DW-1:0] bins_re [0:N-1];
    reg signed [DW-1:0] bins_im [0:N-1];
    integer captured = 0;
    reg signed [DW-1:0] stream [0:255];   // raw output stream, for order comparison
    integer sc = 0;
    always @(posedge clk) if (rst_n && out_valid && sc < 256) begin stream[sc] = out_re; sc = sc + 1; end
    integer nskip = 0;        // skip the pipeline's first frame (priming transient)

    // throughput monitor: the longest unbroken run of out_valid, i.e. how many
    // consecutive clocks bins are produced. A pipelined FFT must sustain 1 bin
    // per clock, so this must reach N.
    integer run_len = 0, max_run = 0;
    always @(posedge clk) begin
        if (rst_n && out_valid) begin
            run_len = run_len + 1;
            if (run_len > max_run) max_run = run_len;
        end else begin
            run_len = 0;
        end
    end

    always @(posedge clk)
        if (rst_n && out_valid) begin
            if (nskip > 0) nskip = nskip - 1;
            else begin
                bins_re[out_index] = out_re;
                bins_im[out_index] = out_im;
                captured = captured + 1;
            end
        end

    task feed;
        input integer mode;         // 0 = impulse, 1 = tone @ bin 8
        integer n;
        begin
            // reset between frames so the previous frame's tail cannot leak in
            rst_n = 1'b0;
            repeat (4) @(negedge clk);
            rst_n = 1'b1;
            repeat (2) @(negedge clk);
            captured = 0;
            sc = 0;
            nskip = (mode == 1) ? N : 0;   // feed a priming frame for continuous input
            for (n = 0; n < ((mode == 1) ? 2*N : N); n = n + 1) begin
                in_valid = 1'b1;
                if (mode == 0)
                    in_re = (n == 0) ? 14'sd256 : 14'sd0;
                else if (mode == 2)
                    in_re = 14'sd64;                 // DC: isolates the sum path
                else
                    in_re = $rtoi(64.0 * $cos(2.0*PI*8.0*n/N));
                in_im = {DW{1'b0}};
                if (mode == 1 && n < 8) $display("    in[%0d] = %0d", n, in_re);
                @(negedge clk);
            end
            in_valid = 0;
            // drain: pipeline latency is N-1 plus slack
            for (n = 0; n < 2*N; n = n + 1) @(negedge clk);
        end
    endtask

    integer peak, mag, peak2;
    localparam integer NSCALE = (LOGN + 1) / 2;    // i odd stages scale

    initial begin
        $display("=== v2_fft_pipe_tb (N=%0d) ===", N);
        repeat (4) @(negedge clk);
        rst_n = 1;
        repeat (4) @(negedge clk);

        // -------------------------------------------------- impulse: flat
        feed(0);
        if (captured != N) begin
            $display("FAIL: impulse captured %0d of %0d bins", captured, N);
            errors = errors + 1;
        end else begin
            begin : dbg
                integer nz;
                nz = 0;
                for (i = 0; i < N; i = i + 1) if (bins_re[i] != 0) nz = nz + 1;
                $display("  impulse: %0d of %0d bins non-zero", nz, N);
                for (i = 0; i < 16; i = i + 1)
                    $display("    bin %0d = %0d", i, bins_re[i]);
            end
            for (i = 0; i < N; i = i + 1) begin
                if (bins_re[i] < 32 - 2 || bins_re[i] > 32 + 2) begin
                    if (errors < 6)
                        $display("  impulse bin %0d = %0d (expected ~32 = 256>>%0d)",
                                 i, bins_re[i], NSCALE);
                    errors = errors + 1;
                end
                if (bins_im[i] < -3 || bins_im[i] > 3) begin
                    if (errors < 6)
                        $display("  impulse bin %0d imag = %0d", i, bins_im[i]);
                    errors = errors + 1;
                end
            end
            $display("  impulse: all %0d bins flat at ~32, imag ~0 OK", N);
        end

        // ----------------------------------------------------- tone @ bin 8
        feed(1);
        if (captured != N) begin
            $display("FAIL: tone captured %0d bins", captured);
            errors = errors + 1;
        end else begin
            begin : dbg2
                integer nz2;
                nz2 = 0;
                for (i = 0; i < N; i = i + 1) if (bins_re[i] != 0) nz2 = nz2 + 1;
                $display("  tone: %0d of %0d bins non-zero", nz2, N);
                for (i = 0; i < 10; i = i + 1)
                    $display("    tone bin %0d = %0d", i, bins_re[i]);
            end
            peak = 0; mag = -1;
            for (i = 0; i < N; i = i + 1) begin
                integer m;
                m = (bins_re[i] < 0) ? -bins_re[i] : bins_re[i];
                if (m > mag) begin mag = m; peak = i; end
            end
            peak2 = (N - peak) % N;
            if (peak !== 8) begin
                $display("FAIL: tone peak at bin %0d, expected 8 (mag %0d)", peak, mag);
                errors = errors + 1;
            end else begin
                $display("  tone: peak bin %0d mag %0d, mirror bin %0d mag %0d OK",
                         peak, mag, peak2,
                         (bins_re[peak2] < 0) ? -bins_re[peak2] : bins_re[peak2]);
            end
        end

        // -------------------------------------------------------- throughput
        // 100 MSPS path: one bin per clock, continuously. The longest run of
        // out_valid must cover a whole frame.
        if (max_run < N) begin
            $display("FAIL: throughput - longest output run %0d bins, expected %0d (1/clk)",
                     max_run, N);
            errors = errors + 1;
        end else begin
            $display("  throughput: %0d consecutive bins at 1/clock = 100 MSPS OK", max_run);
        end

        // ---------------------------------------------------------- DC check
        feed(2);
        begin : dbg3
            integer nz3;
            nz3 = 0;
            for (i = 0; i < N; i = i + 1) if (bins_re[i] != 0) nz3 = nz3 + 1;
            $display("  DC: %0d of %0d bins non-zero; bin0=%0d bin1=%0d",
                     nz3, N, bins_re[0], bins_re[1]);
            $write("  DC stream 60..76:");
            for (i = 60; i < 77; i = i + 1) $write(" %0d", stream[i]);
            $write("\n");
        end

        if (errors !== 0) begin
            $display("FATAL: %0d errors", errors);
            $fatal(1);
        end
        $display("PASS: v2_fft_pipe (impulse flat, tone at bin 8, 1 sample/clk)");
        $finish;
    end

    initial begin
        #2_000_000;
        $fatal(1, "v2_fft_pipe_tb timeout (captured=%0d)", captured);
    end

endmodule

`default_nettype wire
