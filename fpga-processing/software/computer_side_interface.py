#!/usr/bin/env python3
"""
Computer-Side SDR Processing Interface
Implements direct data streaming mode with bandwidth limiting for FPGA processing pipeline
"""

import socket
import struct
import numpy as np
import threading
import time
from typing import Optional, Callable, Tuple
from dataclasses import dataclass
from enum import Enum

class StreamingMode(Enum):
    """Streaming modes for the FPGA processing pipeline"""
    PROCESSED = 0    # FFT processed data
    DIRECT = 1       # Direct ADC data streaming

@dataclass
class SDRConfig:
    """Configuration for SDR system"""
    fpga_ip: str = "192.168.0.2"
    fpga_port: int = 10000
    host_ip: str = "192.168.0.1"
    host_port: int = 10001
    mode: StreamingMode = StreamingMode.PROCESSED
    bandwidth_limit: int = 80  # Percentage (0-100)
    sample_rate: float = 105e6  # 105 MSPS
    fft_size: int = 1024

class SDRInterface:
    """
    Main interface for FPGA-based SDR processing pipeline
    Supports both processed FFT mode and direct streaming mode
    """
    
    def __init__(self, config: Optional[SDRConfig] = None):
        self.config = config or SDRConfig()
        self.socket = None
        self.connected = False
        self.streaming = False
        self.callbacks = []
        self.data_buffer = []
        self.bandwidth_monitor = BandwidthMonitor()
        
        # Initialize UDP socket
        self._setup_socket()
        
    def _setup_socket(self):
        """Setup UDP socket for data reception"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.socket.bind((self.config.host_ip, self.config.host_port))
            self.socket.settimeout(1.0)  # 1 second timeout
        except Exception as e:
            print(f"Error setting up socket: {e}")
            
    def connect(self) -> bool:
        """Connect to FPGA system"""
        try:
            # Send configuration command to FPGA
            config_msg = self._create_config_message()
            self.socket.sendto(config_msg, (self.config.fpga_ip, self.config.fpga_port))
            
            # Wait for acknowledgment
            ack, addr = self.socket.recvfrom(1024)
            if ack == b'ACK':
                self.connected = True
                print(f"Connected to FPGA at {self.config.fpga_ip}")
                return True
            else:
                print("Failed to connect to FPGA")
                return False
                
        except socket.timeout:
            print("Connection timeout")
            return False
        except Exception as e:
            print(f"Connection error: {e}")
            return False
    
    def _create_config_message(self) -> bytes:
        """Create configuration message for FPGA"""
        # Format: [MODE(1)] [BANDWIDTH(1)] [RESERVED(2)]
        mode_byte = self.config.mode.value
        bw_byte = min(max(self.config.bandwidth_limit, 0), 100)
        reserved = struct.pack('H', 0)
        
        return struct.pack('BBH', mode_byte, bw_byte, 0) + b'CONFIG'
    
    def start_streaming(self) -> bool:
        """Start data streaming from FPGA"""
        if not self.connected:
            print("Not connected to FPGA")
            return False
            
        try:
            # Send start streaming command
            start_msg = b'START\x00\x00\x00'
            self.socket.sendto(start_msg, (self.config.fpga_ip, self.config.fpga_port))
            
            # Start streaming thread
            self.streaming = True
            self.stream_thread = threading.Thread(target=self._streaming_loop, daemon=True)
            self.stream_thread.start()
            
            print("Data streaming started")
            return True
            
        except Exception as e:
            print(f"Error starting stream: {e}")
            return False
    
    def stop_streaming(self):
        """Stop data streaming"""
        self.streaming = False
        if hasattr(self, 'stream_thread'):
            self.stream_thread.join(timeout=2.0)
        print("Data streaming stopped")
    
    def _streaming_loop(self):
        """Main streaming loop for data reception and processing"""
        while self.streaming:
            try:
                # Receive UDP packet
                data, addr = self.socket.recvfrom(4096)
                
                # Monitor bandwidth
                self.bandwidth_monitor.update(len(data))
                
                # Parse packet based on mode
                if self.config.mode == StreamingMode.DIRECT:
                    samples = self._parse_direct_stream_packet(data)
                else:
                    samples = self._parse_processed_packet(data)
                
                # Apply bandwidth limiting
                if self.bandwidth_monitor.current_rate > self.config.bandwidth_limit * 10e6:  # Convert to bits/s
                    # Skip some packets if bandwidth exceeded
                    continue
                
                # Call registered callbacks
                for callback in self.callbacks:
                    try:
                        callback(samples)
                    except Exception as e:
                        print(f"Callback error: {e}")
                        
            except socket.timeout:
                continue
            except Exception as e:
                print(f"Streaming error: {e}")
                break
    
    def _parse_direct_stream_packet(self, data: bytes) -> np.ndarray:
        """Parse direct streaming packet (raw ADC data)"""
        # Format: [TIMESTAMP(4)] [SAMPLE_COUNTER(2)] [ADC_DATA(2)] ...
        # Total packet size: 4 + 2*N where N is number of samples
        
        if len(data) < 8:
            return np.array([])
        
        # Extract timestamp and sample counter
        timestamp = struct.unpack('<I', data[0:4])[0]
        sample_counter = struct.unpack('<H', data[4:6])[0]
        
        # Extract ADC samples (10-bit signed)
        samples = []
        for i in range(6, len(data), 2):
            if i + 1 < len(data):
                sample_data = struct.unpack('<H', data[i:i+2])[0]
                # Convert 10-bit signed ADC data
                if sample_data & 0x200:  # Sign bit
                    sample = -(0x400 - sample_data)
                else:
                    sample = sample_data
                samples.append(sample)
        
        return np.array(samples, dtype=np.int16)
    
    def _parse_processed_packet(self, data: bytes) -> np.ndarray:
        """Parse processed packet (FFT data)"""
        # Format: [TIMESTAMP(4)] [FFT_INDEX(4)] [REAL(4)] [IMAG(4)] ...
        if len(data) < 16:
            return np.array([])
        
        # Extract FFT data
        timestamp = struct.unpack('<I', data[0:4])[0]
        fft_index = struct.unpack('<I', data[4:8])[0]
        
        # Extract complex FFT values
        fft_data = []
        for i in range(8, len(data), 8):
            if i + 7 < len(data):
                real_part = struct.unpack('<i', data[i:i+4])[0]
                imag_part = struct.unpack('<i', data[i+4:i+8])[0]
                fft_data.append(complex(real_part, imag_part))
        
        return np.array(fft_data)
    
    def add_callback(self, callback: Callable[[np.ndarray], None]):
        """Add callback for processed data"""
        self.callbacks.append(callback)
    
    def remove_callback(self, callback: Callable[[np.ndarray], None]):
        """Remove callback"""
        if callback in self.callbacks:
            self.callbacks.remove(callback)
    
    def get_bandwidth_stats(self) -> dict:
        """Get current bandwidth statistics"""
        return self.bandwidth_monitor.get_stats()
    
    def close(self):
        """Close connection and cleanup"""
        self.stop_streaming()
        if self.socket:
            self.socket.close()
        self.connected = False

class BandwidthMonitor:
    """Monitor and control bandwidth usage"""
    
    def __init__(self):
        self.bytes_received = 0
        self.start_time = time.time()
        self.current_rate = 0.0
        self.peak_rate = 0.0
        self.samples_received = 0
        
    def update(self, packet_size: int):
        """Update bandwidth statistics"""
        current_time = time.time()
        self.bytes_received += packet_size
        self.samples_received += packet_size // 2  # Assuming 2 bytes per sample
        
        # Calculate current rate (bits per second)
        elapsed = current_time - self.start_time
        if elapsed > 0:
            self.current_rate = (self.bytes_received * 8) / elapsed
            self.peak_rate = max(self.peak_rate, self.current_rate)
    
    def get_stats(self) -> dict:
        """Get bandwidth statistics"""
        elapsed = time.time() - self.start_time
        return {
            'current_rate_mbps': self.current_rate / 1e6,
            'peak_rate_mbps': self.peak_rate / 1e6,
            'total_bytes': self.bytes_received,
            'total_samples': self.samples_received,
            'elapsed_seconds': elapsed
        }

class ComputerSideProcessor:
    """
    Computer-side signal processing for direct streaming mode
    Implements FFT, filtering, and demodulation
    """
    
    def __init__(self, sdr_interface: SDRInterface):
        self.sdr = sdr_interface
        self.fft_buffer = []
        self.fft_size = 1024
        self.sample_rate = 105e6
        
    def setup_fft_processing(self, fft_size: int = 1024, window: str = 'hamming'):
        """Setup FFT processing parameters"""
        self.fft_size = fft_size
        self.window = self._create_window(window, fft_size)
        
        # Register FFT processing callback
        self.sdr.add_callback(self._fft_callback)
        
    def _create_window(self, window_type: str, size: int) -> np.ndarray:
        """Create window function for FFT processing"""
        if window_type.lower() == 'hamming':
            return np.hamming(size)
        elif window_type.lower() == 'hanning':
            return np.hanning(size)
        elif window_type.lower() == 'blackman':
            return np.blackman(size)
        else:  # rectangular
            return np.ones(size)
    
    def _fft_callback(self, samples: np.ndarray):
        """Callback for FFT processing of incoming samples"""
        self.fft_buffer.extend(samples)
        
        # Process when we have enough samples
        while len(self.fft_buffer) >= self.fft_size:
            # Extract window
            window_samples = np.array(self.fft_buffer[:self.fft_size])
            self.fft_buffer = self.fft_buffer[self.fft_size//2:]  # 50% overlap
            
            # Apply window
            windowed_samples = window_samples * self.window
            
            # Compute FFT
            fft_result = np.fft.fft(windowed_samples)
            magnitude = np.abs(fft_result)
            phase = np.angle(fft_result)
            
            # Generate frequency axis
            freqs = np.fft.fftfreq(self.fft_size, 1/self.sample_rate)
            
            # Call processing callbacks
            self._process_fft_result(freqs, magnitude, phase)
    
    def _process_fft_result(self, freqs: np.ndarray, magnitude: np.ndarray, phase: np.ndarray):
        """Process FFT result and generate output"""
        # Convert to dB
        magnitude_db = 20 * np.log10(magnitude + 1e-10)
        
        # Peak detection
        peaks = self._detect_peaks(magnitude_db)
        
        # Call user callbacks with processed data
        for callback in getattr(self, 'processing_callbacks', []):
            try:
                callback(freqs, magnitude_db, phase, peaks)
            except Exception as e:
                print(f"Processing callback error: {e}")
    
    def _detect_peaks(self, magnitude_db: np.ndarray, threshold_db: float = -20) -> list:
        """Detect peaks in magnitude spectrum"""
        peaks = []
        threshold = threshold_db
        
        for i in range(1, len(magnitude_db) - 1):
            if (magnitude_db[i] > magnitude_db[i-1] and 
                magnitude_db[i] > magnitude_db[i+1] and 
                magnitude_db[i] > threshold):
                peaks.append(i)
        
        return peaks
    
    def add_processing_callback(self, callback: Callable):
        """Add callback for processed FFT results"""
        if not hasattr(self, 'processing_callbacks'):
            self.processing_callbacks = []
        self.processing_callbacks.append(callback)

def example_usage():
    """Example usage of the computer-side processing interface"""
    
    # Configure SDR system
    config = SDRConfig(
        fpga_ip="192.168.0.2",
        fpga_port=10000,
        host_ip="192.168.0.1",
        host_port=10001,
        mode=StreamingMode.DIRECT,  # Use direct streaming mode
        bandwidth_limit=80,          # Limit to 80% of 1 Gbit/s
        sample_rate=105e6,
        fft_size=1024
    )
    
    # Initialize interface
    sdr = SDRInterface(config)
    
    # Setup computer-side processing
    processor = ComputerSideProcessor(sdr)
    processor.setup_fft_processing(fft_size=1024, window='hamming')
    
    # Add processing callback
    def process_spectrum(freqs, magnitude_db, phase, peaks):
        print(f"Peak detection: {len(peaks)} peaks found")
        # Additional processing here...
        
        # Example: Print top 5 peaks
        if peaks:
            peak_magnitudes = magnitude_db[peaks]
            sorted_peaks = sorted(zip(peaks, peak_magnitudes), 
                                key=lambda x: x[1], reverse=True)
            print("Top 5 peaks:")
            for i, (peak_idx, magnitude) in enumerate(sorted_peaks[:5]):
                print(f"  Peak {i+1}: {freqs[peak_idx]/1e6:.2f} MHz, {magnitude:.1f} dB")
    
    processor.add_processing_callback(process_spectrum)
    
    # Connect and start streaming
    if sdr.connect():
        if sdr.start_streaming():
            print("Streaming started. Press Ctrl+C to stop...")
            try:
                while True:
                    time.sleep(1)
                    stats = sdr.get_bandwidth_stats()
                    print(f"Bandwidth: {stats['current_rate_mbps']:.2f} Mbps "
                          f"(Peak: {stats['peak_rate_mbps']:.2f} Mbps)")
            except KeyboardInterrupt:
                print("Stopping...")
            finally:
                sdr.stop_streaming()
    
    sdr.close()

if __name__ == "__main__":
    example_usage()
