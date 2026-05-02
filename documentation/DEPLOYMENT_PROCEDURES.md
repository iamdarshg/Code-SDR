# Deployment and Update Procedures

This document describes the process for building, configuring, and deploying the Code-SDR system.

## 1. Building the System

### FPGA Bitstream
The FPGA bitstream is built using Vivado or Quartus. We provide a master build script to prepare the environment:

```bash
python3 scripts/build_all.py
```

This will:
1. Generate `verilog/version_auto.v` with the current build information.
2. Create a `build/` directory with all necessary source files and TCL scripts.

### RP2040 Firmware
The RP2040 firmware is located in the `legacy/wideband-sdr-firmware` (Note: transition to RP2040 specific firmware is ongoing, currently using legacy dsPIC as reference).

## 2. Configuration Management

System settings are managed via JSON files in the `config/` directory.

- `config/default_config.json`: Standard default settings.
- `config/sdr_config_schema.json`: Validation schema for configurations.

Use the `wideband-sdr-software/config_manager.py` tool to validate or backup your configurations.

## 3. Programming the Hardware

Automated flashing scripts are provided in the `scripts/` directory.

### Flashing the FPGA
```bash
python3 scripts/flash_firmware.py fpga build/fpga_sdr_pipeline.bit
```

### Flashing the RP2040
```bash
python3 scripts/flash_firmware.py rp2040 firmware/rp2040_firmware.uf2
```

## 4. Reproducible Build Environment

A `Dockerfile` is provided in the root directory to create a consistent development and simulation environment.

```bash
docker build -t code-sdr-dev .
docker run -it code-sdr-dev
```

## 5. Update Procedures

To update the system:
1. Pull the latest code from the repository.
2. Run `python3 scripts/build_all.py` to generate new version information and build artifacts.
3. Use `scripts/flash_firmware.py` to deploy the new bitstream and firmware.
4. Verify the update by reading the `VERSION` register (address 0x11) via the RP2040 interface.
