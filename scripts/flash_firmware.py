#!/usr/bin/env python3
import sys
import subprocess
import os

def flash_fpga(bitstream_path):
    print(f"Flashing FPGA with {bitstream_path}...")
    # Example using OpenFPGA or OpenOCD
    # subprocess.run(["openocd", "-f", "interface/picoprobe.cfg", "-f", "target/rp2040.cfg", "-c", f"program {bitstream_path} verify reset exit"])
    print("FPGA flashed successfully (simulated).")

def flash_rp2040(firmware_path):
    print(f"Flashing RP2040 with {firmware_path}...")
    # Example using picotool
    # subprocess.run(["picotool", "load", firmware_path, "-x"])
    print("RP2040 flashed successfully (simulated).")

def main():
    if len(sys.argv) < 3:
        print("Usage: flash_firmware.py <target> <file>")
        print("Targets: fpga, rp2040")
        sys.exit(1)

    target = sys.argv[1]
    file_path = sys.argv[2]

    if not os.path.exists(file_path):
        print(f"Error: {file_path} not found.")
        sys.exit(1)

    if target == "fpga":
        flash_fpga(file_path)
    elif target == "rp2040":
        flash_rp2040(file_path)
    else:
        print(f"Unknown target: {target}")
        sys.exit(1)

if __name__ == "__main__":
    main()
