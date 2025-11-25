# LIF-MD6000-6UMG64I FPGA Processing Pipeline Architecture

## Executive Summary

This document defines the comprehensive FPGA processing pipeline architecture for the LIF-MD6000-6UMG64I-based SDR system. The architecture implements a sophisticated multi-stage processing pipeline optimized for real-time SDR operations with high throughput and low latency.

## System Overview

### Processing Pipeline

```
ADC Input → Digital Downconversion → FFT Processing → Ethernet Output
     ↓              ↓                      ↓              ↓
10-bit        NCO Mixing            1024-pt FFT      UDP Packets
Parallel     + Filtering           + Windowing       + MAC Layer
```

### Key Performance Requirements

- **Sample Rate**: 105 MSPS (AD9215BCPZ-105)
- **ADC Interface**: 10-bit parallel with overflow detection
- **Processing Clock**: 100 MHz primary domain
- **Ethernet Clock**: 125 MHz GMII interface
- **FFT Size**: 1024-point complex FFT
- **Throughput**: Sustained 105 MSPS processing
- **Latency**: < 10 µs end-to-end processing

## Architecture Design

### 1. Clock Domain Management

#### Primary Clock Tree
```verilog
// Clock generation and distribution
module clock_manager (
    input  wire        clk_100m,        // Primary 100 MHz system clock
    input  wire        clk_125m_eth,    // 125 MHz Ethernet GMII clock
    input  wire        clk_105m_adc,    // 105 MHz ADC sampling clock
    output wire        clk_processing,  // 100 MHz processing domain
    output wire        clk_eth_tx,      // 125 MHz Ethernet TX
    output wire        clk_eth_rx,      // 125 MHz Ethernet RX
    output wire        clk_adc,         // 105 MHz ADC domain
    output wire        reset_n          // System reset (active low)
);
```

#### Cross-Domain Synchronization
```verilog
// Asynchronous FIFO for cross-domain data transfer
module async_fifo #(
    parameter WIDTH = 32,
    parameter DEPTH = 256
) (
    input  wire        wr_clk,          // Write clock domain
    input  wire        rd_clk,          // Read clock domain
    input  wire        wr_rst_n,        // Write reset (active low)
    input  wire        rd_rst_n,        // Read reset (active low)
    input  wire [WIDTH-1:0] din,         // Data input
    input  wire        wr_en,           // Write enable
    input  wire        rd_en,           // Read enable
    output wire [WIDTH-1:0] dout,        // Data output
    output wire        full,            // FIFO full flag
    output wire        empty            // FIFO empty flag
);
```

### 2. Data Path Width Optimization

#### Internal Data Representation
```verilog
// Data type definitions for consistent width management
`define ADC_WIDTH      10    // ADC parallel interface
`define PROCESS_WIDTH  32    // Internal processing width
`define FFT_IQ_WIDTH   24    // FFT complex data width
`define ETH_BYTE_WIDTH 8     // Ethernet byte interface

// Signed fixed-point arithmetic macros
`define SFI(x, i)      ((x) << (32-i))  // Signed fixed-point with i fractional bits
`define Q(x, i)        (x >> (32-i))    // Convert back to integer
```

### 3. Memory Architecture

#### Distributed Processing Buffers
```verilog
// Ping-pong buffer for continuous processing
module ping_pong_buffer #(
    parameter WIDTH = 32,
    parameter DEPTH = 1024
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [WIDTH-1:0] data_in,     // Input data
    input  wire        data_valid,      // Data valid flag
    input  wire        write_enable,    // Buffer write enable
    input  wire [9:0]  write_addr,      // Write address
    input  wire [9:0]  read_addr,       // Read address
    input  wire        read_enable,     // Buffer read enable
    output wire [WIDTH-1:0] data_out,    // Output data
    output wire        buffer_full,     // Buffer full flag
    output wire        buffer_empty     // Buffer empty flag
);
```

## Processing Pipeline Stages

### Stage 1: ADC Interface and Data Conditioning

#### ADC Interface Module
```verilog
module adc_interface (
    input  wire        clk_adc,         // 105 MHz ADC clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [9:0]  adc_data,        // 10-bit parallel ADC data
    input  wire        adc_valid,       // ADC data valid flag
    input  wire        adc_ovr,         // ADC overflow flag
    output wire [31:0] adc_samples,     // Processed ADC samples
    output wire        sample_valid,    // Sample valid flag
    output wire        overflow_detect  // Overflow detected flag
);
```

#### Data Conditioning Features
- **Parallel-to-serial conversion** for 10-bit samples
- **Overflow detection** with error flagging
- **Sample rate conversion** and synchronization
- **DC offset correction** (optional hardware implementation)

