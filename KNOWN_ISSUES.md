# Known Issues

Findings from code review of the V2 gateware, host tools and firmware.
Everything is either **fixed** (kept for the record) or **open**, with the reason
it was deferred.

Severity: **CRITICAL** = blocks hardware, **MAJOR** = wrong behaviour in some
configuration, **MINOR** = latent or cosmetic.

---

## Fixed

### Critical

| # | Issue | Fix |
|---|---|---|
| 1 | `v2_clock_pll.v` tied `CLKFB_DIV` to `CLKOP_DIV`. With CLKOP feedback the VCO is `f_in·CLKFB_DIV/CLKI_DIV = 400 MHz` and `CLKOP = 100 MHz` — the Ethernet MAC would have been clocked at the ADC rate. | `CLKFB_DIV = 5` (VCO 500 MHz → CLKOP 125 MHz). |
| 2 | **The RP2040 firmware did not compile.** `rp2040_dashboard.ino` printed `drop_pct`, which was never declared — an edit had added the print but not the computation. The whole dashboard was unbuildable. | Compute `drop_pct` from `drop_fraction_for()` and use `offered_mbps()` for the payload figure. |

### Major

| # | Issue | Fix |
|---|---|---|
| 3 | `v2_fft1024.v` loader off-by-one: `load_cnt` is `LOGN` bits so `load_cnt < N` is always true. With the one-cycle `in_valid` skew in `v2_top` the FIFO was read **N+1 times per frame** — one sample discarded, every frame starting one sample later. | `v2_top` bounds reads per frame to exactly `FFT_N`. |
| 4 | `cfg_decim` / `cfg_drop_frac` were written in `clk_eth` and sampled directly in the 100 MHz domain, so a mid-write transition could be read as an arbitrary value. | Stable-value synchroniser: hold until two consecutive ADC-domain samples agree. |
| 5 | **FFT `flags` byte bit layout was inverted.** The RTL emitted `{6'b0, ovf_lat, 1'b1}` (bit1 = overflow) while the header comment and the host both use bit0 = overflow, so the host read the constant `1` as "overflow" and flagged **every** FFT packet. The TB only tested the no-overflow case. | Emit `{6'b0, 1'b1, ovf_lat}`; the TB now asserts the whole byte against the documented layout. |
| 6 | **`link_up` was sampled once at bring-up and then frozen**; `phy_int_n` was in the port list but never referenced, so a cable unplug never updated it. | `v2_phy_manager` re-polls BMSR every 2 ms and on a `phy_int_n` falling edge; the TB now proves link drops and re-asserts. |
| 7 | **CIC `rate_cfg` was not restricted to powers of two.** `log2i()` returns 4 for any unknown value, so the gain-removal shift was always 12 while the real gain is `rate³` — `rate_cfg = 100` gave ≈244× gain into a signed 16-bit output and wrapped. | Snap the rate to the nearest power of two and use the same value for the decimation counter and the gain shift. TB covers `rate_cfg` 48 and 100. |
| 8 | **CIC→FFT FIFO overflow was completely invisible.** The CIC writes unconditionally and a full FIFO drops the write; nothing counted it, so a host choosing a rate the FFT cannot sustain (e.g. `cfg_decim = 1`) would lose most samples silently. | Drop events are toggle-synchronised into `clk_eth`, counted, and folded into `overflow_count` so `sticky_overflow` latches and the dashboard shows it. |

### Minor

