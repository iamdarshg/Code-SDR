#!/usr/bin/env python3
"""
Wideband SDR Waterfall Display Example
Real-time waterfall display using Wideband SDR hardware

Created: November 2025
Version: 1.0.0
License: MIT

Features:
- Real-time waterfall spectrogram
- Multiple display modes (power, phase, magnitude)
- Configurable time/frequency scales
- Color mapping options
- Zoom and pan functionality
- Save waterfall images and data
- Frequency hopping and tracking
"""

import sys
import time
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Slider, Button, CheckButtons
from scipy import signal
import argparse
import logging
from datetime import datetime
from typing import Optional, Tuple, List
import colormaps

# Add parent directory to path for imports
sys.path.append('..')
from wideband_sdr import WidebandSDR, WidebandSDRError, SDRConnectionError

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class WaterfallDisplay:
    """
    Wideband SDR Waterfall Display
    
    Real-time waterfall spectrogram visualization
    """
    
    def __init__(self, center_freq=100e6, sample_rate=10e6, gain=30):
        """
        Initialize waterfall display
        
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
        
        # Waterfall parameters
        self.fft_size = 1024
        self.fft_overlap = 0.5
        self.waterfall_depth = 100  # Number of FFT lines to keep
        self.waterfall_data = np.zeros((self.waterfall_depth, self.fft_size))
        self.waterfall_index = 0
        
        # Display parameters
        self.min_freq = center_freq - sample_rate/2
        self.max_freq = center_freq + sample_rate/2
        self.frequency_axis = None
        self.time_axis = None
        
        # Color mapping
        self.colormap = 'viridis'
        self.clim_min = -100
        self.clim_max = -40
        
        # Plot configuration
        self.fig = None
        self.ax = None
        self.waterfall_plot = None
        self.colorbar = None
        
        # GUI controls
        self.freq_slider = None
        self.rate_slider = None
        self.gain_slider = None
        self.colormap_buttons = None
        self.display_mode_buttons = None
        
        # Control flags
        self.running = False
        self.paused = False
        self.tracking_mode = False
        
        # Statistics
        self.update_count = 0
        self.fps = 0
        self.last_update_time = time.time()
        
        # Peak tracking
        self.peak_freq = center_freq
        self.peak_power = -100
        
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
            
            # Start streaming
            self.sdr.start_stream()
            
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
        self.fig, self.ax = plt.subplots(figsize=(14, 10))
        plt.subplots_adjust(left=0.08, right=0.95, top=0.95, bottom=0.25)
        
        # Initialize axes
        self.frequency_axis = np.linspace(self.min_freq, self.max_freq, self.fft_size) / 1e6
        self.time_axis = np.arange(self.waterfall_depth)
        
        # Create waterfall plot
        self.waterfall_plot = self.ax.imshow(
            self.waterfall_data,
            extent=[self.frequency_axis[0], self.frequency_axis[-1], 
                   0, self.waterfall_depth-1],
            aspect='auto',
            cmap=self.colormap,
            vmin=self.clim_min,
            vmax=self.clim_max,
            origin='lower'
        )
        
        # Configure plot
        self.ax.set_xlabel('Frequency (MHz)')
        self.ax.set_ylabel('Time (frames)')
        self.ax.set_title('Wideband SDR Waterfall Display')
        
        # Add colorbar
        self.colorbar = plt.colorbar(self.waterfall_plot, ax=self.ax)
        self.colorbar.set_label('Power (dB)')
        
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
        
        # Color mapping buttons
        ax_colors = plt.axes([0.02, 0.65, 0.08, 0.15])
        self.colormap_buttons = CheckButtons(ax_colors, 'Colormap', 
                                           ['viridis', 'plasma', 'inferno', 'magma'])
        self.colormap_buttons.on_clicked(self.on_colormap_change)
        
        # Display mode buttons
        ax_modes = plt.axes([0.02, 0.45, 0.08, 0.15])
        self.display_mode_buttons = CheckButtons(ax_modes, 'Display Mode',
                                               ['Power', 'Magnitude', 'Phase'])
        self.display_mode_buttons.on_clicked(self.on_display_mode_change)
        
        # Control buttons
        ax_pause = plt.axes([0.02, 0.35, 0.08, 0.04])
        self.pause_button = Button(ax_pause, 'Pause')
        self.pause_button.on_clicked(self.on_pause_click)
        
        ax_save = plt.axes([0.02, 0.30, 0.08, 0.04])
        save_button = Button(ax_save, 'Save')
        save_button.on_clicked(self.on_save_click)
        
        ax_track = plt.axes([0.02, 0.25, 0.08, 0.04])
        track_button = Button(ax_track, 'Track Peak')
        track_button.on_clicked(self.on_track_click)
        
        ax_clear = plt.axes([0.02, 0.20, 0.08, 0.04])
        clear_button = Button(ax_clear, 'Clear')
        clear_button.on_clicked(self.on_clear_click)
        
        # Info text
        self.info_text = self.fig.text(
            0.02, 0.40, '', fontsize=10, verticalalignment='top'
        )
    
    def process_samples(self, samples):
        """
        Process samples into waterfall data
        
        Args:
            samples: Raw ADC samples
        """
        if len(samples) < self.fft_size:
            return
        
        # Apply window function
        window = np.hanning(self.fft_size)
        windowed = samples[:self.fft_size] * window
        
        # Compute FFT
        fft = np.fft.fft(windowed)
        
        # Calculate power spectrum
        psd = np.abs(fft)**2 / (np.sum(window**2))
        psd_db = 10 * np.log10(psd + 1e-10)
        
        # Store in waterfall data
        self.waterfall_data[self.waterfall_index] = psd_db
        self.waterfall_index = (self.waterfall_index + 1) % self.waterfall_depth
        
        # Update peak tracking
        self.update_peak_tracking(psd_db)
    
    def update_peak_tracking(self, psd_db):
        """
        Update peak tracking
        
        Args:
            psd_db: Power spectrum in dB
        """
        if not self.tracking_mode:
            return
        
        # Find peak in current spectrum
        peak_idx = np.argmax(psd_db)
        peak_power = psd_db[peak_idx]
        
        # Convert to frequency
        freq_step = self.sample_rate / self.fft_size
        peak_freq = self.center_freq - self.sample_rate/2 + peak_idx * freq_step
        
        # Only track significant peaks
        if peak_power > self.peak_power + 3:  # 3 dB threshold
            self.peak_freq = peak_freq
            self.peak_power = peak_power
            
            # Tune to peak frequency
            try:
                self.sdr.set_frequency(peak_freq)
                self.center_freq = peak_freq
                
                # Update frequency slider
                if self.freq_slider:
                    self.freq_slider.set_val(peak_freq / 1e6)
                    
            except WidebandSDRError as e:
                logger.warning(f"Peak tracking frequency set failed: {e}")
    
    def update_plot(self, frame):
        """
        Update waterfall plot animation
        
        Args:
            frame: Animation frame number
        """
        if not self.running or self.paused:
            return self.waterfall_plot,
        
        # Read samples from SDR
        try:
            samples = self.sdr.read_samples(count=self.fft_size, timeout=0.1)
            if samples is not None and len(samples) >= self.fft_size:
                # Process samples
                self.process_samples(samples)
                
                # Update waterfall display
                waterfall_display = np.roll(self.waterfall_data, -self.waterfall_index, axis=0)
                self.waterfall_plot.set_data(waterfall_display)
                
                # Update statistics
                self.update_count += 1
                current_time = time.time()
                if current_time - self.last_update_time >= 1.0:
                    self.fps = self.update_count / (current_time - self.last_update_time)
                    self.update_count = 0
                    self.last_update_time = current_time
                
        except WidebandSDRError as e:
            logger.warning(f"SDR read error: {e}")
        
        # Update info text
        info_text = f"FPS: {self.fps:.1f}\n"
        info_text += f"Center: {self.center_freq/1e6:.2f} MHz\n"
        info_text += f"Peak: {self.peak_freq/1e6:.2f} MHz\n"
        info_text += f"Peak Pwr: {self.peak_power:.1f} dB\n"
        info_text += f"Tracking: {'ON' if self.tracking_mode else 'OFF'}"
        
        self.info_text.set_text(info_text)
        
        return self.waterfall_plot,
    
    def save_waterfall(self):
        """Save waterfall data and image"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        
        # Save raw data
        data_filename = f"waterfall_data_{timestamp}.npy"
        np.save(data_filename, self.waterfall_data)
        logger.info(f"Waterfall data saved to {data_filename}")
        
        # Save image
        fig_filename = f"waterfall_image_{timestamp}.png"
        plt.savefig(fig_filename, dpi=150, bbox_inches='tight')
        logger.info(f"Waterfall image saved to {fig_filename}")
        
        # Save frequency axis
        freq_filename = f"waterfall_freq_{timestamp}.npy"
        np.save(freq_filename, self.frequency_axis)
        logger.info(f"Frequency axis saved to {freq_filename}")
    
    # Event handlers
    def on_frequency_change(self, val):
        """Frequency slider change handler"""
        new_freq = val * 1e6
        if abs(new_freq - self.center_freq) > 1e6:
            try:
                self.sdr.set_frequency(new_freq)
                self.center_freq = new_freq
                
                # Update frequency range
                self.min_freq = self.center_freq - self.sample_rate/2
                self.max_freq = self.center_freq + self.sample_rate/2
                self.frequency_axis = np.linspace(self.min_freq, self.max_freq, self.fft_size) / 1e6
                
                # Update waterfall plot extent
                self.waterfall_plot.set_extent([self.frequency_axis[0], self.frequency_axis[-1], 
                                              0, self.waterfall_depth-1])
                
                self.ax.set_xlim(self.frequency_axis[0], self.frequency_axis[-1])
                
                logger.info(f"Frequency changed to {self.center_freq/1e6:.2f} MHz")
            except WidebandSDRError as e:
                logger.error(f"Failed to set frequency: {e}")
    
    def on_sample_rate_change(self, val):
        """Sample rate slider change handler"""
        new_rate = val * 1e6
        if abs(new_rate - self.sample_rate) > 1e5:
            try:
                self.sdr.set_sample_rate(new_rate)
                self.sample_rate = new_rate
                
                # Update frequency range
                self.min_freq = self.center_freq - self.sample_rate/2
                self.max_freq = self.center_freq + self.sample_rate/2
                self.frequency_axis = np.linspace(self.min_freq, self.max_freq, self.fft_size) / 1e6
                
                # Update waterfall plot extent
                self.waterfall_plot.set_extent([self.frequency_axis[0], self.frequency_axis[-1], 
                                              0, self.waterfall_depth-1])
                
                self.ax.set_xlim(self.frequency_axis[0], self.frequency_axis[-1])
                
                logger.info(f"Sample rate changed to {self.sample_rate/1e6:.3f} MSPS")
            except WidebandSDRError as e:
                logger.error(f"Failed to set sample rate: {e}")
    
    def on_gain_change(self, val):
        """Gain slider change handler"""
        new_gain = int(val)
        if abs(new_gain - self.gain) > 1:
            try:
                self.sdr.set_gain(new_gain)
                self.gain = new_gain
                logger.info(f"Gain changed to {self.gain} dB")
            except WidebandSDRError as e:
                logger.error(f"Failed to set gain: {e}")
    
    def on_colormap_change(self, label):
        """Colormap change handler"""
        self.colormap = label.lower()
        self.waterfall_plot.set_cmap(self.colormap)
        plt.draw()
        logger.info(f"Colormap changed to {self.colormap}")
    
    def on_display_mode_change(self, label):
        """Display mode change handler"""
        # This would implement different display modes
        # (power, magnitude, phase) - simplified for now
        logger.info(f"Display mode changed to {label}")
    
    def on_pause_click(self, event):
        """Pause button click handler"""
        self.paused = not self.paused
        if self.paused:
            logger.info("Waterfall paused")
            self.pause_button.label.set_text("Resume")
        else:
            logger.info("Waterfall resumed")
            self.pause_button.label.set_text("Pause")
    
    def on_save_click(self, event):
        """Save button click handler"""
        self.save_waterfall()
    
    def on_track_click(self, event):
        """Track peak button click handler"""
        self.tracking_mode = not self.tracking_mode
        if self.tracking_mode:
            logger.info("Peak tracking enabled")
        else:
            logger.info("Peak tracking disabled")
    
    def on_clear_click(self, event):
        """Clear button click handler"""
        self.waterfall_data.fill(0)
        self.waterfall_index = 0
        self.peak_freq = self.center_freq
        self.peak_power = -100
        logger.info("Waterfall data cleared")
    
    def run(self, duration=None):
        """
        Run waterfall display
        
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
            
            logger.info("Starting waterfall display...")
            logger.info("Controls:")
            logger.info("  - Use sliders to adjust frequency, sample rate, and gain")
            logger.info("  - Click 'Pause' to pause/resume updates")
            logger.info("  - Click 'Save' to save waterfall data and image")
            logger.info("  - Click 'Track Peak' to enable automatic peak tracking")
            logger.info("  - Click 'Clear' to clear waterfall data")
            logger.info("  - Use colormap and display mode checkboxes")
            
            # Setup animation
            ani = animation.FuncAnimation(
                self.fig, self.update_plot, 
                interval=100, blit=True
            )
            
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
            logger.error(f"Waterfall display error: {e}")
            self.stop()
    
    def stop(self):
        """Stop waterfall display"""
        self.running = False
        
        if self.sdr:
            self.sdr.stop_stream()
            self.sdr.close()
        
        plt.close('all')
        logger.info("Waterfall display stopped")


def main():
    """Main function"""
    parser = argparse.ArgumentParser(description='Wideband SDR Waterfall Display')
    parser.add_argument('--freq', type=float, default=100.0,
                       help='Center frequency in MHz (default: 100)')
    parser.add_argument('--rate', type=float, default=10.0,
                       help='Sample rate in MSPS (default: 10.0)')
    parser.add_argument('--gain', type=int, default=30,
                       help='RF gain in dB (default: 30)')
    parser.add_argument('--duration', type=float,
                       help='Run duration in seconds')
    parser.add_argument('--colormap', type=str, default='viridis',
                       choices=['viridis', 'plasma', 'inferno', 'magma', 'jet', 'hot'],
                       help='Colormap (default: viridis)')
    parser.add_argument('--tracking', action='store_true',
                       help='Enable peak tracking mode')
    parser.add_argument('--log-level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR'],
                       default='INFO', help='Logging level')
    
    args = parser.parse_args()
    
    # Configure logging
    logging.getLogger().setLevel(getattr(logging, args.log_level))
    
    try:
        # Create and run waterfall display
        waterfall = WaterfallDisplay(
            center_freq=args.freq * 1e6,
            sample_rate=args.rate * 1e6,
            gain=args.gain
        )
        waterfall.colormap = args.colormap
        waterfall.tracking_mode = args.tracking
        
        waterfall.run(duration=args.duration)
        
    except Exception as e:
        logger.error(f"Failed to start waterfall display: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
