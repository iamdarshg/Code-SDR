#!/usr/bin/env python3
"""
Code-SDR V2 - SDR-side dashboard + local FFT (raw mode).

Raw mode sends ADC samples to the host; the host does the DSP. This program:

  * receives the UDP sample stream and parses the in-band telemetry header
    (packet sequence, base sample index, FPGA drop counter, bit depth, decimation)
  * computes NETWORK loss from sequence gaps and FPGA-side loss from the drop
    counter, and reports both against the current bit count and sample rate
  * runs an FFT locally (the reason raw mode exists) so you can see the spectrum
  * optionally reads the RP2040 "TEL:" lines over USB serial for the link budget
    (headroom / oversubscription) computed on the controller

Packet payload layout (v2_raw_path):
  [0:4)   packet sequence            (big endian)
  [4:8)   base sample index          (big endian, decimated samples)
  [8:12)  cumulative dropped words   (big endian, from the FIFO overflow)
  [12]    0xA5 magic
  [13]    sample bits (8 or 10)
  [14]    decimation (1, 2, 4, 8, ...)
  [15]    flags: bit0 link_up, bit1 overflow_seen
  [16:)   packed samples: 4 samples per (bits*4+7)//8 bytes

Usage:
  python sdr_dashboard.py --port 10000
  python sdr_dashboard.py --serial COM7
"""

import argparse
import os
import socket
import struct
import sys
import time
from collections import deque

import numpy as np

MAGIC = 0xA5
HDR = 16


# Sample unpacking lives in sdr_common so the dashboard and the spectrum tool
# cannot drift apart. Import it regardless of how this module was loaded (the
# test harness loads it by path, so its directory is not on sys.path for free).
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sdr_common import unpack_samples  # noqa: E402


class LinkStats:
    def __init__(self):
        self.rx = 0
        self.lost_net = 0
        self.first_seq = None
        self.last_seq = None
        self.fpga_drops = 0
        self.last_fpga_drops = 0
        self.samples = 0
        self.bits = None
        self.decim = None
        self.link_up = None
        self.t0 = time.time()
        self.last_report = self.t0

    def update(self, seq, base, drops, flags, bits, decim, nsamp):
        if self.first_seq is None:
            self.first_seq = seq
        if self.last_seq is not None:
            gap = (seq - self.last_seq) & 0xFFFFFFFF
            if gap > 1:
                self.lost_net += gap - 1
        self.last_seq = seq
        self.rx += 1
        self.samples += nsamp
        self.fpga_drops = drops
        self.bits = bits
        self.decim = decim
        self.link_up = flags & 1

    def report(self, fft_db=None, sample_rate=None):
        now = time.time()
        dt = now - self.t0
        if dt <= 0 or self.rx == 0:
            return
        interval = now - self.last_report
        self.last_report = now

        expected = self.rx + self.lost_net
        net_loss = 100.0 * self.lost_net / max(expected, 1)
        srate = self.samples / dt
        mbps = srate * (self.bits or 0) / 1e6
        ddrop = self.fpga_drops - self.last_fpga_drops
        self.last_fpga_drops = self.fpga_drops

        used = (pow(2, self.bits - 1) if self.bits else 1)
        print("=" * 68)
        print(" Code-SDR V2  |  raw mode  |  laptop FFT")
        print(f"  bit depth        : {self.bits} bit    decimation: {self.decim}")
        print(f"  sample rate      : {srate/1e6:.3f} MSPS (nominal {100.0/ (self.decim or 1):.1f})")
        print(f"  payload rate     : {mbps:.1f} Mbps   (ceiling ~993 Mbps @ jumbo 9000 MTU)")
        print(f"  packets received : {self.rx}   expected {expected}")
        print(f"  NETWORK loss     : {self.lost_net} packets ({net_loss:.4f} %)")
        print(f"  FPGA-side drops  : {self.fpga_drops} words total, +{ddrop} this interval")
        print(f"  link up          : {self.link_up}")
        if sample_rate and fft_db is not None:
            peak = int(np.argmax(fft_db))
            fpk = peak * sample_rate / (2 * len(fft_db))
            print(f"  FFT peak         : bin {peak}  ~ {fpk/1e6:.3f} MHz  ({fft_db[peak]:.1f} dB)")
        print("=" * 68)


def serial_reader(port, baud=115200):
    try:
        import serial  # pyserial
    except ImportError:
        print("pyserial not installed; skipping RP2040 serial dashboard")
        return
    ser = serial.Serial(port, baud, timeout=0.1)
    print(f"# reading RP2040 telemetry on {port}")
    while True:
        line = ser.readline().decode(errors="replace").strip()
        if line.startswith("TEL:"):
            parts = line[4:].split(",")
            if len(parts) >= 9:
                (_, bits, decim, mode, msps, payload, ceiling,
                 headroom, required) = parts[:9]
                flag = "OVERSUBSCRIBED" if required == "1" else "ok"
                print(f"[rp2040] {bits}bit /{decim} {msps}MSPS "
                      f"load={payload}Mbps headroom={headroom}Mbps {flag}")
        elif line:
            print(f"[rp2040] {line}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, default=10000)
    ap.add_argument("--bind", default="0.0.0.0")
    ap.add_argument("--serial", default=None, help="RP2040 serial port, e.g. COM7")
    ap.add_argument("--fft", type=int, default=4096, help="local FFT size")
    ap.add_argument("--report", type=float, default=1.0, help="dashboard period (s)")
    args = ap.parse_args()

    if args.serial:
        import threading
        threading.Thread(target=serial_reader, args=(args.serial,), daemon=True).start()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 8 * 1024 * 1024)
    sock.bind((args.bind, args.port))
    sock.settimeout(0.5)
    print(f"# listening on {args.bind}:{args.port}")
    print("# drop the FPGA into raw mode; the laptop does the FFT here.")

    stats = LinkStats()
    window = np.zeros(args.fft, dtype=np.float32)
    filled = 0
    window_fn = np.hanning(args.fft)
    last_report = 0.0

    stats.link_up = None
    while True:
        try:
            data, _ = sock.recvfrom(65536)
        except socket.timeout:
            stats.report()
            continue
        if len(data) < HDR or data[12] != MAGIC:
            continue
        seq, base, drops = struct.unpack(">III", data[:12])
        bits = data[13]
        decim = data[14]
        flags = data[15]
        try:
            samples = unpack_samples(data[HDR:], bits)
        except ValueError:
            continue
        stats.update(seq, base, drops, flags, bits, decim, len(samples))

        # local FFT (rolling window)
        take = min(len(samples), args.fft - filled)
        window[filled:filled + take] = samples[:take]
        filled += take
        if filled == args.fft:
            spec = np.fft.rfft(window * window_fn)
            fft_db = 20 * np.log10(np.abs(spec) + 1e-9)
            filled = 0
        else:
            fft_db = None

        now = time.time()
        if now - last_report >= args.report:
            stats.report(fft_db, 100e6 / max(decim, 1))
            last_report = now


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(0)
