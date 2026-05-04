import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer
import json
import numpy as np
import os
import sys

def load_fixtures():
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    fixture_path = os.path.join(repo_root, 'tests', 'fixtures', 'fft_test_vectors.json')
    if not os.path.exists(fixture_path):
        from tests.fixtures.generate_fft_fixtures import generate_fft_fixtures
        generate_fft_fixtures(1024)
    with open(fixture_path, 'r') as f:
        return json.load(f)

@cocotb.test()
async def test_fft_numerical_precision(dut):
    """Verify FFT output for a single tone signal with exact bin matching."""
    clock = Clock(dut.clk, 9.524, unit="ns")
    cocotb.start_soon(clock.start())

    dut.rst_n.value = 0
    await Timer(100, unit="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    vectors = load_fixtures()
    case = vectors['tone_bin_10']

    # Drive input frame
    for r, i in zip(case['input_real'], case['input_imag']):
        dut.real_in.value = int(r) & 0xFFFFFF
        dut.imag_in.value = int(i) & 0xFFFFFF
        dut.data_valid.value = 1
        await RisingEdge(dut.clk)
    dut.data_valid.value = 0

    # Wait for valid output
    timeout = 2000
    while timeout > 0:
        try:
            if int(dut.fft_valid.value) == 1:
                break
        except Exception:
            pass
        await RisingEdge(dut.clk)
        timeout -= 1
    assert int(dut.fft_valid.value) == 1, "FFT valid never asserted"

    # Collect 1024 bins
    results = []
    for _ in range(1024):
        r = dut.real_out.value.to_signed()
        i = dut.imag_out.value.to_signed()
        idx = int(dut.fft_index.value)
        results.append((idx, np.sqrt(r**2 + i**2)))
        await RisingEdge(dut.clk)

    results.sort()
    mags = [r[1] for r in results]
    peak_bin = np.argmax(mags)
    cocotb.log.info(f"Detected peak at bin {peak_bin} (Mag: {mags[peak_bin]:.2f})")

    assert peak_bin == 10, f"Peak bin mismatch: expected 10, got {peak_bin}"
    assert mags[peak_bin] > 100, f"Signal power too low at bin 10: {mags[peak_bin]}"

@cocotb.test()
async def test_integrated_pipeline_basic(dut):
    """Verify top-level pipeline connectivity and clock lock."""
    # This test intended for TOPLEVEL=fpga_processing_pipeline
    if not hasattr(dut, 'clk_100m_in'):
        return

    clock = Clock(dut.clk_100m_in, 10.0, unit="ns")
    cocotb.start_soon(clock.start())

    dut.rst_n.value = 0
    await Timer(100, unit="ns")
    dut.rst_n.value = 1

    # Wait for PLL locked signal
    timeout = 100
    while int(dut.pll_locked.value) == 0 and timeout > 0:
        await RisingEdge(dut.clk_100m_in)
        timeout -= 1
    assert int(dut.pll_locked.value) == 1, "PLL failed to lock"
    cocotb.log.info("Integrated Pipeline Locked!")
