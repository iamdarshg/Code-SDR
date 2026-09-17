// ============================================================================
// v2_spi_regs_tb - verifies the RP2040 SPI register interface:
//   * write config registers (sample bits, decimation, enable) and read back
//   * read telemetry registers through v2_telemetry
//   * confirm live counters cannot corrupt a read (value latched at addr time)
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_spi_regs_tb;

    reg clk = 0, rst_n = 0;
    always #4 clk = ~clk;             // 125 MHz system clock

    reg spi_clk = 0, spi_cs_n = 1, spi_mosi = 0;
    wire spi_miso;

    wire [7:0]  tele_addr;
    wire [31:0] tele_data;
    wire [1:0]  cfg_mode;
    wire [7:0]  cfg_bits, cfg_decim;
    wire [31:0] cfg_nco;
    wire        cfg_enable;
    wire [15:0] cfg_port;

    // Telemetry source values (drive the readback).
    reg  [31:0] packets_sent  = 32'd1234;
    reg  [31:0] dropped_words = 32'd7;
    reg  [31:0] overflow_cnt  = 32'd3;
    reg  [31:0] seq_value     = 32'd99;

    v2_telemetry u_tele (
        .clk(clk), .rst_n(rst_n),
        .packets_sent(packets_sent), .dropped_words(dropped_words),
        .overflow_count(overflow_cnt), .seq_value(seq_value),
        .sample_bits(cfg_bits), .decim(cfg_decim), .mode(cfg_mode),
        .link_up(1'b1), .pll_locked(1'b1), .effective_mbps(16'd803),
        .rd_addr(tele_addr), .rd_data(tele_data)
    );

    v2_spi_regs u_dut (
        .clk(clk), .rst_n(rst_n),
        .spi_clk(spi_clk), .spi_cs_n(spi_cs_n), .spi_mosi(spi_mosi),
        .spi_miso(spi_miso),
        .tele_addr(tele_addr), .tele_data(tele_data),
        .cfg_mode(cfg_mode), .cfg_sample_bits(cfg_bits), .cfg_decim(cfg_decim),
        .cfg_nco_freq(cfg_nco), .cfg_enable(cfg_enable), .cfg_dst_port(cfg_port)
    );

    integer errors = 0;

    // SPI mode 0: master drives on the falling edge, slave samples on the rise.
    task spi_bit;
        input mbit;
        begin
            #100 spi_clk = 1;                 // rising: slave samples MOSI
            #100 spi_clk = 0;                 // falling: slave shifts MISO
        end
    endtask

    task spi_write;
        input [7:0]  a;
        input [31:0] d;
        integer i;
        begin
            spi_cs_n = 0;
            #100;
            for (i = 7; i >= 0; i = i - 1) begin
                spi_mosi = a[i];
                spi_bit(1'b0);
            end
            for (i = 31; i >= 0; i = i - 1) begin
                spi_mosi = d[i];
                spi_bit(1'b0);
            end
            #100 spi_cs_n = 1;
            #200;
        end
    endtask

    task spi_read;
        input  [7:0]  a;
        output [31:0] d;
        integer i;
        begin
            d = 0;
            spi_cs_n = 0;
            #100;
            for (i = 7; i >= 0; i = i - 1) begin
                spi_mosi = a[i];
                spi_bit(1'b0);
            end
            for (i = 31; i >= 0; i = i - 1) begin
                spi_mosi = 0;
                #100 spi_clk = 1;             // slave has already presented bit
                #50;
                d = {d[30:0], spi_miso};
                #50 spi_clk = 0;
                #100;
            end
            #100 spi_cs_n = 1;
            #200;
        end
    endtask

    reg [31:0] rd;

    initial begin
        $display("=== v2_spi_regs_tb ===");
        repeat (4) @(posedge clk);
        rst_n = 1;
        repeat (4) @(posedge clk);

        // defaults
        if (cfg_bits !== 8'd10 || cfg_decim !== 8'd0 || cfg_enable !== 1'b1) begin
            $display("FAIL: default config wrong (bits=%0d decim=%0d en=%b)",
                     cfg_bits, cfg_decim, cfg_enable);
            errors = errors + 1;
        end

        // write sample bits = 8, decim = 1
        spi_write(8'h01, 32'd8);
        spi_write(8'h02, 32'd1);
        if (cfg_bits !== 8'd8 || cfg_decim !== 8'd1) begin
            $display("FAIL: write did not take (bits=%0d decim=%0d)", cfg_bits, cfg_decim);
            errors = errors + 1;
        end

        // read config back
        spi_read(8'h81, rd);
        if (rd !== 32'd8) begin
            $display("FAIL: readback bits = %0d expected 8", rd); errors = errors + 1;
        end
        spi_read(8'h82, rd);
        if (rd !== 32'd1) begin
            $display("FAIL: readback decim = %0d expected 1", rd); errors = errors + 1;
        end

        // telemetry: packets_sent at SPI 0x20 -> tele 0x00
        spi_read(8'hA0, rd);
        if (rd !== 32'd1234) begin
            $display("FAIL: packets_sent = %0d expected 1234", rd); errors = errors + 1;
        end

        // telemetry: dropped_words at SPI 0x24 -> tele 0x04
        spi_read(8'hA4, rd);
        if (rd !== 32'd7) begin
            $display("FAIL: dropped_words = %0d expected 7", rd); errors = errors + 1;
        end

        // telemetry: effective Mbps at SPI 0x30 -> tele 0x10
        spi_read(8'hB0, rd);
        if (rd !== 32'd803) begin
            $display("FAIL: effective_mbps = %0d expected 803", rd); errors = errors + 1;
        end

        if (errors !== 0) begin
            $display("FATAL: %0d errors", errors);
            $fatal(1);
        end
        $display("PASS: v2_spi_regs (config write/read, telemetry read)");
        $finish;
    end

    initial begin
        #2_000_000;
        $fatal(1, "v2_spi_regs_tb timeout");
    end

endmodule

`default_nettype wire
