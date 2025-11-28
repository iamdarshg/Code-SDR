// ============================================================================
// Asynchronous FIFO Module
// ============================================================================
// Cross-domain FIFO for transferring data between different clock domains
// ============================================================================

`timescale 1ns/1ps

module async_fifo #(
    parameter WIDTH = 32,
    parameter DEPTH = 256
) (
    input  wire        wr_clk,          // Write clock domain
    input  wire        rd_clk,          // Read clock domain
    input  wire        wr_rst_n,        // Write reset (active low)
    input  wire        rd_rst_n,        // Read reset (active low)
    input  wire [WIDTH-1:0] din,         // Data input
    input  wire        wr_en,           // Write enable
    input  wire        rd_en,           // Read enable
    output wire [WIDTH-1:0] dout,        // Data output
    output wire        full,            // FIFO full flag
    output wire        empty            // FIFO empty flag
);

    // ========================================================================
    // Parameter calculations
    // ========================================================================
    
    localparam ADDR_WIDTH = $clog2(DEPTH);
    
    // ========================================================================
    // Write pointer logic (write clock domain)
    // ========================================================================
    
    reg [ADDR_WIDTH:0] wr_ptr_gray;
    reg [ADDR_WIDTH:0] wr_ptr_bin;
    reg [ADDR_WIDTH:0] wr_ptr_gray_next;
    reg [ADDR_WIDTH:0] wr_ptr_bin_next;
    
    // Binary to Gray code conversion
    function [ADDR_WIDTH:0] bin_to_gray(input [ADDR_WIDTH:0] bin);
        bin_to_gray = bin ^ (bin >> 1);
    endfunction
    
    // Binary counter increment
    assign wr_ptr_bin_next = wr_ptr_bin + 1;
    assign wr_ptr_gray_next = bin_to_gray(wr_ptr_bin_next);
    
    always @(posedge wr_clk or negedge wr_rst_n) begin
        if (!wr_rst_n) begin
            wr_ptr_bin <= 'd0;
            wr_ptr_gray <= 'd0;
        end else begin
            if (wr_en && !full) begin
                wr_ptr_bin <= wr_ptr_bin_next;
                wr_ptr_gray <= wr_ptr_gray_next;
            end
        end
    end
    
    // ========================================================================
    // Read pointer logic (read clock domain)
    // ========================================================================
    
    reg [ADDR_WIDTH:0] rd_ptr_gray;
    reg [ADDR_WIDTH:0] rd_ptr_bin;
    reg [ADDR_WIDTH:0] rd_ptr_gray_next;
    reg [ADDR_WIDTH:0] rd_ptr_bin_next;
    
    assign rd_ptr_bin_next = rd_ptr_bin + 1;
    assign rd_ptr_gray_next = bin_to_gray(rd_ptr_bin_next);
    
    always @(posedge rd_clk or negedge rd_rst_n) begin
        if (!rd_rst_n) begin
            rd_ptr_bin <= 'd0;
            rd_ptr_gray <= 'd0;
        end else begin
            if (rd_en && !empty) begin
                rd_ptr_bin <= rd_ptr_bin_next;
                rd_ptr_gray <= rd_ptr_gray_next;
            end
        end
    end
    
    // ========================================================================
    // Gray code synchronization
    // ========================================================================
    
    // Synchronize write pointer to read clock domain
    reg [ADDR_WIDTH:0] wr_ptr_gray_sync1;
    reg [ADDR_WIDTH:0] wr_ptr_gray_sync2;
    
    always @(posedge rd_clk or negedge rd_rst_n) begin
        if (!rd_rst_n) begin
            wr_ptr_gray_sync1 <= 'd0;
            wr_ptr_gray_sync2 <= 'd0;
        end else begin
            wr_ptr_gray_sync1 <= wr_ptr_gray;
            wr_ptr_gray_sync2 <= wr_ptr_gray_sync1;
        end
    end
    
    // Synchronize read pointer to write clock domain
    reg [ADDR_WIDTH:0] rd_ptr_gray_sync1;
    reg [ADDR_WIDTH:0] rd_ptr_gray_sync2;
    
    always @(posedge wr_clk or negedge wr_rst_n) begin
        if (!wr_rst_n) begin
            rd_ptr_gray_sync1 <= 'd0;
            rd_ptr_gray_sync2 <= 'd0;
        end else begin
            rd_ptr_gray_sync1 <= rd_ptr_gray;
            rd_ptr_gray_sync2 <= rd_ptr_gray_sync1;
        end
    end
    
    // ========================================================================
    // Status signal generation
    // ========================================================================

    // Registered flags for stability and safety margin
    reg full_reg;
    reg empty_reg;

    // Full condition: next write pointer would equal synchronized read pointer
    wire full_raw = (wr_ptr_gray_next == rd_ptr_gray_sync2);

    // Empty condition: write pointer equals synchronized read pointer
    wire empty_raw = (wr_ptr_gray_sync2 == rd_ptr_gray);

    always @(posedge wr_clk or negedge wr_rst_n) begin
        if (!wr_rst_n) begin
            full_reg <= 1'b0;
        end else begin
            full_reg <= full_raw;
        end
    end

    always @(posedge rd_clk or negedge rd_rst_n) begin
        if (!rd_rst_n) begin
            empty_reg <= 1'b1;  // Empty at reset
        end else begin
            empty_reg <= empty_raw;
        end
    end

    assign full = full_reg;
    assign empty = empty_reg;
    
    // ========================================================================
    // Memory array (block RAM)
    // ========================================================================

    // Dual-port RAM for FIFO storage
    reg [WIDTH-1:0] mem [0:DEPTH-1];

    // Read data register
    reg [WIDTH-1:0] dout_reg;

    // Write port
    always @(posedge wr_clk) begin
        if (wr_en && !full) begin
            mem[wr_ptr_bin[ADDR_WIDTH-1:0]] <= din;
        end
    end

    // Read port with registered output
    // Read data on read enable, then increment pointer
    always @(posedge rd_clk or negedge rd_rst_n) begin
        if (!rd_rst_n) begin
            dout_reg <= 'd0;
        end else if (rd_en && !empty) begin
            dout_reg <= mem[rd_ptr_bin[ADDR_WIDTH-1:0]];
        end
    end

    assign dout = dout_reg;
    
    // ========================================================================
    // FIFO status counters (optional for monitoring)
    // ========================================================================
    
    // Write pointer difference calculation
    wire [ADDR_WIDTH:0] wr_diff;
    assign wr_diff = wr_ptr_bin - rd_ptr_gray_sync2[ADDR_WIDTH:1];
    
    // FIFO fill level (approximate)
    reg [ADDR_WIDTH:0] fill_level;
    always @(posedge wr_clk or negedge wr_rst_n) begin
        if (!wr_rst_n) begin
            fill_level <= 'd0;
        end else begin
            fill_level <= wr_diff;
        end
    end

endmodule
