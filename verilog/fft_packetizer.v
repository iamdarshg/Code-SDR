`default_nettype none
// ============================================================================
// FFT Packetizer
// ============================================================================
// Accepts one FFT bin per fft_clk and emits one UDP application packet per
// 256-bin subframe in eth_clk. Payload layout:
//   words 0..255: {real[23:8], imag[23:8]}
//   word 256:     0xF17A0000 | subframe_index
//   word 257:     frame_counter
//   word 258:     {start_bin, end_bin}
//   word 259:     {fft_size, mode, overflow, dropped, discontinuity, 5'd0}
//   word 260:     timestamp
//   word 261:     {packet_sequence, subframe_index, 8'hA5}
// Metadata word 259 flags: bit 7 overflow, bit 6 dropped-bin, bit 5 discontinuity.
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
    input  wire [$clog2(FFT_SIZE)-1:0] fft_index,
    input  wire [31:0]           fft_frame_count,
    input  wire                  fft_overflow,
    input  wire [7:0]            mode,
    output wire [31:0]           app_data,
    output wire [15:0]           app_len,
    output wire                  app_valid,
    input  wire                  app_ready,
    output wire                  fifo_full,
    output wire                  fifo_empty
);

    localparam INDEX_WIDTH = $clog2(FFT_SIZE);
    localparam FIFO_WIDTH = 32 + INDEX_WIDTH + 32 + 32 + 2;
    localparam META_WORDS = 6;
    localparam WORDS_PER_PACKET = BINS_PER_SUBFRAME + META_WORDS;
    localparam [15:0] PACKET_BYTES = WORDS_PER_PACKET * 4;
    localparam [15:0] FFT_SIZE_WORD = FFT_SIZE;

    reg [31:0] timestamp_counter;
    reg dropped_bin;
    always @(posedge fft_clk or negedge rst_n) begin
        if (!rst_n) begin
            timestamp_counter <= 32'd0;
            dropped_bin <= 1'b0;
        end else begin
            timestamp_counter <= timestamp_counter + 1'b1;
            if (fft_valid && fifo_full) begin
                dropped_bin <= 1'b1;
            end else if (fft_valid && !fifo_full && fft_index == {INDEX_WIDTH{1'b0}}) begin
                dropped_bin <= 1'b0;
            end
        end
    end

    wire [31:0] bin_word = {fft_real[DATA_WIDTH-1:DATA_WIDTH-16],
                            fft_imag[DATA_WIDTH-1:DATA_WIDTH-16]};
    wire [FIFO_WIDTH-1:0] fifo_din = {
        dropped_bin,
        fft_overflow,
        timestamp_counter,
        fft_frame_count,
        fft_index,
        bin_word
    };
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
        .wr_en(fft_valid && !fifo_full),
        .rd_en(fifo_rd_en),
        .dout(fifo_dout),
        .full(fifo_full),
        .empty(fifo_empty)
    );

    wire fifo_dropped = fifo_dout[FIFO_WIDTH-1];
    wire fifo_overflow = fifo_dout[FIFO_WIDTH-2];
    wire [31:0] fifo_timestamp = fifo_dout[FIFO_WIDTH-3 -: 32];
    wire [31:0] fifo_frame = fifo_dout[FIFO_WIDTH-35 -: 32];
    wire [INDEX_WIDTH-1:0] fifo_index = fifo_dout[31+INDEX_WIDTH:32];
    wire [31:0] fifo_bin_word = fifo_dout[31:0];

    localparam PKT_IDLE = 2'd0;
    localparam PKT_DATA = 2'd1;
    localparam PKT_META = 2'd2;

    reg [1:0] state;
    reg [8:0] data_word_count;
    reg [2:0] meta_word_count;
    reg [31:0] app_data_reg;
    reg app_valid_reg;
    reg fifo_rd_pending;
    reg [31:0] frame_latched;
    reg [31:0] timestamp_latched;
    reg overflow_latched;
    reg dropped_latched;
    reg discontinuity_latched;
    reg [7:0] subframe_latched;
    reg [15:0] packet_sequence;
    reg [INDEX_WIDTH-1:0] expected_index;

    wire start_packet = !fifo_empty && !fifo_rd_pending && !app_valid_reg;
    assign fifo_rd_en = ((state == PKT_IDLE) && start_packet) ||
                        ((state == PKT_DATA) && app_ready && (data_word_count < BINS_PER_SUBFRAME) &&
                         !fifo_empty && !fifo_rd_pending);

    wire [15:0] start_bin = subframe_latched * BINS_PER_SUBFRAME;
    wire [15:0] end_bin = start_bin + BINS_PER_SUBFRAME - 1;

    always @(posedge eth_clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= PKT_IDLE;
            data_word_count <= 9'd0;
            meta_word_count <= 3'd0;
            app_data_reg <= 32'd0;
            app_valid_reg <= 1'b0;
            fifo_rd_pending <= 1'b0;
            frame_latched <= 32'd0;
            timestamp_latched <= 32'd0;
            overflow_latched <= 1'b0;
            dropped_latched <= 1'b0;
            discontinuity_latched <= 1'b0;
            subframe_latched <= 8'd0;
            packet_sequence <= 16'd0;
            expected_index <= {INDEX_WIDTH{1'b0}};
        end else begin
            if (app_ready) begin
                app_valid_reg <= 1'b0;
            end

            if (fifo_rd_en) begin
                fifo_rd_pending <= 1'b1;
            end else if (fifo_rd_pending) begin
                fifo_rd_pending <= 1'b0;
                app_data_reg <= fifo_bin_word;
                app_valid_reg <= 1'b1;
                overflow_latched <= overflow_latched | fifo_overflow;
                dropped_latched <= dropped_latched | fifo_dropped;
                discontinuity_latched <= discontinuity_latched | (fifo_index != expected_index);
                expected_index <= fifo_index + 1'b1;

                if (state == PKT_IDLE) begin
                    frame_latched <= fifo_frame;
                    timestamp_latched <= fifo_timestamp;
                    overflow_latched <= fifo_overflow;
                    dropped_latched <= fifo_dropped;
                    discontinuity_latched <= (fifo_index[7:0] != 8'd0);
                    subframe_latched <= fifo_index[INDEX_WIDTH-1:8];
                    expected_index <= fifo_index + 1'b1;
                    data_word_count <= 9'd1;
                    state <= PKT_DATA;
                end else begin
                    data_word_count <= data_word_count + 1'b1;
                end
            end else if (state == PKT_DATA && app_ready && data_word_count == BINS_PER_SUBFRAME) begin
                meta_word_count <= 3'd0;
                state <= PKT_META;
            end else if (state == PKT_META && app_ready && !app_valid_reg) begin
                app_valid_reg <= 1'b1;
                case (meta_word_count)
                    3'd0: app_data_reg <= 32'hF17A0000 | {24'd0, subframe_latched};
                    3'd1: app_data_reg <= frame_latched;
                    3'd2: app_data_reg <= {start_bin, end_bin};
                    3'd3: app_data_reg <= {FFT_SIZE_WORD, mode, overflow_latched,
                                           dropped_latched, discontinuity_latched, 5'd0};
                    3'd4: app_data_reg <= timestamp_latched;
                    3'd5: app_data_reg <= {packet_sequence, subframe_latched, 8'hA5};
                    default: app_data_reg <= 32'd0;
                endcase

                if (meta_word_count == META_WORDS-1) begin
                    packet_sequence <= packet_sequence + 1'b1;
                    state <= PKT_IDLE;
                    data_word_count <= 9'd0;
                end else begin
                    meta_word_count <= meta_word_count + 1'b1;
                end
            end
        end
    end

    assign app_data = app_data_reg;
    assign app_len = PACKET_BYTES;
    assign app_valid = app_valid_reg;

endmodule
`default_nettype wire
