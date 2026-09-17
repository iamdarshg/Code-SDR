// ============================================================================
// v2_spi_regs - SPI slave register file (mode 0, MSB first) for the RP2040.
//
// Frame:  <addr:8><data:32>   (data MSB first)
//   addr[7] = 0 : write, the 32 following bits are captured
//   addr[7] = 1 : read,  the slave drives 32 bits on MISO
//   addr[6:0]   : register address
//
// The SPI clock is asynchronous to the FPGA, so it is oversampled by the system
// clock and edges are detected; spi_clk is never used as a logic clock.
//
// Reads are served from v2_telemetry (addresses 0x00..0x1F) or from the local
// config registers (0x00..0x05). The telemetry value is latched into a shift
// register one system-clock cycle after the address byte completes, so a live
// counter cannot change mid-frame and corrupt the read.
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module v2_spi_regs (
    input  wire        clk,          // system clock (>= 8x spi_clk)
    input  wire        rst_n,

    input  wire        spi_clk,
    input  wire        spi_cs_n,
    input  wire        spi_mosi,
    output reg         spi_miso,

    output wire [7:0]  tele_addr,
    input  wire [31:0] tele_data,

    output reg  [1:0]  cfg_mode,
    output reg  [7:0]  cfg_sample_bits,
    output reg  [7:0]  cfg_decim,
    output reg  [31:0] cfg_nco_freq,
    output reg         cfg_enable,
    output reg  [15:0] cfg_dst_port
);

    localparam [6:0] A_MODE     = 7'h00;
    localparam [6:0] A_BITS     = 7'h01;
    localparam [6:0] A_DECIM    = 7'h02;
    localparam [6:0] A_NCO      = 7'h03;
    localparam [6:0] A_ENABLE   = 7'h04;
    localparam [6:0] A_DST_PORT = 7'h05;

    reg [2:0] sclk_s, cs_s;
    wire sclk_rise = (sclk_s[2:1] == 2'b01);
    wire sclk_fall = (sclk_s[2:1] == 2'b10);
    wire cs_active = ~cs_s[1];

    reg [5:0]  bit_cnt;
    reg [39:0] shin;
    reg [31:0] shout;
    reg        is_read;
    reg        addr_just;
    reg [6:0]  addr;

    wire [31:0] wdata = {shin[30:0], spi_mosi};   // 32 data bits incl. current

    // Telemetry block occupies SPI addresses 0x20..0x3F, mapped to 0x00..0x1F.
    assign tele_addr = (addr[6:5] == 2'b01) ? {1'b0, addr[4:0]} : {1'b0, addr};

    // Telemetry register (0x00..0x1F) vs local config register (0x00..0x05):
    // telemetry reads use addresses 0x20..0x3F to avoid overlapping writes.
    wire tele_read = (addr[6:5] == 2'b01);      // 0x20..0x3F

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sclk_s   <= 3'b000;
            cs_s     <= 3'b111;
            bit_cnt  <= 6'd0;
            shin     <= 40'd0;
            shout    <= 32'd0;
            is_read  <= 1'b0;
            addr_just<= 1'b0;
            addr     <= 7'd0;
            spi_miso <= 1'b0;
            cfg_mode <= 2'd0;
            cfg_sample_bits <= 8'd10;
            cfg_decim <= 8'd0;   // 0 = use the datapath default
            cfg_nco_freq <= 32'd0;
            cfg_enable <= 1'b1;
            cfg_dst_port <= 16'd10000;
        end else begin
            sclk_s <= {sclk_s[1:0], spi_clk};
            cs_s   <= {cs_s[1:0], spi_cs_n};
            addr_just <= 1'b0;

            if (!cs_active) begin
                bit_cnt  <= 6'd0;
                is_read  <= 1'b0;
                spi_miso <= 1'b0;
            end else begin
                if (sclk_rise) begin
                    shin    <= {shin[38:0], spi_mosi};
                    bit_cnt <= bit_cnt + 6'd1;

                    if (bit_cnt == 6'd7) begin
                        // current bit has not been merged into shin yet
                        addr      <= {shin[5:0], spi_mosi};
                        is_read   <= shin[6];
                        addr_just <= 1'b1;
                    end

                    if (bit_cnt == 6'd39 && !is_read) begin
                        // wdata uses the current bit, which is not yet in shin
                        case (addr)
                            A_MODE:     cfg_mode        <= wdata[1:0];
                            A_BITS:     cfg_sample_bits <= wdata[7:0];
                            A_DECIM:    cfg_decim       <= wdata[7:0];
                            A_NCO:      cfg_nco_freq    <= wdata[31:0];
                            A_ENABLE:   cfg_enable      <= wdata[0];
                            A_DST_PORT: cfg_dst_port    <= wdata[15:0];
                            default: ;
                        endcase
                    end
                end

                if (addr_just) begin
                    if (addr[6:5] == 2'b01)
                        shout <= tele_data;                 // telemetry
                    else case (addr)
                        A_MODE:     shout <= {30'd0, cfg_mode};
                        A_BITS:     shout <= {24'd0, cfg_sample_bits};
                        A_DECIM:    shout <= {24'd0, cfg_decim};
                        A_NCO:      shout <= cfg_nco_freq;
                        A_ENABLE:   shout <= {31'd0, cfg_enable};
                        A_DST_PORT: shout <= {16'd0, cfg_dst_port};
                        default:    shout <= 32'd0;
                    endcase
                end

                if (sclk_fall && is_read && (bit_cnt >= 6'd8)) begin
                    spi_miso <= shout[31];
                    shout    <= {shout[30:0], 1'b0};
                end
            end
        end
    end

endmodule

`default_nettype wire
