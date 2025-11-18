#!/usr/bin/env python3
"""
Python ctypes bridge for Wideband SDR C++ integration
Provides C-compatible interface for both GNU Radio OOT and ExtIO plugin
"""

import ctypes
import numpy as np
import sys
import os
import threading
import time
from typing import Optional, Tuple

# Add the software directory to path for importing
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'wideband-sdr-software'))

try:
    from wideband_sdr import WidebandSDR, SDRError, SDRStreamError
    WIDEBAND_SDR_AVAILABLE = True
except ImportError as e:
    print(f"Warning: WidebandSDR module not available: {e}")
    WIDEBAND_SDR_AVAILABLE = False

class SDRDeviceWrapper:
    """C-compatible wrapper for WidebandSDR device"""
    
    def __init__(self, device_index: int = 0):
        self.device_index = device_index
        self.sdr_device = None
        self.is_open = False
        self.streaming = False
        self.lock = threading.Lock()
        self.sample_buffer = []
        self.buffer_lock = threading.Lock()
        
    def open_device(self) -> bool:
        """Open WidebandSDR device"""
        with self.lock:
            if not WIDEBAND_SDR_AVAILABLE:
                print("WidebandSDR module not available, using dummy mode")
                self.is_open = True  # Dummy mode for testing
                return True
                
            try:
                self.sdr_device = WidebandSDR(device_index=self.device_index)
                success = self.sdr_device.open()
                self.is_open = success
                return success
            except Exception as e:
                print(f"Failed to open device: {e}")
                return False
    
    def close_device(self):
        """Close WidebandSDR device"""
        with self.lock:
            if self.sdr_device and self.is_open:
                self.sdr_device.close()
                self.is_open = False
    
    def set_frequency(self, frequency: int) -> bool:
        """Set device frequency"""
        with self.lock:
            if not self.is_open or not self.sdr_device:
                return False
            try:
                return self.sdr_device.set_frequency(frequency)
            except Exception as e:
                print(f"Error setting frequency: {e}")
                return False
    
    def set_sample_rate(self, sample_rate: int) -> bool:
        """Set device sample rate"""
        with self.lock:
            if not self.is_open or not self.sdr_device:
                return False
            try:
                return self.sdr_device.set_sample_rate(sample_rate)
            except Exception as e:
                print(f"Error setting sample rate: {e}")
                return False
    
    def set_gain(self, gain: int) -> bool:
        """Set device gain"""
        with self.lock:
            if not self.is_open or not self.sdr_device:
                return False
            try:
                return self.sdr_device.set_gain(gain)
            except Exception as e:
                print(f"Error setting gain: {e}")
                return False
    
    def get_frequency(self) -> int:
        """Get current frequency"""
        with self.lock:
            if not self.is_open or not self.sdr_device:
                return 0
            try:
                return self.sdr_device.get_frequency()
            except Exception as e:
                print(f"Error getting frequency: {e}")
                return 0
    
    def get_sample_rate(self) -> int:
        """Get current sample rate"""
        with self.lock:
            if not self.is_open or not self.sdr_device:
                return 0
            try:
                return self.sdr_device.get_sample_rate()
            except Exception as e:
                print(f"Error getting sample rate: {e}")
                return 0
    
    def get_gain(self) -> int:
        """Get current gain"""
        with self.lock:
            if not self.is_open or not self.sdr_device:
                return 0
            try:
                return self.sdr_device.get_gain()
            except Exception as e:
                print(f"Error getting gain: {e}")
                return 0
    
    def start_streaming(self) -> bool:
        """Start sample streaming"""
        with self.lock:
            if not self.is_open or not self.sdr_device:
                return False
                
            try:
                self.streaming = True
                self.sample_buffer.clear()
                
                # Start device streaming with callback
                def sample_callback(samples):
                    # Convert to complex float and store
                    complex_samples = samples.astype(np.float32) / 32768.0
                    with self.buffer_lock:
                        self.sample_buffer.extend(complex_samples)
                
                return self.sdr_device.start_stream(callback=sample_callback)
            except Exception as e:
                print(f"Error starting streaming: {e}")
                self.streaming = False
                return False
    
    def stop_streaming(self) -> bool:
        """Stop sample streaming"""
        with self.lock:
            if not self.is_open or not self.sdr_device:
                return False
                
            try:
                self.streaming = False
                self.sample_buffer.clear()
                return self.sdr_device.stop_stream()
            except Exception as e:
                print(f"Error stopping streaming: {e}")
                return False
    
    def read_samples(self, buffer_ptr: int, max_samples: int) -> int:
        """
        Read samples into provided buffer
        This function is called from C++ via ctypes
        
        Args:
            buffer_ptr: C pointer to sample buffer
            max_samples: Maximum number of samples to read
            
        Returns:
            Number of samples actually read
        """
        ctypes_array = (ctypes.c_float * (max_samples * 2)).from_address(buffer_ptr)
        
        if not self.is_open:
            # Generate test signal
            t = time.time()
            samples_read = min(max_samples, 1024)
            for i in range(samples_read):
                freq = 1000.0  # 1 kHz test signal
                ctypes_array[i * 2] = 0.1 * np.cos(2 * np.pi * freq * (t + i) / 192000.0)
                ctypes_array[i * 2 + 1] = 0.1 * np.sin(2 * np.pi * freq * (t + i) / 192000.0)
            return samples_read
        
        # Read from buffer
        with self.buffer_lock:
            samples_to_read = min(max_samples, len(self.sample_buffer))
            
            for i in range(samples_to_read):
                sample = self.sample_buffer.pop(0)
                if isinstance(sample, np.complex64):
                    ctypes_array[i * 2] = sample.real
                    ctypes_array[i * 2 + 1] = sample.imag
                else:
                    # Handle real samples
                    ctypes_array[i * 2] = sample
                    ctypes_array[i * 2 + 1] = 0.0
            
            return samples_to_read
    
    def get_device_info(self) -> dict:
        """Get device information"""
        if not self.is_open or not self.sdr_device:
            return {
                'device_type': 'Wideband SDR',
                'manufacturer': 'Wideband SDR Project',
                'model': 'Wideband SDR V1.0',
                'status': 'dummy' if not WIDEBAND_SDR_AVAILABLE else 'real'
            }
            
        try:
            return self.sdr_device.get_device_info()
        except Exception as e:
            print(f"Error getting device info: {e}")
            return {'error': str(e)}
    
    def get_statistics(self) -> dict:
        """Get device statistics"""
        if not self.is_open or not self.sdr_device:
            return {'dummy_mode': True}
            
        try:
            stats = self.sdr_device.get_statistics()
            return {
                'samples_processed': stats.samples_processed,
                'packets_dropped': stats.packets_dropped,
                'current_frequency': stats.current_frequency,
                'current_sample_rate': stats.current_sample_rate,
                'current_gain': stats.current_gain
            }
        except Exception as e:
            print(f"Error getting statistics: {e}")
            return {'error': str(e)}

