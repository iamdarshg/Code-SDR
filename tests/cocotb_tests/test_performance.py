import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer
import numpy as np

@cocotb.test()
async def test_performance_characterization(dut):
    """Measure performance: ADC rate, FFT throughput, Latency"""

    # Start clock
    cocotb.start_soon(Clock(dut.clk_100m_in, 10, units="ns").start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1

    # Wait for PLL lock
    await Timer(1, units="us")
    assert dut.pll_locked.value, "PLL Lock failed"

    # Enable system
    # Bit 3 is enable_control
    # We can drive internal wires for performance tests if needed, or use SPI
    # Driving internal wires directly for raw performance measurement
    # But wait, we fixed the port list, so we must use SPI or add debug ports.
    # Let's use the provided SPI write function if we can share it,
    # or just drive the SPI interface.

    from test_fpga_processing_pipeline import spi_write
    await spi_write(dut, 0x03, 1) # Enable

    # 1. Latency Measurement (ADC to Ethernet)
    # Inject one pulse
    start_time = cocotb.utils.get_sim_time('ns')
    dut.adc_data.value = 0x3FF
    dut.adc_valid.value = 1
    await Timer(9.52, units="ns")
    dut.adc_valid.value = 0

    # Wait for Ethernet activity
    success = False
    for _ in range(100000): # 1ms timeout
        await Timer(10, units="ns")
        if dut.gmii_tx_en.value:
            success = True
            break

    assert success, "Latency test failed: No Ethernet activity detected"
    end_time = cocotb.utils.get_sim_time('ns')
    latency = end_time - start_time
    cocotb.log.info(f"End-to-end Latency: {latency} ns")

    # 2. Throughput (Check if processing keeps up)
    # Inject 1000 samples at 105MHz
    for i in range(1000):
        dut.adc_data.value = i & 0x3FF
        dut.adc_valid.value = 1
        await Timer(9.52, units="ns")

    # Check if we got any overflows
    assert not dut.adc_ovr.value, "ADC Overflow detected during throughput test"

    cocotb.log.info("Performance characterization PASSED")
