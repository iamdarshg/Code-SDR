"""
Wideband SDR Recording and Playback System

A comprehensive standalone module for recording and playing back SDR samples
with metadata support, multiple format options, and compression capabilities.

Features:
- Multiple data format support (complex64, complex128, int16, uint16)
- JSON metadata preservation
- Compression support (gzip, zlib)
- Chunked recording for large files
- Progress tracking and statistics
- Robust error handling

Author: Wideband SDR Development Team
Version: 1.0.0
"""

import os
import json
import gzip
import zlib
import time
import struct
from dataclasses import dataclass, asdict
from typing import Optional, Dict, Any, Union, BinaryIO
from pathlib import Path
import numpy as np
import threading
from datetime import datetime


@dataclass
class RecordingMetadata:
    """
    Comprehensive metadata for SDR recordings.
    
    Contains all necessary information to reproduce recording conditions
    and properly interpret the recorded samples.
    """
    # Core recording parameters
    sample_rate: int = 1_000_000  # Samples per second
    frequency: int = 100_000_000  # Center frequency in Hz
    gain: int = 30  # Gain setting in dB
    bandwidth: int = 0  # Bandwidth in Hz (0 = auto)
    
    # File information
    timestamp: float = 0.0  # Unix timestamp of recording start
    duration: float = 0.0  # Actual duration in seconds
    format: str = 'complex64'  # Data format: 'complex64', 'complex128', 'int16', 'uint16'
    compression: str = 'none'  # Compression: 'none', 'gzip', 'zlib'
    
    # Additional metadata
    device_info: Dict[str, Any] = None  # Device information
    description: str = ''  # User description
    location: str = ''  # Recording location
    antenna: str = ''  # Antenna used
    weather: str = ''  # Weather conditions
    
    # Statistics
    sample_count: int = 0  # Total samples recorded
    file_size: int = 0  # File size in bytes
    bit_depth: int = 32  # Bit depth of samples
    
    def __post_init__(self):
        """Initialize default values and validate metadata."""
        if self.timestamp == 0.0:
            self.timestamp = time.time()
        
        if self.device_info is None:
            self.device_info = {
                'software_version': '1.0.0',
                'hardware_version': 'unknown',
                'calibrated': False
            }
        
        # Validate format
        valid_formats = {'complex64', 'complex128', 'int16', 'uint16'}
        if self.format not in valid_formats:
            raise ValueError(f"Invalid format '{self.format}'. Must be one of {valid_formats}")
        
        # Validate compression
        valid_compression = {'none', 'gzip', 'zlib'}
        if self.compression not in valid_compression:
            raise ValueError(f"Invalid compression '{self.compression}'. Must be one of {valid_compression}")
        
        # Set bit depth based on format
        format_bit_depths = {
            'complex64': 32,
            'complex128': 64,
            'int16': 16,
            'uint16': 16
        }
        self.bit_depth = format_bit_depths.get(self.format, 32)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metadata to dictionary for JSON serialization."""
        return asdict(self)
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'RecordingMetadata':
        """Create metadata instance from dictionary."""
        return cls(**data)
    
    def update_statistics(self, sample_count: int, file_size: int, duration: float):
        """Update recording statistics."""
        self.sample_count = sample_count
        self.file_size = file_size
        self.duration = duration


class SDRRecorder:
    """
    Professional SDR sample recorder with comprehensive features.
    
    Handles recording of SDR samples to files with metadata preservation,
    compression support, and progress tracking.
    """
    
    # Supported data formats and their numpy dtypes
    FORMAT_DTYPES = {
        'complex64': np.complex64,
        'complex128': np.complex128,
        'int16': np.int16,
        'uint16': np.uint16
    }
    
    # Compression stream wrappers
    COMPRESSION_WRAPPERS = {
        'none': lambda f, mode: open(f, mode + 'b'),
        'gzip': lambda f, mode: gzip.open(f, mode + 'b'),
        'zlib': lambda f, mode: zlib.open(f, mode + 'b') if hasattr(zlib, 'open') else gzip.open(f, mode + 'b', compresslevel=1)
    }
    
    def __init__(self, filename: str, metadata: RecordingMetadata, 
                 chunk_size: int = 65536, progress_callback=None):
        """
        Initialize SDR recorder.
        
        Args:
            filename: Base filename for recording (without extension)
            metadata: Recording metadata
            chunk_size: Size of data chunks for recording (samples)
            progress_callback: Optional callback function for progress updates
        """
        self.filename = filename
        self.metadata = metadata
        self.chunk_size = chunk_size
        self.progress_callback = progress_callback
        
        # File handles
        self.data_file: Optional[BinaryIO] = None
        self.metadata_file: Optional[str] = None
        
        # Recording state
        self.recording = False
        self.sample_count = 0
        self.start_time = None
        self.thread_lock = threading.Lock()
        
        # Progress tracking
        self.total_samples = 0
        self.progress_lock = threading.Lock()
    
    def start_recording(self, total_samples: Optional[int] = None):
        """
        Start recording to file.
        
        Args:
            total_samples: Total number of samples to record (for progress tracking)
        """
        if self.recording:
            raise RuntimeError("Recording already in progress")
        
        self.start_time = time.time()
        self.total_samples = total_samples or 0
        
        # Determine actual filenames
        data_filename = f"{self.filename}.iq"
        metadata_filename = f"{self.filename}.json"
        
        # Create data file with appropriate compression
        compression_wrapper = self.COMPRESSION_WRAPPERS.get(self.metadata.compression, self.COMPRESSION_WRAPPERS['none'])
        self.data_file = compression_wrapper(data_filename, 'w')
        
        # Write initial metadata
        with open(metadata_filename, 'w') as f:
            json.dump(self.metadata.to_dict(), f, indent=2)
        
        self.metadata_file = metadata_filename
        self.recording = True
        
        print(f"Started recording to {data_filename}")
        print(f"Format: {self.metadata.format}, Compression: {self.metadata.compression}")
    
    def write_samples(self, samples: np.ndarray):
        """
        Write samples to the recording file.
        
        Args:
            samples: NumPy array of samples to write
        """
        if not self.recording:
            raise RuntimeError("Recording not started")
        
        if len(samples) == 0:
            return
        
        # Validate sample format
        expected_dtype = self.FORMAT_DTYPES.get(self.metadata.format)
        if samples.dtype != expected_dtype:
            # Convert samples to expected format
            samples = samples.astype(expected_dtype)
        
        # Write samples to file
        with self.thread_lock:
            try:
                self.data_file.write(samples.tobytes())
                self.sample_count += len(samples)
                
                # Update progress if callback provided
                if self.progress_callback:
                    progress = min(self.sample_count / max(self.total_samples, 1), 1.0) if self.total_samples > 0 else 0
                    self.progress_callback(progress, self.sample_count)
                    
            except Exception as e:
                raise IOError(f"Failed to write samples: {e}")
    
    def write_chunk(self, samples: np.ndarray):
        """
        Write a chunk of samples (alias for write_samples).
        
        Args:
            samples: NumPy array of samples to write
        """
        self.write_samples(samples)
    
    def stop_recording(self):
        """Stop recording and finalize files."""
        if not self.recording:
            raise RuntimeError("No recording in progress")
        
        with self.thread_lock:
            self.recording = False
            
            # Close data file
            if self.data_file:
                self.data_file.close()
                self.data_file = None
            
            # Calculate final statistics
            end_time = time.time()
            actual_duration = end_time - self.start_time
            
            # Get file size
            data_filename = f"{self.filename}.iq"
            file_size = os.path.getsize(data_filename) if os.path.exists(data_filename) else 0
            
            # Update metadata
            self.metadata.update_statistics(self.sample_count, file_size, actual_duration)
            
            # Write updated metadata
            if self.metadata_file:
                with open(self.metadata_file, 'w') as f:
                    json.dump(self.metadata.to_dict(), f, indent=2)
        
        print(f"Recording stopped. Total samples: {self.sample_count}, Duration: {actual_duration:.2f}s")
    
    def is_recording(self) -> bool:
        """Check if recording is in progress."""
        return self.recording
    
    def get_progress(self) -> float:
        """
        Get recording progress (0.0 to 1.0).
        
        Returns:
            Progress as fraction if total_samples set, otherwise 0.0
        """
        if self.total_samples <= 0:
            return 0.0
        return min(self.sample_count / self.total_samples, 1.0)


class SDRPlayback:
    """
    Professional SDR sample playback system.
    
    Handles playback of recorded SDR samples with proper metadata handling
    and format conversion.
    """
    
    # Format dtype mapping for reading
    FORMAT_DTYPES = {
        'complex64': np.complex64,
        'complex128': np.complex128,
        'int16': np.int16,
        'uint16': np.uint16
    }
    
    # Compression stream wrappers
    COMPRESSION_WRAPPERS = {
        'none': lambda f, mode: open(f, mode + 'b'),
        'gzip': lambda f, mode: gzip.open(f, mode + 'b'),
        'zlib': lambda f, mode: zlib.open(f, mode + 'b') if hasattr(zlib, 'open') else gzip.open(f, mode + 'b', compresslevel=1)
    }
    
    def __init__(self, filename: str):
        """
        Initialize SDR playback.
        
        Args:
            filename: Base filename for playback (without extension)
        """
        self.filename = filename
        self.metadata: Optional[RecordingMetadata] = None
        self.data_file: Optional[BinaryIO] = None
        self.file_size = 0
        self.total_samples = 0
        
        # Load metadata
        self._load_metadata()
        
        # Open data file
        self._open_data_file()
    
    def _load_metadata(self):
        """Load metadata from JSON file."""
        metadata_filename = f"{self.filename}.json"
        
        if not os.path.exists(metadata_filename):
            raise FileNotFoundError(f"Metadata file not found: {metadata_filename}")
        
        try:
            with open(metadata_filename, 'r') as f:
                data = json.load(f)
                self.metadata = RecordingMetadata.from_dict(data)
        except Exception as e:
            raise ValueError(f"Failed to load metadata: {e}")
    
    def _open_data_file(self):
        """Open the data file with appropriate compression."""
        data_filename = f"{self.filename}.iq"
        
        if not os.path.exists(data_filename):
            raise FileNotFoundError(f"Data file not found: {data_filename}")
        
        # Get file size
        self.file_size = os.path.getsize(data_filename)
        
        # Calculate total samples
        dtype = self.FORMAT_DTYPES.get(self.metadata.format, np.complex64)
        bytes_per_sample = np.dtype(dtype).itemsize
        self.total_samples = self.file_size // bytes_per_sample
        
        # Open file with appropriate compression
        compression_wrapper = self.COMPRESSION_WRAPPERS.get(self.metadata.compression, self.COMPRESSION_WRAPPERS['none'])
        try:
            self.data_file = compression_wrapper(data_filename, 'r')
        except Exception as e:
            raise IOError(f"Failed to open data file: {e}")
    
    def read_samples(self, count: int, start_offset: int = 0) -> Optional[np.ndarray]:
        """
        Read samples from file.
        
        Args:
            count: Number of samples to read
            start_offset: Starting sample offset
            
        Returns:
            NumPy array of samples, or None if end of file reached
        """
        if not self.data_file:
            raise RuntimeError("Data file not open")
        
        dtype = self.FORMAT_DTYPES.get(self.metadata.format, np.complex64)
        bytes_per_sample = np.dtype(dtype).itemsize
        
        # Calculate file position
        start_byte = start_offset * bytes_per_sample
        
        try:
            # Seek to starting position
            self.data_file.seek(start_byte)
            
            # Read requested number of samples
            bytes_to_read = min(count * bytes_per_sample, self.file_size - start_byte)
            
            if bytes_to_read <= 0:
                return None
            
            data_bytes = self.data_file.read(bytes_to_read)
            
            if not data_bytes:
                return None
            
            # Convert bytes to numpy array
            samples = np.frombuffer(data_bytes, dtype=dtype)
            
            return samples
            
        except Exception as e:
            raise IOError(f"Failed to read samples: {e}")
    
    def read_chunk(self, count: int, start_offset: int = 0) -> Optional[np.ndarray]:
        """
        Read a chunk of samples (alias for read_samples).
        
        Args:
            count: Number of samples to read
            start_offset: Starting sample offset
            
        Returns:
            NumPy array of samples, or None if end of file reached
        """
        return self.read_samples(count, start_offset)
    
    def get_sample_count(self) -> int:
        """Get total number of samples in file."""
        return self.total_samples
    
    def get_file_size(self) -> int:
        """Get file size in bytes."""
        return self.file_size
    
    def get_metadata(self) -> RecordingMetadata:
        """Get recording metadata."""
        if not self.metadata:
            raise RuntimeError("Metadata not loaded")
        return self.metadata
    
    def convert_format(self, target_format: str, target_dtype: type = None) -> 'FormatConverter':
        """
        Create a format converter for this recording.
        
        Args:
            target_format: Target format name
            target_dtype: Target numpy dtype (optional)
            
        Returns:
            FormatConverter instance
        """
        return FormatConverter(self, target_format, target_dtype)
    
    def close(self):
        """Close the data file."""
        if self.data_file:
            self.data_file.close()
            self.data_file = None


class FormatConverter:
    """
    Format converter for SDR samples.
    
    Handles conversion between different data formats with proper scaling
    and type conversion.
    """
    
    def __init__(self, playback: SDRPlayback, target_format: str, target_dtype: type = None):
        """
        Initialize format converter.
        
        Args:
            playback: SDRPlayback instance
            target_format: Target format name
            target_dtype: Target numpy dtype (optional)
        """
        self.playback = playback
        self.target_format = target_format
        
        # Determine target dtype
        if target_dtype is None:
            format_dtypes = {
                'complex64': np.complex64,
                'complex128': np.complex128,
                'int16': np.int16,
                'uint16': np.uint16
            }
            self.target_dtype = format_dtypes.get(target_format, np.complex64)
        else:
            self.target_dtype = target_dtype
    
    def convert_samples(self, samples: np.ndarray) -> np.ndarray:
        """
        Convert samples to target format.
        
        Args:
            samples: Input samples
            
        Returns:
            Converted samples
        """
        source_dtype = samples.dtype
        target_dtype = self.target_dtype
        
        # Handle complex to real conversion
        if np.iscomplexobj(samples) and not np.iscomplexobj(target_dtype):
            # Take real part for real target formats
            samples = np.real(samples)
        
        # Handle real to complex conversion
        elif not np.iscomplexobj(samples) and np.iscomplexobj(target_dtype):
            # Create complex samples with zero imaginary part
            samples = samples.astype(target_dtype.real.dtype)
            samples = samples + 0j
        
        # Handle type conversion with scaling
        else:
            # Determine if scaling is needed
            source_info = np.iinfo(source_dtype) if np.issubdtype(source_dtype, np.integer) else np.finfo(source_dtype)
            target_info = np.iinfo(target_dtype) if np.issubdtype(target_dtype, np.integer) else np.finfo(target_dtype)
            
            # Check if scaling is needed
            if (source_info.max > target_info.max or source_info.min < target_info.min):
                # Scale to target range
                samples = samples.astype(np.float64)  # Convert to float for scaling
                samples = samples / source_info.max * target_info.max
                samples = samples.astype(target_dtype)
            else:
                # Direct conversion
                samples = samples.astype(target_dtype)
        
        return samples
    
    def convert_file(self, output_filename: str, chunk_size: int = 65536) -> bool:
        """
        Convert entire file to target format.
        
        Args:
            output_filename: Output filename
            chunk_size: Size of chunks to process
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Create output metadata
            output_metadata = RecordingMetadata(
                **self.playback.metadata.to_dict()
            )
            output_metadata.format = self.target_format
            
            # Create output recorder
            recorder = SDRRecorder(output_filename, output_metadata)
            recorder.start_recording(self.playback.total_samples)
            
            # Convert in chunks
            offset = 0
            while offset < self.playback.total_samples:
                chunk_size_actual = min(chunk_size, self.playback.total_samples - offset)
                
                # Read chunk
                chunk = self.playback.read_samples(chunk_size_actual, offset)
                if chunk is None or len(chunk) == 0:
                    break
                
                # Convert chunk
                converted_chunk = self.convert_samples(chunk)
                
                # Write converted chunk
                recorder.write_samples(converted_chunk)
                
                offset += len(chunk)
            
            recorder.stop_recording()
            return True
            
        except Exception as e:
            print(f"Conversion failed: {e}")
            return False


