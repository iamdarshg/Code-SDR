`timescale 1ns / 1ps

module udp_ip_stack_tb;

    // DUT parameters
    parameter DATA_WIDTH = 32;
    parameter CLK_PERIOD = 8; // 125 MHz

    // DUT signals
    reg         clk;
    reg         rst_n;

    // Application data interface
    reg [DATA_WIDTH-1:0] app_data;
    reg [15:0]           app_len;
    reg                  app_valid;
    wire                 app_ready;

    // UDP parameters
    reg [31:0]           src_ip;
    reg [31:0]           dst_ip;
    reg [15:0]           src_port;
    reg [15:0]           dst_port;

    // MAC interface
    wire [DATA_WIDTH-1:0] mac_data;
    wire [15:0]           mac_len;
    wire                  mac_valid;

    // Instantiate DUT
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
        .mac_valid(mac_valid)
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
        app_data = 0;
        app_len = 0;
        app_valid = 0;
        src_ip = 32'hC0A80101;    // 192.168.1.1
        dst_ip = 32'hC0A80102;    // 192.168.1.2
        src_port = 16'd12345;
        dst_port = 16'd12346;

        // Reset
        #100;
        rst_n = 1;

        // Wait for stabilization
        #100;

        $display("Testing UDP/IP Stack...");

        // Test packet transmission
        app_data = 32'hDEADBEEF;
        app_len = 16'd4;  // Simple payload
        app_valid = 1;

        // Wait for ready and send
        while (!app_ready) #CLK_PERIOD;
        app_valid = 0;

        $display("Packet data sent, waiting for MAC layer processing...");

        // Wait for transmission complete
        #1000;

        $display("UDP/IP Stack test completed");

        if (mac_len > 0) begin
            $display("✓ Packet assembly functional, MAC length: %0d bytes", mac_len);
        end else begin
            $display("⚠ No MAC output generated");
        end

        $finish;
    end

endmodule
