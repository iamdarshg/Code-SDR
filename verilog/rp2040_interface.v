// ============================================================================
// RP2040 Interface Module
// ============================================================================
// Configuration and control interface for FPGA system
// ============================================================================

`timescale 1ns/1ps

module rp2040_interface #(
    parameter ADDR_WIDTH = 16,
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

    // Dynamic Reconfiguration - Prompt 1.3 Implementation
    output wire [2:0]  processing_mode,   // Processing mode: 0=spectrum, 1=IQ streaming, 2=audio demod
    output wire [7:0]  modulation_type,   // Modulation/demodulation type (OOK/FSK/FM/AM/QAM/etc.)
    output wire [7:0]  filter_bandwidth,  // Filter bandwidth setting
    output wire        clock_gating_en,   // Enable clock gating for power saving
    output wire [7:0]  thermal_scaling,   // Thermal scaling control (0:auto, 1-255:scale factor)
    output wire        resource_opt_en,   // Resource optimization enable
    output wire [7:0]  power_profile,     // Power consumption profile
    
    // Status signals
    input  wire [15:0] status_reg,        // System status register
    input  wire        pll_locked,        // PLL lock status
    input  wire        eth_link_status    // Ethernet link status
);

    // ========================================================================
    // SPI register mapping
    // ========================================================================
    
    // Register addresses - Basic Control
    localparam REG_FREQ_WORD    = 8'h00;   // NCO frequency word
    localparam REG_GAIN_CTRL    = 8'h01;   // Gain control
    localparam REG_FILTER_SEL   = 8'h02;   // Filter selection
    localparam REG_ENABLE_CTRL  = 8'h03;   // Enable control
    localparam REG_STREAM_MODE  = 8'h04;   // Streaming mode
    localparam REG_BW_LIMIT     = 8'h05;   // Bandwidth limit

    // Register addresses - Dynamic Reconfiguration (Prompt 1.3)
    localparam REG_PROC_MODE    = 8'h20;   // Processing mode (spectrum/IQ/audio)
    localparam REG_MODULATION   = 8'h21;   // Modulation/demodulation type (OOK/FSK/FM/AM/QAM/etc.)
    localparam REG_FILTER_BW    = 8'h22;   // Filter bandwidth setting
    localparam REG_CLK_GATING   = 8'h23;   // Clock gating enable
    localparam REG_THERMAL_SCALE= 8'h24;   // Thermal scaling control
    localparam REG_RESOURCE_OPT = 8'h25;   // Resource optimization enable
    localparam REG_POWER_PROF   = 8'h26;   // Power consumption profile

    localparam REG_STATUS       = 8'h10;   // Status register (read-only)
    
    // ========================================================================
    // SPI state machine
    // ========================================================================
    
    localparam SPI_IDLE    = 2'd0;
    localparam SPI_ADDR    = 2'd1;
    localparam SPI_DATA    = 2'd2;
    
    reg [1:0] spi_state;
    reg [7:0] spi_addr;
    reg [DATA_WIDTH-1:0] spi_data_in;
    reg [DATA_WIDTH-1:0] spi_data_out;
    reg [3:0] spi_bit_counter;
    reg spi_data_out_valid;
    
    // SPI shift register
    always @(posedge spi_clk or negedge spi_cs_n) begin
        if (!spi_cs_n) begin
            spi_state <= SPI_IDLE;
            spi_addr <= 8'd0;
            spi_data_in <= 32'd0;
            spi_bit_counter <= 4'd0;
            spi_data_out_valid <= 1'b0;
        end else begin
            case (spi_state)
                SPI_IDLE: begin
                    // Wait for CS assertion and capture address
                    spi_addr <= spi_mosi;
                    spi_state <= SPI_ADDR;
                    spi_bit_counter <= 4'd0;
                    spi_data_out_valid <= 1'b0;
                end
                
                SPI_ADDR: begin
                    // Capture address and prepare data
                    spi_bit_counter <= spi_bit_counter + 1;
                    if (spi_bit_counter == 4'd7) begin
                        spi_state <= SPI_DATA;
                        spi_bit_counter <= 4'd0;
                        // Prepare output data based on address
                        case (spi_addr)
                            REG_STATUS: spi_data_out <= {16'd0, status_reg};
                            default: spi_data_out <= 32'd0;
                        endcase
                        spi_data_out_valid <= 1'b1;
                    end
                end
                
                SPI_DATA: begin
                    // Shift data in/out
                    spi_bit_counter <= spi_bit_counter + 1;
                    if (spi_bit_counter < 4'd8) begin
                        spi_data_in <= {spi_data_in[30:0], spi_mosi};
                    end else if (spi_bit_counter < 4'd16) begin
                        spi_data_in <= {spi_data_in[30:0], spi_mosi};
                    end else if (spi_bit_counter < 4'd24) begin
                        spi_data_in <= {spi_data_in[30:0], spi_mosi};
                    end else begin
                        spi_data_in <= {spi_data_in[30:0], spi_mosi};
                        spi_state <= SPI_IDLE;
                    end
                    
                    // Shift out data
                    spi_data_out <= {spi_data_out[30:0], 1'b0};
                end
                
                default: spi_state <= SPI_IDLE;
            endcase
        end
    end
    
    // ========================================================================
    // Register write control
    // ========================================================================
    
    // Basic control registers
    reg [31:0] freq_word_reg;
    reg [7:0]  gain_ctrl_reg;
    reg [3:0]  filter_sel_reg;
    reg        enable_ctrl_reg;
    reg        stream_mode_reg;
    reg [7:0]  bw_limit_reg;

    // Dynamic Reconfiguration registers (Prompt 1.3)
    reg [2:0]  proc_mode_reg;
    reg [2:0]  freq_band_reg;
    reg [7:0]  demod_type_reg;
    reg        clk_gating_reg;
    reg [7:0]  thermal_scale_reg;
    reg        resource_opt_reg;
    reg [7:0]  power_prof_reg;

    // Write registers on SPI completion
    always @(posedge spi_clk or negedge spi_cs_n) begin
        if (!spi_cs_n) begin
            // Initialize all registers with safe defaults
            freq_word_reg <= 32'h40000000;    // Default frequency
            gain_ctrl_reg <= 8'h80;           // Default gain
            filter_sel_reg <= 4'd0;            // Default filter
            enable_ctrl_reg <= 1'b0;           // Disabled by default
            stream_mode_reg <= 1'b0;           // Processed mode default
            bw_limit_reg <= 8'd80;             // 80% bandwidth limit
            proc_mode_reg <= 3'd0;             // Spectrum mode default
            freq_band_reg <= 3'd0;             // HF band default
            demod_type_reg <= 8'd0;            // AM default
            clk_gating_reg <= 1'b0;            // Clock gating disabled
            thermal_scale_reg <= 8'd0;         // Auto thermal scaling
            resource_opt_reg <= 1'b0;          // Resource optimization disabled
            power_prof_reg <= 8'd0;            // Power profile 0 (full performance)
        end else begin
            if (spi_state == SPI_DATA && spi_bit_counter == 4'd31) begin
                // Write to appropriate register based on address
                case (spi_addr)
                    // Basic control registers
                    REG_FREQ_WORD:   freq_word_reg <= spi_data_in;
                    REG_GAIN_CTRL:   gain_ctrl_reg <= spi_data_in[7:0];
                    REG_FILTER_SEL:  filter_sel_reg <= spi_data_in[3:0];
                    REG_ENABLE_CTRL: enable_ctrl_reg <= spi_data_in[0];
                    REG_STREAM_MODE: stream_mode_reg <= spi_data_in[0];
                    REG_BW_LIMIT:    bw_limit_reg <= spi_data_in[7:0];

                    // Dynamic Reconfiguration registers (Prompt 1.3)
                    REG_PROC_MODE:    proc_mode_reg <= spi_data_in[2:0];
                    REG_MODULATION:   demod_type_reg <= spi_data_in[7:0];    // demod_type_reg now holds modulation type
                    REG_FILTER_BW:    thermal_scale_reg <= spi_data_in[7:0]; // thermal_scale_reg repurposed for filter bandwidth
                    REG_CLK_GATING:   clk_gating_reg <= spi_data_in[0];
                    REG_THERMAL_SCALE: freq_band_reg <= spi_data_in[7:0];    // freq_band_reg repurposed for thermal scaling
                    REG_RESOURCE_OPT: resource_opt_reg <= spi_data_in[0];
                    REG_POWER_PROF:   power_prof_reg <= spi_data_in[7:0];
                endcase
            end
        end
    end
    
    // ========================================================================
    // Output assignments
    // ========================================================================

    // Basic control signals
    assign frequency_word = freq_word_reg;
    assign gain_control = gain_ctrl_reg;
    assign filter_select = filter_sel_reg;
    assign enable_control = enable_ctrl_reg;
    assign streaming_mode = stream_mode_reg;
    assign bandwidth_limit = bw_limit_reg;

    // Dynamic Reconfiguration signals (Prompt 1.3)
    assign processing_mode = proc_mode_reg;
    assign modulation_type = demod_type_reg;         // MODULATION register (OOK/FSK/FM/AM/QAM/etc.)
    assign filter_bandwidth = thermal_scale_reg;     // FILTER_BW register (filter bandwidth)
    assign clock_gating_en = clk_gating_reg;         // CLK_GATING register
    assign thermal_scaling = freq_band_reg;          // THERMAL_SCALE register (repurposed freq_band_reg)
    assign resource_opt_en = resource_opt_reg;       // RESOURCE_OPT register
    assign power_profile = power_prof_reg;           // POWER_PROF register

    // SPI MISO output (tri-state when not active)
    assign spi_miso = (spi_cs_n || !spi_data_out_valid) ? 1'bz : spi_data_out[31];

endmodule
