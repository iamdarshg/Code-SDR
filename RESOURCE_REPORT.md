# Code-SDR V2 — Resource Report (Yosys estimate)

Target: **LIF-MD6000-6JMG80I** — 5,936 LUT4, 180 kbit EBR (20 × 9 kb), no DSP blocks.

Method: `yowasp-yosys` (free, no license) with `synth -noabc -lut 4` → `gate2lut`
mapping. **ABC cannot run under the WASM build** (it needs an external binary), and
`gate2lut` is unoptimized, so these numbers typically **overestimate real ABC/LSE
results by roughly 1.5–2×**. Treat them as an upper bound, not a fit guarantee.
Real numbers require Lattice Diamond (blocked on the licence portal).

## Per-module estimate

| Module | LUT4 (gate2lut) | DFF | EBR (analytical) |
|---|---:|---:|---:|
| v2_rgmii | 10 | 46 | — |
| v2_mdio_master | 254 | 192 | — |
| v2_phy_manager | 1216 | 580 | — |
| v2_telemetry | 266 | 2 | — |
| v2_spi_regs | 334 | 302 | — |
| v2_eth_mac_tx | 1454 | 302 | — |
| v2_udp_ip_tx | 1312 | 318 | — |
| v2_cic_decimator | 1721 | 341 | — |
| v2_fft_packetizer | 604 | 324 | 1 (256 × 32 b) |
| v2_fft1024 | *see note* | *see note* | ~8 |

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
