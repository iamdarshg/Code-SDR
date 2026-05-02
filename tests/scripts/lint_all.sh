#!/bin/bash
# Basic elaboration check using iverilog
set -e
echo "Starting Elaboration Check..."
# Including all required files and dependencies
iverilog -g2012 -o /dev/null -I verilog \
  verilog/fpga_processing_pipeline.v \
  verilog/clock_manager.v \
  verilog/adc_interface.v \
  verilog/async_fifo.v \
  verilog/digital_downconverter.v \
  verilog/nco_generator.v \
  verilog/hamming_window.v \
  verilog/fft_processor.v \
  verilog/udp_ip_stack.v \
  verilog/ethernet_mac.v \
  verilog/rp2040_interface.v \
  verilog/compensation_filter.v \
  verilog/adaptive_gain_scaler.v \
  verilog/average_power_detector.v \
  -DSIMULATION
echo "Elaboration Check PASSED!"
