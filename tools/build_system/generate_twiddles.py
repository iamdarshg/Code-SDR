import math

def generate_twiddles(fft_size=1024, data_width=16):
    """Generate fixed-point twiddle factors for radix-2 FFT."""
    twiddle_real = []
    twiddle_imag = []

    scale = (2**(data_width-1)) - 1

    for k in range(fft_size // 2):
        # Angle for radix-2 DIF: e^(-j * 2pi * k / N)
        angle = -2.0 * math.pi * k / fft_size
        cos_val = math.cos(angle)
        sin_val = math.sin(angle)

        # Convert to fixed-point
        r = int(round(cos_val * scale))
        i = int(round(sin_val * scale))

        # Ensure they fit in signed width
        r = max(-scale-1, min(scale, r))
        i = max(-scale-1, min(scale, i))

        # Convert to hex strings for .mem file
        r_val = r & ((1 << data_width) - 1)
        i_val = i & ((1 << data_width) - 1)

        twiddle_real.append(f"{r_val:04x}")
        twiddle_imag.append(f"{i_val:04x}")

    with open('verilog/twiddle_real.mem', 'w') as f:
        f.write('\n'.join(twiddle_real))

    with open('verilog/twiddle_imag.mem', 'w') as f:
        f.write('\n'.join(twiddle_imag))

if __name__ == "__main__":
    import sys
    size = 1024
    if len(sys.argv) > 1:
        size = int(sys.argv[1])
    generate_twiddles(size)
