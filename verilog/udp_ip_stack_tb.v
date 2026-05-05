`timescale 1ns / 1ps

module udp_ip_stack_tb;

    parameter DATA_WIDTH = 32;
    parameter CLK_PERIOD = 8; // 125 MHz

    reg         clk;
    reg         rst_n;

    reg [DATA_WIDTH-1:0] app_data;
    reg [15:0]           app_len;
    reg                  app_valid;
    wire                 app_ready;

    reg [31:0]           src_ip;
    reg [31:0]           dst_ip;
    reg [15:0]           src_port;
    reg [15:0]           dst_port;

    wire [DATA_WIDTH-1:0] mac_data;
    wire [15:0]           mac_len;
    wire                  mac_valid;
    reg                   mac_ready;
    reg [31:0]            captured_words [0:7];
    integer               captured_count;

    udp_ip_stack dut (
        .clk(clk),
        .rst_n(rst_n),
        .app_data(app_data),
        .app_len(app_len),
        .app_valid(app_valid),
        .app_ready(app_ready),
        .src_ip(src_ip),
        .dst_ip(dst_ip),
        .src_port(src_port),
        .dst_port(dst_port),
        .mac_data(mac_data),
        .mac_len(mac_len),
        .mac_valid(mac_valid),
        .mac_ready(mac_ready)
    );

    initial begin
        clk = 0;
        forever #(CLK_PERIOD/2) clk = ~clk;
    end

    initial begin
        rst_n = 0;
        app_data = 0;
        app_len = 0;
        app_valid = 0;
        mac_ready = 1;
        captured_count = 0;
        src_ip = 32'hC0A80101;    // 192.168.1.1
        dst_ip = 32'hC0A80102;    // 192.168.1.2
        src_port = 16'd12345;
        dst_port = 16'd12346;

        #100;
        rst_n = 1;
        #100;

        $display("Testing UDP/IP Stack...");

        app_data = 32'hDEADBEEF;
        app_len = 16'd4;
        app_valid = 1;

        while (!app_ready) begin
            #CLK_PERIOD;
        end
        @(posedge clk);
        app_valid = 0;

        while (captured_count < 8) begin
            @(posedge clk);
            if (mac_valid && mac_ready) begin
                captured_words[captured_count] = mac_data;
                captured_count = captured_count + 1;
            end
        end

        $display("UDP/IP Stack test completed");

        if (mac_len == 16'd32 && captured_words[2] == 32'h4011B778) begin
            $display("Packet assembly functional, MAC length: %0d bytes", mac_len);
            $display("TEST PASSED");
        end else begin
            $display("Packet assembly failed: mac_len=%0d checksum_word=%08h",
                     mac_len, captured_words[2]);
            $display("TEST FAILED");
        end

        $finish;
    end

endmodule
