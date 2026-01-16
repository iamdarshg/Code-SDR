#!/usr/bin/env python3
"""
Wideband SDR Recording and Playback Example

This example demonstrates the comprehensive recording and playback system
with various formats, compression options, and features.

Features demonstrated:
- Multiple data formats (complex64, complex128, int16, uint16)
- Compression support (none, gzip, zlib)
- Metadata preservation
- Format conversion
- Batch recording
- Progress tracking

Author: Wideband SDR Development Team
Version: 1.0.0
"""

import numpy as np
import time
import os
import sys
from pathlib import Path

# Add the software directory to the path to import the recording module
sys.path.append(str(Path(__file__).parent.parent))

from recording import (
    RecordingMetadata, SDRRecorder, SDRPlayback, 
    FormatConverter, create_test_signal, batch_record
)


def demo_basic_recording():
    """Demonstrate basic recording and playback functionality."""
    print("\n" + "="*60)
    print("BASIC RECORDING AND PLAYBACK DEMO")
    print("="*60)
    
    # Create a test signal
    print("1. Creating test signal...")
    test_signal = create_test_signal(
        duration=3.0,
        sample_rate=2_000_000,
        frequency=1_000_000,
        amplitude=0.8
    )
    print(f"   Generated {len(test_signal)} samples")
    
    # Create comprehensive metadata
    metadata = RecordingMetadata(
        sample_rate=2_000_000,
        frequency=1_000_000,
        gain=30,
        bandwidth=100_000,
        format='complex64',
        compression='gzip',
        description='Basic recording demonstration',
        location='Test Lab',
        antenna='Wire antenna',
        weather='Clear skies'
    )
    
    # Record the signal
    print("2. Recording signal...")
    recorder = SDRRecorder('demo_basic', metadata, chunk_size=8192)
    recorder.start_recording(len(test_signal))
    
    # Write in chunks to simulate real recording
    chunk_size = 1024
    for i in range(0, len(test_signal), chunk_size):
        chunk = test_signal[i:i+chunk_size]
        recorder.write_samples(chunk)
        
        # Show progress
        progress = recorder.get_progress()
        print(f"\r   Progress: {progress:.1%}", end='', flush=True)
    
    recorder.stop_recording()
    print(f"\n   Recording completed!")
    
    # Playback the recording
    print("3. Playing back recording...")
    playback = SDRPlayback('demo_basic')
    
    loaded_metadata = playback.get_metadata()
    print(f"   Metadata loaded: {loaded_metadata.format}")
    print(f"   Sample rate: {loaded_metadata.sample_rate} Hz")
    print(f"   Frequency: {loaded_metadata.frequency} Hz")
    print(f"   Total samples: {playback.get_sample_count()}")
    print(f"   File size: {playback.get_file_size()} bytes")
    
    # Read and analyze samples
    samples = playback.read_samples(1024)
    if samples is not None:
        signal_power = np.mean(np.abs(samples)**2)
        print(f"   Read {len(samples)} samples")
        print(f"   Signal power: {signal_power:.6f}")
    
    playback.close()
    print("   Playback completed!")


def demo_multiple_formats():
    """Demonstrate multiple data format support."""
    print("\n" + "="*60)
    print("MULTIPLE FORMAT SUPPORT DEMO")
    print("="*60)
    
    # Create test signal
    test_signal = create_test_signal(2.0, 1_000_000, 500_000, 0.6)
    
    formats = ['complex64', 'complex128', 'int16', 'uint16']
    
    for fmt in formats:
        print(f"\n1. Testing format: {fmt}")
        
        # Create metadata for this format
        metadata = RecordingMetadata(
            sample_rate=1_000_000,
            frequency=500_000,
            format=fmt,
            compression='none',
            description=f'Format test: {fmt}'
        )
        
        # Record in this format
        recorder = SDRRecorder(f'demo_format_{fmt}', metadata)
        recorder.start_recording(len(test_signal))
        
        # Convert signal to target format
        format_dtypes = {
            'complex64': np.complex64,
            'complex128': np.complex128,
            'int16': np.int16,
            'uint16': np.uint16
        }
        target_dtype = format_dtypes[fmt]
        converted_signal = test_signal.astype(target_dtype)
        
        recorder.write_samples(converted_signal)
        recorder.stop_recording()
        
        # Playback and verify
        playback = SDRPlayback(f'demo_format_{fmt}')
        loaded_samples = playback.read_samples(512)
        
        print(f"   Original shape: {test_signal.shape}, dtype: {test_signal.dtype}")
        print(f"   Recorded shape: {loaded_samples.shape}, dtype: {loaded_samples.dtype}")
        print(f"   File size: {playback.get_file_size()} bytes")
        
        playback.close()


