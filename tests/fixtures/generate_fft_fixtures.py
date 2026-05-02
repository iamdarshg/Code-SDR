import numpy as np
import json
import os

def generate_fft_fixtures(n=1024):
    """Generate FFT test vectors for various cases."""
    fixtures = {}

    # 1. DC Signal
    dc_sig = np.ones(n, dtype=complex) * 512
    fixtures['dc'] = {
        'input_real': dc_sig.real.astype(int).tolist(),
        'input_imag': dc_sig.imag.astype(int).tolist(),
        'expected_peak_bin': 0
    }

    # 2. Single Tone (Bin 10)
    fs = 105e6
    f = 10 * fs / n
    t = np.arange(n) / fs
    tone_sig = np.exp(2j * np.pi * f * t) * 1000
    fixtures['tone_bin_10'] = {
        'input_real': tone_sig.real.astype(int).tolist(),
        'input_imag': tone_sig.imag.astype(int).tolist(),
        'expected_peak_bin': 10
    }

    # 3. Tone Bin 100
    f100 = 100 * fs / n
    tone100 = np.exp(2j * np.pi * f100 * t) * 1000
    fixtures['tone_bin_100'] = {
        'input_real': tone100.real.astype(int).tolist(),
        'input_imag': tone100.imag.astype(int).tolist(),
        'expected_peak_bin': 100
    }

    os.makedirs('tests/fixtures', exist_ok=True)
    with open('tests/fixtures/fft_test_vectors.json', 'w') as f:
        json.dump(fixtures, f, indent=4)

if __name__ == "__main__":
    generate_fft_fixtures(1024)