# Global device wrapper instance
_device_wrapper = SDRDeviceWrapper()

# C-compatible function interfaces for ctypes
def ctypes_open_device(device_index: int) -> int:
    """Open device from C (returns 1 for success, 0 for failure)"""
    return 1 if _device_wrapper.open_device() else 0

def ctypes_close_device():
    """Close device from C"""
    _device_wrapper.close_device()

def ctypes_set_frequency(frequency: int) -> int:
    """Set frequency from C (returns 1 for success, 0 for failure)"""
    return 1 if _device_wrapper.set_frequency(frequency) else 0

def ctypes_set_sample_rate(sample_rate: int) -> int:
    """Set sample rate from C (returns 1 for success, 0 for failure)"""
    return 1 if _device_wrapper.set_sample_rate(sample_rate) else 0

def ctypes_set_gain(gain: int) -> int:
    """Set gain from C (returns 1 for success, 0 for failure)"""
    return 1 if _device_wrapper.set_gain(gain) else 0

def ctypes_get_frequency() -> int:
    """Get frequency from C"""
    return _device_wrapper.get_frequency()

def ctypes_get_sample_rate() -> int:
    """Get sample rate from C"""
    return _device_wrapper.get_sample_rate()

def ctypes_get_gain() -> int:
    """Get gain from C"""
    return _device_wrapper.get_gain()

def ctypes_start_streaming() -> int:
    """Start streaming from C (returns 1 for success, 0 for failure)"""
    return 1 if _device_wrapper.start_streaming() else 0

def ctypes_stop_streaming() -> int:
    """Stop streaming from C (returns 1 for success, 0 for failure)"""
    return 1 if _device_wrapper.stop_streaming() else 0

def ctypes_read_samples(buffer_ptr: int, max_samples: int) -> int:
    """Read samples from C (returns number of samples read)"""
    return _device_wrapper.read_samples(buffer_ptr, max_samples)

def ctypes_get_device_info_json() -> str:
    """Get device info as JSON string"""
    import json
    return json.dumps(_device_wrapper.get_device_info())

def ctypes_get_statistics_json() -> str:
    """Get statistics as JSON string"""
    import json
    return json.dumps(_device_wrapper.get_statistics())

# Module initialization
if __name__ == "__main__":
    print("Wideband SDR Python ctypes bridge loaded")
    print(f"WidebandSDR module available: {WIDEBAND_SDR_AVAILABLE}")
    
    # Test the wrapper
    print("Testing device wrapper...")
    if _device_wrapper.open_device():
        print(f"Device opened: frequency={_device_wrapper.get_frequency()}")
        _device_wrapper.close_device()
        print("Device closed successfully")
    else:
        print("Failed to open device (dummy mode expected if no hardware)")
