import pytest
import numpy as np
import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'wideband-sdr-software'))
from digital_downconverter import DigitalDownconverter

def test_empty_input_signal():
    ddc = DigitalDownconverter(1e6, 2e5, 1e5)
    empty_signal = np.array([], dtype=np.complex64)
    result = ddc.apply_ddc(empty_signal)
    assert result.size == 0

def test_invalid_input_type():
    ddc = DigitalDownconverter(1e6, 2e5, 1e5)
    with pytest.raises(TypeError):
        ddc.apply_ddc("not a numpy array")

def test_zero_bandwidth():
    with pytest.raises(ValueError):
        DigitalDownconverter(1e6, 2e5, 0)

def test_large_decimation_factor():
    # This might not raise an error, but it's good to check behavior
    ddc = DigitalDownconverter(1e6, 2e5, 100) # Very narrow bandwidth -> large decimation
    signal = np.random.randn(10000) + 1j * np.random.randn(10000)
    result = ddc.apply_ddc(signal)
    assert result.size > 0

def test_mismatched_types():
    ddc = DigitalDownconverter(1e6, 2e5, 1e5)
    signal = np.random.randn(1024).astype(np.float32) # Real signal
    with pytest.raises(TypeError):
        ddc.apply_ddc(signal)
