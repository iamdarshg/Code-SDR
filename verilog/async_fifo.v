`default_nettype none
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
    
    localparam ADDR_WIDTH = (DEPTH >= 2) ? $clog2(DEPTH) : 1;
    // Full = next write pointer equals the read pointer with the two MSBs of the
    // Gray pointer inverted. Written as a shift rather than
    // {2'b11, {(ADDR_WIDTH-1){1'b0}}}: at DEPTH=2 (ADDR_WIDTH=1) that
    // replication is zero-width, which IEEE 1364-2001 4.1.1 forbids.
    localparam [ADDR_WIDTH:0] FULL_MASK = (3 << (ADDR_WIDTH - 1));

    initial begin
        if (DEPTH < 2 || (DEPTH & (DEPTH - 1)) != 0)
            $fatal(1, "async_fifo DEPTH must be a power of two and at least 2");
    end
    
    // ========================================================================
    // Write pointer logic (write clock domain)
    // ========================================================================
    
    reg [ADDR_WIDTH:0] wr_ptr_gray;
    reg [ADDR_WIDTH:0] wr_ptr_bin;
    wire [ADDR_WIDTH:0] wr_ptr_gray_next;
    wire [ADDR_WIDTH:0] wr_ptr_bin_next;
    
    // Binary to Gray code conversion
    function [ADDR_WIDTH:0] bin_to_gray(input [ADDR_WIDTH:0] bin);
        bin_to_gray = bin ^ (bin >> 1);
    endfunction
    
    // Binary counter increment
    // The pointer update is gated by !full (below), but `full` itself must not
    // depend on a gated pointer or it becomes combinational. Compute the
    // "increment by one" gray code unconditionally and derive full from that.
    assign wr_ptr_bin_next = wr_ptr_bin + {{ADDR_WIDTH{1'b0}}, (wr_en && !full)};
    assign wr_ptr_gray_next = bin_to_gray(wr_ptr_bin_next);

    wire [ADDR_WIDTH:0] wr_ptr_bin_inc  = wr_ptr_bin + {{ADDR_WIDTH{1'b0}}, 1'b1};
    wire [ADDR_WIDTH:0] wr_ptr_gray_inc = bin_to_gray(wr_ptr_bin_inc);
    
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
    wire [ADDR_WIDTH:0] rd_ptr_gray_next;
    wire [ADDR_WIDTH:0] rd_ptr_bin_next;
    
    assign rd_ptr_bin_next = rd_ptr_bin + {{ADDR_WIDTH{1'b0}}, (rd_en && !empty)};
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

    // Combinational full/empty. `full_reg` was registered, which asserts one
    // cycle late and lets a write land after the last free slot, overwriting
    // unread data (issue #57).
    //
    // Both compares must avoid a combinational loop: full uses an UNGATED next
    // write pointer, empty uses the REGISTERED read pointer (not a gated "next"
    // one). Gating either on the flag it produces loops back on itself.
    wire full_raw  = (wr_ptr_gray_inc == (rd_ptr_gray_sync2 ^ FULL_MASK));
    wire empty_raw = (rd_ptr_gray == wr_ptr_gray_sync2);

    assign full  = full_raw;
    assign empty = empty_raw;
    
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

endmodule
