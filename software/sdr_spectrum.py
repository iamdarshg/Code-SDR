#!/usr/bin/env python3
"""
Code-SDR V2 - live spectrum / waterfall viewer.

Works with either FPGA mode:

  raw mode (UDP 4660)  the FPGA streams ADC samples; this program runs the FFT.
  fft mode (UDP 4661)  the FPGA streams 1024-bin spectra; this program displays.

It also reports link health: sample rate, packet rate, NETWORK loss (from
sequence gaps) and FPGA-side drops (from the in-band counter), plus the
block-floating-point exponent in FFT mode.

Usage:
  python sdr_spectrum.py --mode raw --port 4660
  python sdr_spectrum.py --mode fft --port 4661
  python sdr_spectrum.py --simulate raw --save spectrum.png --frames 20
"""

import argparse
import os
import socket
import struct
import sys
import threading
import time

import numpy as np

MAGIC_RAW = 0xA5
RAW_HDR = 16
FFT_HDR = 16
FFT_BINS = 256          # bins per FFT packet (v2_fft_packetizer)


# --------------------------------------------------------------------- unpack
# Shared with sdr_dashboard via sdr_common (imported regardless of how this
# module was loaded, so its directory must be on sys.path explicitly).
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sdr_common import unpack_samples as unpack_raw  # noqa: E402


def unpack_fft_packet(buf):
    """v2_fft_packetizer payload: 16-byte header + 4-byte bins."""
    seq, frame = struct.unpack(">II", buf[:8])
    first_bin = struct.unpack(">H", buf[8:10])[0]
    scale_exp = buf[10]
    flags = buf[11]
    payload = buf[FFT_HDR:]
    nb = len(payload) // 4
    arr = np.frombuffer(payload[:nb * 4], dtype=">i2").reshape(-1, 2)
    return seq, frame, first_bin, scale_exp, flags, arr


# ---------------------------------------------------------------------- stats
class Stats:
    def __init__(self):
        self.pkts = 0
        self.lost = 0
        self.fpga_drops = 0
        self.samples = 0
        self.last_seq = None
        self.bits = 0
        self.decim = 0
        self.scale_exp = 0
        self.link = None
        self.overflow = 0
        self.t0 = time.time()

    def note_raw(self, seq, drops, flags, bits, decim, nsamp):
        self._seq(seq)
        self.pkts += 1
        self.samples += nsamp
        self.fpga_drops = drops
        self.bits = bits
        self.decim = decim
        self.link = flags & 1
        self.overflow = (flags >> 1) & 1

    def note_fft(self, seq, scale_exp, flags, nbins):
        self._seq(seq)
        self.pkts += 1
        self.samples += nbins
        self.scale_exp = scale_exp
        self.overflow = flags & 1

    def _seq(self, seq):
        if self.last_seq is not None:
            gap = (seq - self.last_seq) & 0xFFFFFFFF
            if gap > 1:
                self.lost += gap - 1
        self.last_seq = seq

    def text(self, mode):
        dt = max(time.time() - self.t0, 1e-9)
        exp = (self.pkts + self.lost)
        line = (f"pkts {self.pkts}  net loss {self.lost} "
                f"({100.0*self.lost/max(exp,1):.3f}%)  fpga drops {self.fpga_drops}")
        if mode == "raw":
            line += f"  {self.bits}bit /{self.decim or 1}  {self.samples/dt/1e6:.2f} MSPS"
        else:
            line += f"  scale_exp {self.scale_exp}  bins {self.samples/dt:,.0f}/s"
        if self.overflow:
            line += "  ** OVERFLOW **"
        return line


# ---------------------------------------------------------------------- source
class UdpSource(threading.Thread):
    def __init__(self, port, bind="0.0.0.0"):
        super().__init__(daemon=True)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 16 * 1024 * 1024)
        self.sock.bind((bind, port))
        self.sock.settimeout(0.5)
        self.stats = Stats()
        self.mode = None            # set by run()
        self.samples = np.zeros(0, dtype=np.float32)
        self.bins = None
        self.lock = threading.Lock()
        self.stop = False
        self.port = port

    def run(self):
        while not self.stop:
            try:
                data, _ = self.sock.recvfrom(65536)
            except socket.timeout:
                continue
            if self.mode == "raw":
                if len(data) < RAW_HDR or data[12] != MAGIC_RAW:
                    continue
                seq, base, drops = struct.unpack(">III", data[:12])
                bits, decim, flags = data[13], data[14], data[15]
                try:
                    s = unpack_raw(data[RAW_HDR:], bits)
                except ValueError:
                    continue
                self.stats.note_raw(seq, drops, flags, bits, decim, len(s))
                with self.lock:
                    self.samples = np.concatenate([self.samples, s.astype(np.float32)])[-65536:]
            elif self.mode == "fft":
                if len(data) < FFT_HDR:
                    continue
                seq, frame, first_bin, exp, flags, arr = unpack_fft_packet(data)
                self.stats.note_fft(seq, exp, flags, arr.shape[0])
                with self.lock:
                    if self.bins is None:
                        self.bins = np.zeros(1024, dtype=np.complex64)
                    lo = min(first_bin, 1023)
                    hi = min(lo + arr.shape[0], 1024)
                    self.bins[lo:hi] = (arr[:hi - lo, 0] + 1j * arr[:hi - lo, 1]) * (2.0 ** exp)


