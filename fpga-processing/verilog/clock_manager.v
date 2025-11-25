// ============================================================================
// Clock Manager Module
// ============================================================================
// Generates and manages all clock domains for the FPGA processing pipeline
// ============================================================================

`timescale 1ns/1ps

module clock_manager (
    input  wire        clk_100m,        // Primary 100 MHz system clock
    input  wire        clk_105m_adc,    // 105 MHz ADC sampling clock
    input  wire        clk_125m_eth,    // 125 MHz Ethernet GMII clock
    input  wire        rst_n,           // System reset (active low)
    output wire        clk_processing,  // 100 MHz processing domain
    output wire        clk_eth_tx,      // 125 MHz Ethernet TX
    output wire        clk_eth_rx,      // 125 MHz Ethernet RX
    output wire        clk_adc,         // 105 MHz ADC domain
    output wire        reset_n          // System reset (active low)
);

    // ========================================================================
    // Clock buffer and distribution
    // ========================================================================
    
    // Primary system clock buffer
    BUFG bufg_processing (
        .I(clk_100m),
        .O(clk_processing)
    );
    
    // ADC domain clock buffer
    BUFG bufg_adc (
        .I(clk_105m_adc),
        .O(clk_adc)
    );
    
    // Ethernet TX clock buffer
    BUFG bufg_eth_tx (
        .I(clk_125m_eth),
        .O(clk_eth_tx)
    );
    
    // Ethernet RX clock buffer (can be different phase)
    BUFG bufg_eth_rx (
        .I(clk_125m_eth),
        .O(clk_eth_rx)
    );
    
    // ========================================================================
    // Reset synchronization
    // ========================================================================
    
    // Global reset synchronization across all domains
    reg [3:0] reset_sync;
    
    always @(posedge clk_processing or negedge rst_n) begin
        if (!rst_n) begin
            reset_sync <= 4'b0000;
        end else begin
            reset_sync <= {reset_sync[2:0], 1'b1};
        end
    end
    
    // Output synchronized reset (active low)
    assign reset_n = reset_sync[3];
    
endmodule
