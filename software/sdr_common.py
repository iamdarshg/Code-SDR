"""Shared host-side helpers for the Code-SDR V2 raw UDP payload.

The FPGA packs a fixed 40-bit (5-byte) word so both sample widths pack exactly
with no padding (verilog/v2_raw_path.v):

    10-bit -> 4 samples per 5-byte word
     8-bit -> 5 samples per 5-byte word (top 8 bits of each 10-bit sample)

Because every sample is byte-aligned in the 8-bit format, unpacking it needs no
regard for the word boundary. The 10-bit format packs across byte boundaries and
does.

This lived in two copies (sdr_dashboard.unpack_samples and sdr_spectrum.unpack_raw)
before being pulled out here.
"""

import numpy as np


def unpack_samples(buf: bytes, bits: int) -> np.ndarray:
    """Unpack a packed sample payload into an array of signed ints.

    The ADC coding is offset binary: 10-bit is biased by 512, 8-bit by 128.
    """
    if bits == 8:
        return np.frombuffer(buf, dtype=np.uint8).astype(np.int16) - 128

    if bits == 10:
        n_words = len(buf) // 5
        if n_words == 0:
            return np.zeros(0, dtype=np.int16)
        raw = np.frombuffer(buf[: n_words * 5], dtype=np.uint8).reshape(-1, 5)
        b0 = raw[:, 0].astype(np.uint16)
        b1 = raw[:, 1].astype(np.uint16)
        b2 = raw[:, 2].astype(np.uint16)
        b3 = raw[:, 3].astype(np.uint16)
        b4 = raw[:, 4].astype(np.uint16)
        s = np.empty((n_words, 4), dtype=np.int16)
        s[:, 0] = ((b0 << 2) | (b1 >> 6)) & 0x3FF
        s[:, 1] = ((b1 << 4) | (b2 >> 4)) & 0x3FF
        s[:, 2] = ((b2 << 6) | (b3 >> 2)) & 0x3FF
        s[:, 3] = ((b3 << 8) | b4) & 0x3FF
        return (s.reshape(-1).astype(np.int32) - 512).astype(np.int16)

    raise ValueError(f"unsupported bit depth {bits}")
