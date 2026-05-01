import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer
import numpy as np

async def spi_write(dut, addr, data):
    dut.spi_cs_n.value = 0
    await Timer(20, units="ns")
    for i in range(8):
        dut.spi_mosi.value = (addr >> (7-i)) & 1
        dut.spi_clk.value = 1
        await Timer(10, units="ns")
        dut.spi_clk.value = 0
        await Timer(10, units="ns")
    for i in range(32):
        dut.spi_mosi.value = (data >> (31-i)) & 1
        dut.spi_clk.value = 1
        await Timer(10, units="ns")
        dut.spi_clk.value = 0
        await Timer(10, units="ns")
    dut.spi_cs_n.value = 1
    await Timer(20, units="ns")

@cocotb.test()
async def test_integrated_pipeline(dut):
    """Robust integrated pipeline test with assertions"""
    cocotb.start_soon(Clock(dut.clk_100m_in, 10, units="ns").start())
    dut.rst_n.value = 0
    dut.spi_cs_n.value = 1
    dut.spi_clk.value = 0
    dut.spi_mosi.value = 0
    dut.adc_valid.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1

    # Wait for lock
    for _ in range(200):
        await RisingEdge(dut.clk_100m_in)
    assert dut.pll_locked.value, "PLL Lock Timeout"

    # Config: Enable system (0x03=1) and set mode to IQ (0x20=1)
    await spi_write(dut, 0x20, 1)
    await spi_write(dut, 0x03, 1)

    await Timer(1, units="us")
    assert dut.processing_mode_out.value == 1, "Mode config failed"

    # Inject 100 samples
    for i in range(100):
        dut.adc_data.value = i & 0x3FF
        dut.adc_valid.value = 1
        await Timer(9.52, units="ns")
    dut.adc_valid.value = 0

    # Check DDC output
    success = False
    for _ in range(5000):
        await Timer(10, units="ns")
        if dut.ddc_valid.value:
            success = True
            break
    assert success, "DDC output timeout"
    cocotb.log.info("Integrated Pipeline Test PASSED")
