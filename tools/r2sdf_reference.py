#!/usr/bin/env python3
"""
R2SDF reference model - the verified oracle for verilog/v2_fft_pipe.v.

This is the executable specification the pipelined FFT must match. It reproduces
the exact stage schedule used in the Verilog and it DOES match numpy's FFT, so
when the hardware disagrees, the bug is in the Verilog, not the architecture.

Verified facts (do not re-derive these):

  1. The schedule below is correct. Per stage i with block M = N >> i and delay
     H = M/2:
         first half  : out = buf[q]; buf[q] = in
         second half : out = buf[q] + in; buf[q] = (buf[q] - in) * W
     with q = cnt mod H, W = exp(-2j*pi*(j<<i)/N), j = cnt - H.

  2. Output framing: the frame is the stream over [N-1, 2N-1), and stream
     position m within that frame maps to bin bitrev(m). (Found by search over
     offsets -1..2 and five candidate index mappings.)

  3. The datapath must FREE-RUN. Gating the counter/delay-buffer update on the
     input valid freezes the pipeline when a burst ends and truncates the frame.
     A continuous stream hides this; a finite burst exposes it.

  4. The output valid must be delayed by the data latency (N-1 through the delay
     lines). Propagating it combinationally puts it N-1 cycles early, so every
     frame is captured as fill/garbage.

Run: python tools/r2sdf_reference.py
"""

import numpy as np


def bitrev(v, logn):
    return int(format(v, "0%db" % logn)[::-1], 2)


def r2sdf(x, n):
    """Reference R2SDF. Feed >= 2N samples; take stream[n-1 : 2n-1]."""
    logn = int(np.log2(n))
    st = [{"M": n >> i, "H": (n >> i) >> 1,
           "buf": [0j] * ((n >> i) >> 1), "cnt": 0} for i in range(logn)]
    out = []
    for s in x:
        v = s
        for i, S in enumerate(st):
            M, H, cnt = S["M"], S["H"], S["cnt"]
            q = cnt % H
            a = S["buf"][q]
            if cnt < H:
                o = a
                S["buf"][q] = v
            else:
                j = cnt - H
                o = a + v
                S["buf"][q] = (a - v) * np.exp(-2j * np.pi * (j << i) / n)
            S["cnt"] = 0 if cnt == M - 1 else cnt + 1
            v = o
        out.append(v)
    return out


def frame_to_bins(stream, n):
    logn = int(np.log2(n))
    fr = stream[n - 1: 2 * n - 1]
    bins = np.zeros(n, dtype=complex)
    for m, val in enumerate(fr):
        bins[bitrev(m, logn)] = val
    return bins


def main():
    n = 64
    cases = {
        "impulse": lambda k: 1.0 if k % n == 0 else 0.0,
        "dc":      lambda k: 1.0,
        "tone1":   lambda k: np.exp(2j * np.pi * 1 * k / n),
        "tone7":   lambda k: np.exp(2j * np.pi * 7 * k / n),
    }
    ok = True
    for name, f in cases.items():
        stream = r2sdf([f(k) for k in range(3 * n)], n)
        got = frame_to_bins(stream, n)
        ref = np.fft.fft([f(k) for k in range(n)])
        err = np.max(np.abs(got - ref))
        status = "OK" if err < 1e-6 else "MISMATCH"
        if err >= 1e-6:
            ok = False
        print(f"  {name:8s} max|err| = {err:.3e}  {status}")
    print("PASS: reference model matches numpy" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
