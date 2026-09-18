#!/usr/bin/env python3
"""
Generate Q-format twiddle ROM .mem files for the pipelined FFT.

The memory-based v2_fft1024 uses 16-bit Q15 tables (verilog/twiddle_*.mem).
The pipelined v2_fft_pipe uses a narrower datapath to afford one multiplier per
stage, so it needs narrower tables.

Usage:
  python tools/gen_twiddles.py --n 64 --width 12 --out verilog
"""

import argparse
import math
import os


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--n", type=int, default=64, help="FFT size")
    ap.add_argument("--width", type=int, default=12, help="twiddle bit width")
    ap.add_argument("--out", default="verilog", help="output directory")
    args = ap.parse_args()

    n, w = args.n, args.width
    scale = (1 << (w - 1)) - 1          # Q(w-1)
    lo, hi = -(1 << (w - 1)), (1 << (w - 1)) - 1

    def clamp(v):
        return max(lo, min(hi, v))

    real, imag = [], []
    for k in range(n // 2):
        ang = -2.0 * math.pi * k / n     # forward FFT: exp(-j2pi k/N)
        real.append(clamp(int(round(math.cos(ang) * scale))))
        imag.append(clamp(int(round(math.sin(ang) * scale))))

    def fmt(v):
        return f"{v & ((1 << w) - 1):0{(w + 3) // 4}x}"

    rp = os.path.join(args.out, f"tw{w}_real.mem")
    ip = os.path.join(args.out, f"tw{w}_imag.mem")
    with open(rp, "w") as f:
        f.write("\n".join(fmt(v) for v in real) + "\n")
    with open(ip, "w") as f:
        f.write("\n".join(fmt(v) for v in imag) + "\n")

    print(f"wrote {rp} ({len(real)} entries, Q{w-1})")
    print(f"wrote {ip} ({len(imag)} entries, Q{w-1})")
    print(f"  first: cos={fmt(real[0])} sin={fmt(imag[0])}")
    print(f"  last : cos={fmt(real[-1])} sin={fmt(imag[-1])}")


if __name__ == "__main__":
    main()