| # | Issue | Fix |
|---|---|---|
| 9 | `v2_fft1024.v`: the stage-final butterfly's guard hit was overwritten by `need_scale <= 1'b0`, excluding it from the BFP decision. | `calc_next` now takes the current butterfly's guard hit into the stage-end decision. |
| 10 | `v2_fft1024.v`: the window was applied to `in_re` only. | Applied to `in_im` as well (identical result when `in_im = 0`). |
| 11 | `v2_fft1024.v`: `overflow_count` was reset but never incremented. | Incremented once per overflowing frame. |
| 12 | `async_fifo.v`: `FULL_MASK` malformed for `ADDR_WIDTH == 1` (zero-width replication). | Written as `(3 << (ADDR_WIDTH-1))` — bit-identical for all widths, legal at `ADDR_WIDTH==1`. |
| 13 | `v2_mdio_master.v`: `mdio_i` sampled without a synchroniser; the MDC rate comment/check disagreed with the actual 125 MHz clock. | Two-flop sync on `mdio_i`; the guard now enforces the real KSZ9031 ceiling (25 MHz) and the comment is corrected. |
| 14 | `v2_top.v`: `effective_mbps` was hard-wired to 0, so telemetry `0x10` and the dashboard's Mbps/headroom always read zero. | Computed from the configured width and rate. |
| 15 | `v2_top.v`: `{{(10-FFT_LOGN){1'b0}}, fft_index}` is a zero-width replication in the default `FFT_N=1024` config (illegal per IEEE 1364-2001 §4.1.1). | Pass `fft_index` directly. |
| 16 | Firmware: a bitstream swap did not re-apply the delta-sigma drop fraction (reconfiguration resets it to 0), leaving the link oversubscribed until the user touched a rate command. | `apply_rate_config()` is called in the mode-switch branch. |
| 17 | `sdr_spectrum.py` bound to UDP 4660/4661 by default, but the FPGA transmits **to** `cfg_dst_port` = 10000 (its reset value, never changed) and only *stamps* 4660/4661 as source ports. The tool received nothing. | Defaults to 10000; added `--fs` for the axis. |
| 18 | Status reported `cfg_bits` as the sample depth, but the raw path packs at the elaboration-time `SAMPLE_BITS`, so a `b 10` write changed the reported depth and the firmware's rate plan without changing the data. | Status reports the true `SAMPLE_BITS`; the firmware no longer issues or advertises a runtime `b` command. |
| 19 | *(Correction)* The docs claimed a Karatsuba operand-sum overflow in `v2_fft_pipe`. **That was wrong** — Verilog extends both sums to the multiply's width; reverting the "fix" still passes. | Docs corrected; the widened wires stay only as a portable clamp. |
| 20 | **`v2_telemetry`'s `packets_dropped` pin was left unconnected in `v2_top`**, so register `0x18` — the deliberate-drop count the firmware displays — read garbage/floating instead of the real count. Found by the new Verilator lint gate; **21 passing simulations all missed it** because nothing asserted on that register. | Connect `.packets_dropped(raw_dropped_pkts)`. |
| 21 | **The RP2040 firmware had two more whole-file errors** beyond the `drop_pct` one: `selftest()` was defined *above* the `REG_*` `#define`s it reads (a `#define` is resolved lexically), and removing the runtime `b` command left an `else if` chain with no leading `if`. Found by the new firmware compile check. | Defines moved above `selftest()`; the chain starts with `if`. |

### New gates (a blocker was that neither existed)

- `tools/lint_verilog.py` — Verilator lint gate over 5 tops, failing on a
  curated defect set (UNDRIVEN, MULTIDRIVEN, LATCH, SELRANGE, PINMISSING,
  MODMISSING, …) and printing the known-benign codes with the reason each is
  silenced. It found #20 and the dead `br/bi/wr/wi` registers in `v2_fft1024`.
- `tools/check_firmware.py` — compiles the sketch for `rp2040:rp2040:rpipico`.
  It found #21. Both are wired into `.github/workflows/rtl.yml`.

### Removed

- **The P0 cluster is gone.** `fpga_processing_pipeline.v` and everything used
  only by it (`adc_interface`, `adaptive_gain_scaler`, `clock_manager`,
  `compensation_filter`, `digital_downconverter`, `ethernet_mac`, `fft_packetizer`,
  `fft_processor`, `hamming_window`, `lifmd6000_clock_pll`, `nco_generator`,
  `rp2040_interface`, `udp_ip_stack`, `cic_decimator`, `signal_processing_testbench`,
  `fpga_testbench`) plus their testbenches, `synthesize.tcl`,
  `fpga_timing_constraints.sdc`, `V2_HARDWARE_COMPATIBILITY.md`, the verified
  `build_diamond.tcl` file list's stale siblings, and the legacy-only cocotb
  tests / fixtures / runners. **Recoverable from git history.**
  Note this includes `adc_interface.v`, which was the subject of P0 issue #59 —
  restore it from history if that fix is still wanted.

---

## Open

### Major

1. **Runtime bit-depth switching is not implemented.** The raw path's packing
   width is the `SAMPLE_BITS` elaboration parameter (the FIFO word is a fixed
   40 bits, 8-bit packs 5 samples, 10-bit packs 4). Both widths are built and
   tested, but they are *build-time* choices — `REG_BITS` is inert by design now
   that status reports the truth. Implementing runtime switching needs: a muxed
   sample slice (`adc_data[9:2]` vs `[9:0]` — a variable-width part-select is
   not legal Verilog), the accumulator as `(acc << bits) | sample`, and a
   runtime samples-per-word and `NSAMP`. *Deferred as a feature, not a fix.*

2. **Hardware paths are gated on macros no build script defines.**
   `V2_USE_VENDOR_PLL` and `V2_USE_VENDOR_DDR` appear nowhere else, so the build
   elaborates the behavioural clock and the fabric RGMII mux — neither
   synthesizable as intended. *This is the Diamond-licence blocker, not a code
   defect: the vendor primitives must be generated by the tool and their ports
   confirmed against FPGA-TN-02015 / -02016 / -02012.*

### Minor

3. **`sdr_spectrum.py`'s frequency axis in FFT mode is still nominal** — the
   axis is built from `--fft` (default 4096) while the FPGA emits 1024 bins.
   Raw mode is correct now that `--fs` exists (default 50 MSPS = 100/decim 2).

4. **`v2_fft_packetizer` can drop one bin at a packet boundary.** When the last
   byte is taken, `wr_idx <= 0` and the intake branch can still see
   `buffer_full == 1` on that same edge, so an `fft_valid` arriving there is
   counted in `bins_dropped` and lost — which desynchronises the next packet's
   `first_bin`. Counted, not silent, and timing-dependent.

5. **Latent register no-ops.** `cfg_enable`, `cfg_nco_freq` and (for the
   datapath) `cfg_mode` are produced by `v2_spi_regs` but not consumed by
   `v2_top`; the datapath cannot actually be gated by software. The firmware only
   ever writes `ENABLE = 1` and never touches `NCO`, so nothing is broken today.

6. **`reset_n = rst_n & eth_locked` is used as an asynchronous reset in the
   100 MHz domain.** `eth_locked` is not synchronous to `clk_100m_in`, so reset
   de-assertion can violate recovery/removal. Wants a per-domain reset
   synchroniser.

7. **`v2_raw_path`'s `overflow_count` (not the gray-coded `ovf_bin`) still drives
   telemetry's `sticky_overflow` across domains.** Monotonic + sticky masks the
   usual tearing damage, but it is still a CDC hazard.

8. **`v2_fft_pipe`'s `run` aligns only the first frame.** `started` latches on
   the first `in_valid` and never clears, so gapped bursts after the first frame
   are no longer block-aligned. Correct for the intended continuous 100 MSPS
   stream.

---

## Verified correct (checked, no change needed)

- Karatsuba operand sums in `v2_fft1024` (same shape as the non-issue in
  `v2_fft_pipe`): **not a bug** — Verilog sizes the sum to the multiply context.
- IPv4 header, checksum, UDP length, MAC preamble/SFD/FCS/IFG and the
  byte-count handshakes across UDP→MAC→source all line up cycle-for-cycle.
- Raw packet header (seq/base/drops/`0xA5`/bits/decim/flags) matches the host
  parsers; 8/10-bit packing round-trips against `sdr_common.unpack_samples`.
- Telemetry register map and status bit packing match the firmware addresses.
- SPI framing, address decode and read/write byte positions are consistent.
- CDC FIFO gray-pointer scheme, `full`/`empty` and `rd_avail` occupancy.
- RGMII nibble order on both the behavioural and vendor DDR paths.

---

## Repo hygiene

- `app_stream_cdc.v`, `async_fifo.v` and `v2_fft_pipe.v` are not instantiated by
  `v2_top`; they are retained deliberately (CI-tested, and `v2_fft_pipe` is the
  verified 100 MSPS FFT).
- No build artifacts are tracked. `outputs/` holds real PCB routing deliverables.
- The authoritative regression is `python tools/run_tests.py` (21 testbenches,
  plus the two extra FFT sizes), also run by `.github/workflows/rtl.yml`.
