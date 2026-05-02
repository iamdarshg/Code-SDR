# Use an appropriate base image for FPGA tools or just a standard Ubuntu for simulation
FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-venv \
    git \
    iverilog \
    verilator \
    gtkwave \
    make \
    cmake \
    g++ \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the repository
COPY . .

# Install Python dependencies
RUN pip3 install --no-cache-dir -e wideband-sdr-software/
RUN pip3 install --no-cache-dir cocotb cocotb-bus cocotb-coverage pytest

# Default command
CMD ["bash"]
