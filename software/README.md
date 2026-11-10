# software/

Host-side tools for the V2 board. Everything here is a single-file, dependency-light
Python script: `numpy` (plus `matplotlib` for the viewer) and nothing else.

| File | Purpose |
|---|---|
| `sdr_common.py` | Shared packet parsing: raw-sample unpacking, FFT bin payload, header fields. Imported by both tools. |
| `sdr_spectrum.py` | Live spectrum + waterfall for either FPGA mode, or offline with `--simulate`. Reports network loss and FPGA-side drops. |
| `sdr_dashboard.py` | Headless link dashboard: sample rate, payload Mbps, network loss, FPGA drop counter; optionally reads the RP2040 `TEL:` CSV over USB serial. Also runs a local FFT (that is what raw mode is for). |
| `test_dashboard.py` | Self-checking test of the parse/stats path against synthetic packets in the exact `v2_raw_path` / `v2_fft_packetizer` formats. Runs directly: `python software/test_dashboard.py`. |

Quick use (from the repository root):

```bash
python software/sdr_spectrum.py --simulate --save spectrum.png --frames 20
python software/sdr_spectrum.py --mode raw
python software/sdr_spectrum.py --mode fft
python software/sdr_dashboard.py --port 10000 --serial COM7
python software/test_dashboard.py
```

The FPGA transmits **to** `cfg_dst_port` (power-on value 10000); 4660/4661 are
only the *source* ports it stamps in the header, so binding those receives
nothing. If a tool shows no data, check the port first.

This is a fresh tree. `LEGACY.md` records what the four removed V1 software trees
(`wideband-sdr-software/`, `gr-wideband_sdr/`, `extio_plugin/`,
`legacy/wideband-sdr-firmware/`) attempted and why they were abandoned.
