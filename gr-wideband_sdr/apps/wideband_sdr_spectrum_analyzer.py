#!/usr/bin/env python3
"""
Simple Wideband SDR Spectrum Analyzer Example

This example demonstrates basic usage of the GNU Radio Wideband SDR module
by creating a simple spectrum analyzer flowgraph.
"""

import numpy as np
import sys
import signal
import time
from typing import Optional

def signal_handler(signum, frame):
    """Handle Ctrl+C gracefully"""
    print("\nExiting spectrum analyzer...")
    sys.exit(0)

def create_spectrum_analyzer_flowgraph(sample_rate: float = 2e6, 
                                      frequency: float = 100e6, 
                                      gain: int = 30,
                                      fft_size: int = 1024):
    """
    Create a simple spectrum analyzer flowgraph
    
    Args:
        sample_rate: Sample rate in Hz
        frequency: Center frequency in Hz  
        gain: RF gain in dB
        fft_size: FFT size for analysis
    """
    
    try:
        # Import GNU Radio components
        from gnuradio import gr
        from gnuradio.wxgui import sink_c
        import wx
        
        # Import our Wideband SDR module
        import gnuradio.wideband_sdr as wideband_sdr
        
    except ImportError as e:
        print(f"Error importing required modules: {e}")
        print("This example requires:")
        print("  - GNU Radio with WX GUI support")
        print("  - The gr-wideband-sdr module to be installed")
        print("  - Python bindings to be compiled")
        return None
    
    class SpectrumAnalyzerFlowgraph(gr.top_block):
        def __init__(self):
            gr.top_block.__init__(self, "Wideband SDR Spectrum Analyzer")
            
            # Create Wideband SDR source block
            print(f"Creating Wideband SDR source...")
            print(f"  Sample Rate: {sample_rate/1e6:.1f} MSPS")
            print(f"  Frequency: {frequency/1e6:.1f} MHz")
            print(f"  Gain: {gain} dB")
            
            try:
                self.sdr_source = wideband_sdr.source(
                    sample_rate=sample_rate,
                    frequency=frequency,
                    gain=gain
                )
                print("✓ Wideband SDR source created successfully")
                
            except Exception as e:
                print(f"✗ Error creating SDR source: {e}")
                print("  This is expected if the module is not yet compiled")
                print("  The example will run in simulation mode")
                
                # Create a simulated source for testing
                self.sdr_source = SimulatedSource(sample_rate, frequency, gain)
            
            # Create FFT sink for visualization
            try:
                self.fft_sink = sink_c(
                    sample_rate=sample_rate,
                    fft_size=fft_size,
                    center_freq=frequency,
                    ref_scale=1.0,
                    y_per_div=10,
                    y_divs=10,
                    fc_pos=0,
                    name="Wideband SDR Spectrum",
                    win='hann',
                    showRf=True
                )
                print("✓ FFT sink created successfully")
                
            except Exception as e:
                print(f"✗ Error creating FFT sink: {e}")
                return None
            
            # Connect the blocks
            self.connect(self.sdr_source, self.fft_sink)
            print("✓ Flowgraph connected successfully")
            
            # Print device info if available
            try:
                info = self.sdr_source.get_device_info()
                print(f"Device: {info.get('model', 'Unknown')}")
                print(f"Firmware: {info.get('firmware_version', 'Unknown')}")
            except:
                pass  # Not critical
                
        def start_streaming(self):
            """Start the SDR streaming"""
            try:
                self.sdr_source.start_streaming()
                print("✓ SDR streaming started")
                return True
            except Exception as e:
                print(f"✗ Error starting streaming: {e}")
                return False
                
        def stop_streaming(self):
            """Stop the SDR streaming"""
            try:
                self.sdr_source.stop_streaming()
                print("✓ SDR streaming stopped")
            except Exception as e:
                print(f"✗ Error stopping streaming: {e}")
    
    class SimulatedSource:
        """Simulated SDR source for testing when hardware is not available"""
        
        def __init__(self, sample_rate, frequency, gain):
            self.sample_rate = sample_rate
            self.frequency = frequency
            self.gain = gain
            self._streaming = False
            self._t = 0.0
            
        def start_streaming(self):
            self._streaming = True
            print("Simulation mode: Using simulated signal")
            
        def stop_streaming(self):
            self._streaming = False
            
        def get_device_info(self):
            return {
                'model': 'Simulated SDR',
                'firmware_version': '1.0.0-sim',
                'mode': 'simulation'
            }
            
        def __call__(self, samples):
            """Generate simulated samples"""
            if not self._streaming:
                return np.zeros(samples, dtype=np.complex64)
            
            # Generate some test signals
            num_samples = len(samples)
            t = np.arange(num_samples) / self.sample_rate + self._t
            
            # Test signals
            signal1 = 0.1 * np.exp(2j * np.pi * 1000 * t)  # 1 kHz test tone
            signal2 = 0.05 * np.exp(2j * np.pi * 5000 * t)  # 5 kHz test tone
            noise = 0.01 * (np.random.randn(num_samples) + 1j * np.random.randn(num_samples))
            
            samples = signal1 + signal2 + noise
            self._t += num_samples / self.sample_rate
            
            return samples
    
    return SpectrumAnalyzerFlowgraph()

def run_spectrum_analyzer():
    """Main function to run the spectrum analyzer"""
    
    # Parse command line arguments
    import argparse
    parser = argparse.ArgumentParser(description='Wideband SDR Spectrum Analyzer')
    parser.add_argument('--sample-rate', type=float, default=2e6,
                       help='Sample rate in Hz (default: 2e6)')
    parser.add_argument('--frequency', type=float, default=100e6,
                       help='Center frequency in Hz (default: 100e6)')
    parser.add_argument('--gain', type=int, default=30,
                       help='RF gain in dB (default: 30)')
    parser.add_argument('--fft-size', type=int, default=1024,
                       help='FFT size (default: 1024)')
    parser.add_argument('--no-gui', action='store_true',
                       help='Run without GUI (text mode only)')
    
    args = parser.parse_args()
    
    # Set up signal handling
    signal.signal(signal.SIGINT, signal_handler)
    
    print("=" * 60)
    print("Wideband SDR Spectrum Analyzer")
    print("=" * 60)
    
    # Create the flowgraph
    tb = create_spectrum_analyzer_flowgraph(
        sample_rate=args.sample_rate,
        frequency=args.frequency,
        gain=args.gain,
        fft_size=args.fft_size
    )
    
    if tb is None:
        print("Failed to create flowgraph")
        return 1
    
    # Start the flowgraph
    try:
        print("\nStarting flowgraph...")
        tb.start()
        
        # Start streaming
        if tb.start_streaming():
            print("\nSpectrum analyzer running...")
            print("Press Ctrl+C to stop")
            
            if args.no_gui:
                # Text mode operation
                stats_count = 0
                while True:
                    time.sleep(1)
                    stats_count += 1
                    
                    try:
                        stats = tb.sdr_source.get_statistics()
                        print(f"\rSamples processed: {stats.get('samples_processed', 0):,}", end='', flush=True)
                    except:
                        pass
                    
                    if stats_count % 10 == 0:  # Every 10 seconds
                        print()  # New line
        else:
            print("Failed to start streaming")
            return 1
            
    except KeyboardInterrupt:
        print("\nStopping...")
        
    finally:
        # Clean shutdown
        tb.stop()
        tb.wait()
        tb.stop_streaming()
        print("\nSpectrum analyzer stopped")
    
    print("\nWideband SDR Spectrum Analyzer completed")
    return 0

if __name__ == '__main__':
    sys.exit(run_spectrum_analyzer())
