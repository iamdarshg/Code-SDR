"""
digital_downconverter.py - Digital Downconversion Module for Wideband SDR

Implements high-performance digital downconversion with NCO, filtering, and decimation
Optimized for real-time signal processing with NumPy/SciPy

Author: Wideband SDR Development Team
Date: 2025-11-19
"""

import numpy as np
import scipy.signal as signal
from typing import Tuple, Optional, Union
import time
from nco_synchronizer import NCOSynchronizer

class DigitalDownconverter:
    """
    High-performance Digital Downconverter (DDC) for SDR applications
    
    Implements:
    - Numerically Controlled Oscillator (NCO) for frequency translation
    - FIR low-pass filtering for bandwidth limitation
    - Configurable decimation for sample rate reduction
    - Real-time processing with optimized performance
    """
    
    def __init__(self, sample_rate: float, center_freq: float, bandwidth: float):
        """
        Initialize Digital Downconverter
        
        Args:
            sample_rate: Input sample rate in Hz
            center_freq: Center frequency to downconvert from in Hz
            bandwidth: Output bandwidth in Hz
        """
        self.sample_rate = sample_rate
        self.center_freq = center_freq
        self.bandwidth = bandwidth
        
        # Calculate decimation factor
        self.decimation_factor = int(np.ceil(sample_rate / (bandwidth * 2)))
        self.output_sample_rate = sample_rate / self.decimation_factor
        
        # NCO frequency (normalized)
        self.nco_freq = center_freq / sample_rate
        
        # Initialize NCO Synchronizer for efficient NCO generation
        self.nco = NCOSynchronizer(sample_rate, -center_freq)

        # Design low-pass filter
        self.filter_coeffs = self._design_lowpass_filter()
        
        # Performance metrics
        self.processed_samples = 0
        self.processing_time = 0.0
        self.throughput = 0.0
        
        # Window function for filtering
        self.window_func = signal.windows.hann
        
        print(f"DDC initialized:")
        print(f"  Input sample rate: {sample_rate/1e6:.1f} MSPS")
        print(f"  Center frequency: {center_freq/1e6:.1f} MHz")
        print(f"  Bandwidth: {bandwidth/1e6:.1f} MHz")
        print(f"  Decimation factor: {self.decimation_factor}")
        print(f"  Output sample rate: {self.output_sample_rate/1e6:.1f} MSPS")
    
    def _design_lowpass_filter(self) -> np.ndarray:
        """
        Design optimized low-pass FIR filter
        
        Returns:
            Filter coefficients array
        """
        # Filter length - trade-off between performance and quality
        # Longer filters = better attenuation but more CPU usage
        # Increased max length from 101 to 257 for better convergence
        filter_length = min(257, int(self.sample_rate / self.bandwidth * 10))
        if filter_length % 2 == 0:
            filter_length += 1  # Ensure odd length for linear phase
            
        # Design FIR filter using firwin for robustness, as remez was failing to converge
        cutoff_freq = self.bandwidth * 0.5  # Cutoff is half the bandwidth
        
        filter_coeffs = signal.firwin(
            numtaps=filter_length,
            cutoff=cutoff_freq,
            window='hann',
            fs=self.sample_rate
        )
        
        # Normalize to unity gain at DC
        filter_coeffs /= np.sum(filter_coeffs)
        
        return filter_coeffs
    
    def apply_ddc(self, samples: np.ndarray, 
                  apply_filter: bool = True,
                  apply_decimation: bool = True) -> np.ndarray:
        """
        Apply complete digital downconversion pipeline
        
        Args:
            samples: Input complex samples (I + jQ)
            apply_filter: Whether to apply low-pass filtering
            apply_decimation: Whether to apply decimation
            
        Returns:
            Downconverted and filtered samples
        """
        start_time = time.perf_counter()
        
        if len(samples) == 0:
            return np.array([], dtype=np.complex64)
        
        # Step 1: Frequency translation using NCO
        downconverted = self._apply_nco(samples)
        
        # Step 2: Low-pass filtering
        if apply_filter:
            downconverted = self._apply_lowpass_filter(downconverted)
        
        # Step 3: Decimation
        if apply_decimation and self.decimation_factor > 1:
            downconverted = self._apply_decimation(downconverted)
        
        # Update performance metrics
        end_time = time.perf_counter()
        processing_time = end_time - start_time
        
        self.processed_samples += len(samples)
        self.processing_time += processing_time
        
        if self.processing_time > 0:
            self.throughput = self.processed_samples / self.processing_time
        
        return downconverted
    
    def _apply_nco(self, samples: np.ndarray) -> np.ndarray:
        """
        Apply Numerically Controlled Oscillator for frequency translation
        
        Args:
            samples: Input complex samples
            
        Returns:
            Frequency-translated samples
        """
        # Generate NCO samples using the efficient NCOSynchronizer
        nco = self.nco.generate_nco_samples(len(samples))
        
        # Apply frequency shift (complex multiplication)
        downconverted = samples * nco
        
        return downconverted
    
    def _apply_lowpass_filter(self, samples: np.ndarray) -> np.ndarray:
        """
        Apply low-pass FIR filter
        
        Args:
            samples: Input samples to filter
            
        Returns:
            Filtered samples
        """
        # For better performance with long filters, use fftconvolve
        # The 'auto' method selects the fastest convolution method
        # A single convolution on the complex signal is more efficient
        # than two separate convolutions on the real and imaginary parts.
        return signal.fftconvolve(samples, self.filter_coeffs, mode='same')
    
    def _apply_decimation(self, samples: np.ndarray) -> np.ndarray:
        """
        Apply decimation with anti-aliasing filtering
        
        Args:
            samples: Input samples to decimate
            
        Returns:
            Decimated samples
        """
        # Simple decimation by taking every Nth sample
        # For better quality, use signal.decimate with FIR filter
        decimated = samples[::self.decimation_factor]
        
        return decimated
    
    def update_parameters(self, center_freq: float = None, 
                         bandwidth: float = None) -> None:
        """
        Update DDC parameters dynamically
        
        Args:
            center_freq: New center frequency in Hz
            bandwidth: New bandwidth in Hz
        """
        updated = False
        
        if center_freq is not None and center_freq != self.center_freq:
            self.center_freq = center_freq
            self.nco_freq = center_freq / self.sample_rate
            self.nco.update_frequency(-center_freq)
            updated = True
            
        if bandwidth is not None and bandwidth != self.bandwidth:
            self.bandwidth = bandwidth
            self.decimation_factor = int(np.ceil(self.sample_rate / (bandwidth * 2)))
            self.output_sample_rate = self.sample_rate / self.decimation_factor
            self.filter_coeffs = self._design_lowpass_filter()
            updated = True
        
        if updated:
            print(f"DDC parameters updated:")
            print(f"  Center frequency: {self.center_freq/1e6:.1f} MHz")
            print(f"  Bandwidth: {self.bandwidth/1e6:.1f} MHz")
            print(f"  Decimation factor: {self.decimation_factor}")
            print(f"  Output sample rate: {self.output_sample_rate/1e6:.1f} MSPS")
    
    def get_frequency_response(self, num_points: int = 1024) -> Tuple[np.ndarray, np.ndarray]:
        """
        Get frequency response of the DDC filter
        
        Args:
            num_points: Number of frequency points
            
        Returns:
            Tuple of (frequencies, magnitude_response) in dB
        """
        # Calculate frequency response
        frequencies, h = signal.freqz(
            self.filter_coeffs, 
            worN=num_points, 
            fs=self.sample_rate
        )
        
        # Convert to magnitude in dB
        magnitude_db = 20 * np.log10(np.abs(h) + 1e-12)
        
        return frequencies, magnitude_db
    
    def get_performance_metrics(self) -> dict:
        """
        Get DDC performance metrics
        
        Returns:
            Dictionary with performance statistics
        """
        return {
            'processed_samples': int(self.processed_samples),
            'total_processing_time': self.processing_time,
            'throughput_samples_per_sec': self.throughput,
            'output_sample_rate': self.output_sample_rate,
            'decimation_factor': self.decimation_factor,
            'filter_length': len(self.filter_coeffs),
            'bandwidth': self.bandwidth,
            'center_frequency': self.center_freq
        }
    
    def reset_metrics(self) -> None:
        """Reset performance metrics"""
        self.processed_samples = 0
        self.processing_time = 0.0
        self.throughput = 0.0




