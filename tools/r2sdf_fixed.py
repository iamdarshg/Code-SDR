#!/usr/bin/env python3
"""
Bit-accurate model of verilog/v2_fft_pipe.v (complex, both paths).

Mirrors the RTL exactly: fixed-point widths, the odd-stage >>1 scaling with its
+1 rounding, the per-stage Q(TW-1) truncation, and the Karatsuba complex
multiply with its RND offset and final right shift.

Run against tools/r2sdf_reference.py (exact arithmetic) on the same input: where
they diverge is a real design bug.
"""

import os
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def load_mem(path):
    vals = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            v = int(line, 16)
            if len(line) == 3 and (v & 0x800):
                v -= 0x1000
            vals.append(v)
    return vals


def wrap(v, bits):
    m = 1 << bits
    v &= (m - 1)
    if v >= (m >> 1):
        v -= m
    return v


def r2sdf_fixed(x, n, dw=14, tw=12):
    logn = n.bit_length() - 1
    twr = load_mem(os.path.join(REPO, "verilog", "tw12_real.mem"))
    twi = load_mem(os.path.join(REPO, "verilog", "tw12_imag.mem"))
    rnd = 1 << (tw - 2)

    st = [{"M": n >> i, "H": (n >> i) >> 1,
           "bre": [0] * ((n >> i) >> 1), "bim": [0] * ((n >> i) >> 1), "cnt": 0}
          for i in range(logn)]

    out = []
    for s in x:
        v_re, v_im = s, 0.0
        for i, S in enumerate(st):
            M, H, cnt = S["M"], S["H"], S["cnt"]
            if not isinstance(v_re, int):
                v_re = int(round(v_re))
                v_im = int(round(v_im))
            q = cnt % H
            a_re, a_im = S["bre"][q], S["bim"][q]

            if (i % 2) == 1:
                d_re, d_im = wrap((v_re + 1) >> 1, dw), wrap((v_im + 1) >> 1, dw)
            else:
                d_re, d_im = wrap(v_re, dw), wrap(v_im, dw)

            if cnt < H:
                o_re, o_im = a_re, a_im
                S["bre"][q], S["bim"][q] = d_re, d_im
            else:
                j = cnt - H
                idx = (j << i) % (n // 2)
                wr, wi = twr[idx], twi[idx]

                o_re = wrap(a_re + d_re, dw)          # sum_re[DW-1:0]
                o_im = wrap(a_im + d_im, dw)

                dif_re = a_re - d_re
                dif_im = a_im - d_im
                p1 = dif_re * wr
                p2 = dif_im * wi
                p3 = (dif_re + dif_im) * (wr + wi)    # widened operand sums
                mr = (p1 - p2 + rnd) >> (tw - 1)
                mi = (p3 - p1 - p2 + rnd) >> (tw - 1)
                S["bre"][q] = wrap(mr, dw)
                S["bim"][q] = wrap(mi, dw)

            S["cnt"] = 0 if cnt == M - 1 else cnt + 1
            v_re, v_im = o_re, o_im
        out.append((v_re, v_im))
    return out


def main():
    sys.path.insert(0, os.path.join(REPO, "tools"))
    from r2sdf_reference import r2sdf as exact

    n = 64
    x = [256.0] + [0.0] * (4 * n - 1)
    got = r2sdf_fixed(x, n)
    ref = exact(x, n)

    print("fixed stream len", len(got))
    print("fixed max abs =", max(abs(a) for a, _ in got))
    print("exact max abs =", max(abs(v.real) for v in ref))

    first = None
    for k in range(len(ref)):
        if abs(got[k][0] - ref[k].real) > 2 or abs(got[k][1] - ref[k].imag) > 2:
            first = k
            break
    print("first divergence (tol 2) at", first)
    if first is not None:
        lo, hi = max(0, first - 3), first + 6
        print("  fixed re:", [got[k][0] for k in range(lo, hi)])
        print("  exact re:", [round(ref[k].real, 1) for k in range(lo, hi)])
    print("fixed[60:70] =", [got[k][0] for k in range(60, 70)])
    print("exact[60:70] =", [round(ref[k].real) for k in range(60, 70)])


if __name__ == "__main__":
    main()
