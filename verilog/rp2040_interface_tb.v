`timescale 1ns / 1ps

module rp2040_interface_tb;

    // DUT parameters
    parameter ADDR_WIDTH = 16;
    parameter DATA_WIDTH = 32;
    parameter SPI_CLK_PERIOD = 100; // 10 MHz SPI clock

    // DUT signals
    // SPI interface
    reg        spi_clk;
    reg        spi_mosi;
    wire       spi_miso;
    reg        spi_cs_n;

    // Control signals
    wire [31:0] frequency_word;
    wire [7:0]  gain_control;
    wire [3:0]  filter_select;
    wire        enable_control;
    wire        streaming_mode;
    wire [7:0]  bandwidth_limit;

    // Dynamic Reconfiguration
    wire [2:0]  processing_mode;
    wire [7:0]  modulation_type;
    wire [7:0]  filter_bandwidth;
    wire        clock_gating_en;
    wire [7:0]  thermal_scaling;
    wire        resource_opt_en;
    wire [7:0]  power_profile;

    // Status signals
    reg [15:0]  status_reg;
    reg         pll_locked;
    reg         eth_link_status;

    // Instantiate DUT
    rp2040_interface dut (
        .spi_clk(spi_clk),
        .spi_mosi(spi_mosi),
        .spi_cs_n(spi_cs_n),
        .spi_miso(spi_miso),
        .frequency_word(frequency_word),
        .gain_control(gain_control),
        .filter_select(filter_select),
        .enable_control(enable_control),
        .streaming_mode(streaming_mode),
        .bandwidth_limit(bandwidth_limit),
        .processing_mode(processing_mode),
        .modulation_type(modulation_type),
        .filter_bandwidth(filter_bandwidth),
        .clock_gating_en(clock_gating_en),
        .thermal_scaling(thermal_scaling),
        .resource_opt_en(resource_opt_en),
        .power_profile(power_profile),
        .status_reg(status_reg),
        .pll_locked(pll_locked),
        .eth_link_status(eth_link_status)
    );

    // SPI clock generation
    initial begin
        spi_clk = 0;
        forever #(SPI_CLK_PERIOD/2) spi_clk = ~spi_clk;
    end

    // Test task to send SPI data
    task spi_send_byte(input [7:0] addr, input [31:0] data);
        integer i;
        begin
            // Start transaction
            spi_cs_n = 0;
            #SPI_CLK_PERIOD;

            // Send address
            for (i = 7; i >= 0; i = i - 1) begin
                spi_mosi = addr[i];
                #SPI_CLK_PERIOD;
            end

            // Send data (32 bits)
            for (i = 31; i >= 0; i = i - 1) begin
                spi_mosi = data[i];
                #SPI_CLK_PERIOD;
            end

            // End transaction
            spi_cs_n = 1;
            #SPI_CLK_PERIOD;
        end
    endtask

    // Test sequence
    initial begin
        // Initialize signals
        spi_cs_n = 1;
        spi_mosi = 0;
        status_reg = 16'hABCD;
        pll_locked = 1;
        eth_link_status = 1;

        // Wait for reset/initialization
        #1000;

        $display("Testing RP2040 Interface...");

        // Test writing to various registers
        $display("Writing to frequency control register...");
        spi_send_byte(8'h00, 32'h12345678);  // Frequency word

        #500; // Wait for SPI transaction to complete

        $display("Frequency word: 0x%h", frequency_word);

        $display("Writing to processing mode register...");
        spi_send_byte(8'h20, 32'h00000002);  // Processing mode

        #500;

        $display("Processing mode: %d", processing_mode);

        $display("Testing gain control...");
        spi_send_byte(8'h01, 32'h0000007F);  // Gain control

        #500;

        $display("Gain control: 0x%h", gain_control);

        // Test modulation type setting
        $display("Setting modulation_type to AM...");
        spi_send_byte(8'h21, 32'h00000001);  // AM modulation (example value)

        #500;

        $display("Modulation type: 0x%h", modulation_type);

        // Test multiple registers
        $display("Testing clock gating enable...");
        spi_send_byte(8'h23, 32'h00000001);  // Enable clock gating

        #500;

        $display("Clock gating: %b", clock_gating_en);

        #1000;

        $display("RP2040 Interface test completed");

        // Verify some outputs
        if (frequency_word == 32'h12345678) begin
            $display("✓ Frequency word write successful");
        end else begin
            $display("✗ Frequency word write failed");
        end

        if (processing_mode == 3'd2) begin
            $display("✓ Processing mode write successful");
        end else begin
            $display("✗ Processing mode write failed");
        end

        $finish;
    end

endmodule