class PolyphaseFilterBank:
    """
    Polyphase Filter Bank for efficient multi-channel DDC
    """
    
    def __init__(self, sample_rate: float, channel_bandwidth: float, num_channels: int):
        """
        Initialize Polyphase Filter Bank
        
        Args:
            sample_rate: Input sample rate in Hz
            channel_bandwidth: Bandwidth per channel in Hz
            num_channels: Number of channels
        """
        self.sample_rate = sample_rate
        self.channel_bandwidth = channel_bandwidth
        self.num_channels = num_channels
        
        # Calculate channel spacing (typically equal to bandwidth)
        self.channel_spacing = sample_rate / num_channels
        
        # Design prototype filter
        self.prototype_filter = self._design_prototype_filter()
        
        # Create polyphase filter bank
        self.polyphase_filters = self._create_polyphase_bank()
        
        print(f"Polyphase Filter Bank initialized:")
        print(f"  Sample rate: {sample_rate/1e6:.1f} MSPS")
        print(f"  Number of channels: {num_channels}")
        print(f"  Channel bandwidth: {channel_bandwidth/1e6:.1f} MHz")
        print(f"  Channel spacing: {self.channel_spacing/1e6:.1f} MHz")
    
    def _design_prototype_filter(self) -> np.ndarray:
        """Design prototype low-pass filter"""
        # Filter length based on transition bandwidth
        transition_bw = self.channel_bandwidth * 0.1  # 10% transition
        # Increased filter length for better performance
        filter_length = int(8 / (transition_bw / self.sample_rate))
        if filter_length % 2 == 0:
            filter_length += 1
            
        # Design filter using firwin for robustness
        cutoff = self.channel_bandwidth / 2.0
        
        coeffs = signal.firwin(
            numtaps=filter_length,
            cutoff=cutoff,
            window='hann',
            fs=self.sample_rate
        )
        
        return coeffs
    
    def _create_polyphase_bank(self) -> list:
        """Create polyphase filter bank"""
        filters = []
        L = self.num_channels
        
        for m in range(L):
            # Extract m-th polyphase component
            polyphase_filter = self.prototype_filter[m::L]
            filters.append(polyphase_filter)
        
        return filters
    
    def process_channel(self, samples: np.ndarray, channel_index: int) -> np.ndarray:
        """
        Process samples for a specific channel
        
        Args:
            samples: Input samples
            channel_index: Channel to process (0 to num_channels-1)
            
        Returns:
            Channel output samples
        """
        if channel_index >= self.num_channels:
            raise ValueError(f"Channel index {channel_index} out of range")
        
        # Apply polyphase filtering
        filtered = signal.lfilter(self.polyphase_filters[channel_index], 1, samples)
        
        # Decimate by number of channels
        decimated = filtered[::self.num_channels]
        
        return decimated