def create_test_signal(duration: float, sample_rate: int, 
                      frequency: int = 1000000, amplitude: float = 0.5) -> np.ndarray:
    """
    Create a test signal for recording.
    
    Args:
        duration: Signal duration in seconds
        sample_rate: Sample rate in Hz
        frequency: Signal frequency in Hz
        amplitude: Signal amplitude (0.0 to 1.0)
        
    Returns:
        Complex numpy array containing test signal
    """
    t = np.arange(int(duration * sample_rate)) / sample_rate
    signal = amplitude * np.exp(2j * np.pi * frequency * t)
    return signal.astype(np.complex64)


def batch_record(sdr_interface, filename: str, duration: float, 
                sample_rate: int = 2000000, frequency: int = 95500000,
                gain: int = 30, format_type: str = 'complex64',
                compression: str = 'gzip') -> bool:
    """
    Batch recording function for SDR interfaces.
    
    Args:
        sdr_interface: SDR interface object with read_samples method
        filename: Base filename for recording
        duration: Recording duration in seconds
        sample_rate: Sample rate in Hz
        frequency: Center frequency in Hz
        gain: Gain setting in dB
        format_type: Data format ('complex64', 'complex128', 'int16', 'uint16')
        compression: Compression type ('none', 'gzip', 'zlib')
        
    Returns:
        True if recording successful, False otherwise
    """
    try:
        # Create metadata
        metadata = RecordingMetadata(
            sample_rate=sample_rate,
            frequency=frequency,
            gain=gain,
            format=format_type,
            compression=compression,
            description=f"Batch recording at {frequency/1e6:.1f} MHz"
        )
        
        # Calculate total samples
        total_samples = int(duration * sample_rate)
        
        # Create recorder
        recorder = SDRRecorder(filename, metadata, chunk_size=8192)
        recorder.start_recording(total_samples)
        
        # Configure SDR
        sdr_interface.set_frequency(frequency)
        sdr_interface.set_sample_rate(sample_rate)
        sdr_interface.set_gain(gain)
        
        # Start recording
        remaining_samples = total_samples
        chunk_size = 8192
        
        while remaining_samples > 0:
            current_chunk_size = min(chunk_size, remaining_samples)
            
            # Read samples from SDR
            samples = sdr_interface.read_samples(current_chunk_size)
            if samples is None or len(samples) == 0:
                break
            
            # Convert format if necessary
            if format_type != 'complex64':
                format_dtypes = {
                    'complex128': np.complex128,
                    'int16': np.int16,
                    'uint16': np.uint16
                }
                target_dtype = format_dtypes.get(format_type, np.complex64)
                samples = samples.astype(target_dtype)
            
            # Write samples
            recorder.write_samples(samples)
            remaining_samples -= len(samples)
            
            # Progress update
            progress = recorder.get_progress()
            print(f"\rRecording progress: {progress:.1%}", end='', flush=True)
        
        recorder.stop_recording()
        print(f"\nRecording completed: {filename}")
        return True
        
    except Exception as e:
        print(f"\nRecording failed: {e}")
        return False


