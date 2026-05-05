// ============================================================================
// FFT Packetizer
// ============================================================================
// Converts natural-order FFT bins into 32-bit application words and carries the
// word length across the FFT-to-Ethernet CDC with the payload.
//
// Data word:
//   [31:16] real[23:8], [15:0] imag[23:8]
//
// Footer words, emitted after each frame for each 256-bin subframe:
//   0: 0xF17A0000 | subframe
//   1: frame_counter[31:0]
//   2: {start_bin[15:0], end_bin[15:0]}
//   3: {fft_size[15:0], mode[7:0], overflow, 7'd0}
//   4: timestamp[31:0]
//   5: {packet_sequence[15:0], subframe[7:0], 8'hA5}
// ============================================================================

`timescale 1ns/1ps

module fft_packetizer #(
    parameter FFT_SIZE = 1024,
    parameter BINS_PER_SUBFRAME = 256,
    parameter DATA_WIDTH = 24
) (
    input  wire                  fft_clk,
    input  wire                  eth_clk,
    input  wire                  rst_n,
    input  wire [DATA_WIDTH-1:0] fft_real,
    input  wire [DATA_WIDTH-1:0] fft_imag,
    input  wire                  fft_valid,
    input  wire [11:0]           fft_index,
    input  wire                  fft_overflow,
    input  wire [7:0]            mode,
    output wire [31:0]           app_data,
    output wire [15:0]           app_len,
    output wire                  app_valid,
    input  wire                  app_ready,
    output wire                  fifo_full,
    output wire                  fifo_empty
);

    localparam FIFO_WIDTH = 48;
    localparam META_WORDS = 6;
    localparam SUBFRAMES = FFT_SIZE / BINS_PER_SUBFRAME;
    localparam [15:0] FFT_SIZE_WORD = FFT_SIZE;

    reg [31:0] timestamp_counter;
    reg [31:0] frame_counter;
    reg [15:0] packet_sequence;
    reg [7:0]  meta_subframe;
    reg [2:0]  meta_word;
    reg        meta_active;
    reg        frame_overflow;
    reg [31:0] frame_timestamp;

    reg [31:0] wr_payload;
    reg [15:0] wr_len;
    reg        wr_en;

    wire [15:0] subframe_start = meta_subframe * BINS_PER_SUBFRAME;
    wire [15:0] subframe_end = subframe_start + BINS_PER_SUBFRAME - 1;

    always @* begin
        wr_payload = 32'd0;
        wr_len = 16'd4;
        wr_en = 1'b0;

        if (meta_active) begin
            wr_en = !fifo_full;
            case (meta_word)
                3'd0: wr_payload = 32'hF17A0000 | {24'd0, meta_subframe};
                3'd1: wr_payload = frame_counter;
                3'd2: wr_payload = {subframe_start, subframe_end};
                3'd3: wr_payload = {FFT_SIZE_WORD, mode, frame_overflow, 7'd0};
                3'd4: wr_payload = frame_timestamp;
                3'd5: wr_payload = {packet_sequence, meta_subframe, 8'hA5};
                default: wr_payload = 32'd0;
            endcase
        end else if (fft_valid) begin
            wr_en = !fifo_full;
            wr_payload = {fft_real[DATA_WIDTH-1:DATA_WIDTH-16], fft_imag[DATA_WIDTH-1:DATA_WIDTH-16]};
        end
    end

    always @(posedge fft_clk or negedge rst_n) begin
        if (!rst_n) begin
            timestamp_counter <= 32'd0;
            frame_counter <= 32'd0;
            packet_sequence <= 16'd0;
            meta_subframe <= 8'd0;
            meta_word <= 3'd0;
            meta_active <= 1'b0;
            frame_overflow <= 1'b0;
            frame_timestamp <= 32'd0;
        end else begin
            timestamp_counter <= timestamp_counter + 1'b1;

            if (fft_valid && fft_index == 12'd0) begin
                frame_overflow <= fft_overflow;
                frame_timestamp <= timestamp_counter;
            end else if (fft_valid) begin
                frame_overflow <= frame_overflow | fft_overflow;
            end

            if (!meta_active && fft_valid && fft_index == FFT_SIZE-1) begin
                meta_active <= 1'b1;
                meta_subframe <= 8'd0;
                meta_word <= 3'd0;
            end else if (meta_active && !fifo_full) begin
                if (meta_word == META_WORDS-1) begin
                    meta_word <= 3'd0;
                    packet_sequence <= packet_sequence + 1'b1;
                    if (meta_subframe == SUBFRAMES-1) begin
                        meta_active <= 1'b0;
                        frame_counter <= frame_counter + 1'b1;
                    end else begin
                        meta_subframe <= meta_subframe + 1'b1;
                    end
                end else begin
                    meta_word <= meta_word + 1'b1;
                end
            end
        end
    end

    wire [FIFO_WIDTH-1:0] fifo_din = {wr_len, wr_payload};
    wire [FIFO_WIDTH-1:0] fifo_dout;
    wire fifo_rd_en;

    async_fifo #(
        .WIDTH(FIFO_WIDTH),
        .DEPTH(2048)
    ) u_fft_packet_fifo (
        .wr_clk(fft_clk),
        .rd_clk(eth_clk),
        .wr_rst_n(rst_n),
        .rd_rst_n(rst_n),
        .din(fifo_din),
        .wr_en(wr_en),
        .rd_en(fifo_rd_en),
        .dout(fifo_dout),
        .full(fifo_full),
        .empty(fifo_empty)
    );

    reg [FIFO_WIDTH-1:0] app_word;
    reg app_valid_reg;
    reg fifo_rd_pending;

    assign fifo_rd_en = !fifo_empty && !fifo_rd_pending && (!app_valid_reg || app_ready);

    always @(posedge eth_clk or negedge rst_n) begin
        if (!rst_n) begin
            app_word <= {FIFO_WIDTH{1'b0}};
            app_valid_reg <= 1'b0;
            fifo_rd_pending <= 1'b0;
        end else begin
            if (fifo_rd_en) begin
                fifo_rd_pending <= 1'b1;
            end else if (fifo_rd_pending) begin
                app_word <= fifo_dout;
                app_valid_reg <= 1'b1;
                fifo_rd_pending <= 1'b0;
            end else if (app_ready) begin
                app_valid_reg <= 1'b0;
            end
        end
    end

    assign app_len = app_word[47:32];
    assign app_data = app_word[31:0];
    assign app_valid = app_valid_reg;

endmodule
