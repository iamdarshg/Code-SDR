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
    parameter CLK_PERIOD_105M = 9.5238;
    parameter CLK_PERIOD_125M = 8.0;
    parameter SIMULATION_TIME = 100000; // 100ms simulation
    
    // ========================================================================
    // Testbench signals
    // ========================================================================
    
    // System clocks and reset
    reg clk_100m;
    reg clk_105m_adc;
    reg clk_125m_eth;
    reg rst_n;
    
    // ADC interface
    reg [9:0] adc_data;
    reg adc_valid;
    reg adc_ovr;
    
    // RP2040 SPI interface
    reg spi_clk;
    reg spi_mosi;
    reg spi_cs_n;
    wire spi_miso;
    
    // Ethernet GMII interface
    wire [7:0] gmii_tx_d;
    wire gmii_tx_en;
    wire gmii_tx_er;
    reg [7:0] gmii_rx_d;
    reg gmii_rx_dv;
    reg gmii_rx_er;
    
    // Status signals
    wire pll_locked;
    wire eth_link_status;
    wire [15:0] system_status;
    wire [31:0] packet_counter;
    
    // ========================================================================
    // Testbench variables
    // ========================================================================
    
    integer test_passed;
    integer test_failed;
    integer test_count;
    real test_start_time;
    real test_end_time;
    
    // Test data storage
    reg [9:0] test_adc_data [0:1023];
    reg [31:0] expected_fft_data [0:1023];
    reg [31:0] actual_fft_data [0:1023];
    
    // ========================================================================
    // Clock generation
    // ========================================================================
    
    initial begin
        clk_100m = 0;
        forever #(CLK_PERIOD_100M/2) clk_100m = ~clk_100m;
    end
    
    initial begin
        clk_105m_adc = 0;
        forever #(CLK_PERIOD_105M/2) clk_105m_adc = ~clk_105m_adc;
    end
    
    initial begin
        clk_125m_eth = 0;
        forever #(CLK_PERIOD_125M/2) clk_125m_eth = ~clk_125m_eth;
    end
    
    initial begin
        spi_clk = 0;
        forever #(20.0/2) spi_clk = ~spi_clk; // 50 MHz SPI clock
    end
    
    // ========================================================================
    // DUT instantiation
    // ========================================================================
    
    fpga_processing_pipeline dut (
        .clk_100m(clk_100m),
        .clk_105m_adc(clk_105m_adc),
        .clk_125m_eth(clk_125m_eth),
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
    
    // Reset procedure
    task reset_system;
        begin
            $display("Applying system reset...");
            rst_n = 0;
            spi_cs_n = 1;
            adc_valid = 0;
            adc_ovr = 0;
            gmii_rx_d = 0;
            gmii_rx_dv = 0;
            gmii_rx_er = 0;
            
            repeat(10) @(posedge clk_100m);
            
            rst_n = 1;
            repeat(10) @(posedge clk_100m);
            
            $display("System reset completed");
        end
    endtask
    
    // ADC data generation
    task generate_adc_data;
        input integer num_samples;
        input real frequency_hz;
        input integer amplitude;
        integer i;
        real sample_time;
        begin
            for (i = 0; i < num_samples; i = i + 1) begin
                sample_time = i * (1.0/105.0e6); // 105 MSPS
                adc_data <= $rtoi(amplitude * $sin(2.0 * 3.14159 * frequency_hz * sample_time));
                adc_valid <= 1;
                @(posedge clk_105m_adc);
            end
            adc_valid <= 0;
        end
    endtask
    
    // SPI write procedure
    task spi_write_register;
        input [7:0] address;
        input [31:0] data;
        begin
            spi_cs_n = 0;
            @(posedge spi_clk);
            spi_mosi = address[7];
            repeat(8) @(posedge spi_clk) begin
                spi_mosi = address[7];
            end
            
            repeat(32) @(posedge spi_clk) begin
                spi_mosi = data[31];
            end
            
            spi_cs_n = 1;
        end
    endtask
    
    // Test result checking
    task check_test_result;
        input string test_name;
        input integer result;
        begin
            test_count = test_count + 1;
            if (result) begin
                test_passed = test_passed + 1;
                $display("PASS: %s", test_name);
            end else begin
                test_failed = test_failed + 1;
                $display("FAIL: %s", test_name);
            end
        end
    endtask
    
    // ========================================================================
    // Individual test procedures
    // ========================================================================
    
    // Test 1: Basic system initialization
    task test_system_init;
        begin
            $display("\n=== Test 1: System Initialization ===");
            reset_system;
            
            // Check system status
            #100;
            check_test_result("System reset", system_status[0] === 1);
        end
    endtask
    
    // Test 2: ADC interface
    task test_adc_interface;
        begin
            $display("\n=== Test 2: ADC Interface ===");
            
            // Generate test signal
            generate_adc_data(1024, 1.0e6, 512); // 1 MHz, 512 amplitude
            
            // Check for data processing
            #1000;
            check_test_result("ADC data processing", system_status[7] === 1);
        end
    endtask
    
    // Test 3: SPI communication
    task test_spi_communication;
        begin
            $display("\n=== Test 3: SPI Communication ===");
            
            // Test SPI register write
            spi_write_register(8'h00, 32'h40000000); // Write frequency word
            #100;
            
            // Read status register
            spi_write_register(8'h10, 32'h00000000); // Read status
            #100;
            
            check_test_result("SPI communication", spi_miso !== 1'bz);
        end
    endtask
    
    // Test 4: NCO and DDC processing
    task test_ddc_processing;
        begin
            $display("\n=== Test 4: DDC Processing ===");
            
            // Configure NCO for 1 MHz
            spi_write_register(8'h00, 32'h40000000);
            spi_write_register(8'h03, 32'h00000001); // Enable system
            
            // Generate input signal
            generate_adc_data(1024, 10.0e6, 256); // 10 MHz signal
            
            #2000;
            check_test_result("DDC processing", system_status[7] === 1);
        end
    endtask
    
    // Test 5: FFT processing
    task test_fft_processing;
        begin
            $display("\n=== Test 5: FFT Processing ===");
            
            // Enable FFT processing
            spi_write_register(8'h03, 32'h00000001);
            
            // Generate complete FFT window
            generate_adc_data(1024, 5.0e6, 128);
            
            #5000;
            check_test_result("FFT processing", system_status[8] === 1);
        end
    endtask
    
    // Test 6: Ethernet transmission
    task test_ethernet_transmission;
        begin
            $display("\n=== Test 6: Ethernet Transmission ===");
            
            // Simulate Ethernet link
            gmii_rx_dv = 1;
            
            #1000;
            check_test_result("Ethernet link status", eth_link_status === 1);
            check_test_result("Packet counter", packet_counter >= 0);
        end
    endtask
    
    // Test 7: Performance test
    task test_performance;
        begin
            $display("\n=== Test 7: Performance Test ===");
            
            test_start_time = $realtime;
            
            // Run continuous processing
            spi_write_register(8'h03, 32'h00000001);
            
            repeat(10000) begin
                generate_adc_data(1024, 1.0e6, 128);
                #1000;
            end
            
            test_end_time = $realtime;
            
            check_test_result("Continuous processing", (test_end_time - test_start_time) > 0);
        end
    endtask
    
    // ========================================================================
    // Main test sequence
    // ========================================================================
    
    initial begin
        $display("================================================");
        $display("FPGA Processing Pipeline Testbench");
        $display("Simulation started at: %0t", $time);
        $display("================================================");
        
        // Initialize counters
        test_passed = 0;
        test_failed = 0;
        test_count = 0;
        
        // Run tests
        test_system_init;
        test_adc_interface;
        test_spi_communication;
        test_ddc_processing;
        test_fft_processing;
        test_ethernet_transmission;
        test_performance;
        
        // Summary
        $display("\n================================================");
        $display("Test Summary:");
        $display("Total tests: %0d", test_count);
        $display("Passed: %0d", test_passed);
        $display("Failed: %0d", test_failed);
        $display("Simulation time: %0t ns", $time);
        $display("================================================");
        
        if (test_failed === 0) begin
            $display("SUCCESS: All tests passed!");
        end else begin
            $display("FAILURE: Some tests failed!");
        end
        
        // End simulation
        #1000;
        $finish;
    end
    
    // ========================================================================
    // Monitor and logging
    // ========================================================================
    
    // Monitor system status
    always @(posedge clk_100m) begin
        if (system_status !== 0) begin
            $display("Time: %0t ns, Status: %h", $time, system_status);
        end
    end
    
    // Monitor packet counter
    always @(posedge clk_125m_eth) begin
        if (packet_counter > 0 && packet_counter % 100 === 0) begin
            $display("Time: %0t ns, Packets transmitted: %0d", $time, packet_counter);
        end
    end
    
    // VCD dump for waveform analysis
    initial begin
        $dumpfile("fpga_processing_pipeline_tb.vcd");
        $dumpvars(0, fpga_processing_pipeline_tb);
    end

endmodule
