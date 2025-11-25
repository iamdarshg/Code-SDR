# FPGA Processing Pipeline - Integration Specifications

## Overview
This document provides comprehensive integration specifications for the LIF-MD6000-6UMG64I FPGA processing pipeline, including hardware interfaces, software integration, and system-level specifications.

## Hardware Integration

### FPGA Pin Assignments

#### System Clocks
```
Pin Name              Bank  Direction  Description
clk_100m             I/O    Input      Primary 100 MHz system clock
clk_105m_adc         I/O    Input      105 MHz ADC sampling clock
clk_125m_eth         I/O    Input      125 MHz Ethernet GMII clock
rst_n               I/O    Input      System reset (active low)
```

#### ADC Interface (AD9215BCPZ-105)
```
Pin Name              Bank  Direction  Description
adc_data[9:0]        I/O    Input      10-bit parallel ADC data
adc_valid            I/O    Input      ADC data valid flag
adc_ovr             I/O    Input      ADC overflow flag
```

#### SPI Interface (RP2040)
```
Pin Name              Bank  Direction  Description
spi_clk              I/O    Input      SPI clock from RP2040
spi_mosi             I/O    Input      SPI master out, slave in
spi_cs_n             I/O    Input      SPI chip select (active low)
spi_miso             I/O    Output     SPI master in, slave out
```

#### Ethernet GMII Interface (KSZ9031RNXCC)
```
Pin Name              Bank  Direction  Description
gmii_tx_d[7:0]       I/O    Output     GMII transmit data
gmii_tx_en           I/O    Output     GMII transmit enable
gmii_tx_er           I/O    Output     GMII transmit error
gmii_tx_clk          I/O    Output     GMII transmit clock
gmii_rx_d[7:0]       I/O    Input      GMII receive data
gmii_rx_dv           I/O    Input      GMII receive data valid
gmii_rx_er           I/O    Input      GMII receive error
gmii_rx_clk          I/O    Input      GMII receive clock
gmii_crs             I/O    Output     GMII carrier sense
gmii_col             I/O    Output     GMII collision detect
```

#### Status and Control Outputs
```
Pin Name              Bank  Direction  Description
pll_locked            I/O    Output     PLL lock status
eth_link_status       I/O    Output     Ethernet link status
system_status[15:0]   I/O    Output     System status register
packet_counter[31:0]  I/O    Output     Packet transmission counter
```

### Clock Distribution Network

#### Primary Clock Tree
```
External Oscillator → FPGA Clock Buffer → Global Clock Distribution
     100 MHz                                    ↓
                                              ↓
                                              ↓
                                       Processing Domain (100 MHz)
                                       ADC Domain (105 MHz)
                                       Ethernet Domain (125 MHz)
```

#### Clock Constraints
```
- All clocks must be driven from low-jitter sources
- Clock skew must be < 100 ps across all clock domains
- Clock-to-clock phase relationships must be deterministic
- Clock dividers/multipliers must maintain < 1% accuracy
```

### Reset Distribution

#### Reset Hierarchy
```
External Reset → FPGA Reset Controller → Distributed Resets
     (Active Low)                              ↓
                                             ↓
                                             ↓
                              Domain-specific reset generation
                              (synchronized to each clock domain)
```

#### Reset Timing
```
- External reset pulse width: > 100 ns
- Reset to first clock edge: > 10 clock cycles
- Reset deassertion must be synchronized to all domains
```

## Software Integration

### RP2040 Communication Protocol

#### SPI Register Map
```
Address    Name              Access  Description
0x00       FREQ_WORD         Write   NCO frequency word (32-bit)
0x01       GAIN_CTRL         Write   Gain control settings (8-bit)
0x02       FILTER_SEL        Write   Filter selection (4-bit)
0x03       ENABLE_CTRL       Write   System enable (1-bit)
0x04       STREAM_MODE       Write   Streaming mode select (1-bit)
0x05       BW_LIMIT          Write   Bandwidth limit setting (8-bit)
0x10       STATUS            Read    System status register (16-bit)
```

