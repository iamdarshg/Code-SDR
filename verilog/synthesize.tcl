# ============================================================================
# FPGA Synthesis Script for LIF-MD6000-6UMG64I
# ============================================================================
# Complete synthesis flow for the SDR processing pipeline
# ============================================================================

# Create project
create_project -name "fpga_sdr_pipeline" -part "LIF-MD6000-6UMG64I"

# Set project properties
set_property target_language Verilog [current_project]

# Add source files
add_files fpga_processing_pipeline.v
add_files clock_manager.v
add_files adc_interface.v
add_files async_fifo.v
add_files digital_downconverter.v
add_files compensation_filter.v
add_files adaptive_gain_scaler.v
add_files average_power_detector.v
add_files nco_generator.v
add_files cic_decimator.v
add_files hamming_window.v
add_files fft_processor.v
add_files udp_ip_stack.v
add_files ethernet_mac.v
add_files rp2040_interface.v

# Add constraints
add_files fpga_timing_constraints.xdc

# Set synthesis properties
set_property synth_design.max_bram_utilization 90 [current_project]
set_property synth_design.max_dsp_utilization 80 [current_project]
set_property synth_design.max_uram_utilization 80 [current_project]

# Synthesis
synth_design -top fpga_processing_pipeline -part LIF-MD6000-6UMG64I

# Report resource utilization
report_utilization -file synth_utilization.rpt

# Report timing
report_timing -file synth_timing.rpt

# Optimize design
opt_design

# Place and route
place_design

route_design

# Generate bitstream
write_bitstream -file fpga_sdr_pipeline.bit

# Generate reports
report_timing_summary -file timing_summary.rpt
report_power -file power_analysis.rpt
report_io -file io_analysis.rpt

puts "Synthesis completed successfully!"
