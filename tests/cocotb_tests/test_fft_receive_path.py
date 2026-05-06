import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer


@cocotb.test()
async def test_integrated_pipeline_clock_and_reset(dut):
    """Top-level smoke test only.

    Detailed FFT numerical coverage lives in test_fft_processor.py. This test
    keeps one lightweight integration check for the fpga_processing_pipeline
    clock/reset path so coverage is not split between strong and weak FFT tests.
    """

    if not hasattr(dut, "clk_100m_in"):
        return

    cocotb.start_soon(Clock(dut.clk_100m_in, 10.0, unit="ns").start())

    dut.rst_n.value = 0
    await Timer(100, unit="ns")
    dut.rst_n.value = 1

    timeout = 100
    while int(dut.pll_locked.value) == 0 and timeout > 0:
        await RisingEdge(dut.clk_100m_in)
        timeout -= 1

    assert int(dut.pll_locked.value) == 1, "PLL failed to lock"