#### Status Register Bits
```
Bit 15-12: Reserved
Bit 11:     Ethernet link status (1=linked, 0=not linked)
Bit 10:     PLL lock status (1=locked, 0=unlocked)
Bit 9:      ADC overflow detected (1=overflow, 0=normal)
Bit 8:      FFT processing active (1=active, 0=idle)
Bit 7:      DDC processing active (1=active, 0=idle)
Bit 6:      ADC FIFO full (1=full, 0=not full)
Bit 5:      ADC FIFO empty (1=empty, 0=not empty)
Bit 4:      System enabled (1=enabled, 0=disabled)
Bit 3-0:    Reserved
```

### Computer Interface Protocol

#### UDP Packet Format
```
Ethernet Header:     14 bytes
IPv4 Header:         20 bytes
UDP Header:          8 bytes
Payload:            Variable
Total:              42+ bytes
```

#### Payload Structure (Processed Mode)
```
Offset  Size    Description
0x00    4       Timestamp (32-bit counter)
0x04    4       FFT bin index
0x08    4       Real component (24-bit padded)
0x0C    4       Imaginary component (24-bit padded)
```

#### Payload Structure (Direct Streaming Mode)
```
Offset  Size    Description
0x00    4       Timestamp (32-bit counter)
0x04    2       Sample counter (10-bit padded)
0x06    2       ADC data (10-bit signed)
```

#### Bandwidth Limiting
```
Bandwidth Limit Setting:
0x00:    0% (disabled)
0x01-0x0F: 10-90% bandwidth
0x64:    100% (full bandwidth)

Effective Data Rate = Full Rate × Bandwidth_Limit / 100

For 105 MSPS, 10-bit samples:
- Raw rate: 1050 Mbps
- With 80% limit: 840 Mbps
- UDP overhead: ~50 Mbps
- Net available: ~790 Mbps
```

## System Integration

### Power Management

#### Power Supply Requirements
```
Supply Rail    Voltage  Current  Tolerance  Purpose
VCCINT         1.1V     2.5A     ±5%        Core logic
VCCAUX         1.8V     0.5A     ±5%        Auxiliary
VCCO_3V3       3.3V     1.0A     ±5%        I/O banks
VCCO_2V5       2.5V     0.3A     ±5%        GMII I/O
```

#### Power Sequencing
```
1. Power on VCCINT and VCCAUX (simultaneously)
2. Wait 10ms for voltage stabilization
3. Enable VCCO rails
4. Enable external components (ADC, Ethernet PHY)
5. Release FPGA reset
6. Initialize software communication
```

### Thermal Management

#### Operating Conditions
```
Parameter       Minimum    Maximum    Units
Ambient Temp    -40        85         °C
FPGA Case Temp  -40        100        °C
Supply Voltage  -5%        +5%        Nominal
```

#### Heat Dissipation
```
- Total power dissipation: ~3.0W
- Requires heatsink with thermal resistance < 15°C/W
- Forced air cooling recommended for continuous operation
- Temperature monitoring via RP2040 ADC
```

### EMI/EMC Considerations

#### Layout Guidelines
```
- Minimum 4-layer PCB with dedicated ground plane
- Separate analog and digital ground planes
- Star-ground connection at FPGA
- Decoupling capacitors: 0.1µF + 10µF per supply rail
- Clock traces: 50Ω impedance, length matched
- Differential pairs: 100Ω impedance, length matched
```

#### Shielding Requirements
```
- Enclosure: Aluminum with RF shielding
- Cable shielding: Braid coverage > 90%
- Connector filtering: Common-mode chokes
- Grounding: Single-point grounding to chassis
```

## Performance Specifications

### Throughput Requirements

#### Processing Pipeline
```
ADC Sample Rate:          105 MSPS
Processing Latency:       < 10 µs
FFT Processing Time:      < 100 µs (1024-point)
Ethernet Throughput:      950 Mbps (theoretical)
Actual Throughput:        750 Mbps (with overhead)
```

