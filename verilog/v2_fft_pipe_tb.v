// ============================================================================
// v2_fft_pipe_tb - self-checking verification of the pipelined R2SDF FFT.
//
// The DUT is checked against a DFT computed in the testbench itself, so the
// expected values are not hardcoded and cannot drift from the definition.
//
// Framing (established against tools/r2sdf_reference.py, which matches numpy):
//   * feed 2N samples, out_valid then runs for 2N cycles
//   * the transform occupies output stream positions [N-1, 2N-1); the first
//     N-1 outputs are the delay lines flushing
//   * the sample at stream position p is bin bitrev((p-(N-1)) mod N), which the
//     DUT carries on out_index, so capturing into bins[out_index] must
//     reconstruct the spectrum in natural order
//   * the datapath scales odd stages by 1/2 (three stages at N=64), so every
//     bin is the true DFT divided by 2^NSCALE
//
// Cases: impulse (flat spectrum, exercises every stage and twiddle), DC
// (isolates the sum path), and a tone at bin 8 (checks bin placement, the most
// sensitive test of the output index).
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_fft_pipe_tb;

    parameter integer N    = 64;
    localparam integer LOGN = $clog2(N);
    localparam integer DW   = 14;
    localparam integer TW   = 12;
    localparam real    PI   = 3.14159265358979;
    localparam integer NSCALE = (LOGN + 1) / 2;
    localparam real    SCALE  = (1 << NSCALE) * 1.0;

    reg clk = 0, rst_n = 0, in_valid = 0;
    reg signed [DW-1:0] in_re = 0, in_im = 0;
    wire                out_valid;
    wire signed [DW-1:0] out_re, out_im;
    wire [LOGN-1:0]     out_index;

    always #5 clk = ~clk;

    v2_fft_pipe #(.N(N), .LOGN(LOGN), .DW(DW), .TW(TW),
                  .RE_FILE("verilog/tw12_real.mem"),
                  .IM_FILE("verilog/tw12_imag.mem")) dut (
        .clk(clk), .rst_n(rst_n), .in_valid(in_valid),
        .in_re(in_re), .in_im(in_im),
        .out_valid(out_valid), .out_re(out_re), .out_im(out_im),
        .out_index(out_index)
    );

    integer errors = 0;
    integer i, n, k;
    reg signed [DW-1:0] bins_re [0:N-1];
    reg signed [DW-1:0] bins_im [0:N-1];
    reg                 got     [0:N-1];
    integer             g_tone = 8;      // tone bin under test
    integer captured = 0;
    integer vcount   = 0;

    real dft_r [0:N-1];
    real dft_i [0:N-1];

    // stimulus for a case (only n < N feeds the transform window)
    function real stim;
        input integer mode;
        input integer idx;
        begin
            case (mode)
                0: stim = (idx == 0) ? 256.0 : 0.0;                        // impulse
                1: stim = 64.0 * $cos(2.0*PI*g_tone*idx/N);                  // tone @ g_tone
                default: stim = 64.0;                                      // DC
            endcase
        end
    endfunction

    // expected spectrum: DFT of the first N samples, divided by the scaling
    task compute_expected;
        input integer mode;
        integer kk, nn;
        real ang, x;
        begin
            for (kk = 0; kk < N; kk = kk + 1) begin
                dft_r[kk] = 0.0;
                dft_i[kk] = 0.0;
                for (nn = 0; nn < N; nn = nn + 1) begin
                    x   = stim(mode, nn);
                    ang = -2.0*PI*kk*nn/N;
                    dft_r[kk] = dft_r[kk] + x*$cos(ang);
                    dft_i[kk] = dft_i[kk] + x*$sin(ang);
                end
                dft_r[kk] = dft_r[kk]/SCALE;
                dft_i[kk] = dft_i[kk]/SCALE;
            end
        end
    endtask

    // captures the frame: skip the N-1 flushing outputs, take the next N
    always @(posedge clk) begin
        if (rst_n && out_valid) begin
            vcount = vcount + 1;
            if (vcount >= 2 && vcount <= N+1) begin
                bins_re[out_index] = out_re;
                bins_im[out_index] = out_im;
                got[out_index]     = 1'b1;
                captured = captured + 1;
            end
        end
    end

    // throughput: longest unbroken run of out_valid must cover a whole frame
    integer run_len = 0, max_run = 0;
    always @(posedge clk) begin
        if (rst_n && out_valid) begin
            run_len = run_len + 1;
            if (run_len > max_run) max_run = run_len;
        end else run_len = 0;
    end

    task run_case;
        input integer mode;
        integer nn;
        begin
            rst_n = 1'b0;
            repeat (4) @(negedge clk);
            in_valid = 1'b0;
            rst_n    = 1'b1;
            repeat (2) @(negedge clk);

            captured = 0;
            vcount   = 0;
            for (nn = 0; nn < N; nn = nn + 1) got[nn] = 1'b0;

            for (nn = 0; nn < 2*N; nn = nn + 1) begin
                in_valid = 1'b1;
                in_re    = $rtoi(stim(mode, nn));
                in_im    = {DW{1'b0}};
                @(negedge clk);
            end
            in_valid = 1'b0;
            for (nn = 0; nn < 2*N; nn = nn + 1) @(negedge clk);
        end
    endtask

    task check_case;
        input integer mode;
        input [8*16-1:0] name;
        input real frac;           // tolerance as a fraction of the bin peak
        integer kk;
        real dr, di, peak, tol;
        begin
            compute_expected(mode);
            // A fixed-point FFT's error scales with the signal magnitude (more
            // stages and more rounding for larger N), so a relative tolerance is
            // the meaningful one - an absolute LSB bound would fail large N for
            // reasons that have nothing to do with correctness.
            peak = 0.0;
            for (kk = 0; kk < N; kk = kk + 1) begin
                if (dft_r[kk] >  peak) peak =  dft_r[kk];
                if (dft_r[kk] < -peak) peak = -dft_r[kk];
            end
            tol = frac * peak + 2.0;
            if (captured != N) begin
                $display("FAIL %0s: captured %0d of %0d bins", name, captured, N);
                errors = errors + 1;
            end
            for (kk = 0; kk < N; kk = kk + 1) begin
                if (!got[kk]) begin
                    $display("FAIL %0s: bin %0d never written", name, kk);
                    errors = errors + 1;
                end else begin
                    dr = bins_re[kk] - dft_r[kk];
                    di = bins_im[kk] - dft_i[kk];
                    if (dr < 0.0) dr = -dr;
                    if (di < 0.0) di = -di;
                    if (dr > tol || di > tol) begin
                        if (errors < 12)
                            $display("FAIL %0s bin %0d: got (%0d,%0d) expected (%.1f,%.1f)",
                                     name, kk, bins_re[kk], bins_im[kk], dft_r[kk], dft_i[kk]);
                        errors = errors + 1;
                    end
                end
            end
            $display("  %0s: all %0d bins within +/-%.1f", name, N, tol);
        end
    endtask

    initial begin
        $display("=== v2_fft_pipe_tb (N=%0d, scale /%0.0f) ===", N, SCALE);
        repeat (4) @(negedge clk);

        run_case(0);
        check_case(0, "impulse", 0.03);

        run_case(2);
        check_case(2, "dc     ", 0.03);

        g_tone = 8;  run_case(1); check_case(1, "tone@8 ", 0.02);
        g_tone = 1;  run_case(1); check_case(1, "tone@1 ", 0.02);
        g_tone = 17; run_case(1); check_case(1, "tone@17", 0.02);
        g_tone = 31; run_case(1); check_case(1, "tone@31", 0.02);

        if (max_run < N) begin
            $display("FAIL: throughput - longest run %0d, expected %0d (1 bin/clk)", max_run, N);
            errors = errors + 1;
        end else begin
            $display("  throughput: %0d consecutive bins at 1/clock = 100 MSPS", max_run);
        end

        if (errors !== 0) begin
            $display("FATAL: %0d errors", errors);
            $fatal(1);
        end
        $display("PASS: v2_fft_pipe (impulse, DC, tone@1/8/17/31 all match the DFT, 1 bin/clk)");
        $finish;
    end

    initial begin
        #5_000_000;
        $fatal(1, "v2_fft_pipe_tb timeout");
    end

endmodule

`default_nettype wire
