// ============================================================================
// Application Stream CDC
// ============================================================================
// Carries a 32-bit application payload and its byte length across clock domains.
// ============================================================================

`timescale 1ns/1ps

module app_stream_cdc (
    input  wire        wr_clk,
    input  wire        rd_clk,
    input  wire        rst_n,
    input  wire [31:0] wr_data,
    input  wire [15:0] wr_len,
    input  wire        wr_valid,
    output wire        wr_full,
    output wire [31:0] rd_data,
    output wire [15:0] rd_len,
    output wire        rd_valid,
    input  wire        rd_ready,
    output wire        rd_empty
);

    wire [47:0] fifo_din = {wr_len, wr_data};
    wire [47:0] fifo_dout;
    wire fifo_rd_en;

    async_fifo #(
        .WIDTH(48),
        .DEPTH(1024)
    ) u_stream_fifo (
        .wr_clk(wr_clk),
        .rd_clk(rd_clk),
        .wr_rst_n(rst_n),
        .rd_rst_n(rst_n),
        .din(fifo_din),
        .wr_en(wr_valid && !wr_full),
        .rd_en(fifo_rd_en),
        .dout(fifo_dout),
        .full(wr_full),
        .empty(rd_empty)
    );

    reg [47:0] rd_word;
    reg rd_valid_reg;
    reg rd_pending;

    assign fifo_rd_en = !rd_empty && !rd_pending && (!rd_valid_reg || rd_ready);

    always @(posedge rd_clk or negedge rst_n) begin
        if (!rst_n) begin
            rd_word <= 48'd0;
            rd_valid_reg <= 1'b0;
            rd_pending <= 1'b0;
        end else begin
            if (fifo_rd_en) begin
                rd_pending <= 1'b1;
            end else if (rd_pending) begin
                rd_word <= fifo_dout;
                rd_valid_reg <= 1'b1;
                rd_pending <= 1'b0;
            end else if (rd_ready) begin
                rd_valid_reg <= 1'b0;
            end
        end
    end

    assign rd_len = rd_word[47:32];
    assign rd_data = rd_word[31:0];
    assign rd_valid = rd_valid_reg;

endmodule
