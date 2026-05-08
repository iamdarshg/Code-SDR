// ============================================================================
// RP2040 Interface Module
// ============================================================================
// Configuration and control interface for FPGA system via SPI
// ============================================================================

`timescale 1ns/1ps
`default_nettype none

module rp2040_interface #(
    parameter DATA_WIDTH = 32
) (
    // SPI interface from RP2040
    input  wire        spi_clk,          // SPI clock
    input  wire        spi_mosi,         // SPI master out, slave in
    input  wire        spi_cs_n,         // SPI chip select (active low)
    output wire        spi_miso,         // SPI master in, slave out
    
    // Control signals
    output wire [31:0] frequency_word,    // NCO frequency control
    output wire [7:0]  gain_control,      // Gain control settings
    output wire [3:0]  filter_select,     // Filter selection
    output wire        enable_control,    // System enable control
    output wire        streaming_mode,    // Direct streaming mode enable
    output wire [7:0]  bandwidth_limit,   // Bandwidth limiting setting

    // Dynamic Reconfiguration
    output wire [2:0]  processing_mode,   // Processing mode
    output wire [7:0]  modulation_type,   // Modulation/demodulation type
    output wire [7:0]  filter_bandwidth,  // Filter bandwidth setting
    output wire        clock_gating_en,   // Enable clock gating
    output wire [7:0]  thermal_scaling,   // Thermal scaling control
    output wire        resource_opt_en,   // Resource optimization enable
    output wire [7:0]  power_profile,     // Power consumption profile

    // Network Configuration
    output wire [47:0] src_mac,
    output wire [47:0] dst_mac,
    output wire [31:0] src_ip,
    output wire [31:0] dst_ip,
    output wire [15:0] src_port,
    output wire [15:0] dst_port,
    
    // Status signals
    input  wire [15:0] status_reg,        // System status register
    input  wire        pll_locked,        // PLL lock status (status only)
    input  wire        eth_link_status,   // Ethernet link status
    input  wire        rst_n              // System reset (active low)
);

    // ========================================================================
    // SPI Register Mapping
    // ========================================================================
    
    localparam REG_FREQ_WORD    = 8'h00;
    localparam REG_GAIN_CTRL    = 8'h01;
    localparam REG_FILTER_SEL   = 8'h02;
    localparam REG_ENABLE_CTRL  = 8'h03;
    localparam REG_STREAM_MODE  = 8'h04;
    localparam REG_BW_LIMIT     = 8'h05;
    localparam REG_STATUS       = 8'h10;
    localparam REG_PROC_MODE    = 8'h20;
    localparam REG_MODULATION   = 8'h21;
    localparam REG_FILTER_BW    = 8'h22;
    localparam REG_CLK_GATING   = 8'h23;
    localparam REG_THERMAL_SCALE= 8'h24;
    localparam REG_RESOURCE_OPT = 8'h25;
    localparam REG_POWER_PROF   = 8'h26;

    localparam REG_SRC_MAC_LO   = 8'h30;
    localparam REG_SRC_MAC_HI   = 8'h31;
    localparam REG_DST_MAC_LO   = 8'h32;
    localparam REG_DST_MAC_HI   = 8'h33;
    localparam REG_SRC_IP       = 8'h34;
    localparam REG_DST_IP       = 8'h35;
    localparam REG_UDP_PORTS    = 8'h36;

    // ========================================================================
    // SPI Transaction FSM
    // ========================================================================
    
    reg [5:0] bit_counter;
    reg [7:0] addr_shift;
    reg [31:0] data_shift_in;
    reg [31:0] data_shift_out;
    
    reg [31:0] freq_word_reg;
    reg [7:0]  gain_ctrl_reg;
    reg [3:0]  filter_sel_reg;
    reg        enable_ctrl_reg;
    reg        stream_mode_reg;
    reg [7:0]  bw_limit_reg;
    reg [2:0]  proc_mode_reg;
    reg [7:0]  mod_type_reg;
    reg [7:0]  filt_bw_reg;
    reg        clk_gate_reg;
    reg [7:0]  therm_scale_reg;
    reg        res_opt_reg;
    reg [7:0]  pwr_prof_reg;

    reg [31:0] src_mac_lo_reg;
    reg [15:0] src_mac_hi_reg;
    reg [31:0] dst_mac_lo_reg;
    reg [15:0] dst_mac_hi_reg;
    reg [31:0] src_ip_reg;
    reg [31:0] dst_ip_reg;
    reg [15:0] src_port_reg;
    reg [15:0] dst_port_reg;

    always @(posedge spi_clk or posedge spi_cs_n) begin
        if (spi_cs_n) begin
            bit_counter <= 6'd0;
            addr_shift <= 8'd0;
            data_shift_in <= 32'd0;
        end else begin
            if (bit_counter < 6'd8) begin
                addr_shift <= {addr_shift[6:0], spi_mosi};
            end else if (bit_counter < 6'd40) begin
                data_shift_in <= {data_shift_in[30:0], spi_mosi};
            end

            if (bit_counter == 6'd7) begin
                case ({addr_shift[6:0], spi_mosi})
                    REG_STATUS: data_shift_out <= {16'd0, status_reg};
                    REG_FREQ_WORD: data_shift_out <= freq_word_reg;
                    default: data_shift_out <= 32'hDEADBEEF;
                endcase
            end else if (bit_counter >= 6'd8) begin
                data_shift_out <= {data_shift_out[30:0], 1'b0};
            end

            bit_counter <= bit_counter + 1;
        end
    end

    // Commit writes on CS deassertion or System Reset
    always @(posedge spi_cs_n or negedge rst_n) begin
        if (!rst_n) begin
            freq_word_reg   <= 32'h40000000;
            gain_ctrl_reg   <= 8'h80;
            filter_sel_reg  <= 4'd0;
            enable_ctrl_reg <= 1'b0;
            stream_mode_reg <= 1'b0;
            bw_limit_reg    <= 8'd80;
            proc_mode_reg   <= 3'd0;
            mod_type_reg    <= 8'd0;
            filt_bw_reg     <= 8'd0;
            clk_gate_reg    <= 1'b0;
            therm_scale_reg <= 8'd0;
            res_opt_reg     <= 1'b0;
            pwr_prof_reg    <= 8'd0;

            src_mac_lo_reg  <= 32'h00000001;
            src_mac_hi_reg  <= 16'h0200;
            dst_mac_lo_reg  <= 32'hFFFFFFFF;
            dst_mac_hi_reg  <= 16'hFFFF;
            src_ip_reg      <= 32'hC0A80002;
            dst_ip_reg      <= 32'hC0A80001;
            src_port_reg    <= 16'd10000;
            dst_port_reg    <= 16'd10001;
        end else begin
            if (bit_counter == 6'd40) begin
                case (addr_shift)
                    REG_FREQ_WORD:   freq_word_reg   <= data_shift_in;
                    REG_GAIN_CTRL:   gain_ctrl_reg   <= data_shift_in[7:0];
                    REG_FILTER_SEL:  filter_sel_reg  <= data_shift_in[3:0];
                    REG_ENABLE_CTRL: enable_ctrl_reg <= data_shift_in[0];
                    REG_STREAM_MODE: stream_mode_reg <= data_shift_in[0];
                    REG_BW_LIMIT:    bw_limit_reg    <= data_shift_in[7:0];
                    REG_PROC_MODE:   proc_mode_reg   <= data_shift_in[2:0];
                    REG_MODULATION:  mod_type_reg    <= data_shift_in[7:0];
                    REG_FILTER_BW:   filt_bw_reg     <= data_shift_in[7:0];
                    REG_CLK_GATING:  clk_gate_reg    <= data_shift_in[0];
                    REG_THERMAL_SCALE: therm_scale_reg <= data_shift_in[7:0];
                    REG_RESOURCE_OPT: res_opt_reg     <= data_shift_in[0];
                    REG_POWER_PROF:  pwr_prof_reg    <= data_shift_in[7:0];

                    REG_SRC_MAC_LO:  src_mac_lo_reg  <= data_shift_in;
                    REG_SRC_MAC_HI:  src_mac_hi_reg  <= data_shift_in[15:0];
                    REG_DST_MAC_LO:  dst_mac_lo_reg  <= data_shift_in;
                    REG_DST_MAC_HI:  dst_mac_hi_reg  <= data_shift_in[15:0];
                    REG_SRC_IP:      src_ip_reg      <= data_shift_in;
                    REG_DST_IP:      dst_ip_reg      <= data_shift_in;
                    REG_UDP_PORTS:   begin
                        src_port_reg <= data_shift_in[31:16];
                        dst_port_reg <= data_shift_in[15:0];
                    end
                    default: ;
                endcase
            end
        end
    end

    assign frequency_word = freq_word_reg;
    assign gain_control = gain_ctrl_reg;
    assign filter_select = filter_sel_reg;
    assign enable_control = enable_ctrl_reg;
    assign streaming_mode = stream_mode_reg;
    assign bandwidth_limit = bw_limit_reg;
    assign processing_mode = proc_mode_reg;
    assign modulation_type = mod_type_reg;
    assign filter_bandwidth = filt_bw_reg;
    assign clock_gating_en = clk_gate_reg;
    assign thermal_scaling = therm_scale_reg;
    assign resource_opt_en = res_opt_reg;
    assign power_profile = pwr_prof_reg;

    assign src_mac = {src_mac_hi_reg, src_mac_lo_reg};
    assign dst_mac = {dst_mac_hi_reg, dst_mac_lo_reg};
    assign src_ip = src_ip_reg;
    assign dst_ip = dst_ip_reg;
    assign src_port = src_port_reg;
    assign dst_port = dst_port_reg;

    assign spi_miso = (spi_cs_n) ? 1'bz : data_shift_out[31];

endmodule
`default_nettype wire
