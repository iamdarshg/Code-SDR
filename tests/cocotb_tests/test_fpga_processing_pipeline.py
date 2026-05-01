# test_fpga_processing_pipeline.py - cocotb integration test for top-level module
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer, FallingEdge
from cocotb.result import TestFailure
import numpy as np

async def spi_write(dut, addr, data):
    """Simulate SPI write to registers"""
    # In a real test, this would toggle spi_clk, mosi, cs_n
    # Here we simulate the internal register updates if possible,
    # but since they are driven by the interface, we'll try to drive the interface pins.

    dut.spi_cs_n.value = 0
    await Timer(20, units="ns")

    # Send address (8 bits)
    for i in range(8):
        dut.spi_mosi.value = (addr >> (7-i)) & 1
        dut.spi_clk.value = 1
        await Timer(10, units="ns")
        dut.spi_clk.value = 0
        await Timer(10, units="ns")

    # Send data (32 bits)
    for i in range(32):
        dut.spi_mosi.value = (data >> (31-i)) & 1
        dut.spi_clk.value = 1
        await Timer(10, units="ns")
        dut.spi_clk.value = 0
        await Timer(10, units="ns")

    dut.spi_cs_n.value = 1
    await Timer(20, units="ns")

@cocotb.test()
async def test_end_to_end_data_flow(dut):
    """End-to-end integration test: ADC -> DDC -> FFT -> Ethernet"""

    # Start clock
    cocotb.start_soon(Clock(dut.clk_100m_in, 10, units="ns").start())

    # Initialize interface signals
    dut.spi_cs_n.value = 1
    dut.spi_clk.value = 0
    dut.spi_mosi.value = 0
    dut.adc_valid.value = 0

    # Reset sequence
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1

    # Wait for PLL lock assertion
    # In simulation, we expect locked to go high after reset sync
    timeout = 100
    while not dut.pll_locked.value and timeout > 0:
        await RisingEdge(dut.clk_100m_in)
        timeout -= 1

    assert dut.pll_locked.value, "PLL failed to lock"

    # Configure System via SPI
    # REG_ENABLE_CTRL (0x03) = 1
    # REG_PROC_MODE (0x20) = 0 (Spectrum)
    await spi_write(dut, 0x20, 0)
    await spi_write(dut, 0x03, 1)

    # Verify internal configuration reflects SPI writes
    await Timer(1, units="us")
    assert dut.processing_mode_out.value == 0, f"Processing mode mismatch: {dut.processing_mode_out.value}"

    # Generate Input Stimulus: 1 MHz sine wave on 105 MSPS ADC
    fs = 105e6
    f_sig = 1e6
    N = 1024
    t = np.arange(N) / fs
    samples = (np.sin(2 * np.pi * f_sig * t) * 511).astype(np.int16)

    cocotb.log.info(f"Injecting {N} ADC samples...")
    for s in samples:
        dut.adc_data.value = int(s) & 0x3FF
        dut.adc_valid.value = 1
        # Drive data on ADC clock edge
        # Since adc_clock is internal but generated from clk_manager,
        # and we know it's ~9.52ns period
        await Timer(9.52, units="ns")

    dut.adc_valid.value = 0

    # Assert activity in pipeline
    # 1. DDC
    success = False
    for _ in range(1000):
        await Timer(10, units="ns")
        if dut.ddc_valid.value:
            success = True
            break
    assert success, "DDC failed to produce valid output"

    # 2. FFT
    success = False
    for _ in range(10000): # FFT takes longer
        await Timer(10, units="ns")
        if dut.fft_valid.value:
            success = True
            break
    assert success, "FFT failed to produce valid output"

    # 3. Ethernet
    success = False
    for _ in range(50000):
        await Timer(10, units="ns")
        if dut.gmii_tx_en.value:
            success = True
            break
    assert success, "Ethernet MAC failed to transmit"

    cocotb.log.info("End-to-end data flow test PASSED")
