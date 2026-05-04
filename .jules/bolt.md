# Bolt ⚡ Performance Journal

## 2025-05-22: Radix-2 DIF SDF FFT Optimization

### Problem
The legacy FFT implementation was unsynthesizable and could not sustain the required 105 MSPS throughput on the LIF-MD6000 FPGA. The previous design relied on `real` types and behavioral loops that do not translate to high-speed hardware logic.

### Solution: Single-path Delay Feedback (SDF)
Implemented a 1024-point Radix-2 Decimation-in-Frequency (DIF) SDF architecture.
- **Throughput:** 1 sample per clock cycle (105 MSPS @ 105 MHz).
- **Resource Efficiency:** Uses $N-1$ complex delay elements ($1023$ total) instead of the $N \log_2 N$ required for parallel architectures.
- **Precision:** Applied fixed-point scaling (>>> 1) at each stage to prevent overflow in the 24-bit datapath without requiring wide guard bits.
- **Deterministic Latency:** Exactly 1033 cycles ($1023$ structural + $10$ pipeline registers) from the first sample of a frame to the first bin of the result.

### Key Learnings
- **Persistence is Key:** In SDF architectures, internal counters must be persistent (not reset by `data_valid` alone) to ensure the second half of a frame flushes through the pipeline with the correct twiddle rotations.
- **Latency Alignment:** Off-by-one errors in the valid-bit pipeline cause catastrophic spectral scrambling in bit-reversed outputs. Precise 1033-cycle matching was required.
- **CDC Safety:** The 105 MHz (FFT) to 125 MHz (Ethernet) transition requires an asynchronous FIFO to prevent meta-stability and frame-dropping during burst spectrum output.

### Performance Impact
- **Throughput:** Increased from 0 MSPS (unsynthesizable) to 105 MSPS (sustained).
- **Latency:** Fixed at 9.84 μs per 1024-point frame.
- **Resource Usage:** Optimized for Lattice iCE40/CrossLink-NX styles (Radix-2 SDF is highly compatible with embedded block RAMs for delays).