# Example usage and testing functions
def example_recording():
    """Example of recording and playback."""
    print("=== SDR Recording Example ===")
    
    # Create test signal
    print("Creating test signal...")
    test_signal = create_test_signal(duration=2.0, sample_rate=1000000, 
                                   frequency=100000, amplitude=0.7)
    
    # Create metadata
    metadata = RecordingMetadata(
        sample_rate=1_000_000,
        frequency=1_000_000,
        gain=30,
        format='complex64',
        compression='gzip',
        description='Test signal recording'
    )
    
    # Record test signal
    print("Recording test signal...")
    recorder = SDRRecorder('test_recording', metadata, chunk_size=1024)
    recorder.start_recording(len(test_signal))
    
    # Write in chunks
    chunk_size = 512
    for i in range(0, len(test_signal), chunk_size):
        chunk = test_signal[i:i+chunk_size]
        recorder.write_samples(chunk)
    
    recorder.stop_recording()
    print("Recording completed!")
    
    # Playback test signal
    print("Playing back recording...")
    playback = SDRPlayback('test_recording')
    
    metadata_read = playback.get_metadata()
    print(f"Loaded metadata: {metadata_read.format}, {metadata_read.sample_rate} Hz")
    print(f"Total samples: {playback.get_sample_count()}")
    
    # Read some samples
    samples = playback.read_samples(2048)
    if samples is not None:
        print(f"Read {len(samples)} samples")
        print(f"Signal power: {np.mean(np.abs(samples)**2):.6f}")
    
    playback.close()
    
    # Test format conversion
    print("Testing format conversion...")
    converter = FormatConverter(playback, 'complex128')
    if samples is not None:
        converted_samples = converter.convert_samples(samples[:100])
        print(f"Converted samples shape: {converted_samples.shape}, dtype: {converted_samples.dtype}")
    
    print("Example completed!")


if __name__ == "__main__":
    # Run example when module is executed directly
    example_recording()