### Stage 2: Digital Downconversion (DDC)

#### NCO Implementation
```verilog
module nco_generator #(
    parameter WIDTH = 32,
    parameter ROM_DEPTH = 256
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [31:0] frequency_word,  // NCO frequency word
    input  wire        enable,          // NCO enable
    output wire [15:0] sine_out,        // Sine output (16-bit)
    output wire [15:0] cosine_out,      // Cosine output (16-bit)
    output wire        valid_out        // Output valid flag
);
```

#### Complex Mixer and Filtering
```verilog
module digital_downconverter #(
    parameter WIDTH = 32
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [31:0] adc_data,        // Input ADC data
    input  wire        data_valid,      // Input data valid
    input  wire [31:0] nco_sine,        // NCO sine component
    input  wire [31:0] nco_cosine,      // NCO cosine component
    output wire [31:0] i_component,     // I (in-phase) component
    output wire [31:0] q_component,     // Q (quadrature) component
    output wire        ddc_valid        // DDC output valid
);
```

#### CIC Decimation Filter
```verilog
module cic_decimator #(
    parameter INPUT_WIDTH = 32,
    parameter OUTPUT_WIDTH = 32,
    parameter STAGES = 3,
    parameter DECIMATION = 8
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [INPUT_WIDTH-1:0] data_in,    // Input data
    input  wire        data_valid,      // Input data valid
    output wire [OUTPUT_WIDTH-1:0] data_out,  // Decimated output
    output wire        output_valid     // Output data valid
);
```

### Stage 3: FFT Processing

#### FFT Processor Architecture
```verilog
module fft_processor #(
    parameter FFT_SIZE = 1024,
    parameter DATA_WIDTH = 24
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [DATA_WIDTH-1:0] real_in,   // Real input data
    input  wire [DATA_WIDTH-1:0] imag_in,   // Imaginary input data
    input  wire        data_valid,      // Input data valid
    output wire [DATA_WIDTH-1:0] real_out,  // Real output data
    output wire [DATA_WIDTH-1:0] imag_out,  // Imaginary output data
    output wire        fft_valid,       // FFT output valid
    output wire [10:0] fft_index        // FFT bin index
);
```

#### Windowing Implementation
```verilog
module hamming_window #(
    parameter WIDTH = 24,
    parameter FFT_SIZE = 1024
) (
    input  wire        clk,             // Processing clock
    input  wire        rst_n,           // Reset (active low)
    input  wire [WIDTH-1:0] data_in,     // Input data
    input  wire        data_valid,      // Input data valid
    output wire [WIDTH-1:0] data_out,    // Windowed output
    output wire        output_valid     // Output valid flag
);
```

### Stage 4: Ethernet Interface

#### MAC Layer Implementation
```verilog
module ethernet_mac #(
    parameter DATA_WIDTH = 32
) (
    // System interface
    input  wire        clk,             // 125 MHz system clock
    input  wire        rst_n,           // Reset (active low)
    
    // GMII interface
    output wire [7:0]  gmii_tx_d,       // GMII transmit data
    output wire        gmii_tx_en,      // GMII transmit enable
    output wire        gmii_tx_er,      // GMII transmit error
    input  wire [7:0]  gmii_rx_d,       // GMII receive data
    input  wire        gmii_rx_dv,      // GMII receive data valid
    input  wire        gmii_rx_er,      // GMII receive error
    
    // Data interface
    input  wire [DATA_WIDTH-1:0] packet_data,    // Packet data to transmit
    input  wire [15:0]  packet_len,               // Packet length
    input  wire        packet_valid,              // Packet valid flag
    output wire        packet_ack,                // Packet acknowledged
    
    // Status signals
    output wire        link_status,               // Ethernet link status
    output wire [31:0] packet_counter             // Packet counter
);
```

#### UDP/IP Protocol Stack
```verilog
module udp_ip_stack #(
    parameter DATA_WIDTH = 32
) (
    input  wire        clk,             // 125 MHz system clock
    input  wire        rst_n,           // Reset (active low)
    
    // Application data interface
    input  wire [DATA_WIDTH-1:0] app_data,   // Application data
    input  wire [15:0]  app_len,             // Application data length
    input  wire        app_valid,            // Application data valid
    output wire        app_ready,            // Application ready
    
    // UDP parameters
    input  wire [31:0] src_ip,               // Source IP address
    input  wire [31:0] dst_ip,               // Destination IP address
    input  wire [15:0] src_port,             // Source UDP port
    input  wire [15:0] dst_port,             // Destination UDP port
    
    // MAC interface
    output wire [DATA_WIDTH-1:0] mac_data,   // MAC layer data
    output wire [15:0]  mac_len,             // MAC packet length
    output wire        mac_valid             // MAC data valid
);
```