def demo_compression():
    """Demonstrate compression support."""
    print("\n" + "="*60)
    print("COMPRESSION SUPPORT DEMO")
    print("="*60)
    
    # Create larger test signal for compression benefits
    test_signal = create_test_signal(5.0, 5_000_000, 2_000_000, 0.7)
    
    compressions = ['none', 'gzip', 'zlib']
    
    print(f"Original signal size: {len(test_signal) * 8} bytes (complex64)")
    
    for compression in compressions:
        print(f"\n1. Testing compression: {compression}")
        
        metadata = RecordingMetadata(
            sample_rate=5_000_000,
            frequency=2_000_000,
            compression=compression,
            description=f'Compression test: {compression}'
        )
        
        recorder = SDRRecorder(f'demo_compression_{compression}', metadata)
        recorder.start_recording(len(test_signal))
        recorder.write_samples(test_signal)
        recorder.stop_recording()
        
        # Check file size
        data_file = f'demo_compression_{compression}.iq'
        if os.path.exists(data_file):
            file_size = os.path.getsize(data_file)
            compression_ratio = file_size / (len(test_signal) * 8)
            print(f"   Compressed file size: {file_size} bytes")
            print(f"   Compression ratio: {compression_ratio:.2%}")
            
            # Test playback
            playback = SDRPlayback(f'demo_compression_{compression}')
            samples = playback.read_samples(256)
            if samples is not None:
                print(f"   Successfully read {len(samples)} samples")
            playback.close()


def demo_format_conversion():
    """Demonstrate format conversion capabilities."""
    print("\n" + "="*60)
    print("FORMAT CONVERSION DEMO")
    print("="*60)
    
    # Create original recording
    test_signal = create_test_signal(2.0, 1_000_000, 750_000, 0.5)
    
    metadata = RecordingMetadata(
        sample_rate=1_000_000,
        frequency=750_000,
        format='complex64',
        compression='gzip',
        description='Original recording for conversion'
    )
    
    print("1. Creating original recording...")
    recorder = SDRRecorder('demo_conversion_original', metadata)
    recorder.start_recording(len(test_signal))
    recorder.write_samples(test_signal)
    recorder.stop_recording()
    
    # Test conversion to different formats
    target_formats = ['complex128', 'int16', 'uint16']
    
    for target_format in target_formats:
        print(f"\n2. Converting to {target_format}...")
        
        playback = SDRPlayback('demo_conversion_original')
        converter = FormatConverter(playback, target_format)
        
        # Test sample conversion
        test_samples = test_signal[:256]
        converted_samples = converter.convert_samples(test_samples)
        
        print(f"   Original: {test_samples.dtype}")
        print(f"   Converted: {converted_samples.dtype}")
        print(f"   Samples converted: {len(converted_samples)}")
        
        # Test file conversion
        output_file = f'demo_conversion_{target_format}'
        success = converter.convert_file(output_file)
        
        if success:
            # Verify converted file
            converted_playback = SDRPlayback(output_file)
            print(f"   File conversion successful")
            print(f"   Output file size: {converted_playback.get_file_size()} bytes")
            converted_playback.close()
        else:
            print(f"   File conversion failed")


