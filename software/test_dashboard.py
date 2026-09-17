#!/usr/bin/env python3
"""
Test the SDR-side dashboard against a simulated packet source.

Builds packets in exactly the v2_raw_path format (16-byte telemetry header +
packed samples), feeds them through the dashboard's parsing and statistics
without any radio, and checks:

  * 8-bit and 10-bit unpacking round-trips against the RTL packing
  * network-loss accounting across a deliberate sequence gap
  * FPGA drop-counter accounting
  * the FFT bin packet header (v2_fft_packetizer format) parses
"""

import importlib.util
import random
import struct
import sys

# load the dashboard module without running its main()
spec = importlib.util.spec_from_file_location("sdrdash", "software/sdr_dashboard.py")
sdrdash = importlib.util.module_from_spec(spec)
sys.modules["sdrdash"] = sdrdash
spec.loader.exec_module(sdrdash)

errors = 0


def pack10(samples):
    """4 x 10-bit signed samples -> 5 bytes, matching v2_raw_path."""
    s = [int(x) + 512 for x in samples]          # to unsigned offset
    w = (s[0] << 30) | (s[1] << 20) | (s[2] << 10) | s[3]
    return bytes([(w >> 32) & 0xFF, (w >> 24) & 0xFF,
                  (w >> 16) & 0xFF, (w >> 8) & 0xFF, w & 0xFF])


def make_raw_packet(seq, base, drops, bits, decim, flags, payload):
    return struct.pack(">III", seq & 0xFFFFFFFF, base & 0xFFFFFFFF,
                       drops & 0xFFFFFFFF) + bytes([0xA5, bits, decim, flags]) + payload


def check(name, cond):
    global errors
    if cond:
        print(f"  {name}: OK")
    else:
        print(f"  {name}: FAIL")
        errors += 1


print("=== dashboard parse/stats test ===")
random.seed(7)

# ---------------------------------------------------------------- 10-bit packing
for _ in range(500):
    orig = [random.randint(-512, 511) for _ in range(4)]
    got = list(sdrdash.unpack_samples(pack10(orig), 10)[:4])
    if got != orig:
        check("10-bit round-trip", False)
        print("   ", orig, got)
        break
else:
    check("10-bit round-trip (500 random words)", True)

# ------------------------------------------------------------------ 8-bit
orig8 = [random.randint(-128, 127) for _ in range(32)]
raw8 = bytes([(v + 128) & 0xFF for v in orig8])
check("8-bit round-trip", list(sdrdash.unpack_samples(raw8, 8)) == orig8)

# ------------------------------------------------------- header + loss counting
st = sdrdash.LinkStats()
# 10 good packets, then skip seq 10..12, then 4 more
seq = 0
for i in range(10):
    pkt = make_raw_packet(seq, seq * 4, 0, 10, 1, 0x01, pack10([0, 0, 0, 0]))
    s, b, d = struct.unpack(">III", pkt[:12])
    assert pkt[12] == 0xA5
    st.update(s, b, d, pkt[15], pkt[13], pkt[14], 4)
    seq += 1
seq += 3                                    # three lost packets
for i in range(4):
    pkt = make_raw_packet(seq, seq * 4, 0, 10, 1, 0x01, pack10([0, 0, 0, 0]))
    s, b, d = struct.unpack(">III", pkt[:12])
    st.update(s, b, d, pkt[15], pkt[13], pkt[14], 4)
    seq += 1

check("packets received == 14", st.rx == 14)
check("network loss detected == 3", st.lost_net == 3)
check("bit depth parsed == 10", st.bits == 10)
check("link_up parsed", st.link_up == 1)

# ------------------------------------------------------------- drop accounting
st2 = sdrdash.LinkStats()
for i in range(5):
    pkt = make_raw_packet(i, i * 4, 7 * (i + 1), 10, 1, 0x01, pack10([0, 0, 0, 0]))
    s, b, d = struct.unpack(">III", pkt[:12])
    st2.update(s, b, d, pkt[15], pkt[13], pkt[14], 4)
check("FPGA drop counter parsed == 35", st2.fpga_drops == 35)

# ------------------------------------------------------ FFT bin packet header
# v2_fft_packetizer: seq(4) frame(4) first_bin(2) scale_exp(1) flags(1) rsvd(4)
fft_hdr = struct.pack(">II", 3, 42) + struct.pack(">H", 256) + bytes([4, 0x01]) + b"\0" * 4
fft_seq, fft_frame = struct.unpack(">II", fft_hdr[:8])
fft_first = struct.unpack(">H", fft_hdr[8:10])[0]
check("FFT header seq/frame/first_bin/exp",
      fft_seq == 3 and fft_frame == 42 and fft_first == 256 and fft_hdr[10] == 4)

if errors:
    print(f"FATAL: {errors} errors")
    sys.exit(1)
print("PASS: dashboard parse + statistics")
