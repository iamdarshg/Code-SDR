#!/usr/bin/env python3
"""
Wideband SDR Spectrum Analyzer Example
Real-time spectrum analysis using Wideband SDR hardware

Created: November 2025
Version: 1.0.0
License: MIT

Features:
- Real-time spectrum display
- Frequency sweep capability
- Power measurement
- Peak detection
- Save/load configurations
- Multiple window functions
- Zoom and pan functionality
"""

import sys
import time
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Slider, Button
from scipy import signal
import argparse
import logging

# Add parent directory to path for imports
sys.path.append('..')
from wideband_sdr import WidebandSDR, WidebandSDRError, SDRConnectionError

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class SpectrumAnalyzer:
    """
    Wideband SDR Spectrum Analyzer
    
    Real-time spectrum analysis with matplotlib visualization
    """
    
    def __init__(self, center_freq=100e6, sample_rate=10e6, gain=30):
        """
        Initialize spectrum analyzer
        
        Args:
            center_freq: Center frequency in Hz
            sample_rate: Sample rate in samples per second
            gain: RF gain in dB
        """
        self.center_freq = center_freq
        self.sample_rate = sample_rate
        self.gain = gain
        
        # SDR device
        self.sdr = None
        
        # Data buffers
        self.raw_samples = []
        self.fft_data = None
        self.frequency_axis = None
        
        # Processing parameters
        self.fft_size = 1024
        self.window = 'hann'
        self.overlap = 0.5
        
        # Plot configuration
        self.fig = None
        self.ax = None
        self.line = None
        self.peak_line = None
        
        # GUI controls
        self.freq_slider = None
        self.rate_slider = None
        self.gain_slider = None
        self.pause_button = None
        self.sweep_button = None
        
        # Control flags
        self.running = False
        self.paused = False
        self.sweeping = False
        
        # Statistics
        self.spectrum_count = 0
        self.peak_frequency = 0
        self.peak_power = 0
        
    def initialize_sdr(self):
        """Initialize SDR connection"""
        try:
            logger.info("Initializing SDR...")
            self.sdr = WidebandSDR()
            self.sdr.open()
            
            # Set initial parameters
            self.sdr.set_frequency(self.center_freq)
            self.sdr.set_sample_rate(self.sample_rate)
            self.sdr.set_gain(self.gain)
            
            logger.info(f"SDR initialized: {self.center_freq/1e6:.2f} MHz, "
                       f"{self.sample_rate/1e6:.2f} MSPS, {self.gain} dB")
            
        except SDRConnectionError as e:
            logger.error(f"Failed to connect to SDR: {e}")
            raise
        except WidebandSDRError as e:
            logger.error(f"SDR configuration error: {e}")
            raise
    
    def setup_plot(self):
        """Setup matplotlib plot and GUI controls"""
        # Create figure and axis
        self.fig, self.ax = plt.subplots(figsize=(12, 8))
        plt.subplots_adjust(left=0.1, right=0.95, top=0.95, bottom=0.25)
        
        # Initialize frequency axis
        self.frequency_axis = np.linspace(
            self.center_freq - self.sample_rate/2,
            self.center_freq + self.sample_rate/2,
            self.fft_size
        ) / 1e6  # Convert to MHz
        
        # Initialize spectrum line
        self.line, = self.ax.plot(self.frequency_axis, np.zeros(self.fft_size))
        self.peak_line, = self.ax.plot([], [], 'ro', markersize=8, label='Peak')
        
        # Configure plot
        self.ax.set_xlabel('Frequency (MHz)')
        self.ax.set_ylabel('Power (dBm)')
        self.ax.set_title('Wideband SDR Spectrum Analyzer')
        self.ax.grid(True, alpha=0.3)
        self.ax.legend()
        self.ax.set_xlim(self.frequency_axis[0], self.frequency_axis[-1])
        
        # Setup GUI controls
        self.setup_controls()
        
    def setup_controls(self):
        """Setup interactive controls"""
        # Frequency slider
        ax_freq = plt.axes([0.1, 0.15, 0.8, 0.03])
        self.freq_slider = Slider(
            ax_freq, 'Frequency (MHz)', 
            1, 10000, valinit=self.center_freq/1e6, valfmt='%.1f'
        )
        self.freq_slider.on_changed(self.on_frequency_change)
        
        # Sample rate slider
        ax_rate = plt.axes([0.1, 0.10, 0.8, 0.03])
        self.rate_slider = Slider(
            ax_rate, 'Sample Rate (MSPS)', 
            0.001, 105, valinit=self.sample_rate/1e6, valfmt='%.3f'
        )
        self.rate_slider.on_changed(self.on_sample_rate_change)
        
        # Gain slider
        ax_gain = plt.axes([0.1, 0.05, 0.8, 0.03])
        self.gain_slider = Slider(
            ax_gain, 'Gain (dB)', 
            0, 63, valinit=self.gain, valfmt='%d'
        )
        self.gain_slider.on_changed(self.on_gain_change)
        
        # Control buttons
        ax_pause = plt.axes([0.02, 0.30, 0.08, 0.04])
        self.pause_button = Button(ax_pause, 'Pause')
        self.pause_button.on_clicked(self.on_pause_click)
        
        ax_sweep = plt.axes([0.02, 0.25, 0.08, 0.04])
        self.sweep_button = Button(ax_sweep, 'Sweep')
        self.sweep_button.on_clicked(self.on_sweep_click)
        
        ax_reset = plt.axes([0.02, 0.20, 0.08, 0.04])
        reset_button = Button(ax_reset, 'Reset')
        reset_button.on_clicked(self.on_reset_click)
        
        # Info text
        self.info_text = self.fig.text(
            0.02, 0.35, '', fontsize=10, verticalalignment='top'
        )
    
    def process_samples(self, samples):
        """
        Process raw samples into spectrum
        
        Args:
            samples: Raw ADC samples
        """
        if len(samples) < self.fft_size:
            return
        
        # Take FFT chunk
        chunk = samples[:self.fft_size]
        
        # Apply window function
        if self.window == 'hann':
            window = np.hanning(self.fft_size)
        elif self.window == 'hamming':
            window = np.hamming(self.fft_size)
        elif self.window == 'blackman':
            window = np.blackman(self.fft_size)
        else:
            window = np.ones(self.fft_size)
        
        windowed = chunk * window
        
        # Compute FFT
        fft = np.fft.fft(windowed)
        psd = np.abs(fft)**2 / (np.sum(window**2))
        psd_db = 10 * np.log10(psd + 1e-10)  # Add small value to avoid log(0)
        
        # Frequency axis
        freqs = np.fft.fftfreq(self.fft_size, 1/self.sample_rate)
        freqs = np.fft.fftshift(freqs)
        psd_db = np.fft.fftshift(psd_db)
        
        # Update frequency axis
        self.frequency_axis = (freqs + self.center_freq) / 1e6
        
        # Store spectrum
        self.fft_data = psd_db
        
        # Update peak detection
        self.update_peak_detection()
        
    def update_peak_detection(self):
        """Update peak detection and statistics"""
        if self.fft_data is None or len(self.fft_data) == 0:
            return
        
        # Find peak
        peak_idx = np.argmax(self.fft_data)
        self.peak_frequency = self.frequency_axis[peak_idx]
        self.peak_power = self.fft_data[peak_idx]
        
        # Update statistics
        self.spectrum_count += 1
        
    def update_plot(self, frame):
        """
        Update plot animation
        
        Args:
            frame: Animation frame number
        """
        if not self.running or self.paused:
            return self.line,
        
        # Read samples from SDR
        try:
            samples = self.sdr.read_samples(count=self.fft_size, timeout=0.1)
            if samples is not None and len(samples) >= self.fft_size:
                # Process samples
                self.process_samples(samples)
                
                # Update plot
                if self.fft_data is not None:
                    self.line.set_data(self.frequency_axis, self.fft_data)
                    
                    # Update peak marker
                    if self.peak_power > -100:  # Only show significant peaks
                        self.peak_line.set_data([self.peak_frequency], [self.peak_power])
                        self.peak_line.set_visible(True)
                    else:
                        self.peak_line.set_visible(False)
                    
                    # Auto-scale y-axis if needed
                    if self.spectrum_count % 10 == 0:  # Update every 10 frames
                        y_min = np.min(self.fft_data) - 10
                        y_max = np.max(self.fft_data) + 10
                        self.ax.set_ylim(y_min, y_max)
                
        except WidebandSDRError as e:
            logger.warning(f"SDR read error: {e}")
        
        # Update info text
        stats = self.sdr.get_statistics() if self.sdr else None
        info_text = f"Peak: {self.peak_frequency:.2f} MHz, {self.peak_power:.1f} dBm\n"
        info_text += f"Spectra: {self.spectrum_count}\n"
        if stats:
            info_text += f"Rate: {self.sample_rate/1e6:.3f} MSPS, Gain: {self.gain} dB"
        
        self.info_text.set_text(info_text)
        
        return self.line, self.peak_line
    
    def start_sweep(self):
        """Start frequency sweep"""
        if self.sweeping:
            return
        
        self.sweeping = True
        logger.info("Starting frequency sweep...")
        
        # Sweep parameters
        start_freq = 50e6   # 50 MHz
        end_freq = 2000e6   # 2 GHz
        step_freq = 5e6     # 5 MHz steps
        dwell_time = 0.1    # 100ms dwell
        
        freq = start_freq
        peak_powers = []
        freq_points = []
        
        while freq <= end_freq and self.sweeping:
            try:
                # Tune to frequency
                self.sdr.set_frequency(freq)
                time.sleep(dwell_time)
                
                # Measure peak power
                samples = self.sdr.read_samples(count=self.fft_size*2)
                if samples is not None:
                    self.process_samples(samples)
                    if self.peak_power > -100:
                        peak_powers.append(self.peak_power)
                        freq_points.append(freq / 1e6)
                        logger.info(f"Peak at {freq/1e6:.1f} MHz: {self.peak_power:.1f} dBm")
                
                freq += step_freq
                
            except WidebandSDRError as e:
                logger.error(f"Sweep error at {freq/1e6:.1f} MHz: {e}")
                break
        
        self.sweeping = False
        
        # Plot sweep results
        if freq_points:
            plt.figure(figsize=(10, 6))
            plt.plot(freq_points, peak_powers, 'b-', linewidth=2)
            plt.xlabel('Frequency (MHz)')
            plt.ylabel('Peak Power (dBm)')
            plt.title('Frequency Sweep Results')
            plt.grid(True, alpha=0.3)
            plt.show()
    
    # Event handlers
    def on_frequency_change(self, val):
        """Frequency slider change handler"""
        new_freq = val * 1e6
        if abs(new_freq - self.center_freq) > 1e6:  # Only update if significant change
            try:
                self.sdr.set_frequency(new_freq)
                self.center_freq = new_freq
                logger.info(f"Frequency changed to {self.center_freq/1e6:.2f} MHz")
            except WidebandSDRError as e:
                logger.error(f"Failed to set frequency: {e}")
    
    def on_sample_rate_change(self, val):
        """Sample rate slider change handler"""
        new_rate = val * 1e6
        if abs(new_rate - self.sample_rate) > 1e5:  # Only update if significant change
            try:
                self.sdr.set_sample_rate(new_rate)
                self.sample_rate = new_rate
                logger.info(f"Sample rate changed to {self.sample_rate/1e6:.3f} MSPS")
            except WidebandSDRError as e:
                logger.error(f"Failed to set sample rate: {e}")
    
    def on_gain_change(self, val):
        """Gain slider change handler"""
        new_gain = int(val)
        if abs(new_gain - self.gain) > 1:  # Only update if significant change
            try:
                self.sdr.set_gain(new_gain)
                self.gain = new_gain
                logger.info(f"Gain changed to {self.gain} dB")
            except WidebandSDRError as e:
                logger.error(f"Failed to set gain: {e}")
    
    def on_pause_click(self, event):
        """Pause button click handler"""
        self.paused = not self.paused
        if self.paused:
            logger.info("Paused")
            self.pause_button.label.set_text("Resume")
        else:
            logger.info("Resumed")
            self.pause_button.label.set_text("Pause")
    
    def on_sweep_click(self, event):
        """Sweep button click handler"""
        if not self.sweeping:
            self.start_sweep()
    
    def on_reset_click(self, event):
        """Reset button click handler"""
        self.spectrum_count = 0
        self.peak_frequency = 0
        self.peak_power = -100
        if self.sdr:
            self.sdr.reset_statistics()
        logger.info("Statistics reset")
    
    def run(self, duration=None):
        """
        Run spectrum analyzer
        
        Args:
            duration: Run duration in seconds (None for infinite)
        """
        try:
            # Initialize SDR
            self.initialize_sdr()
            
            # Setup plot
            self.setup_plot()
            
            # Start
            self.running = True
            
            logger.info("Starting spectrum analyzer...")
            logger.info("Controls:")
            logger.info("  - Use sliders to adjust frequency, sample rate, and gain")
            logger.info("  - Click 'Pause' to pause/resume updates")
            logger.info("  - Click 'Sweep' to perform frequency sweep")
            logger.info("  - Click 'Reset' to clear statistics")
            
            # Setup animation
            ani = animation.FuncAnimation(
                self.fig, self.update_plot, 
                interval=100, blit=True
            )
            
            # Start time
            start_time = time.time()
            
            # Run animation
            plt.show()
            
            # Run for specified duration
            if duration:
                time.sleep(duration)
                self.stop()
            
        except KeyboardInterrupt:
            logger.info("Interrupted by user")
            self.stop()
        except Exception as e:
            logger.error(f"Spectrum analyzer error: {e}")
            self.stop()
    
    def stop(self):
        """Stop spectrum analyzer"""
        self.running = False
        
        if self.sdr:
            self.sdr.stop_stream()
            self.sdr.close()
        
        plt.close('all')
        logger.info("Spectrum analyzer stopped")


def main():
    """Main function"""
    parser = argparse.ArgumentParser(description='Wideband SDR Spectrum Analyzer')
    parser.add_argument('--freq', type=float, default=100.0,
                       help='Center frequency in MHz (default: 100)')
    parser.add_argument('--rate', type=float, default=10.0,
                       help='Sample rate in MSPS (default: 10.0)')
    parser.add_argument('--gain', type=int, default=30,
                       help='RF gain in dB (default: 30)')
    parser.add_argument('--duration', type=float,
                       help='Run duration in seconds')
    parser.add_argument('--log-level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR'],
                       default='INFO', help='Logging level')
    
    args = parser.parse_args()
    
    # Configure logging
    logging.getLogger().setLevel(getattr(logging, args.log_level))
    
    try:
        # Create and run spectrum analyzer
        analyzer = SpectrumAnalyzer(
            center_freq=args.freq * 1e6,
            sample_rate=args.rate * 1e6,
            gain=args.gain
        )
        
        analyzer.run(duration=args.duration)
        
    except Exception as e:
        logger.error(f"Failed to start spectrum analyzer: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
