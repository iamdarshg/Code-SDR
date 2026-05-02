# ============================================================================
# SDC Constraints for Code-SDR
# ============================================================================

# Define Clocks
create_clock -name clk_100m -period 10.000 [get_ports clk_100m_in]
create_clock -name clk_105m -period 9.524 [get_pins u_clock_manager/clk_105m_adc]
create_clock -name clk_125m -period 8.000 [get_pins u_clock_manager/clk_125m_eth]

# Clock Groups (Asynchronous)
set_clock_groups -asynchronous -group {clk_100m} -group {clk_105m} -group {clk_125m}

# False Paths for Reset
set_false_path -from [get_ports rst_n]

# I/O Delays
set_input_delay -clock clk_105m 2.0 [get_ports adc_data[*]]
set_output_delay -clock clk_125m 2.0 [get_ports gmii_tx_d[*]]
