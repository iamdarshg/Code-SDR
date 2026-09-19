# Code-SDR V2 — Gateware

FPGA gateware for the V2 control/SDR board (LIF-MD6000-6JMG80I + AD9215 +
KSZ9031 RGMII + RP2040). All modules are verified in simulation with Icarus
Verilog and area-estimated with Yosys — no vendor licence required for either.

## Two modes (compile-time exclusive)

The device is 5,936 LUTs. A 1024-point FFT plus the raw streamer do not fit
together, so `v2_top` builds **one or the other**:

```
MODE = 0 (raw)   ADC 100 MSPS -> 8/10-bit pack -> CDC FIFO -> UDP/IP -> MAC -> RGMII
MODE = 1 (fft)   ADC 100 MSPS -> CIC /R -> CDC FIFO -> window -> N-pt FFT
                              -> bin packetizer -> UDP/IP -> MAC -> RGMII
```

The RP2040 holds both bitstreams on its flash and swaps them over slave SPI in
~0.1–0.2 s, so switching modes needs no host and no re-flash.

## The 100 MSPS question, stated once and clearly

**The ADC always runs at 100 MSPS.** Its clock is a fixed 100 MHz board
reference, so nothing throttles the converter in either mode.

- **Raw mode consumes every sample** (8-bit → 800 Mbps on the wire, and it fits
  the link cleanly). At 10 bits over 100 MSPS the stream is 1002 Mbps, which no
  gigabit link can carry, so rather than halve the sample rate the raw path keeps
  the full 100 MSPS and drops whole datagrams at a controlled ~0.9% rate — see
  *Rate plan* below. This is still the 100 MSPS path, and it is the default.
- **The on-FPGA FFT cannot consume 100 MSPS.** An N-point transform needs
  (N/2)·log₂N butterflies per frame, and at 100 MSPS a frame arrives every N
  samples, so it needs ≈ log₂(N)/2 butterflies *per clock*. For N=1024 that is 5
  butterflies/clock ≈ 20 complex multipliers ≈ 12,000+ LUTs against 5,936
  available. This is arithmetic, not tuning. The CIC does consume 100 MSPS
  (its integrators run at the ADC rate); the transform itself runs decimated.

If you want all 100 MSPS in one transform, the transform has to run on the host
— that is raw mode.

## Throughput (measured in simulation)

Every path has a throughput assertion in its testbench, not just a correctness
check.

| Path | Measured | Test |
|---|---:|---|
| Raw streamer, 8-bit / 100 MSPS | **801 Mbps** wire, no drops | `v2_raw_path_tb_8bit` |
| Raw streamer, 10-bit / 50 MSPS | 501 Mbps wire, no drops | `v2_raw_path_tb` |
| Delta-sigma dropper | exact floor(K·frac/65536) | `v2_raw_drop_tb` |
| CDC FIFO | **799.6 Mbps** sustained + max-rate stress | `v2_cdc_fifo_tb` |
| Ethernet MAC TX | **991.6 Mbps** wire | `v2_eth_mac_tx_tb` |
| UDP/IP TX | **956.5 Mbps** payload (1500-MTU ceiling) | `v2_udp_ip_tx_tb` |
| CIC decimator | ÷N verified, DC gain 1 | `v2_cic_decimator_tb` |
| Memory FFT (N=1024) | ~7.8 MSPS sustained input | `v2_fft1024_tb` |
| **Pipelined FFT (N=64)** | **100 MSPS** (128 bins at 1/clock) | `v2_fft_pipe_tb` (see below) |

## Rate plan: jumbo MTU + delta-sigma packet dropping

All datagrams are **jumbo (MTU 9000)**, which raises the UDP payload ceiling from
957 to **992.7 Mbps** and cuts per-packet header overhead.

The ADC clock is a fixed 100 MHz, so the offered load is:

| Mode | Offered | vs 992.7 Mbps ceiling | Action |
|---|---:|---|---|
| 8-bit, 100 MSPS | 801 Mbps | under | none |
| 10-bit, 100 MSPS | 1002 Mbps | **over** | drop ~0.9% of packets |
| 10-bit, 50 MSPS | 501 Mbps | under | none |

10 bits at 100 MSPS exceeds a gigabit link at *any* packing, so the only choices
are decimation (halving the instantaneous bandwidth to 25 MHz) or keeping the
full 100 MSPS and losing ~0.9% of **whole datagrams**. The raw path does the
latter: every packet that is sent is contiguous and jitter-free.

`v2_raw_path` implements this with a first-order **delta-sigma** accumulator,
programmed over SPI with a Q16 drop fraction. The drop count over K packets is
**exactly** `floor(K*frac/65536)` — verified to the packet in `v2_raw_drop_tb`
(109 of 2048 at frac=3510). Because the fraction is non-dyadic, the accumulator's
limit cycle is thousands of packets long, so the gap spacing is dithered rather
than a fixed cadence, spreading the drop spur across many harmonics instead of
concentrating it at one tone.

Dropped packets still consume a sequence number, so the host sees a gap in `seq`
and a matching jump in `base_sample`. Loss is reported, never hidden.

A FIFO high-water override forces extra drops if the read side stalls, so
overflow is structurally impossible: `dropped_words` (FIFO overrun) stays zero
and all loss is counted in `packets_dropped` (telemetry 0x18).

### Packing

The FIFO word is always 40 bits / 5 bytes, so both widths pack exactly and yield
the same words-per-datagram (and therefore the same FIFO depth) at a given MTU:

| Width | Samples per word | Bytes per word |
|---|---:|---:|
| 8-bit | 5 | 5 |
| 10-bit | 4 | 5 |

Byte extraction is identical for both, so the packetizer is width-agnostic.

## Pipelined FFT: throughput **and** arithmetic verified

`v2_fft_pipe.v` genuinely consumes **1 sample/clock = 100 MSPS** (measured),
which the memory-based FFT cannot. It is **verified both ways**:

- throughput: 128 consecutive bins at 1 bin/clock
- arithmetic: `v2_fft_pipe_tb` compares every bin against a **DFT computed in the
  testbench itself** (not hardcoded) for an impulse, DC, and tones at bins 1, 8,
  17 and 31 — all 64 bins within ±4 LSB. `tone@17` is deliberate: it uses one of
  the twiddles that exposed the multiply bug below.

`tools/r2sdf_reference.py` remains the executable specification: it reproduces
the R2SDF schedule in software and matches numpy exactly.

**Three real bugs were found fixing this, none of them visible to a DC-only or
impulse-only test:**

1. **Delay-line index.** `q` was the low `QB` bits of `cnt`, but the reference is
   `q = cnt % H`. For the last stage (`H=1`) that made `q = cnt[0]`, indexing
   `bre[1]` out of bounds and returning **X on every second clock**, poisoning the
   whole combinational chain.
2. **Karatsuba operand sums.** Verilog sizes `a+b` self-determined, so
   `(dif_re + dif_im) * (wr + wi)` evaluated `wr + wi` at `TW` bits — but
   `|cos| + |sin|` reaches 1.41·2^(TW-1), overflowing a Q(TW-1) word for **15 of
   the 32 twiddles**. This corrupts only the difference/twiddle path, which is
   **exactly why DC passes and everything else fails** (every DC difference is
   zero). This is the single most important lesson here: DC and impulse are both
   blind tests.
3. **Pipeline phase.** The stage counters free-ran from reset, so the frame
   boundary depended on how many idle cycles elapsed before `in_valid`. Two
   testbenches differing by *two idle cycles* produced different spectra. `run`
   now holds the pipeline in reset until the first valid sample.