def simulate_source(mode, n_frames, fft_size=4096):
    """Deterministic stand-in packet generator for off-hardware testing."""
    rng = np.random.default_rng(3)
    t = 0
    for f in range(n_frames):
        if mode == "raw":
            n = fft_size
            k = np.arange(n)
            x = (200 * np.sin(2 * np.pi * 64 * (k + t) / n)
                 + rng.normal(0, 6, n)).astype(np.int16)
            yield ("raw", f, x)
        else:
            k = np.arange(1024)
            spec = np.zeros(1024, dtype=np.complex64)
            spec[64] = 16000
            spec[960] = 16000
            spec += rng.normal(0, 3, 1024) + 1j * rng.normal(0, 3, 1024)
            yield ("fft", f, spec.astype(np.complex64))
        t += fft_size


# ----------------------------------------------------------------------- main
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--mode", choices=["raw", "fft"], default="raw")
    ap.add_argument("--port", type=int, default=None)
    ap.add_argument("--bind", default="0.0.0.0")
    ap.add_argument("--simulate", action="store_true")
    ap.add_argument("--fft", type=int, default=4096, help="host FFT size (raw mode)")
    ap.add_argument("--fs", type=float, default=None,
                    help="sample rate (Hz) for the frequency axis; default 50e6 raw "
                         "(=100 MSPS/decim 2) or 6.25e6 for the FFT path")
    ap.add_argument("--save", default=None, help="render and exit instead of showing")
    ap.add_argument("--frames", type=int, default=60, help="frames before exit in --save mode")
    args = ap.parse_args()

    # The FPGA transmits TO cfg_dst_port, whose power-on value is 10000
    # (v2_spi_regs A_DST_PORT reset) and which nothing else changes - src_port
    # 4660/4661 is only what the packets are stamped with. A receiver must bind
    # the destination port, so the default here has to be 10000.
    port = args.port if args.port else 10000
    fs = args.fs if args.fs else (50.0e6 if args.mode == "raw" else 6.25e6)

    import matplotlib
    if args.save:
        matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    freq = np.fft.rfftfreq(args.fft, d=1.0 / fs)
    win = np.hanning(args.fft)
    waterfall = np.full((120, len(freq) if args.mode == "raw" else 512), -120.0)

    src = None
    if not args.simulate:
        src = UdpSource(port, args.bind)
        src.mode = args.mode
        src.start()

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(11, 7),
                                   gridspec_kw={"height_ratios": [2, 1.4]})
    line, = ax1.plot([], [], lw=0.8)
    ax1.set_title("Code-SDR V2 spectrum")
    ax1.set_xlabel("frequency (MHz)")
    ax1.set_ylabel("dBFS")
    ax1.set_xlim(freq[0], freq[-1])
    ax1.set_ylim(-100, 5)
    ax1.grid(alpha=0.3)
    im = ax2.imshow(waterfall, aspect="auto", origin="lower", cmap="viridis",
                    extent=[freq[0], freq[-1], 0, waterfall.shape[0]])
    ax2.set_xlabel("frequency (MHz)")
    ax2.set_ylabel("time")
    txt = ax1.text(0.01, 0.97, "", transform=ax1.transAxes, va="top", fontsize=9)

    def get_sample():
        if args.simulate:
            return None
        with src.lock:
            if args.mode == "raw":
                return src.samples.copy()
            return None if src.bins is None else src.bins.copy()

    sim = simulate_source(args.mode, args.frames * 4, args.fft) if args.simulate else None

    for n in range(args.frames):
        if args.mode == "raw":
            if args.simulate:
                _, _, x = next(sim)
                data = x.astype(np.float32)
            else:
                data = get_sample()
                if data is None or len(data) < args.fft:
                    plt.pause(0.05)
                    continue
            spec = np.fft.rfft(data[-args.fft:] * win)
            fft_db = 20 * np.log10(np.abs(spec) / (args.fft / 2) + 1e-12)
            stat = src.stats.text("raw") if src else f"sim frame {n}"
        else:
            if args.simulate:
                _, _, b = next(sim)
                bins = b
            else:
                bins = get_sample()
                if bins is None:
                    plt.pause(0.05)
                    continue
            half = bins[:512]
            fft_db = 20 * np.log10(np.abs(half) / 512 + 1e-12)
            stat = src.stats.text("fft") if src else f"sim frame {n}"

        line.set_data(freq[:len(fft_db)], fft_db)
        waterfall = np.roll(waterfall, 1, axis=0)
        w = fft_db[:waterfall.shape[1]]
        waterfall[0, :len(w)] = w
        im.set_data(waterfall)
        txt.set_text(stat)

        if args.save:
            if n % max(1, args.frames // 3) == 0:
                fig.canvas.draw()
        else:
            plt.pause(0.02)

    if args.save:
        fig.savefig(args.save, dpi=110, bbox_inches="tight")
        print(f"wrote {args.save}")
        peak = int(np.argmax(fft_db))
        print(f"peak bin {peak}  {fft_db[peak]:.1f} dBFS  ({stat})")
    else:
        plt.show()

    if src:
        src.stop = True
    return 0


if __name__ == "__main__":
    sys.exit(main())