## Integration Architecture

### RP2040 Interface

#### Configuration and Control Interface
```verilog
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
    
    // Status signals
    input  wire [15:0] status_reg,        // System status register
    input  wire        pll_locked,        // PLL lock status
    input  wire        eth_link_status    // Ethernet link status
);
```

## Resource Utilization Estimates

### Logic Elements Usage
```
Total Available:     ~16,000 LE (LIF-MD6000-6UMG64I)
Stage 1 (ADC):         ~800 LE
Stage 2 (DDC):       ~3,200 LE
Stage 3 (FFT):       ~8,000 LE
Stage 4 (Ethernet):   ~3,000 LE
Control Logic:       ~1,000 LE
Estimated Total:    ~16,000 LE (100% utilization)
```

### Memory Blocks Usage
```
Total Available:     ~32 M4K blocks
ADC Buffers:            ~4 blocks
DDC FIR Filters:       ~8 blocks
FFT Buffers:          ~16 blocks
Ethernet TX/RX:        ~4 blocks
Estimated Total:     ~32 blocks (100% utilization)
```

### DSP Block Usage
```
Total Available:     ~12 DSP blocks
Complex Multipliers:   ~8 blocks (NCO mixing, FFT twiddle factors)
FIR Filters:           ~4 blocks (Decimation filters)
Estimated Total:     ~12 blocks (100% utilization)
```

## Performance Optimization Strategies

### 1. Pipeline Optimization
- **Pipelined Architecture**: Each processing stage fully pipelined
- **Register Balancing**: Equal pipeline depth across all stages
- **Critical Path Optimization**: Minimized combinational logic

### 2. Memory Optimization
- **Dual-Port RAM**: Simultaneous read/write operations
- **Circular Buffers**: Continuous streaming without gaps
- **Block RAM Sharing**: Efficient memory utilization

### 3. DSP Optimization
- **Fixed-Point Arithmetic**: Optimized for FPGA DSP blocks
- **Resource Sharing**: Reused multipliers for multiple operations
- **Parallel Processing**: Multiple processing elements

## Testing and Validation

### Simulation Testbenches
1. **ADC Interface Testbench**: Validates ADC data acquisition
2. **DDC Testbench**: Tests digital downconversion functionality
3. **FFT Testbench**: Validates FFT processing accuracy
4. **Ethernet Testbench**: Tests network communication
5. **Integration Testbench**: End-to-end system validation

### Timing Constraints
```tcl
# Clock constraints
create_clock -period 10.0 [get_ports clk_100m]    ;# 100 MHz system clock
create_clock -period 8.0  [get_ports clk_105m_adc] ;# 105 MHz ADC clock
create_clock -period 8.0  [get_ports clk_125m_eth] ;# 125 MHz Ethernet clock

# Input delays
set_input_delay -clock [get_clocks clk_105m_adc] -max 2.0 [get_ports adc_data*]
set_input_delay -clock [get_clocks clk_105m_adc] -min 1.0 [get_ports adc_data*]

# Output delays
set_output_delay -clock [get_clocks clk_125m_eth] -max 2.0 [get_ports gmii_*]
set_output_delay -clock [get_clocks clk_125m_eth] -min 1.0 [get_ports gmii_*]
```

## Documentation and Integration

### Interface Specifications
- **FPGA-RP2040 Interface**: Complete specification for system control
- **ADC Interface**: Detailed timing and signal specifications
- **Ethernet Interface**: GMII protocol implementation guide
- **Memory Interface**: Block RAM organization and access patterns

### Integration Guidelines
- **Power Sequencing**: Proper startup and shutdown procedures
- **Reset Management**: Synchronized reset distribution
- **Clock Distribution**: Optimal clock tree design
- **Pin Assignments**: Recommended FPGA pin allocations

## Conclusion

This FPGA processing pipeline architecture provides a comprehensive foundation for implementing high-performance SDR functionality on the LIF-MD6000-6UMG64I device. The design balances processing power, resource utilization, and flexibility while maintaining compatibility with the existing RP2040 control system.

The architecture supports:
- Real-time 105 MSPS processing
- Flexible digital downconversion
- High-resolution spectral analysis
- Gigabit Ethernet data transmission
- Seamless RP2040 integration

Next steps include detailed Verilog implementation and comprehensive testing validation.
