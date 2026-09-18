// ============================================================================
// v2_fft_packetizer - buffers FFT binbuf and frames them as UDP payloads.
//
// The FFT emits one bin every other cycle, but the MAC must be fed a continuous
// byte stream once a frame starts, so a full packet's worth of binbuf is buffered
// first (one 256 x 32-bit RAM = 1 EBR), then streamed out.
//
// Payload layout:
//   bytes 0..15 : header
//       0..3   packet sequence
//       4..7   FFT frame counter
//       8..9   first bin index in this packet
//       10     scale_exp (block-floating-point exponent for this frame)
//       11     flags: bit0 overflow, bit1 FFT present
//       12..15 reserved
//   bytes 16..  : binbuf, 4 bytes each, big-endian {re_hi, re_lo, im_hi, im_lo}
//
// A frame's binbuf arrive in natural order (0..N-1), so packets are aligned and
// first_bin_index is simply the index of the first bin written into the packet.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_fft_packetizer #(
    parameter integer BINS_PER_PKT = 256,
    parameter integer HEADER_BYTES = 16,
    parameter integer W            = 16
) (
    input  wire                clk,
    input  wire                rst_n,

    // from the FFT (clk domain)
    input  wire signed [W-1:0] fft_re,
    input  wire signed [W-1:0] fft_im,
    input  wire                fft_valid,
    input  wire [9:0]          fft_index,
    input  wire [31:0]         fft_frame,
    input  wire [4:0]          fft_scale_exp,
    input  wire                fft_overflow,

    // to the UDP/IP layer
    input  wire                upstream_busy,
    output wire [7:0]          p_data,
    output wire                p_valid,
    input  wire                p_ready,
    output wire                send,
    output wire [15:0]         payload_len,
    output wire [31:0]         packet_seq,

    output reg  [31:0]         packets_sent,
    output reg  [31:0]         bins_dropped
);

    localparam integer PKT_BYTES = HEADER_BYTES + BINS_PER_PKT*4;
    localparam integer IDXW      = $clog2(BINS_PER_PKT) + 1;   // one extra bit to hold BINS_PER_PKT

    // ------------------------------------------------------------- bin buffer
    reg [31:0] binbuf [0:BINS_PER_PKT-1];

    reg [IDXW-1:0] wr_idx;     // binbuf written into the staging buffer
    reg [IDXW-1:0] rd_idx;     // binbuf read out
    reg [9:0]  first_bin;
    reg [31:0] frame_lat, seq;
    reg [4:0]  exp_lat;
    reg        ovf_lat;
    reg        have_pkt;       // a full packet is staged
    reg        sending;
    reg [15:0] byte_i;

    wire buffer_full = (wr_idx == BINS_PER_PKT[IDXW-1:0]);

    assign send        = have_pkt && !sending && !upstream_busy;
    assign payload_len = PKT_BYTES[15:0];
    assign packet_seq  = seq;

    // ------------------------------------------------------------ bin intake
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wr_idx    <= {IDXW{1'b0}};
            first_bin <= 10'd0;
            frame_lat <= 32'd0;
            exp_lat   <= 5'd0;
            ovf_lat   <= 1'b0;
        end else begin
            if (fft_valid && !buffer_full) begin
                if (wr_idx == {IDXW{1'b0}}) begin
                    first_bin <= fft_index;
                    frame_lat <= fft_frame;
                    exp_lat   <= fft_scale_exp;
                    ovf_lat   <= fft_overflow;
                end
                binbuf[wr_idx] <= {fft_re, fft_im};
                wr_idx <= wr_idx + {{(IDXW-1){1'b0}}, 1'b1};
            end else if (fft_valid && buffer_full) begin
                bins_dropped <= bins_dropped + 32'd1;
            end
        end
    end

    // -------------------------------------------------------------- TX stream
    // header byte / bin byte multiplexer
    reg [7:0] hdr_byte;
    always @* begin
        case (byte_i)
            16'd0:  hdr_byte = seq[31:24];
            16'd1:  hdr_byte = seq[23:16];
            16'd2:  hdr_byte = seq[15:8];
            16'd3:  hdr_byte = seq[7:0];
            16'd4:  hdr_byte = frame_lat[31:24];
            16'd5:  hdr_byte = frame_lat[23:16];
            16'd6:  hdr_byte = frame_lat[15:8];
            16'd7:  hdr_byte = frame_lat[7:0];
            16'd8:  hdr_byte = {6'b0, first_bin[9:8]};
            16'd9:  hdr_byte = first_bin[7:0];
            16'd10: hdr_byte = {3'b0, exp_lat};
            16'd11: hdr_byte = {6'b0, ovf_lat, 1'b1};
            default: hdr_byte = 8'h00;
        endcase
    end

    // byte within the current bin (0..3)
    wire [15:0] bin_byte_i = byte_i - HEADER_BYTES[15:0];
    wire [1:0]  bb = bin_byte_i[1:0];
    wire [31:0] cur = binbuf[rd_idx];

    reg [7:0] bin_byte;
    always @* begin
        case (bb)
            2'd0:    bin_byte = cur[31:24];
            2'd1:    bin_byte = cur[23:16];
            2'd2:    bin_byte = cur[15:8];
            default: bin_byte = cur[7:0];
        endcase
    end

    assign p_data  = (byte_i < HEADER_BYTES[15:0]) ? hdr_byte : bin_byte;
    assign p_valid = sending;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            have_pkt     <= 1'b0;
            sending      <= 1'b0;
            byte_i       <= 16'd0;
            rd_idx       <= {IDXW{1'b0}};
            seq          <= 32'd0;
            packets_sent <= 32'd0;
            bins_dropped <= 32'd0;
        end else begin
            if (!have_pkt) begin
                if (buffer_full) begin
                    have_pkt <= 1'b1;
                    byte_i   <= 16'd0;
                    rd_idx   <= {IDXW{1'b0}};
                end
            end else if (!sending) begin
                if (!upstream_busy) begin
                    sending <= 1'b1;
                    byte_i  <= 16'd0;
                end
            end else if (p_ready) begin
                if (byte_i == PKT_BYTES - 1) begin
                    sending      <= 1'b0;
                    have_pkt     <= 1'b0;
                    seq          <= seq + 32'd1;
                    packets_sent <= packets_sent + 32'd1;
                    // free the staging buffer only after the packet is gone
                    wr_idx <= {IDXW{1'b0}};
                end else begin
                    byte_i <= byte_i + 16'd1;
                    // advance the bin pointer when its last byte is taken
                    if (byte_i >= HEADER_BYTES[15:0] &&
                        bb == 2'd3)
                        rd_idx <= rd_idx + {{(IDXW-1){1'b0}}, 1'b1};
                end
            end
        end
    end

endmodule

`default_nettype wire