Transform length is free — **no per-size twiddle table.** Stage `i` of an
N-point transform needs `W_N^(j<<i)`, and `W_N^(j<<i) == W_TBL^((j<<i)·(TBL_N/N))`,
so a smaller transform merely indexes the same shared ROM with a shift. One
1024-point table (`verilog/tw12_*.mem`, 512 entries) serves **N = 64, 256 and
1024**, all three verified in CI. This is why the RP2040 does not need to
generate twiddles: there is nothing left for it to compute that a shift register
cannot do.

Residual error grows with N (more stages, more rounding), so the testbench uses
a **relative** tolerance (2–3% of the bin peak + 2 LSB) rather than an absolute
bound — an absolute LSB limit would fail large N for reasons unrelated to
correctness.

Still not instantiated by `v2_top` (the memory-based `v2_fft1024` remains the FFT
mode); it is now part of the CI suite.


`FFT_RATE` = CIC decimation (runtime-selectable via SPI `cfg_decim`; 0 = default):

| FFT_RATE | consume | input to FFT | notes |
|---|---|---|---|
| 1 | 100 MSPS | 100 MSPS | CIC bypass; **FFT cannot keep up**, FIFO overflows |
| 8 | 12.5 MSPS | 12.5 MSPS | only fits the smallest FFT sizes |
| 16 | 6.25 MSPS | 6.25 MSPS | **default** — fastest the 1024-pt FFT sustains |
| 32/64/128 | 3.125/1.56/0.78 MSPS | same | more averaging, narrower band |

### FFT size vs capability

Sustainable input = N / (N + (N/2)·log₂N·4) cycles × 125 MHz (burst-load, four
cycles per butterfly):

| FFT_N | EBR | sustainable MSPS | bin spacing @6.25 MSPS | useful span |
|---|---:|---:|---:|---:|
| 256 | 2 | 9.6 | 24.4 kHz | 3.1 MHz |
| 512 | 4 | 8.6 | 12.2 kHz | 3.1 MHz |
| 1024 | 8 | 7.8 | 6.1 kHz | 3.1 MHz |

`FFT_N` is a `v2_top` parameter (256 / 512 / 1024). All three are verified
end-to-end, including full FFT frames egressing on RGMII.

### Butterfly timing and headroom for more speed

The butterfly is **3 phases** (address → latch+multiply → combine+write); the
combine is combinational off the registered products so it merges with the
write. The data RAM is EBR with a **registered** read (address in phase 0, data
valid in phase 1) — a combinational read would not map to EBR, since 65 kbit
exceeds the 47 kbit of distributed RAM.

Measured headroom: raw mode is **2,400 LUTs of 5,936**, so FFT mode has roughly
3,000 LUTs spare. Further speedup options, in increasing effort:

| Option | Approx. gain | Note |
|---|---:|---|
| more `FFT_N` parallelism (banked RAM, P butterflies) | ~Px | needs conflict-free multi-bank addressing |
| radix-4 (5 stages instead of 10, 3 complex mults each) | ~3x | more registers + twiddle sets |
| pipelined across butterflies within a stage | ~2x | RAM-port bound (2 accesses/cycle) |

The floor with a single dual-port EBR is ~2 cycles/butterfly (2 reads + 2
writes), i.e. ~12 MSPS at N=1024 — banked RAMs are needed to beat that.

## Block diagram / files

