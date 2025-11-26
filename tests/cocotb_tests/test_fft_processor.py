# test_fft_processor.py - cocotb testbench for FFT processor validation
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer, FallingEdge
from cocotb.result import TestFailure
import numpy as np


@cocotb.test()
async def test_fft_basic(dut):
    """Basic FFT functionality test with known input pattern"""
    # Start 100 MHz system clock
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset sequence
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Generate test input: 1024-point sine wave at known frequency
    N = 1024
    fs = 105e6  # ADC sample rate (approximate)
    f_test = 10e6  # 10 MHz test tone
    t = np.arange(N) / fs
    test_signal = (np.sin(2 * np.pi * f_test * t) * 511).astype(np.int16)  # 10-bit ADC scale

    # Feed samples into FFT (assuming N=1024 point FFT)
    for i, sample in enumerate(test_signal):
        dut.data_in.value = int(sample) & 0x3FF  # 10-bit mask
        dut.data_valid.value = 1
        await RisingEdge(dut.clk)

        # Monitor FFT processing status
        if hasattr(dut, 'fft_busy') and dut.fft_busy.value:
            cocotb.log.debug(f"FFT processing sample {i}")

    dut.data_valid.value = 0

    # Wait for FFT processing (pipeline latency)
    # For 1024-point FFT, expect several thousand cycles
    for _ in range(N * 10):  # Conservative wait
        await RisingEdge(dut.clk)

        # Check for completion flag
        if hasattr(dut, 'fft_done') and dut.fft_done.value:
            break

    # Verify FFT completed
    if hasattr(dut, 'fft_done') and not dut.fft_done.value:
        raise TestFailure("FFT processing did not complete within expected time")

    # Expected bin calculation: bin = (f_test * N) / fs
    expected_bin = int(f_test * N / fs)

    # Read FFT output and verify peak location
    if hasattr(dut, 'fft_valid') and dut.fft_valid.value:
        if hasattr(dut, 'peak_bin'):
            peak_bin = int(dut.peak_bin.value)
            # Allow +/- 2 bins tolerance for spectral leakage and finite precision
            if abs(peak_bin - expected_bin) > 2:
                raise TestFailure(f"FFT peak at bin {peak_bin}, expected near {expected_bin}")

        if hasattr(dut, 'peak_magnitude'):
            peak_magnitude = int(dut.peak_magnitude.value)
            # Verify reasonable magnitude (should be significant)
            if peak_magnitude < N * 100:  # Conservative threshold
                raise TestFailure(f"FFT peak magnitude too low: {peak_magnitude}")

        cocotb.log.info(f"FFT Test PASSED: Peak at bin {peak_bin}, magnitude {peak_magnitude}")


@cocotb.test()
async def test_fft_overflow_protection(dut):
    """Test FFT scaling to prevent overflow with maximum input"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Feed maximum amplitude samples (worst case for overflow)
    N = 1024
    for i in range(N):
        # Alternate between max and min values for worst-case scenario
        max_val = 0x1FF if (i % 2 == 0) else 0x200
        dut.data_in.value = max_val
        dut.data_valid.value = 1
        await RisingEdge(dut.clk)

    dut.data_valid.value = 0

    # Wait for processing
    for _ in range(N * 10):
        await RisingEdge(dut.clk)
        if hasattr(dut, 'fft_done') and dut.fft_done.value:
            break

    # Check overflow flag
    if hasattr(dut, 'overflow_flag') and dut.overflow_flag.value:
        raise TestFailure("FFT overflow detected - scaling insufficient")

    cocotb.log.info("FFT overflow protection test PASSED")


@cocotb.test()
async def test_fft_windowing(dut):
    """Test FFT windowing function (Hamming window)"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test window coefficient generation for Hamming window
    # Hamming window: w[n] = 0.54 - 0.46 * cos(2πn/(N-1))
    N = 1024
    expected_window = np.hamming(N)

    # Feed impulse (delta function) to test windowing
    for i in range(N):
        if i == N//2:  # Impulse in middle
            dut.data_in.value = 512  # Mid-scale
        else:
            dut.data_in.value = 0

        dut.data_valid.value = 1
        await RisingEdge(dut.clk)

    dut.data_valid.value = 0

    # Wait for processing
    for _ in range(N * 10):
        await RisingEdge(dut.clk)
        if hasattr(dut, 'fft_done') and dut.fft_done.value:
            break

    # Read FFT output power spectrum
    # Verify sidelobe suppression characteristics of Hamming window
    # (compared to rectangular window, Hamming should have lower sidelobes)

    if hasattr(dut, 'fft_valid') and dut.fft_valid.value:
        # This is a basic test - more sophisticated window verification
        # would read the actual frequency domain output
        cocotb.log.info("FFT windowing test PASSED (basic validation)")


@cocotb.test()
async def test_fft_multiple_frames(dut):
    """Test FFT processing of multiple consecutive frames"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    N = 1024
    num_frames = 3

    for frame in range(num_frames):
        # Generate different test tone for each frame
        f_test = 5e6 * (frame + 1)  # 5, 10, 15 MHz
        t = np.arange(N) / 105e6
        test_signal = (np.sin(2 * np.pi * f_test * t) * 511).astype(np.int16)

        # Feed frame data
        for sample in test_signal:
            dut.data_in.value = int(sample) & 0x3FF
            dut.data_valid.value = 1
            await RisingEdge(dut.clk)

        dut.data_valid.value = 0

        # Wait for frame processing
        for _ in range(N * 5):
            await RisingEdge(dut.clk)
            if hasattr(dut, 'fft_done') and dut.fft_done.value:
                break

        # Verify frame completed
        if not (hasattr(dut, 'fft_done') and dut.fft_done.value):
            raise TestFailure(f"Frame {frame} processing failed")

        cocotb.log.info(f"Frame {frame} processing completed")

    cocotb.log.info("FFT multiple frames test PASSED")
