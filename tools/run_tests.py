#!/usr/bin/env python3
"""
Run the Code-SDR V2 RTL regression with Icarus Verilog.

Compiles and runs every testbench, printing PASS/FAIL per test and exiting
non-zero if any fails. Works locally and in CI (expects `iverilog` and `vvp` on
PATH).

Usage:
  python tools/run_tests.py            # all tests
  python tools/run_tests.py --filter raw
"""

import argparse
import os
import shutil
import subprocess
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(REPO, "outputs")

# top-level testbench -> source files (excluding the TB itself)
TESTS = [
    ("v2_mdio_tb",             ["verilog/v2_mdio_tb.v", "verilog/v2_mdio_master.v"]),
    ("v2_mdio_single_read_tb", ["verilog/v2_mdio_single_read_tb.v", "verilog/v2_mdio_master.v"]),
    ("v2_phy_manager_tb",      ["verilog/v2_phy_manager_tb.v", "verilog/v2_phy_manager.v",
                                "verilog/v2_mdio_master.v"]),
    ("v2_cdc_fifo_tb",         ["verilog/v2_cdc_fifo_tb.v", "verilog/v2_cdc_fifo.v"]),
    ("v2_eth_mac_tx_tb",       ["verilog/v2_eth_mac_tx_tb.v", "verilog/v2_eth_mac_tx.v"]),
    ("v2_udp_ip_tx_tb",        ["verilog/v2_udp_ip_tx_tb.v", "verilog/v2_udp_ip_tx.v",
                                "verilog/v2_eth_mac_tx.v"]),
    ("v2_raw_path_tb",         ["verilog/v2_raw_path_tb.v", "verilog/v2_raw_path.v",
                                "verilog/v2_cdc_fifo.v", "verilog/v2_udp_ip_tx.v",
                                "verilog/v2_eth_mac_tx.v"]),
    ("v2_raw_drop_tb",         ["verilog/v2_raw_drop_tb.v", "verilog/v2_raw_path.v",
                                "verilog/v2_cdc_fifo.v"]),
    ("v2_spi_regs_tb",         ["verilog/v2_spi_regs_tb.v", "verilog/v2_spi_regs.v",
                                "verilog/v2_telemetry.v"]),
    ("v2_cic_decimator_tb",    ["verilog/v2_cic_decimator_tb.v", "verilog/v2_cic_decimator.v"]),
    ("v2_fft1024_tb",          ["verilog/v2_fft1024_tb.v", "verilog/v2_fft1024.v"]),
    # pipelined FFT: verified against a DFT computed in the testbench itself
    ("v2_fft_pipe_tb",        ["verilog/v2_fft_pipe_tb.v", "verilog/v2_fft_pipe.v"]),
    ("v2_fft_packetizer_tb",   ["verilog/v2_fft_packetizer_tb.v", "verilog/v2_fft_packetizer.v"]),
    ("v2_rgmii_tb",            ["verilog/v2_rgmii_tb.v", "verilog/v2_rgmii.v"]),
    ("app_stream_cdc_tb",      ["verilog/app_stream_cdc_tb.v", "verilog/app_stream_cdc.v",
                                "verilog/async_fifo.v"]),
    ("v2_top_tb",              ["verilog/v2_top_tb.v", "verilog/v2_top.v",
                                "verilog/v2_clock_pll.v", "verilog/v2_phy_manager.v",
                                "verilog/v2_mdio_master.v", "verilog/v2_rgmii.v",
                                "verilog/v2_raw_path.v", "verilog/v2_cdc_fifo.v",
                                "verilog/v2_udp_ip_tx.v", "verilog/v2_eth_mac_tx.v",
                                "verilog/v2_telemetry.v", "verilog/v2_spi_regs.v",
                                "verilog/v2_cic_decimator.v", "verilog/v2_fft1024.v",
                                "verilog/v2_fft_packetizer.v"]),
]


def run_one(iv, vvp, name, files, params):
    os.makedirs(OUT, exist_ok=True)
    exe = os.path.join(OUT, f"{name}.vvp")
    cmd = [iv, "-g2012", "-s", name, "-o", exe] + params + files
    r = subprocess.run(cmd, cwd=REPO, capture_output=True, text=True)
    if r.returncode != 0:
        return False, f"COMPILE FAIL: {r.stderr.strip().splitlines()[:1]}"
    r = subprocess.run([vvp, exe], cwd=REPO, capture_output=True, text=True, timeout=1800)
    out = r.stdout + r.stderr
    if "PASS" in out and "FATAL" not in out:
        line = [l for l in out.splitlines() if l.strip().startswith("PASS")]
        return True, (line[-1].strip() if line else "PASS")
    bad = [l for l in out.splitlines() if "FATAL" in l or "FAIL" in l]
    return False, (bad[-1].strip() if bad else "no PASS marker")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--filter", default=None)
    args = ap.parse_args()

    iv = shutil.which("iverilog")
    vvp = shutil.which("vvp")
    if not iv or not vvp:
        print("iverilog/vvp not found on PATH")
        return 2

    total = passed = 0
    for name, files in TESTS:
        if args.filter and args.filter not in name:
            continue
        total += 1
        ok, msg = run_one(iv, vvp, name, files, [])
        print(f"{name:<26} {'PASS' if ok else 'FAIL'}  {msg}")
        passed += 1 if ok else 0

    # both radio modes of the integrated top
    for mode in (0, 1):
        name = f"v2_top_tb_mode{mode}"
        total += 1
        base = next(f for n, f in TESTS if n == "v2_top_tb")
        ok, msg = run_one(iv, vvp, "v2_top_tb", base, [f"-Pv2_top_tb.MODE={mode}"])
        print(f"{name:<26} {'PASS' if ok else 'FAIL'}  {msg}")
        passed += 1 if ok else 0

    # raw path at BOTH widths: 8-bit packs 5 samples/word, 10-bit packs 4.
    # (The default entry above runs 10-bit; this adds the full-speed 8-bit mode.)
    for bits, decim in ((8, 1),):
        name = f"v2_raw_path_tb_{bits}bit"
        total += 1
        base = next(f for n, f in TESTS if n == "v2_raw_path_tb")
        ok, msg = run_one(iv, vvp, "v2_raw_path_tb", base,
                          [f"-Pv2_raw_path_tb.SAMPLE_BITS={bits}",
                           f"-Pv2_raw_path_tb.DECIM={decim}"])
        print(f"{name:<26} {'PASS' if ok else 'FAIL'}  {msg}")
        passed += 1 if ok else 0

    # pipelined FFT at other transform lengths: proves the one shared twiddle
    # ROM really does serve every N (the default entry above runs N=64).
    for fft_n in (256, 1024):
        name = f"v2_fft_pipe_tb_n{fft_n}"
        total += 1
        files = next(f for tb, f in TESTS if tb == "v2_fft_pipe_tb")
        ok, msg = run_one(iv, vvp, "v2_fft_pipe_tb", files,
                          [f"-Pv2_fft_pipe_tb.N={fft_n}"])
        print(f"{name:<26} {'PASS' if ok else 'FAIL'}  {msg}")
        passed += 1 if ok else 0


    print(f"--- {passed}/{total} passed ---")
    return 0 if passed == total else 1


if __name__ == "__main__":
    sys.exit(main())
