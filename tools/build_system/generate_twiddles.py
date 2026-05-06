import math

def generate_twiddles(fft_size=1024, data_width=16):
    """Generate fixed-point twiddle factors and window coefficients."""
    twiddle_real = []
    twiddle_imag = []

    scale = (2**(data_width-1)) - 1

    for k in range(fft_size // 2):
        angle = -2.0 * math.pi * k / fft_size
        cos_val = math.cos(angle)
        sin_val = math.sin(angle)
        r = int(round(cos_val * scale))
        i = int(round(sin_val * scale))
        r = max(-scale-1, min(scale, r))
        i = max(-scale-1, min(scale, i))
        twiddle_real.append(f"{r & ((1 << data_width) - 1):04x}")
        twiddle_imag.append(f"{i & ((1 << data_width) - 1):04x}")

    with open('verilog/twiddle_real.mem', 'w') as f:
        f.write('\n'.join(twiddle_real))
    with open('verilog/twiddle_imag.mem', 'w') as f:
        f.write('\n'.join(twiddle_imag))

    # Window Coefficients (Hamming)
    window_coeffs = []
    for i in range(fft_size):
        # Hamming window: w(n) = 0.54 - 0.46*cos(2*pi*n/(N-1))
        val = 0.54 - 0.46 * math.cos(2.0 * math.pi * i / (fft_size - 1))
        w = int(round(val * scale))
        window_coeffs.append(f"{w & ((1 << data_width) - 1):04x}")

    with open('verilog/window_coeff.mem', 'w') as f:
        f.write('\n'.join(window_coeffs))

if __name__ == "__main__":
    import sys
    size = 1024
    if len(sys.argv) > 1:
        size = int(sys.argv[1])
    generate_twiddles(size)
