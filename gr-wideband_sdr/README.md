# GNU Radio Wideband SDR OOT Module

This is an Out-Of-Tree (OOT) module for GNU Radio that provides integration with the Wideband SDR device.

## Overview

The Wideband SDR GNU Radio module provides real-time I/Q sample streaming from the Wideband SDR hardware, supporting frequencies from 1 MHz to 10 GHz with configurable sample rates and gain control.

## Features

- **Wideband Coverage**: 1 MHz - 10 GHz frequency range
- **High Performance**: Up to 105 MSPS sample rate
- **Real-time Streaming**: USB 2.0 High-Speed interface
- **GNU Radio Integration**: Native GNU Radio Companion blocks
- **Python Bindings**: Complete PyBind11 integration
- **Configurable Gain**: 0-63 dB LNA gain control
- **Built-in DSP**: DC offset correction and filtering

## Installation

### Prerequisites

- GNU Radio 3.8 or later
- Python 3.8 or later
- CMake 3.8.2 or later
- C++11 compatible compiler
- PyBind11
- Boost libraries

### Building

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

### Python Installation

```bash
cd build
make python-install
```

## Usage

### GNU Radio Companion

1. Launch GNU Radio Companion
2. Find the "Wideband SDR Source" block under the "Wideband SDR" category
3. Configure parameters (frequency, sample rate, gain)
4. Connect to FFT Sink or Waterfall display

### Python API

```python
import gnuradio.wideband_sdr as wideband_sdr

# Create source block
source = wideband_sdr.source(sample_rate=2e6, frequency=100e6, gain=30)

# Configure device
source.set_frequency(95.5e6)  # FM radio frequency
source.set_gain(45)           # Increase gain for weak signals

# Start streaming
source.start_streaming()

# Read samples
samples = source.read_samples(1024)
print(f"Received {len(samples)} samples")

# Get statistics
stats = source.get_statistics()
print(f"Samples processed: {stats['samples_processed']}")

source.stop_streaming()
```

### Flowgraph Example

```python
#!/usr/bin/env python3

import numpy as np
from gnuradio import gr
import gnuradio.wideband_sdr as wideband_sdr
from gnuradio.wxgui import sink_c
import wx

class wideband_sdr_spectrum_analyzer(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self, "Wideband SDR Spectrum Analyzer")

        # Create Wideband SDR source
        self.sdr_source = wideband_sdr.source(sample_rate=10e6, 
                                             frequency=100e6, 
                                             gain=30)

        # Create spectrum sink
        self.spectrum_sink = sink_c(
            sample_rate=10e6,
            fft_size=1024,
            center_freq=100e6,
            ref_scale=1.0,
            y_per_div=10,
            y_divs=10,
            fc_pos=0
        )

        # Connect blocks
        self.connect(self.sdr_source, self.spectrum_sink)

if __name__ == '__main__':
    app = wx.App()
    tb = wideband_sdr_spectrum_analyzer()
    tb.start()
    app.MainLoop()
```

## Block Parameters

### Wideband SDR Source

- **Sample Rate**: Sample rate in samples per second (1 kSPS - 105 MSPS)
- **Frequency**: Center frequency in Hz (1 MHz - 10 GHz)
- **RF Gain**: Front-end LNA gain in dB (0-63 dB)
- **Device Index**: USB device index (0-based, for multiple devices)

## Advanced Features

### Device Information

```python
info = source.get_device_info()
print(f"Device: {info['model']}")
print(f"Firmware: {info['firmware_version']}")
print(f"Serial: {info['serial_number']}")
```

### Statistics and Monitoring

```python
stats = source.get_statistics()
print(f"Throughput: {stats['samples_processed']} samples")
print(f"Dropped: {stats['packets_dropped']} packets")
print(f"Buffer: {stats['buffer_utilization']}% utilization")
```

### Real-time Configuration

```python
# Change frequency during operation
source.set_frequency(433.92e6)  # 70cm amateur band

# Adjust gain dynamically
source.set_gain(60)  # Higher gain for weak signals

# Monitor streaming status
if source.is_streaming():
    print("Device is streaming")
```

## Development

### Building from Source

```bash
# Clone repository
git clone https://github.com/iamdarshg/Code-SDR.git
cd Code-SDR/gr-wideband_sdr

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# Install
sudo make install

# Python bindings
make python-install
```

### Testing

```bash
# Run unit tests
ctest

# Run integration tests
python3 test_integration.py

# Test Python bindings
python3 test_python_bindings.py
```

## Troubleshooting

### Device Not Detected

1. Check USB connection
2. Verify device permissions on Linux:
   ```bash
   sudo usermod -a -G plugdev $USER
   ```
3. Check device enumeration:
   ```bash
   lsusb | grep 04D8
   ```

### Compilation Errors

1. Ensure GNU Radio development headers are installed
2. Check CMake configuration:
   ```bash
   cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
   ```
3. Verify compiler version compatibility

### Runtime Issues

1. Check sample rate compatibility with hardware
2. Verify antenna connection
3. Monitor system resources during high sample rates
4. Check buffer overflow statistics

## Performance Optimization

### High Sample Rates

- Use dedicated USB 3.0+ controller
- Increase kernel buffer sizes:
  ```bash
  sudo sysctl -w net.core.rmem_max=16777216
  ```
- Monitor USB bandwidth utilization

### Low Latency

- Configure real-time priorities
- Use dedicated CPU cores
- Optimize buffer sizes for your application

## Support

- **Documentation**: [Link to full documentation]
- **Issues**: [GitHub Issues]
- **Forum**: [Community Forum]
- **Email**: darsh.gupta@example.com

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- GNU Radio community for the excellent framework
- Wideband SDR Project contributors
- USB communication libraries
