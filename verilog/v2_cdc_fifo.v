// ============================================================================
// v2_cdc_fifo - dual-clock (asynchronous) FIFO for the Code-SDR V2 datapath.
//
// Standard Cummings-style design:
//   * binary + gray pointer per domain
//   * two-flop synchronisers on the gray pointers
//   * combinational full/empty derived from the *next* pointer, which is safe
//     because the far-side pointer is only ever observed synchronised.
//
// Used to cross the 100 MHz ADC/processing domain into the 125 MHz Ethernet
// domain. Both raw and FFT modes depend on it.
//
// Identical wr_clk/rd_clk is legal in simulation; hardware must use a real
// dual-clock block RAM (Lattice EBR supports independent read/write clocks).
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_cdc_fifo #(
    parameter integer WIDTH      = 32,
    parameter integer ADDR_WIDTH = 9    // depth = 2**ADDR_WIDTH (>= 4)
) (
    input  wire              wr_clk,
    input  wire              wr_rst_n,
    input  wire              wr_en,
    input  wire [WIDTH-1:0]  din,
    output wire              full,

    input  wire              rd_clk,
    input  wire              rd_rst_n,
    input  wire              rd_en,
    output wire [WIDTH-1:0]  dout,
    output wire              empty,
    output reg  [ADDR_WIDTH:0] rd_avail   // conservative words readable now
);

    localparam integer DEPTH = 1 << ADDR_WIDTH;
    localparam integer MSB   = ADDR_WIDTH;

    initial begin
        if (ADDR_WIDTH < 2)
            $fatal(1, "v2_cdc_fifo requires ADDR_WIDTH >= 2 (DEPTH >= 4)");
    end

    // ---------------------------------------------------------------- storage
    reg [WIDTH-1:0] mem [0:DEPTH-1];

    // ------------------------------------------------------------ write domain
    reg  [MSB:0] wr_bin;
    reg  [MSB:0] wr_gray;
    wire         wr_do        = wr_en && !full;
    wire [MSB:0] wr_bin_inc   = wr_bin + {{MSB{1'b0}}, 1'b1};
    wire [MSB:0] wr_gray_inc  = (wr_bin_inc >> 1) ^ wr_bin_inc;

    // ------------------------------------------------------------- read domain
    reg  [MSB:0] rd_bin;
    reg  [MSB:0] rd_gray;
    wire         rd_do        = rd_en && !empty;
    wire [MSB:0] rd_bin_inc   = rd_bin + {{MSB{1'b0}}, 1'b1};
    wire [MSB:0] rd_gray_inc  = (rd_bin_inc >> 1) ^ rd_bin_inc;

    // ------------------------------------------- synchronisers (2 flop + ASYNC_REG)
    (* ASYNC_REG = "TRUE" *) reg [MSB:0] wr_gray_s1, wr_gray_s2;
    (* ASYNC_REG = "TRUE" *) reg [MSB:0] rd_gray_s1, rd_gray_s2;

    always @(posedge rd_clk or negedge rd_rst_n) begin
        if (!rd_rst_n) begin
            wr_gray_s1 <= {MSB+1{1'b0}};
            wr_gray_s2 <= {MSB+1{1'b0}};
        end else begin
            wr_gray_s1 <= wr_gray;
            wr_gray_s2 <= wr_gray_s1;
        end
    end

    always @(posedge wr_clk or negedge wr_rst_n) begin
        if (!wr_rst_n) begin
            rd_gray_s1 <= {MSB+1{1'b0}};
            rd_gray_s2 <= {MSB+1{1'b0}};
        end else begin
            rd_gray_s1 <= rd_gray;
            rd_gray_s2 <= rd_gray_s1;
        end
    end

    // ---------------------------------------------------------------- flags
    // full  : next write pointer catches the synchronised read pointer with the
    //         two MSBs inverted -> all but one slot used, no pointer aliasing.
    // empty : the domain-local read pointer equals the synchronised write ptr.
    assign full  = (wr_gray_inc == {~rd_gray_s2[MSB:MSB-1], rd_gray_s2[MSB-2:0]});
    assign empty = (rd_gray == wr_gray_s2);

    // -------------------------------------------------------------- pointers
    always @(posedge wr_clk or negedge wr_rst_n) begin
        if (!wr_rst_n) begin
            wr_bin  <= {MSB+1{1'b0}};
            wr_gray <= {MSB+1{1'b0}};
        end else if (wr_do) begin
            wr_bin  <= wr_bin_inc;
            wr_gray <= wr_gray_inc;
        end
    end

    always @(posedge rd_clk or negedge rd_rst_n) begin
        if (!rd_rst_n) begin
            rd_bin  <= {MSB+1{1'b0}};
            rd_gray <= {MSB+1{1'b0}};
        end else if (rd_do) begin
            rd_bin  <= rd_bin_inc;
            rd_gray <= rd_gray_inc;
        end
    end

    // ------------------------------------------------------------- memory ports
    always @(posedge wr_clk) begin
        if (wr_do)
            mem[wr_bin[ADDR_WIDTH-1:0]] <= din;
    end

    reg [WIDTH-1:0] dout_r;

    always @(posedge rd_clk or negedge rd_rst_n) begin
        if (!rd_rst_n)
            dout_r <= {WIDTH{1'b0}};
        else if (rd_do)
            dout_r <= mem[rd_bin[ADDR_WIDTH-1:0]];
    end

    assign dout = dout_r;

    // ---------------------------------------------------------- occupancy count
    // The write clock (100 MHz) is slower than the read clock (125 MHz), so at
    // most one write can be observed per read clock. Counting synchronised
    // write-pointer changes therefore gives an exact lower bound on available
    // words without decoding gray code. Used by the packetizer to start a frame
    // only when a whole datagram's worth of samples is present.
    reg [MSB:0] wr_gray_s2_d;
    wire        wr_event = (wr_gray_s2 != wr_gray_s2_d);

    always @(posedge rd_clk or negedge rd_rst_n) begin
        if (!rd_rst_n) begin
            wr_gray_s2_d <= {MSB+1{1'b0}};
            rd_avail     <= {MSB+1{1'b0}};
        end else begin
            wr_gray_s2_d <= wr_gray_s2;
            case ({wr_event, rd_do})
                2'b10:   rd_avail <= rd_avail + {{MSB{1'b0}}, 1'b1};
                2'b01:   rd_avail <= rd_avail - {{MSB{1'b0}}, 1'b1};
                default: rd_avail <= rd_avail;
            endcase
        end
    end

endmodule

`default_nettype wire
