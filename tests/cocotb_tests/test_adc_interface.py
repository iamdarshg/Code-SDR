# test_adc_interface.py - cocotb testbench for ADC interface validation
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer
from cocotb.result import TestFailure
import numpy as np

@cocotb.test()
async def test_adc_basic_functionality(dut):
    """Basic ADC interface functionality test"""
    # Start 105 MHz ADC clock
    adc_clock = Clock(dut.clk_adc, 9.52, units="ns")
    cocotb.start_soon(adc_clock.start())

    # Reset sequence
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk_adc)

    # Test ADC data capture
    test_samples = [
        0x1FF,  # Max positive (10-bit)
        0x000,  # Zero
        0x200,  # Max negative
    ]

    for s in test_samples:
        dut.adc_data.value = s
        dut.adc_valid.value = 1
        await RisingEdge(dut.clk_adc)
        await RisingEdge(dut.clk_adc) # Pipeline delay

        captured = int(dut.adc_samples.value)
        # Note: dc_corrected might be different if avg_count is non-zero,
        # but right after reset it should match.
        if (captured & 0x3FF) != (s & 0x3FF):
             raise TestFailure(f"Data mismatch: expected {s}, got {captured & 0x3FF}")

    cocotb.log.info("ADC interface basic functionality test PASSED")

@cocotb.test()
async def test_adc_overflow_detection(dut):
    """Test ADC overflow detection"""
    adc_clock = Clock(dut.clk_adc, 9.52, units="ns")
    cocotb.start_soon(adc_clock.start())

    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk_adc)

    # Trigger overflow (requires 3 consecutive cycles)
    for _ in range(5):
        dut.adc_ovr.value = 1
        await RisingEdge(dut.clk_adc)

    assert dut.overflow_detect.value == 1, "Overflow flag not set"
    cocotb.log.info("ADC overflow detection test PASSED")
