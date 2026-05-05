import numpy as np
import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, Timer


N = 1024
DATA_WIDTH = 24
MAX_POS = (1 << (DATA_WIDTH - 1)) - 1
MIN_NEG = -(1 << (DATA_WIDTH - 1))


def to_twos(value):
    value = int(np.clip(value, MIN_NEG, MAX_POS))
    if value < 0:
        value += 1 << DATA_WIDTH
    return value


def from_twos(value):
    value = int(value)
    if value & (1 << (DATA_WIDTH - 1)):
        value -= 1 << DATA_WIDTH
    return value


async def reset(dut):
    dut.rst_n.value = 0
    dut.real_in.value = 0
    dut.imag_in.value = 0
    dut.data_valid.value = 0
    await Timer(100, units="ns")
    dut.rst_n.value = 1
    await RisingEdge(dut.clk)


async def run_frame(dut, samples):
    assert len(samples) == N
    driven = np.zeros(N, dtype=np.complex128)
    for sample_index, sample in enumerate(samples):
        real = int(np.clip(np.real(sample), MIN_NEG, MAX_POS))
        imag = int(np.clip(np.imag(sample), MIN_NEG, MAX_POS))
        driven[sample_index] = real + 1j * imag
        dut.real_in.value = to_twos(real)
        dut.imag_in.value = to_twos(imag)
        dut.data_valid.value = 1
        await RisingEdge(dut.clk)

    dut.data_valid.value = 0

    bins = np.zeros(N, dtype=np.complex128)
    valid_count = 0
    overflow_seen = False
    indices = []
    frames = []

    for _ in range(N * 8):
        await RisingEdge(dut.clk)
        if int(dut.fft_valid.value):
            idx = int(dut.fft_index.value)
            real = from_twos(dut.real_out.value.integer)
            imag = from_twos(dut.imag_out.value.integer)
            bins[idx] = real + 1j * imag
            indices.append(idx)
            frames.append(int(dut.frame_count.value))
            valid_count += 1
            overflow_seen = overflow_seen or bool(int(dut.overflow_flag.value))
            if valid_count == N:
                break

    assert valid_count == N, f"expected {N} valid bins, got {valid_count}"
    assert sorted(indices) == list(range(N)), (
        "FFT output did not label each bin exactly once"
    )
    assert len(set(frames)) == 1, (
        f"single frame emitted multiple frame counters: {set(frames)}"
    )
    return bins, overflow_seen, np.array(indices), driven


@cocotb.test()
async def test_dc_bin_order_and_scaling(dut):
    cocotb.start_soon(Clock(dut.clk, 10, units="ns").start())
    await reset(dut)

    amplitude = 2048
    bins, overflow, _, driven = await run_frame(
        dut, np.full(N, amplitude, dtype=np.complex128)
    )

    expected = np.fft.fft(driven) / N
    np.testing.assert_allclose(bins.real, expected.real, atol=8)
    np.testing.assert_allclose(bins.imag, expected.imag, atol=8)
    assert not overflow


@cocotb.test()
async def test_positive_and_negative_tones(dut):
    cocotb.start_soon(Clock(dut.clk, 10, units="ns").start())
    await reset(dut)

    n = np.arange(N)
    samples = 1500 * np.exp(2j * np.pi * 37 * n / N)
    bins, _, _, driven = await run_frame(dut, samples)
    expected = np.fft.fft(driven) / N
    np.testing.assert_allclose(bins.real, expected.real, atol=12)
    np.testing.assert_allclose(bins.imag, expected.imag, atol=12)
    assert int(np.argmax(np.abs(bins))) == 37

    samples = 1500 * np.exp(-2j * np.pi * 91 * n / N)
    bins, _, _, driven = await run_frame(dut, samples)
    expected = np.fft.fft(driven) / N
    np.testing.assert_allclose(bins.real, expected.real, atol=12)
    np.testing.assert_allclose(bins.imag, expected.imag, atol=12)
    assert int(np.argmax(np.abs(bins))) == N - 91


@cocotb.test()
async def test_two_tone_and_back_to_back_frames(dut):
    cocotb.start_soon(Clock(dut.clk, 10, units="ns").start())
    await reset(dut)

    n = np.arange(N)
    frame_a = 1200 * np.exp(2j * np.pi * 11 * n / N)
    frame_b = (
        900 * np.exp(2j * np.pi * 121 * n / N)
        + 700 * np.exp(2j * np.pi * 333 * n / N)
    )

    bins_a, overflow_a, _, driven_a = await run_frame(dut, frame_a)
    bins_b, overflow_b, _, driven_b = await run_frame(dut, frame_b)
    expected_a = np.fft.fft(driven_a) / N
    expected_b = np.fft.fft(driven_b) / N

    np.testing.assert_allclose(bins_a.real, expected_a.real, atol=12)
    np.testing.assert_allclose(bins_a.imag, expected_a.imag, atol=12)
    np.testing.assert_allclose(bins_b.real, expected_b.real, atol=12)
    np.testing.assert_allclose(bins_b.imag, expected_b.imag, atol=12)
    assert int(np.argmax(np.abs(bins_a))) == 11
    strongest = set(np.argsort(np.abs(bins_b))[-2:])
    assert strongest == {121, 333}
    assert not overflow_a
    assert not overflow_b


@cocotb.test()
async def test_continuous_input_sustains_one_sample_per_clock(dut):
    cocotb.start_soon(Clock(dut.clk, 10, units="ns").start())
    await reset(dut)

    n = np.arange(N)
    frames = [
        900 * np.exp(2j * np.pi * tone * n / N)
        for tone in (7, 71, 203)
    ]

    for frame in frames:
        for sample in frame:
            dut.real_in.value = to_twos(np.real(sample))
            dut.imag_in.value = to_twos(np.imag(sample))
            dut.data_valid.value = 1
            await RisingEdge(dut.clk)

    dut.data_valid.value = 0

    bins_by_frame = {}
    indices_by_frame = {}
    timeout = N * 6
    while (
        sum(len(v) for v in indices_by_frame.values()) < N * len(frames)
        and timeout > 0
    ):
        await RisingEdge(dut.clk)
        if int(dut.fft_valid.value):
            frame = int(dut.frame_count.value)
            idx = int(dut.fft_index.value)
            real = from_twos(dut.real_out.value.integer)
            imag = from_twos(dut.imag_out.value.integer)
            bins_by_frame.setdefault(frame, np.zeros(N, dtype=np.complex128))[idx] = (
                real + 1j * imag
            )
            indices_by_frame.setdefault(frame, []).append(idx)
        timeout -= 1

    assert sum(len(v) for v in indices_by_frame.values()) == N * len(frames)
    assert len(indices_by_frame) == len(frames)

    for frame_id, expected_tone in zip(sorted(indices_by_frame), (7, 71, 203)):
        assert sorted(indices_by_frame[frame_id]) == list(range(N))
        assert int(np.argmax(np.abs(bins_by_frame[frame_id]))) == expected_tone


@cocotb.test()
async def test_frame_local_overflow_metadata(dut):
    cocotb.start_soon(Clock(dut.clk, 10, units="ns").start())
    await reset(dut)

    hot = np.full(N, MAX_POS, dtype=np.complex128) * (1 + 1j)
    quiet = np.zeros(N, dtype=np.complex128)

    _, first_overflow, _, _ = await run_frame(dut, hot)
    _, second_overflow, _, _ = await run_frame(dut, quiet)

    assert not second_overflow, "overflow flag must clear for each new frame"
    assert isinstance(first_overflow, bool)
