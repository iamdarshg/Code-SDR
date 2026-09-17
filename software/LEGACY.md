# Removed Software — What It Attempted

All four software trees were removed to start fresh under `software/`.
Last verified against commit history before removal (Sept 2026).
Full source recoverable from git history (`git log -- <path>`).

## 1. `wideband-sdr-software/` — Python host driver + DSP helpers
Attempted function: PyUSB host driver for the SDR (1 MHz – 10 GHz claims):
frequency/gain/sample-rate commands, async sample streaming with
callback-or-queue delivery, thread-safe multi-device support.
Helpers: `antenna_controller.py` (antenna select/diversity),
`lna_controller.py` (BGA614 gain), `calibration.py` (+ `test_calibration.py`),
`dc_offset_corrector.py`, `digital_downconverter.py` (host-side DDC mirror of
the FPGA block), `recording.py`, `computer_side_interface.py`, packaging in
`setup.py`, demos in `examples/` (frequency scanner, recording, spectrum
analyzer, waterfall display). Entry point: `wideband_sdr.py` (`WidebandSDR`).

## 2. `gr-wideband_sdr/` — GNU Radio out-of-tree module
Attempted function: native GNU Radio / GRC integration exposing a
"Wideband SDR Source" block (1 MHz – 10 GHz, up to 105 MSPS, 0–63 dB gain,
USB 2.0 HS streaming). Layout: `include/gnuradio/wideband_sdr/source.h`,
`lib/source_impl.{h,cc}`, pybind11 bindings in `python/bindings.cc`,
GRC block def in `grc/wideband_sdr_source.xml`, demo app in
`apps/wideband_sdr_spectrum_analyzer.py`, CMake build (`CMakeLists.txt`,
`lib/CMakeLists.txt`, `python/CMakeLists.txt`). Documented in its `README.md`
(GNU Radio ≥ 3.8, CMake ≥ 3.8.2, pybind11, Boost).

## 3. `extio_plugin/` — Windows ExtIO bridge (HDSDR / SDR#)
Attempted function: `ExtIO.dll` shim so HDSDR/SDR# could use the hardware
(1 MHz – 10 GHz, 1 kSPS – 105 MSPS, 0–63 dB gain, multi-device enum),
built on the Python driver via pybind11. Files: ExtIO C ABI in
`include/extio.h`, wrapper in `include/wideband_sdr_extio.h` +
`src/extio_impl.cpp` / `src/wideband_sdr_extio.cpp`, version resource in
`src/version_resource.rc`, CMake + VS2019 build, install notes in `README.md`
(Zadig WinUSB, VID 0x04D8 / PID 0x000A), task tracking in
`extio_plugin_tasks.md`.

## 4. `legacy/wideband-sdr-firmware/` — dsPIC33 device firmware (pre-RP2040)
Attempted function: firmware for the abandoned dsPIC33AK256MC505 MCU target
(XC16 toolchain, `Makefile`), predating the LIF-MD6000 FPGA + RP2040
architecture. `main.c`: USB bulk streaming, 16 KB ping-pong DMA buffers,
command set (set frequency / sample rate / gain, start/stop stream, status,
calibrate, power), watchdog/error recovery. Peripherals: `adc_dma.*` (AD9215
capture), `adf4351.*` (PLL synth), `usb_device.*` (USB stack),
`antenna_control.c`, `lna_control.c`, `dc_offset.*`, `dsp_fft.*` (on-chip FFT).
Superseded by FPGA RTL in `verilog/` + RP2040 SPI control
(`verilog/rp2040_interface.v`); host side moves to the new `software/`.
