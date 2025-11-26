# test_async_fifo.py - cocotb testbench for asynchronous FIFO validation
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer, FallingEdge
from cocotb.result import TestFailure
import random


@cocotb.test()
async def test_fifo_basic_write_read(dut):
    """Basic FIFO write/read functionality test"""
    # Start write clock (ADC domain - 105 MHz)
    write_clock = Clock(dut.wr_clk, 9.52, units="ns")
    cocotb.start_soon(write_clock.start())

    # Start read clock (system domain - 100 MHz)
    read_clock = Clock(dut.rd_clk, 10, units="ns")
    cocotb.start_soon(read_clock.start())

    # Reset sequence
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1

    # Sync to both clocks
    await RisingEdge(dut.wr_clk)
    await RisingEdge(dut.rd_clk)

    # Test basic write/read cycle
    test_data = [0xAA, 0xBB, 0xCC, 0xDD, 0xEE]

    # Write data
    for data in test_data:
        # Wait for write ready
        while not dut.wr_ready.value:
            await RisingEdge(dut.wr_clk)

        dut.wr_data.value = data
        dut.wr_en.value = 1
        await RisingEdge(dut.wr_clk)
        dut.wr_en.value = 0

    # Read data back
    read_data = []
    for _ in test_data:
        # Wait for read ready
        while not dut.rd_ready.value:
            await RisingEdge(dut.rd_clk)

        dut.rd_en.value = 1
        await RisingEdge(dut.rd_clk)
        read_data.append(int(dut.rd_data.value))
        dut.rd_en.value = 0

    # Verify data integrity
    if read_data != test_data:
        raise TestFailure(f"Data corruption in FIFO: wrote {test_data}, read {read_data}")

    cocotb.log.info("FIFO basic write/read test PASSED")


@cocotb.test()
async def test_fifo_full_empty_flags(dut):
    """Test FIFO full/empty flag management"""
    write_clock = Clock(dut.wr_clk, 9.52, units="ns")
    cocotb.start_soon(write_clock.start())

    read_clock = Clock(dut.rd_clk, 10, units="ns")
    cocotb.start_soon(read_clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1

    await RisingEdge(dut.wr_clk)
    await RisingEdge(dut.rd_clk)

    # Write until FIFO is full (assuming FIFO_DEPTH)
    fifo_depth = 16  # Typical depth, adjust based on actual implementation
    write_count = 0

    for i in range(fifo_depth + 2):  # Write more than depth
        if dut.wr_ready.value:  # Can write
            dut.wr_data.value = i & 0xFF
            dut.wr_en.value = 1
            await RisingEdge(dut.wr_clk)
            dut.wr_en.value = 0
            write_count += 1
            cocotb.log.debug(f"Wrote {write_count} items")
        else:
            # FIFO full
            break

    # Verify FIFO reports full correctly
    if not dut.full.value:
        raise TestFailure("FIFO did not report full when expected")

    # Read until FIFO is empty
    read_count = 0
    while dut.rd_ready.value:
        dut.rd_en.value = 1
        await RisingEdge(dut.rd_clk)
        dut.rd_en.value = 0
        read_count += 1
        cocotb.log.debug(f"Read {read_count} items")

    # Verify FIFO reports empty correctly
    if not dut.empty.value:
        raise TestFailure("FIFO did not report empty when expected")

    # Verify read count matches write count (within FIFO capacity)
    if read_count > write_count:
        raise TestFailure(f"Read more items ({read_count}) than written ({write_count})")

    cocotb.log.info("FIFO full/empty flags test PASSED")


@cocotb.test()
async def test_fifo_asynchronous_operation(dut):
    """Test FIFO operation with different write/read clock rates"""
    # Write clock: 105 MHz (ADC domain)
    write_clock = Clock(dut.wr_clk, 9.52, units="ns")
    cocotb.start_soon(write_clock.start())

    # Read clock: 100 MHz (system domain) - different rate
    read_clock = Clock(dut.rd_clk, 10, units="ns")
    cocotb.start_soon(read_clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1

    await RisingEdge(dut.wr_clk)
    await RisingEdge(dut.rd_clk)

    # Perform continuous write/read operations
    test_duration = 1000  # Test for reasonable duration

    # Start writing task
    write_task = cocotb.start_soon(write_continuous(dut, test_duration))

    # Start reading task
    read_task = cocotb.start_soon(read_continuous(dut, test_duration))

    # Wait for both tasks to complete
    await write_task
    await read_task

    cocotb.log.info("FIFO asynchronous operation test PASSED")


async def write_continuous(dut, duration):
    """Continuous write operation"""
    data = 0
    for _ in range(duration):
        if dut.wr_ready.value:
            dut.wr_data.value = data & 0xFF
            dut.wr_en.value = 1
            await RisingEdge(dut.wr_clk)
            dut.wr_en.value = 0
            data += 1
        else:
            await RisingEdge(dut.wr_clk)


async def read_continuous(dut, duration):
    """Continuous read operation"""
    for _ in range(duration):
        if dut.rd_ready.value:
            dut.rd_en.value = 1
            await RisingEdge(dut.rd_clk)
            dut.rd_en.value = 0
        else:
            await RisingEdge(dut.rd_clk)


@cocotb.test()
async def test_fifo_overflow_underflow_protection(dut):
    """Test FIFO overflow/underflow protection"""
    write_clock = Clock(dut.wr_clk, 9.52, units="ns")
    cocotb.start_soon(write_clock.start())

    read_clock = Clock(dut.rd_clk, 10, units="ns")
    cocotb.start_soon(read_clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1

    await RisingEdge(dut.wr_clk)
    await RisingEdge(dut.rd_clk)

    # Test overflow: try to write when full
    # First fill FIFO
    fifo_depth = 16
    for i in range(fifo_depth):
        if dut.wr_ready.value:
            dut.wr_data.value = i & 0xFF
            dut.wr_en.value = 1
            await RisingEdge(dut.wr_clk)
            dut.wr_en.value = 0

    # Verify full
    if not dut.full.value:
        await Timer(100, units="ns")  # Wait for synchronization
        if not dut.full.value:
            raise TestFailure("FIFO should be full after writing to capacity")

    # Try to write one more (should not cause overflow issues)
    dut.wr_data.value = 0xFF
    dut.wr_en.value = 1
    await RisingEdge(dut.wr_clk)
    dut.wr_en.value = 0

    # Read all data
    read_count = 0
    while dut.rd_ready.value:
        dut.rd_en.value = 1
        await RisingEdge(dut.rd_clk)
        dut.rd_en.value = 0
        read_count += 1

    # Verify FIFO empty
    if not dut.empty.value:
        raise TestFailure("FIFO should be empty after reading all data")

    # Try to read one more (should not cause underflow issues)
    dut.rd_en.value = 1
    await RisingEdge(dut.rd_clk)
    dut.rd_en.value = 0

    # Verify read_count equals written count (within FIFO capacity)
    if read_count != fifo_depth:
        raise TestFailure(f"Read count mismatch: wrote {fifo_depth}, read {read_count}")

    cocotb.log.info("FIFO overflow/underflow protection test PASSED")
