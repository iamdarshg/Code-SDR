#!/usr/bin/env python3
"""
Wideband SDR Frequency Scanner Example
Automated frequency scanning and signal detection using Wideband SDR hardware

Created: November 2025
Version: 1.0.0
License: MIT

Features:
- Automated frequency scanning
- Signal strength measurement
- Peak detection and logging
- Multiple scan modes (continuous, scheduled)
- Custom frequency ranges
- Signal database and reporting
- CSV export functionality
"""

import sys
import time
import numpy as np
import argparse
import logging
from datetime import datetime
from typing import List, Tuple, Dict, Optional
import csv
import json

# Add parent directory to path for imports
sys.path.append('..')
from wideband_sdr import WidebandSDR, WidebandSDRError, SDRConnectionError

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class FrequencyScanner:
    """
    Wideband SDR Frequency Scanner
    
    Automated frequency scanning with signal detection
    """
    
    def __init__(self, sdr: Optional[WidebandSDR] = None):
        """
        Initialize frequency scanner
        
        Args:
            sdr: Optional pre-initialized SDR instance
        """
        self.sdr = sdr if sdr else WidebandSDR()
        self.connected = False
        
        # Scan configuration
        self.start_frequency = 50e6     # 50 MHz
        self.end_frequency = 2000e6     # 2 GHz
        self.step_frequency = 5e6       # 5 MHz steps
        self.dwell_time = 0.1          # 100ms dwell time
        
        # Detection parameters
        self.threshold_dbm = -80        # Detection threshold in dBm
        self.fft_size = 1024
        self.sample_rate = 10e6         # 10 MSPS
        
        # Results storage
        self.scan_results = []          # List of detected signals
        self.frequency_points = []      # Scanned frequencies
        self.power_readings = []        # Power measurements
        
        # Control flags
        self.running = False
        self.paused = False
        
        # Statistics
        self.signals_detected = 0
        self.frequencies_scanned = 0
        self.scan_start_time = None
        
    def connect_sdr(self) -> bool:
        """
        Connect to SDR device
        
        Returns:
            bool: Success status
        """
        try:
            logger.info("Connecting to SDR...")
            self.sdr.open()
            self.connected = True
            
            # Configure SDR for scanning
            self.sdr.set_sample_rate(self.sample_rate)
            self.sdr.set_gain(30)  # Default gain
            
            logger.info("SDR connected and configured")
            return True
            
        except SDRConnectionError as e:
            logger.error(f"Failed to connect to SDR: {e}")
            return False
    
    def disconnect_sdr(self):
        """Disconnect from SDR device"""
        if self.connected:
            try:
                self.sdr.stop_stream()
                self.sdr.close()
                self.connected = False
                logger.info("SDR disconnected")
            except Exception as e:
                logger.error(f"Error disconnecting SDR: {e}")
    
    def set_scan_parameters(self, start_freq: float, end_freq: float, 
                           step_freq: float, dwell_time: float):
        """
        Set scan parameters
        
        Args:
            start_freq: Start frequency in Hz
            end_freq: End frequency in Hz
            step_freq: Step frequency in Hz
            dwell_time: Dwell time in seconds
        """
        self.start_frequency = start_freq
        self.end_frequency = end_freq
        self.step_frequency = step_freq
        self.dwell_time = dwell_time
        
        logger.info(f"Scan parameters set:")
        logger.info(f"  Range: {start_freq/1e6:.1f} - {end_freq/1e6:.1f} MHz")
        logger.info(f"  Step: {step_freq/1e6:.1f} MHz")
        logger.info(f"  Dwell: {dwell_time*1000:.0f} ms")
    
    def measure_power_at_frequency(self, frequency: float) -> Tuple[float, float]:
        """
        Measure power at specific frequency
        
        Args:
            frequency: Frequency in Hz
            
        Returns:
            Tuple[float, float]: (peak_power_dbm, average_power_dbm)
        """
        if not self.connected:
            return -100.0, -100.0
        
        try:
            # Tune to frequency
            self.sdr.set_frequency(frequency)
            
            # Wait for tuning
            time.sleep(0.01)
            
            # Read samples
            samples = self.sdr.read_samples(count=self.fft_size*2, timeout=0.1)
            if samples is None or len(samples) < self.fft_size:
                return -100.0, -100.0
            
            # Compute power spectrum
            window = np.hanning(self.fft_size)
            windowed = samples[:self.fft_size] * window
            
            # FFT and power calculation
            fft = np.fft.fft(windowed)
            psd = np.abs(fft)**2 / (np.sum(window**2))
            psd_db = 10 * np.log10(psd + 1e-10)
            
            # Convert to dBm (approximate)
            peak_power = np.max(psd_db) - 30  # Rough conversion to dBm
            avg_power = np.mean(psd_db) - 30
            
            return peak_power, avg_power
            
        except Exception as e:
            logger.warning(f"Power measurement error at {frequency/1e6:.1f} MHz: {e}")
            return -100.0, -100.0
    
    def scan_frequency(self, frequency: float) -> Dict:
        """
        Scan single frequency
        
        Args:
            frequency: Frequency in Hz
            
        Returns:
            Dict: Scan result
        """
        peak_power, avg_power = self.measure_power_at_frequency(frequency)
        
        # Determine if signal detected
        signal_detected = peak_power > self.threshold_dbm
        
        # Create result dictionary
        result = {
            'frequency': frequency,
            'frequency_mhz': frequency / 1e6,
            'peak_power_dbm': peak_power,
            'average_power_dbm': avg_power,
            'signal_detected': signal_detected,
            'timestamp': datetime.now().isoformat(),
            'threshold_dbm': self.threshold_dbm
        }
        
        # Store in results list
        self.scan_results.append(result)
        self.frequency_points.append(frequency / 1e6)
        self.power_readings.append(peak_power)
        
        # Update statistics
        self.frequencies_scanned += 1
        if signal_detected:
            self.signals_detected += 1
            
        return result
    
    def run_single_scan(self) -> List[Dict]:
        """
        Run single frequency scan
        
        Returns:
            List[Dict]: Scan results
        """
        if not self.connected:
            logger.error("SDR not connected")
            return []
        
        logger.info("Starting frequency scan...")
        logger.info(f"Scanning {self.start_frequency/1e6:.1f} - {self.end_frequency/1e6:.1f} MHz")
        
        self.scan_start_time = datetime.now()
        self.running = True
        
        # Clear previous results
        self.scan_results.clear()
        self.frequency_points.clear()
        self.power_readings.clear()
        
        current_freq = self.start_frequency
        
        try:
            while current_freq <= self.end_frequency and self.running:
                if not self.paused:
                    # Scan frequency
                    result = self.scan_frequency(current_freq)
                    
                    # Log detection
                    if result['signal_detected']:
                        logger.info(f"SIGNAL DETECTED: {result['frequency_mhz']:.2f} MHz, "
                                   f"Peak: {result['peak_power_dbm']:.1f} dBm")
                    
                    # Progress logging
                    if self.frequencies_scanned % 10 == 0:
                        progress = ((current_freq - self.start_frequency) / 
                                  (self.end_frequency - self.start_frequency)) * 100
                        logger.info(f"Scan progress: {progress:.1f}% ({self.frequencies_scanned} frequencies)")
                
                # Move to next frequency
                current_freq += self.step_frequency
                
                # Dwell time
                if not self.paused:
                    time.sleep(self.dwell_time)
                
        except KeyboardInterrupt:
            logger.info("Scan interrupted by user")
        except Exception as e:
            logger.error(f"Scan error: {e}")
        
        self.running = False
        
        # Final statistics
        scan_duration = datetime.now() - self.scan_start_time
        logger.info(f"Scan completed in {scan_duration.total_seconds():.1f} seconds")
        logger.info(f"Total frequencies scanned: {self.frequencies_scanned}")
        logger.info(f"Signals detected: {self.signals_detected}")
        
        return self.scan_results
    
    def run_continuous_scan(self, duration_minutes: float = 60.0):
        """
        Run continuous scanning for specified duration
        
        Args:
            duration_minutes: Scan duration in minutes
        """
        if not self.connected:
            logger.error("SDR not connected")
            return
        
        end_time = time.time() + (duration_minutes * 60)
        scan_count = 0
        
        logger.info(f"Starting continuous scan for {duration_minutes:.1f} minutes")
        
        try:
            while time.time() < end_time and self.running:
                scan_count += 1
                logger.info(f"Running scan #{scan_count}")
                
                # Perform scan
                results = self.run_single_scan()
                
                # Save results after each scan
                if results:
                    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
                    filename = f"scan_results_{timestamp}.csv"
                    self.save_results_csv(results, filename)
                
                # Brief pause between scans
                time.sleep(1.0)
                
        except KeyboardInterrupt:
            logger.info("Continuous scan interrupted by user")
        except Exception as e:
            logger.error(f"Continuous scan error: {e}")
    
    def scan_preset_bands(self) -> List[Dict]:
        """
        Scan preset frequency bands
        
        Returns:
            List[Dict]: Scan results for all bands
        """
        if not self.connected:
            logger.error("SDR not connected")
            return []
        
        # Define preset bands
        preset_bands = [
            ('HF', 3e6, 30e6, 100e3),      # 3-30 MHz, 100 kHz steps
            ('VHF Low', 30e6, 88e6, 1e6),   # 30-88 MHz, 1 MHz steps
            ('VHF High', 88e6, 108e6, 500e3), # 88-108 MHz, 500 kHz steps
            ('UHF', 300e6, 1000e6, 5e6),   # 300-1000 MHz, 5 MHz steps
            ('UHF-L', 1000e6, 2000e6, 10e6), # 1000-2000 MHz, 10 MHz steps
        ]
        
        all_results = []
        
        for band_name, start_freq, end_freq, step_freq in preset_bands:
            logger.info(f"Scanning {band_name} band ({start_freq/1e6:.1f}-{end_freq/1e6:.1f} MHz)")
            
            # Set scan parameters for this band
            self.set_scan_parameters(start_freq, end_freq, step_freq, 0.1)
            
            # Run scan
            results = self.run_single_scan()
            all_results.extend(results)
        
        # Save combined results
        if all_results:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"preset_bands_scan_{timestamp}.csv"
            self.save_results_csv(all_results, filename)
        
        return all_results
    
    def detect_swept_frequency_signals(self, frequency_range: Tuple[float, float], 
                                     duration_seconds: float = 30.0) -> List[Dict]:
        """
        Detect signals using swept frequency technique
        
        Args:
            frequency_range: (start_freq, end_freq) in Hz
            duration_seconds: Sweep duration in seconds
            
        Returns:
            List[Dict]: Detected signals
        """
        if not self.connected:
            logger.error("SDR not connected")
            return []
        
        start_freq, end_freq = frequency_range
        logger.info(f"Swept frequency detection: {start_freq/1e6:.1f}-{end_freq/1e6:.1f} MHz")
        
        # Calculate sweep parameters
        total_bandwidth = end_freq - start_freq
        num_sweeps = int(duration_seconds * 10)  # 10 sweeps per second
        samples_per_sweep = int(self.sample_rate * 0.1)  # 100ms per sweep
        
        detected_signals = []
        
        try:
            self.sdr.start_stream()
            
            for sweep in range(num_sweeps):
                if not self.running:
                    break
                
                # Sweep across frequency range
                current_freq = start_freq
                while current_freq <= end_freq:
                    # Tune and measure
                    self.sdr.set_frequency(current_freq)
                    samples = self.sdr.read_samples(count=1024, timeout=0.1)
                    
                    if samples is not None and len(samples) >= 1024:
                        # Quick power measurement
                        peak_power, _ = self.measure_power_at_frequency(current_freq)
                        
                        # Store if signal detected
                        if peak_power > self.threshold_dbm:
                            signal = {
                                'frequency': current_freq,
                                'frequency_mhz': current_freq / 1e6,
                                'peak_power_dbm': peak_power,
                                'sweep': sweep,
                                'timestamp': datetime.now().isoformat()
                            }
                            detected_signals.append(signal)
                    
                    current_freq += total_bandwidth / 100.0  # 100 steps per sweep
                
                # Brief pause between sweeps
                time.sleep(0.1)
                
        except Exception as e:
            logger.error(f"Swept frequency detection error: {e}")
        finally:
            self.sdr.stop_stream()
        
        # Remove duplicates and sort
        unique_signals = self._remove_duplicate_signals(detected_signals)
        unique_signals.sort(key=lambda x: x['frequency'])
        
        logger.info(f"Detected {len(unique_signals)} unique signals")
        
        return unique_signals
    
    def _remove_duplicate_signals(self, signals: List[Dict], freq_tolerance_hz: float = 1e6) -> List[Dict]:
        """
        Remove duplicate signal detections
        
        Args:
            signals: List of signal detections
            freq_tolerance_hz: Frequency tolerance in Hz
            
        Returns:
            List[Dict]: Unique signals
        """
        if not signals:
            return []
        
        unique_signals = []
        unique_signals.append(signals[0])
        
        for signal in signals[1:]:
            # Check if frequency is already detected
            is_duplicate = False
            for unique_signal in unique_signals:
                if abs(signal['frequency'] - unique_signal['frequency']) < freq_tolerance_hz:
                    # Update if this detection is stronger
                    if signal['peak_power_dbm'] > unique_signal['peak_power_dbm']:
                        unique_signal['peak_power_dbm'] = signal['peak_power_dbm']
                        unique_signal['detections'] = unique_signal.get('detections', 1) + 1
                    else:
                        unique_signal['detections'] = unique_signal.get('detections', 1) + 1
                    is_duplicate = True
                    break
            
            if not is_duplicate:
                signal['detections'] = 1
                unique_signals.append(signal)
        
        return unique_signals
    
    def save_results_csv(self, results: List[Dict], filename: str):
        """
        Save scan results to CSV file
        
        Args:
            results: List of scan results
            filename: Output filename
        """
        if not results:
            logger.warning("No results to save")
            return
        
        try:
            with open(filename, 'w', newline='') as csvfile:
                if results:
                    fieldnames = results[0].keys()
                    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                    writer.writeheader()
                    writer.writerows(results)
            
            logger.info(f"Results saved to {filename}")
            
        except Exception as e:
            logger.error(f"Error saving results: {e}")
    
    def save_results_json(self, results: List[Dict], filename: str):
        """
        Save scan results to JSON file
        
        Args:
            results: List of scan results
            filename: Output filename
        """
        if not results:
            logger.warning("No results to save")
            return
        
        try:
            with open(filename, 'w') as jsonfile:
                json.dump(results, jsonfile, indent=2)
            
            logger.info(f"Results saved to {filename}")
            
        except Exception as e:
            logger.error(f"Error saving results: {e}")
    
    def generate_report(self, results: List[Dict], filename: str = None):
        """
        Generate scan report
        
        Args:
            results: List of scan results
            filename: Optional output filename
        """
        if not results:
            logger.warning("No results for report")
            return
        
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        if filename is None:
            filename = f"scan_report_{timestamp}.txt"
        
        try:
            with open(filename, 'w') as f:
                f.write("WIDEBAND SDR FREQUENCY SCAN REPORT\n")
                f.write("=" * 50 + "\n\n")
                f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
                f.write(f"Total frequencies scanned: {self.frequencies_scanned}\n")
                f.write(f"Signals detected: {self.signals_detected}\n")
                f.write(f"Detection threshold: {self.threshold_dbm} dBm\n\n")
                
                # Signal summary
                detected_signals = [r for r in results if r['signal_detected']]
                if detected_signals:
                    f.write("DETECTED SIGNALS:\n")
                    f.write("-" * 30 + "\n")
                    f.write("Frequency (MHz) | Peak Power (dBm) | Average Power (dBm)\n")
                    f.write("-" * 60 + "\n")
                    
                    for signal in sorted(detected_signals, key=lambda x: x['frequency_mhz']):
                        f.write(f"{signal['frequency_mhz']:13.2f} | "
                               f"{signal['peak_power_dbm']:14.1f} | "
                               f"{signal['average_power_dbm']:16.1f}\n")
                else:
                    f.write("No signals detected above threshold.\n")
            
            logger.info(f"Report generated: {filename}")
            
        except Exception as e:
            logger.error(f"Error generating report: {e}")
    
    def start_scan(self):
        """Start scanning"""
        self.running = True
        self.paused = False
        logger.info("Scanner started")
    
    def stop_scan(self):
        """Stop scanning"""
        self.running = False
        self.paused = False
        logger.info("Scanner stopped")
    
    def pause_scan(self):
        """Pause scanning"""
        self.paused = True
        logger.info("Scanner paused")
    
    def resume_scan(self):
        """Resume scanning"""
        self.paused = False
        logger.info("Scanner resumed")
    
    def get_statistics(self) -> Dict:
        """
        Get scanner statistics
        
        Returns:
            Dict: Scanner statistics
        """
        scan_duration = 0
        if self.scan_start_time:
            scan_duration = (datetime.now() - self.scan_start_time).total_seconds()
        
        return {
            'frequencies_scanned': self.frequencies_scanned,
            'signals_detected': self.signals_detected,
            'scan_duration_seconds': scan_duration,
            'detection_rate': self.signals_detected / max(self.frequencies_scanned, 1),
            'current_frequency': self.frequency_points[-1] if self.frequency_points else 0,
            'running': self.running,
            'paused': self.paused
        }


