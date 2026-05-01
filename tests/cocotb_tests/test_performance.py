import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer
import time
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

    # 1. Latency Measurement (ADC to Ethernet)
    # Inject one pulse
    dut.adc_data.value = 0x3FF
    dut.adc_valid.value = 1
    start_time = cocotb.utils.get_sim_time('ns')

    await Timer(9.52, units="ns")
    dut.adc_valid.value = 0

    # Wait for Ethernet activity
    # This is a heuristic - wait for tx_en
    while not dut.gmii_tx_en.value:
        await Timer(10, units="ns")
        if cocotb.utils.get_sim_time('ns') - start_time > 100000: # 100us timeout
            break

    end_time = cocotb.utils.get_sim_time('ns')
    latency = end_time - start_time
    cocotb.log.info(f"End-to-end Latency: {latency} ns")

    # 2. Throughput (Check if processing keeps up)
    # Inject 1000 samples at 105MHz
    success_count = 0
    for i in range(1000):
        dut.adc_data.value = i & 0x3FF
        dut.adc_valid.value = 1
        await Timer(9.52, units="ns")
        if not dut.adc_ovr.value: # Heuristic check
            success_count += 1

    cocotb.log.info(f"Throughput check: {success_count} samples processed without overflow")

    cocotb.log.info("Performance characterization COMPLETED")
