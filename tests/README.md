# tests/

**The authoritative regression for this project is `python tools/run_tests.py`**
(21 Icarus Verilog testbenches, run by `.github/workflows/rtl.yml` on every push).
Use that, not this directory.

## What is left here

`cocotb_tests/test_async_fifo.py` is the only surviving test in this directory.
It is a cocotb test for `verilog/async_fifo.v`, kept because that module is still
used by `verilog/app_stream_cdc.v` and covered by the Icarus suite.

Run it with cocotb/pytest and Verilator if you have them:

```
pip install -r tests/requirements.txt
cd tests/cocotb_tests && pytest -p pytest_cocotb --sim=verilator \
    --toplevel=async_fifo test_async_fifo.py
```

## History

This directory used to hold the P0 simulation framework — cocotb tests, a
Makefile, shell/batch runners and JSON fixtures for the first-generation
pipeline (`fpga_processing_pipeline`, `adc_interface`, `nco_generator`,
`fft_processor`, `ethernet_mac`, `udp_ip_stack`, `digital_downconverter`,
`compensation_filter`, `adaptive_gain_scaler`, `clock_manager`, `hamming_window`,
`rp2040_interface`).

That design was superseded by the V2 gateware under `verilog/v2_*.v` and has
been removed, together with `verilog/synthesize.tcl` and the legacy-only tests,
runners and fixtures that referenced it. It is recoverable from git history.
