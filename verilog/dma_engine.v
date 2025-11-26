// ============================================================================
// DMA Engine for Ethernet Interface
// ============================================================================
// Scatter/gather DMA with bandwidth limiting for SDR data streaming
// ============================================================================

`timescale 1ns/1ps

module dma_engine #(
    parameter DATA_WIDTH = 32,
    parameter MAX_DESCRIPTORS = 16,
    parameter MAX_PACKET_SIZE = 65536  // Enhanced for 900+ Mbps IQ streaming
) (
    // System interface
    input  wire        clk,               // System clock
    input  wire        rst_n,             // Reset (active low)

    // Ethernet interface
    input  wire [DATA_WIDTH-1:0] eth_data_in,    // Data from Ethernet MAC
    input  wire        eth_valid,         // Data valid from Ethernet
    output wire        eth_ack,           // Acknowledge to Ethernet

    output wire [DATA_WIDTH-1:0] eth_data_out,   // Data to Ethernet MAC
    output wire        eth_valid_out,     // Data valid to Ethernet
    input  wire        eth_ack_in,        // Acknowledge from Ethernet

    // Memory interface
    output wire [15:0] mem_addr,          // Memory address
    output wire [DATA_WIDTH-1:0] mem_data_out, // Data to memory
    output wire        mem_wr_en,         // Memory write enable
    input  wire [DATA_WIDTH-1:0] mem_data_in,  // Data from memory
    output wire        mem_rd_en,         // Memory read enable

    // Control interface
    input  wire [15:0] ctrl_addr,         // Control register address
    input  wire [DATA_WIDTH-1:0] ctrl_data_in,  // Control data input
    output wire [DATA_WIDTH-1:0] ctrl_data_out, // Control data output
    input  wire        ctrl_wr_en,        // Control write enable
    input  wire        ctrl_rd_en,        // Control read enable

    // Status signals
    output wire        dma_busy,          // DMA engine busy
    output wire        dma_error,         // DMA error occurred
    output wire        dma_complete,      // DMA transfer complete

    // Interrupt signals
    output wire        interrupt_tx,      // TX interrupt
    output wire        interrupt_rx       // RX interrupt
);

    // ========================================================================
    // DMA Control Registers
    // ========================================================================

    // Status register (read-only)
    reg         dma_enable;
    reg         dma_tx_enable;
    reg         dma_rx_enable;
    reg         bandwidth_limit;
    reg [15:0]  bandwidth_limit_value;
    reg [7:0]   interrupt_mask;

    // Descriptor management
    reg [15:0]  descriptor_base_addr;    // Base address of descriptor table
    reg [7:0]   current_descriptor;      // Current descriptor being processed
    reg [7:0]   total_descriptors;       // Total number of active descriptors

    // Transfer statistics
    reg [31:0]  bytes_transferred_rx;
    reg [31:0]  bytes_transferred_tx;
    reg [15:0]  packets_received;
    reg [15:0]  packets_transmitted;

    // ========================================================================
    // Descriptor Table
    // ========================================================================

    // Descriptor structure (8 words per descriptor)
    // Word 0: Control flags and status
    // Word 1: Source address (for TX) or destination address (for RX)
    // Word 2: Packet length in bytes
    // Word 3: Next descriptor pointer
    // Word 4-7: Reserved

    reg [DATA_WIDTH-1:0] descriptor_table [0:MAX_DESCRIPTORS*8-1];

    // ========================================================================
    // Scatter/Gather Descriptor Structure
    // ========================================================================

    typedef struct packed {
        logic [15:0] control;      // Control flags
        logic [15:0] status;       // Status flags
        logic [31:0] buffer_addr;  // Buffer address
        logic [15:0] length;       // Transfer length
        logic [15:0] next_desc;    // Next descriptor offset
        logic [47:0] reserved;     // Reserved for future use
    } dma_descriptor_t;

    // Control flag definitions
    localparam DESC_OWN      = 16'h8000;  // Ownership (1=DMA, 0=CPU)
    localparam DESC_VALID    = 16'h4000;  // Descriptor valid
    localparam DESC_SOP      = 16'h2000;  // Start of packet
    localparam DESC_EOP      = 16'h1000;  // End of packet
    localparam DESC_INTR     = 16'h0800;  // Generate interrupt
    localparam DESC_WRAP     = 16'h0400;  // Wrap to beginning

    // ========================================================================
    // DMA State Machines
    // ========================================================================

    localparam DMA_IDLE      = 4'd0;
    localparam DMA_FETCH_DESC= 4'd1;
    localparam DMA_READ_MEM  = 4'd2;
    localparam DMA_TX_ETH    = 4'd3;
    localparam DMA_RX_ETH    = 4'd4;
    localparam DMA_WRITE_MEM = 4'd5;
    localparam DMA_UPDATE_DESC= 4'd6;
    localparam DMA_COMPLETE  = 4'd7;
    localparam DMA_ERROR     = 4'd8;

    reg [3:0] dma_state;
    reg [3:0] dma_rx_state;
    reg [3:0] dma_tx_state;

    reg [15:0] current_buffer_addr;
    reg [15:0] current_length;
    reg [15:0] bytes_remaining;
    reg [15:0] current_desc_ptr;
    reg         current_is_last;

    // Transfer buffers
    reg [DATA_WIDTH-1:0] tx_buffer [0:255];  // TX buffer
    reg [DATA_WIDTH-1:0] rx_buffer [0:255];  // RX buffer
    reg [7:0] tx_buffer_rd_ptr;
    reg [7:0] tx_buffer_wr_ptr;
    reg [7:0] rx_buffer_rd_ptr;
    reg [7:0] rx_buffer_wr_ptr;
    reg [7:0] tx_buffer_count;
    reg [7:0] rx_buffer_count;

    // Bandwidth limiting
    reg [31:0] bandwidth_counter;
    reg         bandwidth_throttle;

    // ========================================================================
    // Control Register Access
    // ========================================================================

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            dma_enable <= 1'b0;
            dma_tx_enable <= 1'b0;
            dma_rx_enable <= 1'b0;
            bandwidth_limit <= 1'b1;
            bandwidth_limit_value <= 16'h0040;  // 64 KB/s default
            interrupt_mask <= 8'h00;
            descriptor_base_addr <= 16'h0000;
            current_descriptor <= 8'h00;
            total_descriptors <= 8'h00;
        end else begin
            // Control register writes
            if (ctrl_wr_en) begin
                case (ctrl_addr[7:0])
                    8'h00: begin  // Control register
                        dma_enable         <= ctrl_data_in[0];
                        dma_tx_enable      <= ctrl_data_in[1];
                        dma_rx_enable      <= ctrl_data_in[2];
                        bandwidth_limit    <= ctrl_data_in[3];
                    end
                    8'h04: bandwidth_limit_value <= ctrl_data_in[15:0];
                    8'h08: interrupt_mask        <= ctrl_data_in[7:0];
                    8'h0C: descriptor_base_addr  <= ctrl_data_in[15:0];
                    8'h10: current_descriptor    <= ctrl_data_in[7:0];
                    8'h14: total_descriptors     <= ctrl_data_in[7:0];
                endcase
            end
        end
    end

    // Control register reads
    reg [DATA_WIDTH-1:0] ctrl_data_out_reg;
    always @(*) begin
        case (ctrl_addr[7:0])
            8'h00: ctrl_data_out_reg = {24'd0, interrupt_mask, 3'd0, bandwidth_limit, dma_rx_enable, dma_tx_enable, dma_enable};
            8'h04: ctrl_data_out_reg = {16'd0, bandwidth_limit_value};
            8'h08: ctrl_data_out_reg = {24'd0, interrupt_mask};
            8'h0C: ctrl_data_out_reg = {16'd0, descriptor_base_addr};
            8'h10: ctrl_data_out_reg = {24'd0, current_descriptor};
            8'h14: ctrl_data_out_reg = {24'd0, total_descriptors};
            8'h18: ctrl_data_out_reg = bytes_transferred_rx;   // RX bytes
            8'h1C: ctrl_data_out_reg = bytes_transferred_tx;   // TX bytes
            8'h20: ctrl_data_out_reg = {16'd0, packets_received};  // RX packets
            8'h24: ctrl_data_out_reg = {16'd0, packets_transmitted}; // TX packets
            default: ctrl_data_out_reg = 32'd0;
        endcase
    end
    assign ctrl_data_out = ctrl_data_out_reg;

    // ========================================================================
    // RX DMA State Machine
    // ========================================================================

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            dma_rx_state <= DMA_IDLE;
            rx_buffer_wr_ptr <= 8'd0;
            rx_buffer_count <= 8'd0;
            bytes_transferred_rx <= 32'd0;
            packets_received <= 16'd0;
        end else if (dma_enable && dma_rx_enable) begin
            case (dma_rx_state)
                DMA_IDLE: begin
                    if (eth_valid && (rx_buffer_count < 8'd240)) begin
                        dma_rx_state <= DMA_RX_ETH;
                        rx_buffer_wr_ptr <= rx_buffer_wr_ptr + 1;
                        rx_buffer[rx_buffer_wr_ptr] <= eth_data_in;
                        rx_buffer_count <= rx_buffer_count + 1;
                        bytes_transferred_rx <= bytes_transferred_rx + 4;
                    end
                end

                DMA_RX_ETH: begin
                    if (eth_valid && (rx_buffer_count < 8'd255)) begin
                        rx_buffer_wr_ptr <= rx_buffer_wr_ptr + 1;
                        rx_buffer[rx_buffer_wr_ptr] <= eth_data_in;
                        rx_buffer_count <= rx_buffer_count + 1;
                        bytes_transferred_rx <= bytes_transferred_rx + 4;
                    end else begin
                        // Buffer full or no more data - write to memory
                        dma_rx_state <= DMA_WRITE_MEM;
                        rx_buffer_rd_ptr <= 8'd0;
                        current_length <= {rx_buffer_count, 2'b00};  // Convert to bytes
                    end
                end

                DMA_WRITE_MEM: begin
                    if (rx_buffer_rd_ptr < rx_buffer_count) begin
                        // Write to memory
                        mem_addr <= current_buffer_addr + rx_buffer_rd_ptr;
                        mem_data_out <= rx_buffer[rx_buffer_rd_ptr];
                        mem_wr_en <= 1'b1;
                        rx_buffer_rd_ptr <= rx_buffer_rd_ptr + 1;
                    end else begin
                        dma_rx_state <= DMA_UPDATE_DESC;
                        mem_wr_en <= 1'b0;
                        packets_received <= packets_received + 1;
                    end
                end

                DMA_UPDATE_DESC: begin
                    // Update descriptor and prepare for next transfer
                    dma_rx_state <= DMA_IDLE;
                    rx_buffer_count <= 8'd0;
                    rx_buffer_wr_ptr <= 8'd0;
                end

                default: dma_rx_state <= DMA_IDLE;
            endcase
        end else begin
            dma_rx_state <= DMA_IDLE;
            rx_buffer_count <= 8'd0;
        end
    end

    // ========================================================================
    // TX DMA State Machine
    // ========================================================================

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            dma_tx_state <= DMA_IDLE;
            tx_buffer_rd_ptr <= 8'd0;
            tx_buffer_count <= 8'd0;
            bytes_transferred_tx <= 32'd0;
            packets_transmitted <= 16'd0;
        end else if (dma_enable && dma_tx_enable) begin
            case (dma_tx_state)
                DMA_IDLE: begin
                    if (current_descriptor < total_descriptors && !bandwidth_throttle) begin
                        dma_tx_state <= DMA_FETCH_DESC;
                        current_desc_ptr <= {current_descriptor, 3'b000};  // 8 words per descriptor
                    end
                end

                DMA_FETCH_DESC: begin
                    // Read descriptor from memory
                    mem_addr <= descriptor_base_addr + current_desc_ptr;
                    mem_rd_en <= 1'b1;

                    // Simulate descriptor read (in practice, would need multiple cycles)
                    current_buffer_addr <= mem_data_in[31:16];  // Buffer address from descriptor
                    current_length <= mem_data_in[15:0];       // Length from descriptor
                    current_is_last <= (current_descriptor == total_descriptors - 1);

                    dma_tx_state <= DMA_READ_MEM;
                    mem_rd_en <= 1'b0;
                end

                DMA_READ_MEM: begin
                    if (tx_buffer_count < 8'd255) begin
                        mem_addr <= current_buffer_addr;
                        mem_rd_en <= 1'b1;

                        // Read from memory and store in TX buffer
                        tx_buffer[tx_buffer_wr_ptr] <= mem_data_in;
                        tx_buffer_wr_ptr <= tx_buffer_wr_ptr + 1;
                        tx_buffer_count <= tx_buffer_count + 1;
                        current_buffer_addr <= current_buffer_addr + 4;
                        current_length <= current_length - 4;

                        if (current_length <= 4) begin
                            mem_rd_en <= 1'b0;
                            dma_tx_state <= DMA_TX_ETH;
                            tx_buffer_rd_ptr <= 8'd0;
                        end
                    end
                end

                DMA_TX_ETH: begin
                    if (tx_buffer_rd_ptr < tx_buffer_count && eth_ack_in) begin
                        eth_data_out <= tx_buffer[tx_buffer_rd_ptr];
                        eth_valid_out <= 1'b1;
                        tx_buffer_rd_ptr <= tx_buffer_rd_ptr + 1;
                        bytes_transferred_tx <= bytes_transferred_tx + 4;
                    end else if (tx_buffer_rd_ptr >= tx_buffer_count) begin
                        eth_valid_out <= 1'b0;
                        dma_tx_state <= DMA_UPDATE_DESC;
                        packets_transmitted <= packets_transmitted + 1;
                    end else begin
                        eth_valid_out <= 1'b0;
                    end
                end

                DMA_UPDATE_DESC: begin
                    if (current_is_last) begin
                        current_descriptor <= 8'd0;
                        dma_tx_state <= DMA_COMPLETE;
                    end else begin
                        current_descriptor <= current_descriptor + 1;
                        dma_tx_state <= DMA_IDLE;
                    end
                    tx_buffer_count <= 8'd0;
                    tx_buffer_wr_ptr <= 8'd0;
                end

                DMA_COMPLETE: begin
                    dma_tx_state <= DMA_IDLE;
                end

                default: dma_tx_state <= DMA_IDLE;
            endcase
        end else begin
            dma_tx_state <= DMA_IDLE;
            eth_valid_out <= 1'b0;
            tx_buffer_count <= 8'd0;
        end
    end

    // ========================================================================
    // Bandwidth Limiting
    // ========================================================================

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            bandwidth_counter <= 32'd0;
            bandwidth_throttle <= 1'b0;
        end else if (bandwidth_limit) begin
            if (bandwidth_counter >= {16'd0, bandwidth_limit_value}) begin
                bandwidth_throttle <= 1'b0;
                bandwidth_counter <= 32'd0;
            end else begin
                bandwidth_counter <= bandwidth_counter + 1;
                if (bandwidth_counter >= {16'd0, bandwidth_limit_value} * 3 / 4) begin
                    bandwidth_throttle <= 1'b1;
                end
            end
        end else begin
            bandwidth_throttle <= 1'b0;
            bandwidth_counter <= 32'd0;
        end
    end

    // ========================================================================
    // Ethernet Interface Signals
    // ========================================================================

    assign eth_ack = (dma_rx_state == DMA_RX_ETH) && (rx_buffer_count < 8'd240);

    // ========================================================================
    // Status and Interrupt Signals
    // ========================================================================

    assign dma_busy = (dma_rx_state != DMA_IDLE) || (dma_tx_state != DMA_IDLE);
    assign dma_complete = (dma_tx_state == DMA_COMPLETE);
    assign dma_error = 1'b0;  // No error handling implemented

    assign interrupt_rx = (packets_received[0] && interrupt_mask[0]);  // RX interrupt enable
    assign interrupt_tx = (packets_transmitted[0] && interrupt_mask[1]);  // TX interrupt enable

endmodule
