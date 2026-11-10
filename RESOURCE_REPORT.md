# Code-SDR V2 — Resource Report (Yosys estimate)

Target: **LIF-MD6000-6JMG80I** — 5,936 LUT4, 180 kbit EBR (20 × 9 kb), no DSP blocks.

Method: **native Yosys + real ABC** (msys2 `mingw-w64-x86_64-yosys`, free, no
licence) with `abc -lut 4`, keeping memories out of the logic count
(`memory -nomap`) so EBR arrays are not turned into mux/FF logic. The earlier
`gate2lut` estimates in this document were ~2.5x pessimistic; the ABC numbers
below supersede them.

## Measured (ABC)

| Build | LUT4 | FFs | RAM |
|---|---:|---:|---:|
| **MODE=0 (raw)** | **2,400** | ~1,150 | 2 |
| MODE=1 (FFT) | *not yet measurable — see below* | | |

**Raw mode fits comfortably**: 2,400 of 5,936 LUTs (~40%), ~1,150 of the FFs,
and 2 RAM blocks. That is a real ABC place-independent figure, not an estimate.

### Why FFT mode has no number yet — and a real bug it exposed

ABC cannot finish on the FFT build, and the reason matters: `v2_fft1024` reads
its memories **combinationally** (`ar <= mem_re[idx_a]` in the same cycle the
address is formed). With no BRAM target Yosys builds 1024-entry 1-read mux
trees, which ABC cannot optimise in reasonable time.

On the real device those arrays are ~65 kbit, which **exceeds the 47 kbit of
distributed RAM**, so they must live in EBR — and **EBR has a registered output**.
A combinational read therefore cannot synthesise as written, which is why
`v2_fft1024` now registers its read addresses (`ra_a`/`ra_b`) one cycle ahead and
uses a 3-phase butterfly (address → latch+multiply → combine+write). That RTL fix
landed in commit `63dd970`. The reason FFT mode still has no area number is
downstream of the RTL: with no BRAM target Yosys elaborates the 1024-entry arrays
as mux trees, and ABC cannot optimise those in reasonable time.

The FFT's *logic* is still bounded analytically: three 16x16 multipliers
(Karatsuba complex multiply) plus the sequencer. A 16x16 LUT4 multiplier is
roughly 200-300 LUTs, so the FFT is on the order of **1,000-1,500 LUTs**, which
plus the measured raw-mode infrastructure (~2,400 LUTs, and MODE=1 drops the
raw packetiser while adding the CIC ~1,700 `gate2lut` / ~600 ABC and the
packetiser ~600 `gate2lut`) should fit — but that is an estimate, not a
measurement.


`v2_fft1024` reports 299,724 "LUTs" / 66,190 "DFFs" under Yosys purely because there
is no BRAM target: its memories (`mem_re`/`mem_im`, the twiddle ROMs, the window
ROM) sink into flip-flops. That figure is meaningless. Analytically:

| FFT memory | Bits | EBR |
|---|---:|---:|
| mem_re + mem_im (1024 × 16 × 2) | 32,768 | 4 |
| twiddle cos/sin (512 × 16 × 2) | 16,384 | 2 |
| window ROM (1024 × 16) | 16,384 | 2 |
| **FFT total** | **65,536** | **≈ 8** |

The FFT's *logic* is one Karatsuba complex multiplier (3 × 16×16 ≈ 450 LUT) plus
the sequencer/address generator (≈ 400 LUT) — roughly **850 LUT**.

`FFT_N` is selectable (256 / 512 / 1024). Memory scales as `FFT_N × 64` bits and
the logic is essentially size-independent:

| FFT_N | EBR | sustainable MSPS |
|---|---:|---:|
| 256 | 2 | 8.9 |
| 512 | 4 | 6.6 |
| 1024 | 8 | 6.0 |

## Mode totals

**MODE = 0 (raw)** — the 100 MSPS path.
```
logic  ≈ 4.8k LUT4 (gate2lut)   EBR ≈ 4   (raw CDC FIFO 1024 × 32)
```

**MODE = 1 (1024-pt FFT)**
```
logic  ≈ 7.8k LUT4 (gate2lut)   EBR ≈ 9
```

### Honest conclusion

- **Raw mode** is comfortably within budget; even the pessimistic `gate2lut`
  figure fits.
- **FFT mode is over the 5,936-LUT budget under `gate2lut`** (≈7.8k). Since
  `gate2lut` typically overestimates by 1.5–2×, the real ABC/LSE result is
  plausibly 4–5k and may fit — but **this is not proven and must be confirmed in
  Diamond.** If it does not fit, the levers are, in order of least harm:
  1. reduce the FFT to 512 or 256 points (halves memory and control logic),
  2. drop `STAGES` in the CIC from 3 to 2,
  3. trim `v2_eth_mac_tx`/`v2_udp_ip_tx` (the two largest logic consumers).

The two modes are **compile-time exclusive**, so only one is ever built. That was
a deliberate choice: a 1024-point FFT plus the raw streamer would not fit
together regardless of optimisation.

## Not covered here

- Timing analysis, PLL configuration, DDR I/O cells, and true LUT/EBR/FF counts
  all require Lattice Diamond. The `.lpf` in `verilog/v2_top.lpf` carries the pin
  constraints; its `IO_TYPE`s and the RGMII DDR-cell substitution are flagged TODO.
- Power, and any first-article measurement, obviously require hardware.
