// ============================================================================
// v2_cic_decimator_tb - DC gain, rate, and low-frequency tone checks.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_cic_decimator_tb;

    localparam integer IN_W = 10, ST = 3, RATE = 8, OUT_W = 16;

    reg clk = 0, rst_n = 0, in_valid = 0;
    reg signed [IN_W-1:0] din = 0;
    wire out_valid;
    wire signed [OUT_W-1:0] dout;
    wire [31:0] s_in, s_out;
    integer errors = 0;

    always #5 clk = ~clk;

    v2_cic_decimator #(.IN_WIDTH(IN_W), .STAGES(ST), .RATE(RATE), .OUT_WIDTH(OUT_W))
    dut (
        .clk(clk), .rst_n(rst_n), .in_valid(in_valid), .din(din), .rate_cfg(8'd0),
        .out_valid(out_valid), .dout(dout),
        .samples_in(s_in), .samples_out(s_out)
    );

    integer i;
    integer nout;
    integer minv, maxv, lastv;

    // ------------------------------------------------ test 1: DC gain = 1
    initial begin
        $display("=== v2_cic_decimator_tb ===");
        repeat (4) @(negedge clk);
        rst_n = 1;
        repeat (4) @(negedge clk);

        in_valid = 1;
        din = 10'sd100;
        nout = 0; minv = 100000; maxv = -100000;
        for (i = 0; i < 512; i = i + 1) begin
            @(negedge clk);
            if (out_valid) begin
                nout = nout + 1;
                if (nout > 40) begin          // skip the settling transient
                    if (dout < minv) minv = dout;
                    if (dout > maxv) maxv = dout;
                    lastv = dout;
                end
            end
        end
        if (nout == 0) begin
            $display("FAIL: no outputs"); errors = errors + 1;
        end else if (minv < 99 || maxv > 101) begin
            $display("FAIL: DC out of range after settle: min=%0d max=%0d (expected ~100)",
                     minv, maxv);
            errors = errors + 1;
        end else begin
            $display("  DC gain: output = %0d (input 100), settled range [%0d,%0d] OK",
                     lastv, minv, maxv);
        end

        // rate check
        if (s_out * RATE > s_in + RATE || s_out * RATE + RATE < s_in) begin
            $display("FAIL: decimation ratio off: in=%0d out=%0d", s_in, s_out);
            errors = errors + 1;
        end else begin
            $display("  rate: %0d in -> %0d out (ratio %0d) OK", s_in, s_out, RATE);
        end

        // -------------------------------------------- test 2: low-freq tone
        // ~fs/64 tone, amplitude 400; amplitude should survive the CIC passband
        begin
            integer k;
            integer peak;
            peak = 0;
            for (k = 0; k < 4096; k = k + 1) begin
                // simple 4-level approximation of a slow sine (period 64 samples)
                case ((k / 8) % 4)
                    0: din = 10'sd400;
                    1: din = 10'sd0;
                    2: din = -10'sd400;
                    default: din = 10'sd0;
                endcase
                @(negedge clk);
                if (out_valid && k > 512) begin
                    if (dout > peak) peak = dout;
                    if (-dout > peak) peak = -dout;
                end
            end
            if (peak < 200) begin
                $display("FAIL: low-frequency tone attenuated too much (peak=%0d, input 400)", peak);
                errors = errors + 1;
            end else begin
                $display("  tone: low-frequency peak %0d of 400 preserved OK", peak);
            end
        end

        if (errors !== 0) begin
            $display("FATAL: %0d errors", errors);
            $fatal(1);
        end
        $display("PASS: v2_cic_decimator (DC gain, rate, tone)");
        $finish;
    end

    initial begin
        #2_000_000;
        $fatal(1, "v2_cic_decimator_tb timeout");
    end

endmodule

`default_nettype wire