def demo_progress_tracking():
    """Demonstrate progress tracking functionality."""
    print("\n" + "="*60)
    print("PROGRESS TRACKING DEMO")
    print("="*60)
    
    # Create progress callback
    def progress_callback(progress, samples_recorded):
        percentage = progress * 100
        print(f"\r   Progress: {percentage:5.1f}% ({samples_recorded:>8} samples)", end='', flush=True)
    
    # Create test signal
    test_signal = create_test_signal(2.0, 1_000_000, 1_500_000, 0.6)
    
    metadata = RecordingMetadata(
        sample_rate=1_000_000,
        frequency=1_500_000,
        description='Progress tracking demonstration'
    )
    
    print("1. Recording with progress tracking...")
    recorder = SDRRecorder('demo_progress', metadata, 
                          chunk_size=512, progress_callback=progress_callback)
    recorder.start_recording(len(test_signal))
    
    # Write in small chunks to show progress
    chunk_size = 128
    for i in range(0, len(test_signal), chunk_size):
        chunk = test_signal[i:i+chunk_size]
        recorder.write_samples(chunk)
        time.sleep(0.01)  # Small delay to see progress updates
    
    recorder.stop_recording()
    print(f"\n   Recording completed with full progress tracking!")
    
    # Show final statistics
    final_metadata = recorder.metadata
    print(f"   Final samples recorded: {final_metadata.sample_count}")
    print(f"   Final duration: {final_metadata.duration:.2f} seconds")


def demo_metadata_features():
    """Demonstrate comprehensive metadata features."""
    print("\n" + "="*60)
    print("METADATA FEATURES DEMO")
    print("="*60)
    
    # Create rich metadata
    detailed_metadata = RecordingMetadata(
        sample_rate=10_000_000,
        frequency=3_500_000_000,  # 3.5 GHz
        gain=45,
        bandwidth=1_000_000,
        format='complex128',
        compression='gzip',
        description='Advanced metadata demonstration',
        location='Rooftop Lab, Building A',
        antenna='Yagi-Uda array',
        weather='Partly cloudy, 22°C',
        device_info={
            'software_version': '2.1.0',
            'hardware_version': 'v1.3',
            'calibrated': True,
            'temperature': 35.2,
            'humidity': 65.0
        }
    )
    
    print("1. Creating recording with detailed metadata...")
    
    # Create small test signal
    test_signal = create_test_signal(0.5, 1_000_000, 1_000_000, 0.4)
    
    recorder = SDRRecorder('demo_metadata', detailed_metadata)
    recorder.start_recording(len(test_signal))
    recorder.write_samples(test_signal)
    recorder.stop_recording()
    
    print("2. Reading and verifying metadata...")
    playback = SDRPlayback('demo_metadata')
    loaded_metadata = playback.get_metadata()
    
    print(f"   Sample rate: {loaded_metadata.sample_rate} Hz")
    print(f"   Frequency: {loaded_metadata.frequency} Hz")
    print(f"   Gain: {loaded_metadata.gain} dB")
    print(f"   Bandwidth: {loaded_metadata.bandwidth} Hz")
    print(f"   Format: {loaded_metadata.format}")
    print(f"   Compression: {loaded_metadata.compression}")
    print(f"   Description: {loaded_metadata.description}")
    print(f"   Location: {loaded_metadata.location}")
    print(f"   Antenna: {loaded_metadata.antenna}")
    print(f"   Weather: {loaded_metadata.weather}")
    print(f"   Device info: {loaded_metadata.device_info}")
    print(f"   Bit depth: {loaded_metadata.bit_depth}")
    
    playback.close()


def cleanup_demo_files():
    """Clean up all demo files."""
    print("\n" + "="*60)
    print("CLEANUP")
    print("="*60)
    
    # Find all demo files
    demo_files = []
    for file in os.listdir('.'):
        if file.startswith('demo_'):
            demo_files.append(file)
    
    print(f"Found {len(demo_files)} demo files")
    
    for file in demo_files:
        try:
            os.remove(file)
            print(f"   Removed: {file}")
        except Exception as e:
            print(f"   Failed to remove {file}: {e}")
    
    print("Demo files cleaned up!")


def main():
    """Main demonstration function."""
    print("WIDEBAND SDR RECORDING AND PLAYBACK SYSTEM")
    print("Comprehensive Feature Demonstration")
    print("="*60)
    
    # Run all demonstrations
    try:
        demo_basic_recording()
        demo_multiple_formats()
        demo_compression()
        demo_format_conversion()
        demo_progress_tracking()
        demo_metadata_features()
        
    except Exception as e:
        print(f"\nDemo failed with error: {e}")
        import traceback
        traceback.print_exc()
    
    finally:
        # Ask user if they want to clean up demo files
        print("\n" + "="*60)
        response = input("Clean up demo files? (y/N): ").lower().strip()
        if response in ['y', 'yes']:
            cleanup_demo_files()
        else:
            print("Demo files preserved for inspection")


if __name__ == "__main__":
    main()
