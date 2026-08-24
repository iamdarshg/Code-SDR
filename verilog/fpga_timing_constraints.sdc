# ============================================================================
# FPGA Timing Constraints for LIF-MD6000-6UMG64I
# ============================================================================
# Issue #12 receive path timing model:
#   100 MHz board reference clock feeds a generated PLL/IP wrapper.
#   The direct ADC/DSP/FFT domain is 100 MHz; a PLL provides 125 MHz Ethernet.
# ============================================================================

create_clock -name clk_100m_in -period 10.000 [get_ports clk_100m_in]
create_clock -name spi_clk -period 50.000 [get_ports spi_clk]

# Board/project PLL wrapper generated outside this RTL repository.
create_generated_clock -name clk_125m_eth \
    -source [get_ports clk_100m_in] \
    -divide_by 4 -multiply_by 5 \
    [get_pins -hierarchical *u_lifmd6000_clock_pll*clk_125m_eth]

set_clock_groups -asynchronous \
    -group [get_clocks clk_100m_in] \
    -group [get_clocks clk_125m_eth] \
    -group [get_clocks spi_clk]

# ADC input timing at 100 MSPS. Board-level values should be tightened with
# measured trace and ADC datasheet numbers before final bitstream signoff.
set_input_delay -clock [get_clocks clk_100m_in] -max 2.0 [get_ports {adc_data[*] adc_valid adc_ovr}]
set_input_delay -clock [get_clocks clk_100m_in] -min 0.5 [get_ports {adc_data[*] adc_valid adc_ovr}]

# GMII timing in the 125 MHz Ethernet domain.
set_output_delay -clock [get_clocks clk_125m_eth] -max 2.0 [get_ports {gmii_tx_d[*] gmii_tx_en gmii_tx_er}]
set_output_delay -clock [get_clocks clk_125m_eth] -min 0.5 [get_ports {gmii_tx_d[*] gmii_tx_en gmii_tx_er}]
set_input_delay -clock [get_clocks clk_125m_eth] -max 2.0 [get_ports {gmii_rx_d[*] gmii_rx_dv gmii_rx_er}]
set_input_delay -clock [get_clocks clk_125m_eth] -min 0.5 [get_ports {gmii_rx_d[*] gmii_rx_dv gmii_rx_er}]

# SPI control-plane timing.
set_input_delay -clock [get_clocks spi_clk] -max 5.0 [get_ports {spi_mosi spi_cs_n}]
set_input_delay -clock [get_clocks spi_clk] -min 1.0 [get_ports {spi_mosi spi_cs_n}]
set_output_delay -clock [get_clocks spi_clk] -max 5.0 [get_ports spi_miso]
set_output_delay -clock [get_clocks spi_clk] -min 1.0 [get_ports spi_miso]

set_false_path -from [get_ports rst_n]

# Async FIFOs own these domain crossings in RTL.
set_false_path -from [get_clocks clk_100m_in] -to [get_clocks clk_125m_eth]
set_false_path -from [get_clocks clk_125m_eth] -to [get_clocks clk_100m_in]
