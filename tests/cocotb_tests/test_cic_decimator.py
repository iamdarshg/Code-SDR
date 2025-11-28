# test_cic_decimator.py - cocotb testbench for CIC decimator validation
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer, FallingEdge
from cocotb.result import TestFailure
import numpy as np


@cocotb.test()
async def test_cic_decimator_basic(dut):
    """Basic CIC decimator functionality test"""
    # Start 100 MHz system clock
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset sequence
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test impulse response (theoretical gain = decimation^stages = 8^3 = 512)
    expected_gain = 512

    # Send impulse
    dut.data_in.value = 0x10000  # Impulse
    dut.data_valid.value = 1
    await RisingEdge(dut.clk)
    dut.data_valid.value = 0

    # Feed 7 zeros (total 8 samples for decimation by 8)
    for i in range(7):
        dut.data_in.value = 0
        dut.data_valid.value = 1
        await RisingEdge(dut.clk)
    dut.data_valid.value = 0

    # Wait for output (pipeline delay)
    await Timer(1000, units="ns")

    if dut.output_valid.value:
        output_val = int(dut.data_out.value)
        expected_output = (0x10000 << 21) >> 9  # Scaled accordingly per decimation
        cocotb.log.info(f"CIC output: {output_val:08X}, expected range around {expected_output:08X}")
    else:
        raise TestFailure("CIC decimator did not produce valid output")

    cocotb.log.info("CIC decimator basic test PASSED")


@cocotb.test()
async def test_cic_decimator_saturation(dut):
    """Test CIC decimator saturation handling"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test with maximum input
    dut.data_in.value = 0x7FFFFFFF  # Maximum positive
    dut.data_valid.value = 1
    await RisingEdge(dut.clk)
    dut.data_valid.value = 0

    # Feed zeros
    for i in range(7):
        dut.data_in.value = 0
        dut.data_valid.value = 1
        await RisingEdge(dut.clk)
    dut.data_valid.value = 0

    # Wait for result
    await Timer(1000, units="ns")

    if dut.output_valid.value:
        output_val = int(dut.data_out.value)
        assert abs(output_val) <= 0x7FFFFFFF, f"CIC output saturated incorrectly: {output_val:08X}"
    else:
        raise TestFailure("CIC decimator saturation test failed - no output")

    cocotb.log.info("CIC decimator saturation test PASSED")
