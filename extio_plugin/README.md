# WidebandSDR ExtIO Plugin

## Overview

This directory contains the Windows ExtIO plugin implementation for HDSDR and SDR# compatibility with the Wideband SDR hardware.

## Features

- **HDSDR/SDR# Compatibility**: Full ExtIO interface implementation
- **Wideband Frequency Range**: 1 MHz to 10 GHz support
- **Multiple Sample Rates**: From 1 kSPS to 105 MSPS
- **Gain Control**: 0-63 dB programmable gain
- **Real-time Streaming**: High-performance sample streaming
- **Multi-device Support**: Multiple SDR device enumeration
- **Python Integration**: Built-in integration with WidebandSDR Python driver

## Build Requirements

- Visual Studio 2019 or later
- CMake 3.15 or later
- Python 3.8+ development headers
- pybind11 library

## Building

1. **Configure with CMake**:
   ```bash
   mkdir build
   cd build
   cmake .. -G "Visual Studio 16 2019"
   ```

2. **Build**:
   ```bash
   cmake --build . --config Release
   ```

3. **The ExtIO.dll will be created in the build directory**

## Installation

1. **Copy ExtIO.dll** to your HDSDR/SDR# plugin directory
2. **Install Python Dependencies**:
   ```bash
   pip install pyusb numpy scipy
   ```
3. **Install Windows Driver** (if needed):
   - Use Zadig to install WinUSB driver for Wideband SDR
   - VID: 0x04D8, PID: 0x000A

## Usage

### HDSDR Setup

1. Open HDSDR
2. Go to Options → I/O Options
3. Select "WidebandSDR ExtIO Plugin" as the hardware
4. Configure frequency, sample rate, and gain as needed
5. Click "Start"

### SDR# Setup

1. Install SDR# (if not already installed)
2. Copy ExtIO.dll to the SDR# ExtIOPlugins directory
3. Restart SDR#
4. Select "Wideband SDR" from the source dropdown
5. Configure settings and start

## Plugin Functions

The plugin implements the standard ExtIO interface:

- `InitHW()` - Initialize hardware and return device information
- `StartHW()` - Start hardware operation at specified frequency  
- `StopHW()` - Stop hardware operation
- `SetHWLO()` - Set local oscillator frequency
- `GetHWLO()` - Get current LO frequency
- `GetStatus()` - Get hardware status
- `GetTune()` - Get tuned frequency
- `GetMode()` - Get current mode
- `GetGain()` - Get current gain
- `GetHWSR()` - Get current sample rate

## Device Information

**Hardware**: Wideband SDR (1 MHz - 10 GHz)  
**Manufacturer**: Wideband SDR Project  
**Channels**: 1  
**Frequency Range**: 1 MHz - 10 GHz  
**Sample Rate Range**: 1 kSPS - 105 MSPS  
**Gain Range**: 0 - 63 dB  

## Supported Modes

- AM (Amplitude Modulation)
- AMN (AM Narrow)
- AMS (AM Sync)
- LSB (Lower Sideband)
- USB (Upper Sideband)
- FM (Frequency Modulation)
- FMN (FM Narrow)
- WFM (Wideband FM)
- DIGL (Digital LSB)
- DIGU (Digital USB)

## Troubleshooting

### Device Not Detected

1. **Check USB Connection**: Ensure Wideband SDR is connected
2. **Install Driver**: Use Zadig to install WinUSB driver
3. **Check Permissions**: Run as administrator if needed
4. **Verify Python**: Ensure Python dependencies are installed

### Plugin Won't Load

1. **Check Architecture**: Ensure plugin matches HDSDR/SDR# architecture (x64/x86)
2. **Check Dependencies**: Ensure all required DLLs are present
3. **Check Python**: Verify Python installation and dependencies

### Poor Performance

1. **Sample Rate**: Reduce sample rate if experiencing dropouts
2. **Buffer Size**: Increase USB buffer size in device manager
3. **CPU Usage**: Close other applications to reduce CPU load

## Development

### Project Structure

```
extio_plugin/
├── CMakeLists.txt           # Build configuration
├── include/                 # Header files
│   ├── extio.h             # ExtIO interface definitions
│   └── wideband_sdr_extio.h # Plugin implementation header
├── src/                     # Source files
│   ├── extio_impl.cpp      # ExtIO interface implementation
│   ├── wideband_sdr_extio.cpp # Plugin class implementation
│   └── version_resource.rc # Windows version resource
└── build/                   # Build output directory
```

### Key Components

- **ExtIO Interface**: Standard ExtIO plugin interface for HDSDR/SDR#
- **WidebandSDRExtIO**: Main plugin class that wraps Python driver
- **Sample Buffering**: Thread-safe buffer management for streaming
- **Device Management**: USB device enumeration and control
- **Error Handling**: Comprehensive error handling and logging

### Extending the Plugin

The plugin is designed to be easily extended:

1. **Python Integration**: Add new Python driver calls
2. **Sample Processing**: Implement custom signal processing
3. **GUI Integration**: Add device control GUI
4. **Additional Hardware**: Support for multiple device types

## License

Copyright (C) 2025 Wideband SDR Project. All rights reserved.

## Version History

**Version 1.0.0** (2025-11-18)
- Initial ExtIO plugin implementation
- HDSDR and SDR# compatibility
- Complete device control interface
- Python integration layer
- Real-time streaming support
- Comprehensive error handling
