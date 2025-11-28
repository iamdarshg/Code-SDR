# test_ethernet_mac.py - cocotb testbench for Ethernet MAC validation
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer, FallingEdge
from cocotb.result import TestFailure
import numpy as np


@cocotb.test()
async def test_ethernet_mac_packet_tx(dut):
    """Test Ethernet MAC packet transmission"""
    # Start 125 MHz GMII clock
    clock = Clock(dut.clk, 8, units="ns")
    cocotb.start_soon(clock.start())

    # Reset sequence
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test packet transmission
    dut.packet_data.value = 0xDEADBEEF
    dut.packet_len.value = 4
    dut.packet_valid.value = 1

    # Wait for packet acknowledgment
    while not dut.packet_ack.value:
        await RisingEdge(dut.clk)

    dut.packet_valid.value = 0

    # Monitor TX output for preamble detection
    preamble_count = 0
    sfd_count = 0
    for _ in range(100):  # Monitor 100 clock cycles
        await RisingEdge(dut.clk)
        if dut.gmii_tx_en.value:
            tx_byte = int(dut.gmii_tx_d.value)
            if tx_byte == 0x55:
                preamble_count += 1
            elif tx_byte == 0xD5:
                sfd_count += 1

    assert preamble_count >= 7, f"Expected at least 7 preamble bytes, got {preamble_count}"
    assert sfd_count == 1, f"Expected 1 SFD byte, got {sfd_count}"

    cocotb.log.info("Ethernet MAC packet TX test PASSED")


@cocotb.test()
async def test_ethernet_mac_link_status(dut):
    """Test Ethernet MAC link status detection"""
    clock = Clock(dut.clk, 8, units="ns")
    cocotb.start_soon(clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test link detection
    # Initially no link
    assert not dut.link_status.value, "Link status should be low initially"

    # Simulate incoming preamble
    dut.gmii_rx_dv.value = 1
    dut.gmii_rx_d.value = 0x55
    await RisingEdge(dut.clk)

    dut.gmii_rx_dv.value = 0
    await RisingEdge(dut.clk)

    # Link should be detected
    assert dut.link_status.value, "Link status not detected after preamble"

    cocotb.log.info("Ethernet MAC link status test PASSED")
