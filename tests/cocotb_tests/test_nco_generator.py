# test_nco_generator.py - cocotb testbench for NCO generator validation
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer, FallingEdge
from cocotb.result import TestFailure
import numpy as np
import math


@cocotb.test()
async def test_nco_basic_functionality(dut):
    """Basic NCO frequency synthesis test"""
    # Start 100 MHz system clock
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset sequence
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test NCO with 10 MHz output frequency
    # For 32-bit phase accumulator, frequency_word = (f_out * 2^32) / f_clock
    f_out = 10e6  # 10 MHz
    f_clock = 100e6  # 100 MHz
    expected_freq_word = int((f_out * 2**32) / f_clock)

    dut.freq_word.value = expected_freq_word

    # Collect samples for spectral analysis
    samples = []
    sample_count = 1000

    for i in range(sample_count):
        await RisingEdge(dut.clk)

        # Capture NCO sine and cosine outputs (16-bit signed)
        sine_sample = int(dut.sine_out.value)
        cosine_sample = int(dut.cosine_out.value)

        # Convert to signed representation
        sine_signed = sine_sample if sine_sample < 32768 else sine_sample - 65536
        cosine_signed = cosine_sample if cosine_sample < 32768 else cosine_sample - 65536

        samples.append(complex(sine_signed, cosine_signed))

    # Perform FFT analysis on collected samples
    samples_array = np.array(samples)
    fft_result = np.fft.fft(samples_array)
    freqs = np.fft.fftfreq(len(samples_array), 1/f_clock)

    # Find peak frequency
    peak_idx = np.argmax(np.abs(fft_result))
    measured_freq = abs(freqs[peak_idx])

    # Check frequency accuracy (within 1% tolerance)
    frequency_error = abs(measured_freq - f_out) / f_out
    if frequency_error > 0.01:
        raise TestFailure(".3f")

    cocotb.log.info(".3f")


@cocotb.test()
async def test_nco_phase_accumulator(dut):
    """Test NCO phase accumulator and rollover"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test phase accumulator rollover with frequency word = 1
    dut.freq_word.value = 1  # Minimum frequency step

    # Monitor phase accumulator over multiple cycles
    initial_phase = int(dut.phase_accumulator.value)
    await RisingEdge(dut.clk)

    # Phase should increment by 1
    first_phase = int(dut.phase_accumulator.value)
    if first_phase != (initial_phase + 1) % (2**32):
        raise TestFailure(f"Phase accumulator incorrect increment: initial={initial_phase}, first={first_phase}")

    # Test full rollover (2^32)
    full_cycles = 2**16  # Reasonable test duration
    dut.freq_word.value = 2**16  # Faster increment for testing

    for i in range(full_cycles):
        await RisingEdge(dut.clk)

    final_phase = int(dut.phase_accumulator.value)
    expected_phase = (full_cycles * 2**16) % (2**32)

    # Allow small tolerance for timing
    if abs(final_phase - expected_phase) > 10:
        raise TestFailure(f"Phase accumulator rollover failed: expected ~{expected_phase}, got {final_phase}")

    cocotb.log.info("NCO phase accumulator test PASSED")


@cocotb.test()
async def test_nco_frequency_resolution(dut):
    """Test NCO frequency resolution and minimum step size"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test minimum frequency resolution
    # With 100 MHz clock, minimum frequency step = 100e6 / 2^32 ≈ 0.023 Hz
    min_freq_word = 1
    min_frequency = (min_freq_word * 100e6) / (2**32)

    dut.freq_word.value = min_freq_word

    # Collect samples over extended period to measure low frequency
    samples = []
    sample_count = 10000  # Need many samples for sub-Hz resolution

    for i in range(sample_count):
        await RisingEdge(dut.clk)
        sine_sample = int(dut.sine_out.value)
        sine_signed = sine_sample if sine_sample < 32768 else sine_sample - 65536
        samples.append(sine_signed)

    # Perform FFT to measure actual frequency
    samples_array = np.array(samples, dtype=np.float64)
    fft_result = np.fft.fft(samples_array)
    freqs = np.fft.fftfreq(len(samples_array), 1/100e6)

    # Find peak (should be at DC and min_frequency)
    peak_idx = np.argmax(np.abs(fft_result[1:])) + 1  # Skip DC component
    measured_freq = abs(freqs[peak_idx])

    # Verify frequency resolution
    if abs(measured_freq - min_frequency) > min_frequency * 0.1:
        raise TestFailure(f"Frequency resolution test failed: expected ~{min_frequency:.6f} Hz, measured {measured_freq:.6f} Hz")

    cocotb.log.info(".6f")


@cocotb.test()
async def test_nco_dynamic_frequency_update(dut):
    """Test dynamic frequency changes during operation"""
    clock = Clock(dut.clk, 10, units="ns")
    cocotb.start_soon(clock.start())

    # Reset
    dut.rst_n.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)

    # Test frequency hopping
    test_frequencies = [1e6, 5e6, 10e6, 25e6, 50e6]  # 1, 5, 10, 25, 50 MHz

    for f_out in test_frequencies:
        freq_word = int((f_out * 2**32) / 100e6)
        dut.freq_word.value = freq_word

        # Allow NCO to stabilize (phase accumulator settling)
        for _ in range(10):
            await RisingEdge(dut.clk)

        # Quick frequency verification by checking phase increment rate
        initial_phase = int(dut.phase_accumulator.value)
        await RisingEdge(dut.clk)
        second_phase = int(dut.phase_accumulator.value)
        await RisingEdge(dut.clk)
        third_phase = int(dut.phase_accumulator.value)

        # Calculate phase increments
        increment1 = (second_phase - initial_phase) % (2**32)
        increment2 = (third_phase - second_phase) % (2**32)

        # Should be consistent with frequency word
        if abs(increment1 - freq_word) > 1 or abs(increment2 - freq_word) > 1:
            raise TestFailure(f"Dynamic frequency update failed for {f_out/1e6:.1f} MHz: inconsistent phase increments")

    cocotb.log.info("NCO dynamic frequency update test PASSED")
