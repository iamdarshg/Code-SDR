# ============================================================================
# Quartus Synthesis Script for LIF-MD6000-6UMG64I Cyclone V
# ============================================================================
# High-performance synthesis flow optimized for maximum speed
# ============================================================================

# Create or open project
if {[project_exists fpga_sdr_pipeline]} {
    project_open fpga_sdr_pipeline
} else {
    project_new fpga_sdr_pipeline -family "Cyclone V" -part "LIF-MD6000-6UMG64I"
}

# Set project properties for high performance
set_global_assignment -name FAMILY "Cyclone V"
set_global_assignment -name DEVICE LIF-MD6000-6UMG64I
set_global_assignment -name TOP_LEVEL_ENTITY fpga_processing_pipeline
set_global_assignment -name PROJECT_OUTPUT_DIRECTORY output_files
set_global_assignment -name NUM_PARALLEL_PROCESSORS ALL

# Optimization settings for maximum performance (speed over area)
set_global_assignment -name OPTIMIZATION_MODE "HIGH PERFORMANCE (EFFORT = 3)"
set_global_assignment -name OPTIMIZE_TIMING "ON"
set_global_assignment -name OPTIMIZE_POWER "OFF"
set_global_assignment -name OPTIMIZE_FOR_SPEED "ON"
set_global_assignment -name OPTIMIZE_MULTI_CORNER_TIMING "ON"
set_global_assignment -name FINAL_PLACEMENT_OPTIMIZATION "ALWAYS"
set_global_assignment -name PHYSICAL_SYNTHESIS_EFFORT "EXTRA"
set_global_assignment -name ROUTER_TIMING_OPTIMIZATION_LEVEL "MAXIMUM"
set_global_assignment -name FITTER_EFFORT "AUTO FIT WITH AUTO SEED"
set_global_assignment -name SEED 1

# Resource utilization targets (allow high usage for performance)
set_global_assignment -name AUTO_PACKED_REGISTERS_STRATIXII "ON"
set_global_assignment -name REMOVE_REDUNDANT_LOGIC_CELLS "OFF"
set_global_assignment -name STATE_MACHINE_PROCESSING "AUTO"
set_global_assignment -name SYNTH_GATED_CLOCK_CONVERSION "OFF"
set_global_assignment -name STRATIX_OPTIMIZATION_TECHNIQUE SPEED

# Add Verilog source files
set_global_assignment -name VERILOG_FILE fpga_processing_pipeline.v
set_global_assignment -name VERILOG_FILE clock_manager.v
set_global_assignment -name VERILOG_FILE adc_interface.v
set_global_assignment -name VERILOG_FILE async_fifo.v
set_global_assignment -name VERILOG_FILE digital_downconverter.v
set_global_assignment -name VERILOG_FILE compensation_filter.v
set_global_assignment -name VERILOG_FILE adaptive_gain_scaler.v
set_global_assignment -name VERILOG_FILE average_power_detector.v
set_global_assignment -name VERILOG_FILE nco_generator.v
set_global_assignment -name VERILOG_FILE cic_decimator.v
set_global_assignment -name VERILOG_FILE hamming_window.v
set_global_assignment -name VERILOG_FILE fft_processor.v
set_global_assignment -name VERILOG_FILE udp_ip_stack.v
set_global_assignment -name VERILOG_FILE ethernet_mac.v
set_global_assignment -name VERILOG_FILE rp2040_interface.v

# Add timing constraints
set_global_assignment -name SDC_FILE fpga_timing_constraints.sdc

# Add PLL IP cores for clock generation
set_global_assignment -name QIP_FILE clocks/clocks.qip

# Compilation settings for maximum frequency targets
set_global_assignment -name OPTIMIZE_HOLD_TIMING "ALL PATHS"
set_global_assignment -name OPTIMIZE_MULTI_CORNER_TIMING "ALL CORNERS"
set_global_assignment -name TIMING_ANALYSIS_MODE "MULTICORNER TIMING ANALYSIS"

# DSP block optimization
set_global_assignment -name DSP_BALANCING "LOGIC ELEMENTS"
set_global_assignment -name DSP_BLOCK_BALANCING "SPEED OPTIMIZATION"

# Run analysis and synthesis with extra effort
execute_module -tool map -args "--effort=extra"
execute_module -tool fit -args {"--effort=extra" "--seed=1"}

# Generate optimization reports
create_timing_netlist -post_map
create_timing_netlist -post_fit

# Export design for timing analysis
export_assignments

# Run final timing analysis
update_timing_netlist

# Report generation
load_package report
load_report

# Generate comprehensive reports
set panel_names [get_report_panel_names]
foreach panel_name $panel_names {
    set panel_id [get_report_panel_id $panel_name]
    write_report_panel -file "reports/${panel_name}.rpt" $panel_id
}

# Resource utilization report
report_resources -file reports/resource_utilization.rpt

# Power analysis
report_power -file reports/power_analysis.rpt

# Generate bitstream
execute_module -tool asm

puts "Quartus Synthesis completed successfully for LIF-MD6000-6UMG64I!"
puts "Clock targets:"
puts "  System Clock: 600MHz"
puts "  FFT Clock: 1.2GHz"
puts "  Ethernet Clock: 250MHz"
puts "  ADC Clock: 105MHz"
puts ""
puts "Check timing reports for fMAX achievement."