def demo_digital_downconverter():
    """Demonstration of Digital Downconverter functionality"""
    print("=== Digital Downconverter Demonstration ===\n")
    
    # Parameters
    sample_rate = 100e6  # 100 MSPS
    center_freq = 95.5e6  # FM radio band
    bandwidth = 200e3    # 200 kHz bandwidth
    
    # Create DDC
    ddc = DigitalDownconverter(sample_rate, center_freq, bandwidth)
    
    # Generate test signal (FM station at center frequency + 75 kHz offset)
    t = np.arange(0, 0.01, 1/sample_rate)  # 10ms of data
    test_signal = np.exp(1j * 2 * np.pi * (center_freq + 75e3) * t)
    
    # Add noise
    noise_power = 0.01
    test_signal += np.sqrt(noise_power/2) * (np.random.randn(len(t)) + 1j * np.random.randn(len(t)))
    
    print(f"Generated test signal: {len(test_signal)} samples")
    print(f"Signal duration: {len(t)/sample_rate*1000:.2f} ms")
    
    # Apply DDC
    print("\nApplying digital downconversion...")
    downconverted = ddc.apply_ddc(test_signal)
    
    # Results
    print(f"\nDownconversion Results:")
    print(f"  Output samples: {len(downconverted)}")
    print(f"  Output duration: {len(downconverted)/ddc.output_sample_rate*1000:.2f} ms")
    print(f"  Compression ratio: {len(test_signal)/len(downconverted):.1f}:1")
    
    # Performance metrics
    metrics = ddc.get_performance_metrics()
    print(f"\nPerformance Metrics:")
    for key, value in metrics.items():
        print(f"  {key}: {value}")
    
    # Frequency response
    print(f"\nGenerating frequency response plot...")
    freqs, mag_response = ddc.get_frequency_response(2048)
    
    # Find -3dB point
    mag_response_normalized = mag_response - mag_response[0]
    cutoff_idx = np.where(mag_response_normalized <= -3.0)[0]
    if len(cutoff_idx) > 0:
        cutoff_freq = freqs[cutoff_idx[0]] / 1e6
        print(f"  -3dB cutoff frequency: {cutoff_freq:.2f} MHz")
    
    return ddc, test_signal, downconverted


if __name__ == "__main__":
    # Run demonstration
    ddc, input_signal, output_signal = demo_digital_downconverter()
    
    print(f"\n=== Demonstration Complete ===")
    print(f"Input signal length: {len(input_signal)} samples")
    print(f"Output signal length: {len(output_signal)} samples")
    print(f"Processing successful!")
