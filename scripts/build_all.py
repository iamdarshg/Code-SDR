#!/usr/bin/env python3
import subprocess
import os
import sys
import shutil

def run_command(command, cwd=None):
    print(f"Running: {command}")
    result = subprocess.run(command, shell=True, cwd=cwd)
    if result.returncode != 0:
        print(f"Command failed with return code {result.returncode}")
        sys.exit(1)

def main():
    # 1. Generate version files
    print("Generating version files...")
    run_command("python3 tools/build_system/generate_version.py")

    # 2. Prepare build directory
    build_dir = "build"
    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)
    os.makedirs(build_dir)

    # 3. Run synthesis (Vivado/Quartus depending on environment)
    # This is a placeholder since we don't have the actual tools in the sandbox
    # but we will copy the scripts and provide instructions.
    print("Preparing synthesis scripts...")
    shutil.copy("verilog/synthesize.tcl", build_dir)
    shutil.copy("verilog/synthesize_quartus.tcl", build_dir)

    # Copy all verilog files to build/verilog for a clean build environment
    verilog_build_dir = os.path.join(build_dir, "verilog")
    os.makedirs(verilog_build_dir)
    for f in os.listdir("verilog"):
        if f.endswith(".v") or f.endswith(".sdc") or f.endswith(".xdc"):
            shutil.copy(os.path.join("verilog", f), verilog_build_dir)

    print(f"Build environment prepared in {build_dir}/")
    print("To run synthesis, use Vivado or Quartus with the provided TCL scripts.")

if __name__ == "__main__":
    main()
