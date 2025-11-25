# ============================================================================
# FPGA Timing Constraints for LIF-MD6000-6UMG64I (Quartus SDC Format)
# ============================================================================
# High-performance timing constraints optimized for maximum speed
# ============================================================================

# Primary system clock constraints - Push to maximum frequency (target 600MHz)
create_clock -period 1.667 [get_ports clk_600m]        ;# 600 MHz system clock (target max for Cyclone V)
create_clock -period 9.5238 [get_ports clk_105m_adc]   ;# 105 MHz ADC sampling clock (fixed)
create_clock -period 4.0 [get_ports clk_250m_eth]      ;# 250 MHz Ethernet clock (UDP optimized)
create_clock -period 0.833 [get_ports clk_1200m_fft]   ;# 1.2 GHz FFT clock (maximum performance target)

# FFT dedicated clock domain - Maximum frequency constraints
create_clock -period 0.833 -name fft_core_clock [get_pins fft_processor/clk]
set_clock_groups -asynchronous -group [get_clocks clk_1200m_fft] -group [get_clocks clk_600m]
set_clock_groups -asynchronous -group [get_clocks clk_1200m_fft] -group [get_clocks clk_105m_adc]

# ADC input timing constraints (105MHz domain - fixed for ADC compatibility)
set_input_delay -clock [get_clocks clk_105m_adc] -max 2.0 [get_ports adc_data*]
set_input_delay -clock [get_clocks clk_105m_adc] -min 1.0 [get_ports adc_data*]
set_input_delay -clock [get_clocks clk_105m_adc] -max 1.0 [get_ports adc_valid]
set_input_delay -clock [get_clocks clk_105m_adc] -min 0.5 [get_ports adc_ovr]

# High-speed Ethernet timing constraints (250MHz domain for UDP optimization)
set_output_delay -clock [get_clocks clk_250m_eth] -max 1.5 [get_ports gmii_tx_*]
set_output_delay -clock [get_clocks clk_250m_eth] -min 0.5 [get_ports gmii_tx_*]
set_input_delay -clock [get_clocks clk_250m_eth] -max 1.5 [get_ports gmii_rx_*]
set_input_delay -clock [get_clocks clk_250m_eth] -min 0.5 [get_ports gmii_rx_*]

# SPI interface constraints (push to max for control)
create_clock -period 10.0 [get_ports spi_clk]         ;# 100 MHz SPI clock (increased from 50MHz)
set_input_delay -clock [get_clocks spi_clk] -max 3.0 [get_ports spi_mosi]
set_input_delay -clock [get_clocks spi_clk] -min 1.0 [get_ports spi_mosi]
set_output_delay -clock [get_clocks spi_clk] -max 3.0 [get_ports spi_miso]
set_output_delay -clock [get_clocks spi_clk] -min 1.0 [get_ports spi_miso]

# False path constraints for asynchronous domain crossings
set_false_path -from [get_clocks clk_105m_adc] -to [get_clocks clk_600m]
set_false_path -from [get_clocks clk_600m] -to [get_clocks clk_105m_adc]
set_false_path -from [get_clocks clk_250m_eth] -to [get_clocks clk_600m]
set_false_path -from [get_clocks clk_600m] -to [get_clocks clk_250m_eth]
set_false_path -from [get_clocks clk_1200m_fft] -to [get_clocks clk_600m]
set_false_path -from [get_clocks clk_1200m_fft] -to [get_clocks clk_105m_adc]

# Clock domain crossing maximum delays (optimized for speed)
set_max_delay -from [get_clocks clk_105m_adc] -to [get_clocks clk_600m] 5.0
set_max_delay -from [get_clocks clk_600m] -to [get_clocks clk_250m_eth] 2.0
set_max_delay -from [get_clocks clk_250m_eth] -to [get_clocks clk_600m] 2.0
set_max_delay -from [get_clocks clk_600m] -to [get_clocks clk_1200m_fft] 1.0

# Multi-cycle paths for high-throughput FFT processing (allow more cycles for timing)
set_multicycle_path -from [get_clocks clk_600m] -to [get_clocks clk_1200m_fft] -setup 2
set_multicycle_path -from [get_clocks clk_600m] -to [get_clocks clk_1200m_fft] -hold 1

# False paths for reset synchronization (allow timing violations on reset paths)
set_false_path -from [get_ports rst_n] -to [get_clocks clk_600m]
set_false_path -from [get_ports rst_n] -to [get_clocks clk_105m_adc]
set_false_path -from [get_ports rst_n] -to [get_clocks clk_250m_eth]
set_false_path -from [get_ports rst_n] -to [get_clocks clk_1200m_fft]

# I/O standards for high-speed operation (LIF-MD6000-6UMG64I)
set_instance_assignment -name IO_STANDARD "2.5V" -to clk_600m
set_instance_assignment -name IO_STANDARD "2.5V" -to clk_105m_adc
set_instance_assignment -name IO_STANDARD "2.5V" -to clk_250m_eth
set_instance_assignment -name IO_STANDARD "2.5V" -to clk_1200m_fft
set_instance_assignment -name IO_STANDARD "2.5V" -to rst_n

set_instance_assignment -name IO_STANDARD "2.5V" -to adc_data*
set_instance_assignment -name IO_STANDARD "2.5V" -to adc_valid
set_instance_assignment -name IO_STANDARD "2.5V" -to adc_ovr

set_instance_assignment -name IO_STANDARD "2.5V" -to spi_*
set_instance_assignment -name IO_STANDARD "2.5V" -to gmii_*

# High-speed I/O drive strength and slew rate
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to gmii_tx_en
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to gmii_tx_d*
set_instance_assignment -name SLEW_RATE 2 -to gmii_tx_*

# PLL configuration for clock generation (Cyclone V specific)
set_instance_assignment -name PLL_COMPENSATION_MODE DIRECT -to "*pll*"
set_instance_assignment -name USE_HIGH_SPEED_MODE true -to "*pll*"

# Location constraints for critical timing paths
set_instance_assignment -name PLL_LOCATION PLL_1 -to system_pll
set_instance_assignment -name GLOBAL_SIGNAL "GLOBAL CLOCK" -to clk_600m
set_instance_assignment -name GLOBAL_SIGNAL "GLOBAL CLOCK" -to clk_1200m_fft
set_instance_assignment -name GLOBAL_SIGNAL "GLOBAL CLOCK" -to clk_250m_eth

# Metastability protection for external inputs
set_instance_assignment -name SYNCHRONIZER_IDENTIFICATION "FORCED IF ASYNCHRONOUS" -to rst_n
set_instance_assignment -name SYNCHRONIZER_IDENTIFICATION "FORCED IF ASYNCHRONOUS" -to [get_ports *async*]

# Resource utilization constraints (optimized for speed, not area)
set_instance_assignment -name OPTIMIZE_FOR_SPEED true -to [get_cells *]
set_instance_assignment -name AUTO_PACKED_REGISTERS_STRATIXII ON -to [get_cells *fft*]
set_instance_assignment -name AUTO_PACKED_REGISTERS_STRATIXII ON -to [get_cells *udp*]

# Critical path optimizations
set_instance_assignment -name CRITICAL_CHAIN true -to [get_cells *adc_interface*]
set_instance_assignment -name CRITICAL_CHAIN true -to [get_cells *fft_processor*]
set_instance_assignment -name CRITICAL_CHAIN true -to [get_cells *ethernet_mac*]

# Report timing for FFT critical path
report_timing -from [get_cells *fft_processor*] -to [get_cells *fft_processor*] -file fft_timing.rpt
