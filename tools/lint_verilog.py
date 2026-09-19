#!/usr/bin/env python3
"""
Verilator lint gate for the Code-SDR V2 gateware.

Icarus runs the behaviour; Verilator catches the static class of bug that
simulation does not, because a passing simulation only exercises the paths it
happens to drive. This gate was added after review found a zero-width
replication, an inverted header bit-layout and dead registers - all of which a
linter flags immediately.

Policy: lint every V2 top with -Wall, print everything, and FAIL on the warning
codes that indicate a real defect. Codes that are known-benign here are listed
explicitly with the reason, so silencing one is a visible decision rather than a
blanket -Wno.

Usage:
    python tools/lint_verilog.py            # all tops
    python tools/lint_verilog.py --filter top
"""

import argparse
import os
import re
import shutil
import subprocess
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Consistent with tools/run_tests.py: top -> sources (excluding the top itself)
COMMON = [
    "verilog/v2_clock_pll.v", "verilog/v2_phy_manager.v", "verilog/v2_mdio_master.v",
    "verilog/v2_rgmii.v", "verilog/v2_raw_path.v", "verilog/v2_cdc_fifo.v",
    "verilog/v2_udp_ip_tx.v", "verilog/v2_eth_mac_tx.v", "verilog/v2_telemetry.v",
    "verilog/v2_spi_regs.v", "verilog/v2_cic_decimator.v", "verilog/v2_fft1024.v",
    "verilog/v2_fft_packetizer.v",
]

TOPS = {
    "v2_top": ["verilog/v2_top.v"] + COMMON,
    # v2_top in FFT mode too - it is a generate branch, so it needs its own pass
    "v2_top_fft": None,
    "v2_fft_pipe": ["verilog/v2_fft_pipe.v"],
    "app_stream_cdc": ["verilog/app_stream_cdc.v", "verilog/async_fifo.v"],
    "async_fifo": ["verilog/async_fifo.v"],
}

# Warning codes that indicate a genuine defect: fail the gate on these.
FATAL_CODES = {
    "UNDRIVEN",        # dead register / net nothing drives
    "MULTIDRIVEN",     # two drivers on one net
    "LATCH",           # inferred latch in combinational logic
    "IMPLICIT",        # undeclared signal (implicit wire)
    "SELRANGE",        # constant index out of range - catches zero-width faults
    "CASEINCOMPLETE",  # incomplete case in combinational logic
    "ALWCOMBORDER",    # variable used before set in always_comb
    "BLKANDNBLK",      # mixed blocking and non-blocking on one signal
    "PINMISSING",      # instance port left unconnected
    "MODMISSING",      # instantiated module not provided
    "COMBDLY",         # non-blocking assignment in combinational logic
    "GENCLK",          # generated or gated clock
    "SYNCASYNCNET",    # net used with both sync and async resets
}

# Known-benign here, each with the reason. These are printed but do not fail.
BENIGN_CODES = {
    "DECLFILENAME":  "file name need not match module name",
    "UNUSEDSIGNAL":  "ports/regs kept for interface clarity",
    "UNUSED":        "unused parameter/localparam in some configurations",
    "WIDTHEXPAND":   "intentional width extension in arithmetic expressions",
    "WIDTHE":        "same family as WIDTHEXPAND",
    "WIDTHTRUNC":    "deliberate Q-format truncation at each rounding stage",
    "BLKSEQ":        "blocking assignments inside functions and $readmemh",
    "PROCASSINIT":   "declaration-time initialiser, fine for ROMs",
    "SIDEEFFECT":    "$readmemh on a memory array",
    "TIMESCALEMOD":  "timescale not set on every module",
    "VARHIDDEN":     "local shadows an outer name",
    "PINCONNECTEMPTY": "deliberately unconnected output",
}


def find_verilator():
    """Return (argv_prefix, env). Handles the msys2 install layout on Windows."""
    exe = shutil.which("verilator")
    if exe:
        return [exe], os.environ.copy()

    for cand in (
        r"C:\msys64\mingw64\bin\verilator_bin.exe",
        r"C:\msys64\mingw64\bin\verilator",
    ):
        if os.path.exists(cand):
            env = os.environ.copy()
            root = r"C:\msys64\mingw64\share\verilator"
            if os.path.isdir(root):
                env["VERILATOR_ROOT"] = root
            return [cand], env
    return None, None


def lint(argv, env, name, files):
    top = "v2_top" if name == "v2_top_fft" else name
    cmd = argv + [
        "--lint-only", "-Wall", "--timing",
        "-Wno-DECLFILENAME",
        "--top-module", top,
    ] + (["-GMODE=1"] if name == "v2_top_fft" else []) + files
    r = subprocess.run(cmd, cwd=REPO, capture_output=True, text=True, env=env)
    return r.stdout + r.stderr


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--filter", default=None)
    args = ap.parse_args()

    argv, env = find_verilator()
    if argv is None:
        print("verilator not found (install it, or set VERILATOR_ROOT for an "
              "msys2 install)")
        return 2
    print(f"verilator: {argv[0]}\n")

    total = failed = 0
    for name, files in TOPS.items():
        if args.filter and args.filter not in name:
            continue
        if files is None:                      # v2_top in FFT mode
            files = ["verilog/v2_top.v"] + COMMON
        total += 1
        out = lint(argv, env, name, files)

        codes = re.findall(r"%(?:Warning|Error)-([A-Z0-9]+)", out)
        fatal = sorted({c for c in codes if c in FATAL_CODES})
        benign = sorted({c for c in codes if c in BENIGN_CODES})
        unknown = sorted({c for c in codes if c not in FATAL_CODES and c not in BENIGN_CODES})
        # Verilator ends a --lint-only run with "%Error: Exiting due to N warning(s)"
        # purely because warnings were emitted; that is not itself a failure.
        errors = any("%Error" in l and "Exiting due to" not in l
                     for l in out.splitlines())

        if fatal or unknown or errors:
            failed += 1
            print(f"{name:<16} FAIL")
            for c in fatal:
                print(f"   FATAL   {c}")
            for c in unknown:
                print(f"   UNCLASSIFIED {c}  (add to FATAL or BENIGN with a reason)")
            if errors:
                for line in out.splitlines():
                    if "%Error" in line:
                        print("   " + line.strip())
        else:
            note = f"  (benign: {', '.join(benign)})" if benign else ""
            print(f"{name:<16} PASS{note}")

    print(f"\n--- {total - failed}/{total} tops clean ---")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
