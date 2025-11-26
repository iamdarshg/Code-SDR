# test_adc_interface.py - cocotb testbench for ADC interface validation
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer, FallingEdge
from cocotb.result import TestFailure
import numpy as np


@cocotb.test()
async def test_adc_basic_functionality(dut):
    """Basic ADC interface functionality test"""
    # Start 105 MHz ADC clock (ADC sampling rate)
    adc_clock = Clock(dut.adc_clk, 9.52, units="ns")  # ~105 MHz
    cocotb.start_soon(adc_clock.start())

    # Start 100 MHz system clock
    sys_clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(sys_clock.start())

    # Reset sequence
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test ADC data capture
    test_samples = [
        0x1FF,  # Max positive (10-bit)
        0x000,  # Zero
        0x200,  # Max negative (10-bit signed interpretation)
        0x100,  # Mid-scale positive
        0x0FF   # Mid-scale negative
    ]

    for expected_sample in test_samples:
        # Drive ADC data inputs
        dut.adc_data.value = expected_sample

        # Wait for data processing
        await RisingEdge(dut.clk)

        # Verify data capture and conversion
        captured_data = int(dut.data_out.value)

        # Check data integrity (10-bit ADC data should be preserved)
        if (captured_data & 0x3FF) != expected_sample:
            raise TestFailure(f"ADC data corruption: expected {expected_sample}, got {captured_data & 0x3FF}")

    cocotb.log.info("ADC interface basic functionality test PASSED")


@cocotb.test()
async def test_adc_overflow_detection(dut):
    """Test ADC overflow detection and flagging"""
    adc_clock = Clock(dut.adc_clk, 9.52, units="ns")
    cocotb.start_soon(adc_clock.start())

    sys_clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(sys_clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test overflow conditions
    # Normal range: 0x000 to 0x3FF (10-bit)
    # Overflow condition: ADC data outside this range

    dut.adc_data.value = 0x400  # Overflow value
    await RisingEdge(dut.clk)

    if not dut.overflow_flag.value:
        raise TestFailure("Overflow flag not set for out-of-range ADC data")

    # Test normal operation
    dut.adc_data.value = 0x200  # Normal value
    await RisingEdge(dut.clk)

    if dut.overflow_flag.value:
        raise TestFailure("Overflow flag incorrectly set for valid ADC data")

    cocotb.log.info("ADC overflow detection test PASSED")


@cocotb.test()
async def test_adc_data_rate_conversion(dut):
    """Test ADC data rate conversion and synchronization"""
    adc_clock = Clock(dut.adc_clk, 9.52, units="ns")  # 105 MHz
    cocotb.start_soon(adc_clock.start())

    sys_clock = Clock(dut.clk, 10, units="ns")  # 100 MHz
    cocotb.start_soon(sys_clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test data rate matching
    sample_count = 1000
    valid_data_count = 0

    for i in range(sample_count):
        dut.adc_data.value = i & 0x3FF  # 10-bit counter

        await RisingEdge(dut.adc_clk)  # ADC sample edge

        # Check for valid data at system clock
        if dut.data_valid.value:
            valid_data_count += 1

    # Verify data rate (should see valid data flags)
    if valid_data_count == 0:
        raise TestFailure("No valid data output detected")

    expected_rate_ratio = 105.0 / 100.0  # ADC to system clock ratio
    actual_ratio = valid_data_count / (sample_count / expected_rate_ratio)

    # Allow ±10% tolerance
    if abs(actual_ratio - 1.0) > 0.1:
        raise TestFailure(".2f")

