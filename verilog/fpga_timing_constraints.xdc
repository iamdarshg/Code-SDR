# ============================================================================
# FPGA Timing Constraints for LIF-MD6000-6UMG64I
# ============================================================================
# Complete timing constraints for the SDR processing pipeline
# ============================================================================

# Primary system clock constraints
create_clock -period 10.0 [get_ports clk_100m]          ;# 100 MHz system clock
create_clock -period 9.5238 [get_ports clk_105m_adc]    ;# 105 MHz ADC sampling clock  
create_clock -period 8.0 [get_ports clk_125m_eth]       ;# 125 MHz Ethernet GMII clock

# ADC input timing constraints
set_input_delay -clock [get_clocks clk_105m_adc] -max 2.0 [get_ports adc_data*]
set_input_delay -clock [get_clocks clk_105m_adc] -min 1.0 [get_ports adc_data*]
set_input_delay -clock [get_clocks clk_105m_adc] -max 1.0 [get_ports adc_valid]
set_input_delay -clock [get_clocks clk_105m_adc] -min 0.5 [get_ports adc_ovr]

# GMII output timing constraints  
set_output_delay -clock [get_clocks clk_125m_eth] -max 2.0 [get_ports gmii_tx_*]
set_output_delay -clock [get_clocks clk_125m_eth] -min 1.0 [get_ports gmii_tx_*]

# SPI interface constraints
create_clock -period 20.0 [get_ports spi_clk]          ;# 50 MHz SPI clock
set_input_delay -clock [get_clocks spi_clk] -max 5.0 [get_ports spi_mosi]
set_input_delay -clock [get_clocks spi_clk] -min 2.0 [get_ports spi_mosi]
set_output_delay -clock [get_clocks spi_clk] -max 5.0 [get_ports spi_miso]
set_output_delay -clock [get_clocks spi_clk] -min 2.0 [get_ports spi_miso]

# False path constraints for asynchronous domains
set_false_path -from [get_clocks clk_105m_adc] -to [get_clocks clk_100m]
set_false_path -from [get_clocks clk_100m] -to [get_clocks clk_105m_adc]
set_false_path -from [get_clocks clk_125m_eth] -to [get_clocks clk_100m]
set_false_path -from [get_clocks clk_100m] -to [get_clocks clk_125m_eth]

# Clock domain crossing constraints
set_max_delay -from [get_clocks clk_105m_adc] -to [get_clocks clk_100m] 10.0
set_max_delay -from [get_clocks clk_100m] -to [get_clocks clk_125m_eth] 8.0
set_max_delay -from [get_clocks clk_125m_eth] -to [get_clocks clk_100m] 8.0

# Multi-cycle paths for FFT processing
set_multicycle_path -from [get_clocks clk_100m] -to [get_clocks clk_100m] -setup 10
set_multicycle_path -from [get_clocks clk_100m] -to [get_clocks clk_100m] -hold 5

# False paths for reset synchronization
set_false_path -from [get_ports rst_n] -to [get_clocks clk_100m]
set_false_path -from [get_ports rst_n] -to [get_clocks clk_105m_adc]
set_false_path -from [get_ports rst_n] -to [get_clocks clk_125m_eth]

# I/O buffer type constraints
set_property IOSTANDARD LVCMOS25 [get_ports clk_100m]
set_property IOSTANDARD LVCMOS25 [get_ports clk_105m_adc]
set_property IOSTANDARD LVCMOS25 [get_ports clk_125m_eth]
set_property IOSTANDARD LVCMOS25 [get_ports rst_n]

set_property IOSTANDARD LVCMOS25 [get_ports adc_data*]
set_property IOSTANDARD LVCMOS25 [get_ports adc_valid]
set_property IOSTANDARD LVCMOS25 [get_ports adc_ovr]

set_property IOSTANDARD LVCMOS25 [get_ports spi_clk]
set_property IOSTANDARD LVCMOS25 [get_ports spi_mosi]
set_property IOSTANDARD LVCMOS25 [get_ports spi_cs_n]
set_property IOSTANDARD LVCMOS25 [get_ports spi_miso]

# GMII I/O standards
set_property IOSTANDARD LVCMOS25 [get_ports gmii_tx_*]
set_property IOSTANDARD LVCMOS25 [get_ports gmii_rx_*]
set_property DRIVE 12 [get_ports gmii_tx_en]
set_property DRIVE 12 [get_ports gmii_tx_d*]
set_property SLEW FAST [get_ports gmii_tx_*]

# Location constraints for clock buffers
set_property LOC BUFGCTRL_X0Y0 [get_cells bufg_processing]
set_property LOC BUFGCTRL_X0Y1 [get_cells bufg_adc]
set_property LOC BUFGCTRL_X0Y2 [get_cells bufg_eth_tx]
set_property LOC BUFGCTRL_X0Y3 [get_cells bufg_eth_rx]

# Area constraints for processing modules
set_area_group processing_pipeline -range {START:END} -modules {clock_manager adc_interface digital_downconverter}
set_area_group ddc_block -range {START:END} -modules {nco_generator cic_decimator}
set_area_group fft_block -range {START:END} -modules {fft_processor hamming_window}
set_area_group ethernet_block -range {START:END} -modules {ethernet_mac udp_ip_stack}
set_area_group control_block -range {START:END} -modules {rp2040_interface async_fifo}
