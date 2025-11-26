# test_rp2040_interface.py - cocotb testbench for RP2040 interface validation
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer, FallingEdge
from cocotb.result import TestFailure
import numpy as np


@cocotb.test()
async def test_rp2040_register_write(dut):
    """Test RP2040 interface register write operations"""
    # Start SPI clock
    spi_clock = Clock(dut.spi_clk, 100, units="ns")  # 10 MHz SPI
    cocotb.start_soon(spi_clock.start())

    # Initialize
    dut.spi_cs_n.value = 1
    dut.spi_mosi.value = 0
    dut.status_reg.value = 16'hABCD
    dut.pll_locked.value = 1
    dut.eth_link_status.value = 1

    # Wait a bit for initialization
    await Timer(500, units="ns")

    # Test frequency word write (register 0x00)
    test_freq = 0x12345678

    # Start SPI transaction
    dut.spi_cs_n.value = 0
    await Timer(10, units="ns")  # CS setup

    # Send address byte (0x00 for frequency word)
    addr_byte = 0x00
    for i in range(8):
        dut.spi_mosi.value = (addr_byte >> (7-i)) & 1
        await RisingEdge(dut.spi_clk)

    # Send frequency word (32 bits)
    for i in range(32):
        dut.spi_mosi.value = (test_freq >> (31-i)) & 1
        await RisingEdge(dut.spi_clk)

    # End transaction
    dut.spi_cs_n.value = 1
    await Timer(100, units="ns")

    # Wait for register write to settle
    await Timer(200, units="ns")

    # Verify write
    assert int(dut.frequency_word.value) == test_freq, f"Frequency word write failed: expected {test_freq:08X}, got {int(dut.frequency_word.value):08X}"

    cocotb.log.info("RP2040 interface register write test PASSED")


@cocotb.test()
async def test_rp2040_status_read(dut):
    """Test RP2040 interface status register reading"""
    spi_clock = Clock(dut.spi_clk, 100, units="ns")
    cocotb.start_soon(spi_clock.start())

    # Status register read test (register 0x10)
    dut.status_reg.value = 16'hABCD

    # Start SPI transaction for status read
    dut.spi_cs_n.value = 0
    await Timer(10, units="ns")

    # Send address byte (0x10 for status register)
    addr_byte = 0x10
    for i in range(8):
        dut.spi_mosi.value = (addr_byte >> (7-i)) & 1
        await RisingEdge(dut.spi_clk)

    # Read back status (16 bits)
    read_data = 0
    for i in range(16):
        await RisingEdge(dut.spi_clk)
        read_data = (read_data << 1) | int(dut.spi_miso.value)

    # End transaction
    dut.spi_cs_n.value = 1

    # Verify read (should be 16'hABCD in upper 16 bits)
    expected_status = 0xABCD
    assert read_data == expected_status, f"Status read failed: expected {expected_status:04X}, got {read_data:04X}"

    cocotb.log.info("RP2040 interface status read test PASSED")
