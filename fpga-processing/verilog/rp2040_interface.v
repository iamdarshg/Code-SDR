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
    
    // Status signals
    input  wire [15:0] status_reg,        // System status register
    input  wire        pll_locked,        // PLL lock status
    input  wire        eth_link_status    // Ethernet link status
);

    // ========================================================================
    // SPI register mapping
    // ========================================================================
    
    // Register addresses
    localparam REG_FREQ_WORD    = 8'h00;   // NCO frequency word
    localparam REG_GAIN_CTRL    = 8'h01;   // Gain control
    localparam REG_FILTER_SEL   = 8'h02;   // Filter selection
    localparam REG_ENABLE_CTRL  = 8'h03;   // Enable control
    localparam REG_STREAM_MODE  = 8'h04;   // Streaming mode
    localparam REG_BW_LIMIT     = 8'h05;   // Bandwidth limit
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
    
    reg [31:0] freq_word_reg;
    reg [7:0]  gain_ctrl_reg;
    reg [3:0]  filter_sel_reg;
    reg        enable_ctrl_reg;
    reg        stream_mode_reg;
    reg [7:0]  bw_limit_reg;
    
    // Write registers on SPI completion
    always @(posedge spi_clk or negedge spi_cs_n) begin
        if (!spi_cs_n) begin
            freq_word_reg <= 32'h40000000;    // Default frequency
            gain_ctrl_reg <= 8'h80;           // Default gain
            filter_sel_reg <= 4'd0;            // Default filter
            enable_ctrl_reg <= 1'b0;           // Disabled by default
            stream_mode_reg <= 1'b0;           // Processed mode default
            bw_limit_reg <= 8'd80;             // 80% bandwidth limit
        end else begin
            if (spi_state == SPI_DATA && spi_bit_counter == 4'd31) begin
                // Write to appropriate register based on address
                case (spi_addr)
                    REG_FREQ_WORD:   freq_word_reg <= spi_data_in;
                    REG_GAIN_CTRL:   gain_ctrl_reg <= spi_data_in[7:0];
                    REG_FILTER_SEL:  filter_sel_reg <= spi_data_in[3:0];
                    REG_ENABLE_CTRL: enable_ctrl_reg <= spi_data_in[0];
                    REG_STREAM_MODE: stream_mode_reg <= spi_data_in[0];
                    REG_BW_LIMIT:    bw_limit_reg <= spi_data_in[7:0];
                endcase
            end
        end
    end
    
    // ========================================================================
    // Output assignments
    // ========================================================================
    
    assign frequency_word = freq_word_reg;
    assign gain_control = gain_ctrl_reg;
    assign filter_select = filter_sel_reg;
    assign enable_control = enable_ctrl_reg;
    assign streaming_mode = stream_mode_reg;
    assign bandwidth_limit = bw_limit_reg;
    
    // SPI MISO output (tri-state when not active)
    assign spi_miso = (spi_cs_n || !spi_data_out_valid) ? 1'bz : spi_data_out[31];

endmodule