def main():
    """Main function"""
    parser = argparse.ArgumentParser(description='Wideband SDR Frequency Scanner')
    parser.add_argument('--start-freq', type=float, default=50.0,
                       help='Start frequency in MHz (default: 50)')
    parser.add_argument('--end-freq', type=float, default=2000.0,
                       help='End frequency in MHz (default: 2000)')
    parser.add_argument('--step-freq', type=float, default=5.0,
                       help='Step frequency in MHz (default: 5)')
    parser.add_argument('--dwell-time', type=float, default=0.1,
                       help='Dwell time in seconds (default: 0.1)')
    parser.add_argument('--threshold', type=float, default=-80.0,
                       help='Detection threshold in dBm (default: -80)')
    parser.add_argument('--preset-bands', action='store_true',
                       help='Scan preset frequency bands')
    parser.add_argument('--continuous', type=float, metavar='MINUTES',
                       help='Run continuous scanning for specified minutes')
    parser.add_argument('--output', type=str,
                       help='Output filename prefix')
    parser.add_argument('--log-level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR'],
                       default='INFO', help='Logging level')
    
    args = parser.parse_args()
    
    # Configure logging
    logging.getLogger().setLevel(getattr(logging, args.log_level))
    
    try:
        # Create scanner
        scanner = FrequencyScanner()
        
        # Connect to SDR
        if not scanner.connect_sdr():
            sys.exit(1)
        
        # Configure scanner
        scanner.set_scan_parameters(
            start_freq=args.start_freq * 1e6,
            end_freq=args.end_freq * 1e6,
            step_freq=args.step_freq * 1e6,
            dwell_time=args.dwell_time
        )
        scanner.threshold_dbm = args.threshold
        
        # Determine output filename
        if args.output:
            csv_filename = f"{args.output}.csv"
            json_filename = f"{args.output}.json"
            report_filename = f"{args.output}_report.txt"
        else:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            csv_filename = f"scan_results_{timestamp}.csv"
            json_filename = f"scan_results_{timestamp}.json"
            report_filename = f"scan_report_{timestamp}.txt"
        
        # Start scanning
        scanner.start_scan()
        
        try:
            if args.preset_bands:
                # Scan preset bands
                logger.info("Scanning preset frequency bands...")
                results = scanner.scan_preset_bands()
                
            elif args.continuous:
                # Continuous scanning
                logger.info(f"Running continuous scan for {args.continuous} minutes...")
                scanner.run_continuous_scan(args.continuous)
                results = scanner.scan_results
                
            else:
                # Single scan
                logger.info("Running single frequency scan...")
                results = scanner.run_single_scan()
            
            # Save results
            if results:
                scanner.save_results_csv(results, csv_filename)
                scanner.save_results_json(results, json_filename)
                scanner.generate_report(results, report_filename)
                
                # Print summary
                stats = scanner.get_statistics()
                logger.info("SCAN SUMMARY:")
                logger.info(f"  Frequencies scanned: {stats['frequencies_scanned']}")
                logger.info(f"  Signals detected: {stats['signals_detected']}")
                logger.info(f"  Detection rate: {stats['detection_rate']*100:.1f}%")
                
                detected_signals = [r for r in results if r['signal_detected']]
                if detected_signals:
                    logger.info("Top 5 detected signals:")
                    top_signals = sorted(detected_signals, 
                                       key=lambda x: x['peak_power_dbm'], 
                                       reverse=True)[:5]
                    for signal in top_signals:
                        logger.info(f"  {signal['frequency_mhz']:.2f} MHz: {signal['peak_power_dbm']:.1f} dBm")
        
        finally:
            scanner.stop_scan()
            scanner.disconnect_sdr()
    
    except KeyboardInterrupt:
        logger.info("Scanner interrupted by user")
    except Exception as e:
        logger.error(f"Scanner failed: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