#### Direct Streaming Mode
```
Raw ADC Rate:             1050 Mbps
UDP Overhead:             100 Mbps
Max Net Rate:             950 Mbps
Limited Rate (80%):       760 Mbps
Packet Loss Target:       < 0.01%
```

### Reliability Requirements

#### Mean Time Between Failures (MTBF)
```
FPGA Logic:               > 100,000 hours
Memory (Block RAM):       > 50,000 hours
I/O Interfaces:           > 50,000 hours
```

#### Error Handling
```
- ADC overflow detection and reporting
- Ethernet link monitoring and recovery
- CRC error detection and correction
- FIFO overflow/underflow protection
- Watchdog timer for system recovery
```

## Software Integration

### Host Computer Requirements

#### Minimum System Requirements
```
CPU:         Intel i5 or AMD equivalent (3.0 GHz)
Memory:      8 GB RAM minimum, 16 GB recommended
Network:     Gigabit Ethernet
OS:          Windows 10/11, Linux (Ubuntu 20.04+), macOS 11+
Driver:      Raw socket access for UDP reception
```

#### Recommended Configuration
```
CPU:         Intel i7 or AMD equivalent (3.5+ GHz)
Memory:      32 GB RAM
Network:     10 Gigabit Ethernet (for highest performance)
Storage:     SSD with > 1 GB/s write speed
```

### Development Environment

#### Required Libraries
```
- Python 3.8+ with NumPy, SciPy
- UDP socket programming (built-in)
- Real-time data processing capabilities
- FFT libraries (FFTW, NumPy FFT)
- Signal processing libraries (SciPy Signal)
```

#### API Interface
```python
class SDRInterface:
    def __init__(self, ip_address="192.168.0.2", port=10000):
        # Initialize connection
        
    def set_frequency(self, frequency_hz):
        # Set NCO frequency
        
    def enable_streaming(self, mode="processed", bandwidth_limit=80):
        # Enable data streaming
        
    def read_samples(self, count=1024):
        # Read processed samples
        
    def read_fft_data(self, count=1024):
        # Read FFT data
```

## Testing and Validation

### Hardware Testing
```
1. Power-up and reset sequence validation
2. Clock generation and distribution verification
3. I/O timing and signal integrity testing
4. ADC interface and data acquisition testing
5. Ethernet communication and protocol validation
6. RP2040 SPI communication testing
```

### Software Testing
```
1. UDP packet reception and parsing
2. Real-time data processing performance
3. FFT computation accuracy validation
4. Bandwidth limiting functionality
5. Error handling and recovery testing
6. Multi-platform compatibility testing
```

### Integration Testing
```
1. End-to-end data path validation
2. Performance under maximum load
3. Continuous operation reliability
4. Temperature and power stress testing
5. EMI/EMC compliance testing
6. Field deployment and validation
```

## Deployment Considerations

### Installation Requirements
```
- Environmental: Temperature controlled (-10°C to +50°C)
- Power: Stable power supply with UPS backup
- Network: Dedicated Gigabit Ethernet segment
- Monitoring: Temperature and performance monitoring
- Maintenance: Access for firmware updates and servicing
```

### Configuration Management
```
- FPGA bitstream versioning and configuration
- RP2040 firmware management
- Software driver compatibility matrix
- Hardware revision tracking
- Field configuration backup and recovery
```

## Troubleshooting Guide

### Common Issues and Solutions
```
Issue: No Ethernet link
Solution: Check cable connection, verify IP configuration

Issue: High packet loss
Solution: Reduce bandwidth limit, check network hardware

Issue: ADC overflow
Solution: Adjust gain settings, check input signal level

Issue: FFT processing errors
Solution: Verify clock frequencies, check timing constraints

Issue: RP2040 communication timeout
Solution: Reset system, verify SPI connections
```

This integration specification provides the comprehensive framework for successful deployment and operation of the FPGA processing pipeline within the broader SDR system architecture.
