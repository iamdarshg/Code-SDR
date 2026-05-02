// ============================================================================
// FPGA Processing Pipeline Testbench
// ============================================================================
// Comprehensive testbench for SDR FPGA processing pipeline validation
// ============================================================================

`timescale 1ns/1ps

module fpga_processing_pipeline_tb;

    // ========================================================================
    // Testbench parameters
    // ========================================================================
    
    parameter CLK_PERIOD_100M = 10.0;
    
    // ========================================================================
    // Testbench signals
    // ========================================================================
    
    reg clk_100m;
    reg rst_n;
    
    reg [9:0] adc_data;
    reg adc_valid;
    reg adc_ovr;
    
    reg spi_clk;
    reg spi_mosi;
    reg spi_cs_n;
    wire spi_miso;
    
    wire [7:0] gmii_tx_d;
    wire gmii_tx_en;
    wire gmii_tx_er;
    reg [7:0] gmii_rx_d;
    reg gmii_rx_dv;
    reg gmii_rx_er;
    
    wire pll_locked;
    wire eth_link_status;
    wire [15:0] system_status;
    wire [31:0] packet_counter;
    
    // ========================================================================
    // Clock generation
    // ========================================================================
    
    initial begin
        clk_100m = 0;
        forever #(CLK_PERIOD_100M/2) clk_100m = ~clk_100m;
    end
    
    // ========================================================================
    // DUT instantiation
    // ========================================================================
    
    fpga_processing_pipeline dut (
        .clk_100m_in(clk_100m),
        .rst_n(rst_n),
        .adc_data(adc_data),
        .adc_valid(adc_valid),
        .adc_ovr(adc_ovr),
        .spi_clk(spi_clk),
        .spi_mosi(spi_mosi),
        .spi_cs_n(spi_cs_n),
        .spi_miso(spi_miso),
        .gmii_tx_d(gmii_tx_d),
        .gmii_tx_en(gmii_tx_en),
        .gmii_tx_er(gmii_tx_er),
        .gmii_rx_d(gmii_rx_d),
        .gmii_rx_dv(gmii_rx_dv),
        .gmii_rx_er(gmii_rx_er),
        .pll_locked(pll_locked),
        .eth_link_status(eth_link_status),
        .system_status(system_status),
        .packet_counter(packet_counter)
    );
    
    // ========================================================================
    // Test procedures
    // ========================================================================
    
    task spi_write;
        input [7:0] addr;
        input [31:0] data;
        integer i;
        begin
            spi_cs_n = 0;
            #20;
            for (i = 0; i < 8; i = i + 1) begin
                spi_mosi = addr[7-i];
                spi_clk = 1; #10;
                spi_clk = 0; #10;
            end
            for (i = 0; i < 32; i = i + 1) begin
                spi_mosi = data[31-i];
                spi_clk = 1; #10;
                spi_clk = 0; #10;
            end
            spi_cs_n = 1;
            #20;
        end
    endtask

    initial begin
        $display("Starting FPGA Pipeline Integration Test...");
        rst_n = 0;
        spi_cs_n = 1;
        spi_clk = 0;
        spi_mosi = 0;
        adc_valid = 0;
        adc_data = 0;
        adc_ovr = 0;
        gmii_rx_dv = 0;
        
        #100;
        rst_n = 1;
        
        // Wait for lock
        wait(pll_locked);
        $display("PLL Locked.");
        
        // Configure mode
        spi_write(8'h20, 32'd1); // IQ Mode
        spi_write(8'h03, 32'd1); // Enable
        
        #1000;

        // Inject data
        repeat(100) @(posedge clk_100m) begin
            adc_data <= adc_data + 1;
            adc_valid <= 1;
        end
        adc_valid <= 0;

        #5000;
        if (packet_counter > 0) $display("TEST PASSED: Packets transmitted.");
        else $display("TEST FAILED: No packets transmitted.");
        
        $finish;
    end

endmodule
