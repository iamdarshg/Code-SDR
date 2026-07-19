# FPGA Processing Pipeline - Integration Specifications

## Overview
This document defines the V2 `LIF-MD6000-6JMG80I` hardware interface. Legacy
processing-pipeline material below it is retained only as implementation
background.

> V2 hardware uses `LIF-MD6000-6JMG80I`, a shared 100 MHz ADC/reference
> clock, and RGMII. Legacy UMG64/105 MHz/GMII names and constraints are not
> valid for the V2 PCB.

## Hardware Integration

### FPGA Pin Assignments

#### System Clocks
```
Signal / ball           Direction  Description
ADC_CLK / D9            Input      Shared AD9215/FPGA 100 MHz sampling clock
RGMII_RXC_FPGA / K6     Input      PHY receive clock
RGMII_TXC_FPGA / J7     Output     PHY transmit clock through 33 ohm damping
FPGA_RESET_N / H2       Input      CRESET_B, active low
```

#### ADC Interface (AD9215BCPZ-105)
```
Signal                  Direction  Description
ADC_D[9:0]              Input      10-bit parallel data, sampled on ADC_CLK
ADC_OR                  Input      AD9215 out-of-range indication
```

The AD9215 has no `adc_valid` output. Gateware must treat each ADC clock edge
as a sample and use `ADC_OR` only as an overload indicator.

#### SPI Interface (RP2040)
```
Pin Name              Bank  Direction  Description
spi_clk              I/O    Input      SPI clock from RP2040
spi_mosi             I/O    Input      SPI master out, slave in
spi_cs_n             I/O    Input      SPI chip select (active low)
spi_miso             I/O    Output     SPI master in, slave out
```

#### Ethernet RGMII Interface (KSZ9031RNX)
```
Pin Name              Bank  Direction  Description
rgmii_txd[3:0]       I/O    Output     DDR transmit data
rgmii_tx_en          I/O    Output     DDR transmit enable/control
rgmii_txc            I/O    Output     125 MHz transmit clock at 1 Gb/s
rgmii_rxd[3:0]       I/O    Input      DDR receive data
rgmii_rx_dv          I/O    Input      DDR receive data/control
rgmii_rxc            I/O    Input      PHY receive clock
mdc                  I/O    Output     PHY management clock
mdio                 I/O    Bidirectional PHY management data
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
SiT8209 100 MHz -> CDCLVC1104 -> AD9215 and FPGA ADC capture
                                -> LMX2592 reference
                                -> ADF4360-1 reference
KSZ9031 RGMII RXC/TXC domains -> FPGA RGMII DDR interface
```

#### Clock Constraints
```
- All clocks must be driven from low-jitter sources
- Clock skew must be < 100 ps across all clock domains
- ADC data/clock and each RGMII DDR domain must meet device timing
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
0x00    4       FFT metadata:
                [31:22] = Bin Index (0-1023)
                [21]    = Overflow Flag
                [20]    = Reserved
                [19:0]  = FFT Real Component (Upper 20 bits)
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

For 100 MSPS, 10-bit samples:
- Raw ADC rate: 1000 Mbps before framing
- Raw full-rate samples do not fit within GbE payload capacity
- FPGA must filter and resample/decimate (for example 80 MSPS)
- 80 MSPS x 10 bit is 800 Mbps before packet overhead
```

## System Integration

### Power Management

#### Power Supply Requirements
```
Supply Rail    Voltage  Capacity  Tolerance  Purpose
+1V2_CORE      1.2V     2.0A      +/-5%      FPGA core/GPLL and PHY core
+2V5_AUX       2.5V     0.5A      +/-5%      FPGA auxiliary
+3V3_DIG       3.3V     2.0A      +/-5%      FPGA I/O, RP2040 and PHY I/O
```

Each used FPGA supply group is fed through its own ferrite/filter island as
specified in `hardware/redesign/POWER_TREE.csv`. The unused D-PHY analog/PLL
groups are intentionally unpowered.

#### Power Sequencing
```
1. Start +3V3_DIG/VCCIO before or together with +1V2_CORE and +2V5_AUX.
2. Require every rail to rise monotonically and settle within +/-5%.
3. Keep `FPGA_RESET_N` low until the rails and configuration controller are
   ready.
4. Release `FPGA_RESET_N`, perform slave-SPI configuration, then require
   `FPGA_CDONE` high.
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
- One uninterrupted In1 ground plane under RF and high-speed signals
- Partition return currents physically; do not split the signal reference plane
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
ADC Sample Rate:          100 MSPS
Processing Latency:       < 10 µs
FFT Processing Time:      < 100 µs (1024-point)
Ethernet Throughput:      950 Mbps (theoretical)
Actual Throughput:        750 Mbps (with overhead)
```

#### Direct Streaming Mode
```
Raw ADC Rate:             1000 Mbps
Example resampled rate:   800 Mbps at 80 MSPS x 10 bit
Packet rate:              Must include Ethernet/IP/UDP overhead
Direct full-rate mode:    Not supported over 1 GbE
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