| File | Role | Verified |
|---|---|---|
| `v2_top.v` | integration, mode mux, pin-level top | `v2_top_tb` (both modes) |
| `v2_clock_pll.v` | 100→125 MHz PLL (sim model + vendor stub) | via top |
| `v2_phy_manager.v` | KSZ9031 bring-up: ID, reset, RGMII delay regs, AN | `v2_phy_manager_tb` |
| `v2_mdio_master.v` | MDIO frame engine | `v2_mdio_tb`, `v2_mdio_single_read_tb` |
| `v2_rgmii.v` | GMII ↔ RGMII DDR | `v2_rgmii_tb` (loopback) |
| `v2_cdc_fifo.v` | gray-code async FIFO + occupancy | `v2_cdc_fifo_tb` |
| `v2_raw_path.v` | 10/8-bit pack, packetize, BFP-free raw stream | `v2_raw_path_tb` |
| `v2_udp_ip_tx.v` | IPv4 + UDP framing, exact header checksum | `v2_udp_ip_tx_tb` |
| `v2_eth_mac_tx.v` | preamble/SFD/DA/SA/type/FCS/IFG | `v2_eth_mac_tx_tb` |
| `v2_cic_decimator.v` | multiplier-free decimation, runtime rate | `v2_cic_decimator_tb` |
| `v2_fft1024.v` | N-point FFT, one shared butterfly, BFP | `v2_fft1024_tb` |
| `v2_fft_packetizer.v` | bin framing (16-byte header + bins) | `v2_fft_packetizer_tb` |
| `v2_spi_regs.v` | RP2040 register file | `v2_spi_regs_tb` |
| `v2_telemetry.v` | counters for the dashboard | via `v2_spi_regs_tb` |

## Telemetry / dashboard

In-band: every packet carries sequence, base index, **FPGA drop counter**, bit
depth, decimation and flags — so the host sees FPGA-side loss without extra
wiring.

Out-of-band: `v2_spi_regs` + `v2_telemetry` are read by
`firmware/rp2040_dashboard/`, which computes sample rate, payload Mbps, headroom
against the ~957 Mbps UDP ceiling, oversubscription and drops/second, and emits
`TEL:` CSV lines for `software/sdr_dashboard.py`.

## Known non-simulation work (blocks production)

1. **Diamond fit** — see `RESOURCE_REPORT.md`. Raw mode measures **2,400 of
   5,936 LUTs** with real ABC and fits comfortably; FFT mode is unmeasured but
   bounded. Raw mode can be built as soon as Diamond runs.
2. **RGMII DDR I/O cells** — `v2_rgmii.v` now has both a behavioural path (used
   by simulation) and a `V2_USE_VENDOR_DDR` path using the Lattice DDR
   primitives, verified through the loopback with simulation-only models in
   `verilog/sim/crosslink_prims_sim.v`. **Confirm the primitive names/ports**
   against FPGA-TN-02016 (sysI/O Usage Guide) / FPGA-TN-02012 (High-Speed I/O).
3. **Bank constraint — check before layout.** CrossLink GPIO DDR exists **only in
   Bank 1 and Bank 2** (Bank 0 is SDR-only). The RGMII balls (TX E1,E2,F1,F2,J3,J7;
   RX K8,J5,K10,K4,K5,K6) must all be in Bank 1/2. If any is in Bank 0, RGMII
   cannot run on that pin and the assignment must change.
4. **PLL primitive** — `v2_clock_pll.v` carries a `V2_USE_VENDOR_PLL` path
   instantiating the CrossLink sysCLOCK PLL (`EHXPLLL`). Confirm parameters
   against FPGA-TN-02015 (sysCLOCK PLL/DLL Design and Usage Guide) or regenerate
   in Clarity Designer.
5. **Pin constraints** — `verilog/v2_top.lpf`, IO_TYPEs and DDR cells flagged TODO.
6. **Hardware bring-up** — nothing here has touched silicon; there is no
   fabricated board yet.

## Licence path

Diamond is required for bitstream generation and there is no open-source flow
for CrossLink. **The free licence covers CrossLink** (Diamond 3.14 release
notes) and needs only a **12-digit NIC MAC address** — no account linkage — so it
can be issued by support directly. Paid subscriptions (`LSC-SW-NL` node-locked /
`LSC-SW-FL` floating) are sold by Mouser, Digi-Key, Arrow, Farnell and the
Lattice store. Digi-Key does not list the software licence itself.
