"""
test_fft_ddc_performance.py - Performance Benchmarking Suite for Wideband SDR

Comprehensive performance testing for FFT acceleration and digital downconversion
Tests throughput, latency, accuracy, and resource utilization

Author: Wideband SDR Development Team
Date: 2025-11-19
"""

import numpy as np
import time
import sys
import os
import json
import logging
from typing import Dict, List, Tuple, Any
import matplotlib.pyplot as plt
from scipy import signal
import argparse

# Add the software directory to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from wideband_sdr_software.digital_downconverter import DigitalDownconverter

class PerformanceBenchmark:
    """
    Comprehensive performance benchmarking suite for FFT and DDC operations
    """
    
    def __init__(self):
        """Initialize benchmark suite"""
        self.results = {}
        self.test_signals = {}
        
    def generate_test_signals(self) -> Dict[str, np.ndarray]:
        """
        Generate various test signals for benchmarking
        
        Returns:
            Dictionary of test signals
        """
        logging.info("Generating test signals...")
        
        # Signal parameters
        sample_rate = 100e6  # 100 MSPS
        duration = 0.1  # 100ms of data
        t = np.arange(0, duration, 1/sample_rate)
        
        test_signals = {}
        
        # 1. Single tone signal
        freq_tone = 95.5e6  # FM radio frequency
        test_signals['single_tone'] = np.exp(1j * 2 * np.pi * freq_tone * t)
        
        # 2. Multi-tone signal (5 tones)
        frequencies = [95.0e6, 95.5e6, 96.0e6, 96.5e6, 97.0e6]
        multi_tone = np.zeros(len(t), dtype=complex)
        for freq in frequencies:
            multi_tone += np.exp(1j * 2 * np.pi * freq * t)
        test_signals['multi_tone'] = multi_tone / len(frequencies)
        
        # 3. Wideband noise
        noise_power = 0.01
        test_signals['wideband_noise'] = np.sqrt(noise_power/2) * (
            np.random.randn(len(t)) + 1j * np.random.randn(len(t))
        )
        
        # 4. AM signal (carrier + modulation)
        carrier_freq = 1e6
        mod_freq = 1e3
        am_signal = (1 + 0.5 * np.sin(2 * np.pi * mod_freq * t)) * np.exp(1j * 2 * np.pi * carrier_freq * t)
        test_signals['am_signal'] = am_signal
        
        # 5. FM signal
        mod_freq = 500
        freq_deviation = 50e3
        phase = 2 * np.pi * carrier_freq * t + freq_deviation/mod_freq * np.sin(2 * np.pi * mod_freq * t)
        test_signals['fm_signal'] = np.exp(1j * phase)
        
        # 6. Chirp signal (linear frequency sweep)
        f_start = 1e6
        f_end = 5e6
        chirp_freq = f_start + (f_end - f_start) * t / duration
        test_signals['chirp'] = np.exp(1j * 2 * np.pi * chirp_freq * t)
        
        print(f"Generated {len(test_signals)} test signals:")
        for name, sig in test_signals.items():
            print(f"  {name}: {len(sig)} samples ({len(sig)/sample_rate*1000:.1f} ms)")
        
        self.test_signals = test_signals
        return test_signals
    
    def benchmark_fft_performance(self, signal_type: str = 'single_tone', 
                                 fft_sizes: List[int] = None) -> Dict[str, Any]:
        """
        Benchmark FFT computation performance
        
        Args:
            signal_type: Type of test signal to use
            fft_sizes: List of FFT sizes to test
            
        Returns:
            Dictionary with benchmark results
        """
        if fft_sizes is None:
            fft_sizes = [256, 512, 1024, 2048, 4096, 8192]
        
        logging.info(f"Starting FFT Performance Benchmark for signal: {signal_type}")
        logging.info(f"FFT sizes to be tested: {fft_sizes}")
        
        results = {
            'signal_type': signal_type,
            'fft_sizes': fft_sizes,
            'computation_times': [],
            'throughput_msps': [],
            'memory_usage': [],
            'accuracy_metrics': []
        }
        
        # Use test signal
        test_signal = self.test_signals[signal_type]
        
        for fft_size in fft_sizes:
            print(f"\nTesting FFT size: {fft_size}")
            
            # Prepare data
            num_chunks = len(test_signal) // fft_size
            chunks = np.array_split(test_signal[:num_chunks * fft_size], num_chunks)
            
            # Warm-up run
            for chunk in chunks[:1]:
                fft_result = np.fft.fft(chunk, n=fft_size)
            
            # Benchmark runs
            num_runs = 10
            times = []
            
            for run in range(num_runs):
                start_time = time.perf_counter()
                
                for chunk in chunks:
                    fft_result = np.fft.fft(chunk, n=fft_size)
                
                end_time = time.perf_counter()
                times.append(end_time - start_time)
            
            # Calculate statistics
            avg_time = np.mean(times)
            std_time = np.std(times)
            total_samples = len(chunks) * fft_size
            throughput_msps = total_samples / avg_time / 1e6
            
            print(f"  Average time: {avg_time*1000:.2f} ms")
            print(f"  Std deviation: {std_time*1000:.2f} ms")
            print(f"  Throughput: {throughput_msps:.1f} MSPS")
            
            results['computation_times'].append(avg_time)
            results['throughput_msps'].append(throughput_msps)
            
            # Memory usage estimation
            memory_bytes = fft_size * 16  # complex64 = 8 bytes per component, 2 components
            results['memory_usage'].append(memory_bytes)
            
            # Accuracy test (verify energy conservation)
            if len(chunks) > 0:
                test_fft = np.fft.fft(chunks[0], n=fft_size)
                energy_input = np.sum(np.abs(chunks[0])**2)
                energy_output = np.sum(np.abs(test_fft)**2) / fft_size
                energy_error = abs(energy_output - energy_input) / energy_input
                results['accuracy_metrics'].append(energy_error)
                
                print(f"  Energy conservation error: {energy_error*100:.4f}%")
        
        self.results['fft_performance'] = results
        return results
    
    def benchmark_ddc_performance(self, test_configurations: List[Dict] = None) -> Dict[str, Any]:
        """
        Benchmark Digital Downconverter performance
        
        Args:
            test_configurations: List of DDC configurations to test
            
        Returns:
            Dictionary with benchmark results
        """
        if test_configurations is None:
            test_configurations = [
                {'sample_rate': 50e6, 'center_freq': 95.5e6, 'bandwidth': 200e3},
                {'sample_rate': 100e6, 'center_freq': 95.5e6, 'bandwidth': 200e3},
                {'sample_rate': 100e6, 'center_freq': 1e9, 'bandwidth': 1e6},
                {'sample_rate': 50e6, 'center_freq': 2.4e9, 'bandwidth': 5e6},
                {'sample_rate': 20e6, 'center_freq': 433e6, 'bandwidth': 100e3},
            ]
        
        logging.info("Starting DDC Performance Benchmark")
        logging.info(f"Testing {len(test_configurations)} configurations")
        
        results = {
            'configurations': test_configurations,
            'throughput_msps': [],
            'processing_times': [],
            'decimation_factors': [],
            'output_bw_utilization': [],
            'filter_performance': []
        }
        
        for i, config in enumerate(test_configurations):
            print(f"\nConfiguration {i+1}:")
            print(f"  Sample rate: {config['sample_rate']/1e6:.1f} MSPS")
            print(f"  Center frequency: {config['center_freq']/1e6:.1f} MHz")
            print(f"  Bandwidth: {config['bandwidth']/1e6:.1f} MHz")
            
            # Create DDC
            ddc = DigitalDownconverter(
                config['sample_rate'],
                config['center_freq'], 
                config['bandwidth']
            )
            
            # Generate test signal
            signal_length = int(config['sample_rate'] * 0.01)  # 10ms of data
            test_signal = np.random.randn(signal_length) + 1j * np.random.randn(signal_length)
            
            # Warm-up
            _ = ddc.apply_ddc(test_signal[:1024])
            
            # Benchmark runs
            num_runs = 5
            times = []
            
            for run in range(num_runs):
                start_time = time.perf_counter()
                output = ddc.apply_ddc(test_signal)
                end_time = time.perf_counter()
                times.append(end_time - start_time)
            
            # Calculate statistics
            avg_time = np.mean(times)
            throughput_msps = signal_length / avg_time / 1e6
            
            print(f"  Average processing time: {avg_time*1000:.2f} ms")
            print(f"  Throughput: {throughput_msps:.1f} MSPS")
            print(f"  Output samples: {len(output)}")
            print(f"  Compression ratio: {signal_length/len(output):.1f}:1")
            
            results['throughput_msps'].append(throughput_msps)
            results['processing_times'].append(avg_time)
            results['decimation_factors'].append(ddc.decimation_factor)
            
            # Filter performance metrics
            freqs, mag_response = ddc.get_frequency_response(1024)
            bandwidth_utilization = np.sum(mag_response > -3) / len(mag_response)
            results['output_bw_utilization'].append(bandwidth_utilization)
            
            # Get performance metrics from DDC
            ddc_metrics = ddc.get_performance_metrics()
            results['filter_performance'].append(ddc_metrics)
        
        self.results['ddc_performance'] = results
        return results
    
    def benchmark_combined_pipeline(self) -> Dict[str, Any]:
        """
        Benchmark combined FFT + DDC pipeline
        
        Returns:
            Dictionary with benchmark results
        """
        logging.info("Starting Combined Pipeline Benchmark")
        
        # Configuration
        sample_rate = 100e6
        center_freq = 95.5e6
        bandwidth = 200e3
        fft_size = 2048
        
        results = {
            'configuration': {
                'sample_rate': sample_rate,
                'center_freq': center_freq,
                'bandwidth': bandwidth,
                'fft_size': fft_size
            },
            'processing_times': [],
            'memory_efficiency': [],
            'real_time_factor': []
        }
        
        # Create components
        ddc = DigitalDownconverter(sample_rate, center_freq, bandwidth)
        
        # Generate test data (1 second worth)
        signal_length = int(sample_rate * 1.0)  # 1 second
        test_signal = np.random.randn(signal_length) + 1j * np.random.randn(signal_length)
        
        # Process in chunks
        chunk_size = 8192
        chunks = [test_signal[i:i+chunk_size] for i in range(0, len(test_signal), chunk_size)]
        
        # Benchmark pipeline
        num_runs = 3
        total_times = []
        
        for run in range(num_runs):
            print(f"Run {run+1}/{num_runs}")
            
            start_time = time.perf_counter()
            processed_samples = 0
            
            for chunk in chunks:
                # Apply DDC
                downconverted = ddc.apply_ddc(chunk)
                
                # Apply FFT
                fft_result = np.fft.fft(downconverted, n=fft_size)
                
                processed_samples += len(chunk)
            
            end_time = time.perf_counter()
            total_time = end_time - start_time
            total_times.append(total_time)
            
            # Calculate real-time factor
            real_time_factor = total_time / 1.0  # 1 second of data
            results['real_time_factor'].append(real_time_factor)
            
            print(f"  Processing time: {total_time:.2f} s")
            print(f"  Real-time factor: {real_time_factor:.2f}x")
        
        # Overall statistics
        avg_time = np.mean(total_times)
        std_time = np.std(total_times)
        avg_throughput = signal_length / avg_time / 1e6
        
        results['processing_times'] = total_times
        results['average_throughput_msps'] = avg_throughput
        results['std_processing_time'] = std_time
        
        print(f"\nPipeline Results:")
        print(f"  Average processing time: {avg_time:.2f} ± {std_time:.2f} s")
        print(f"  Average throughput: {avg_throughput:.1f} MSPS")
        print(f"  Input data: {signal_length} samples")
        print(f"  Processing efficiency: {'Real-time capable' if avg_time < 1.0 else 'Requires optimization'}")
        
        self.results['combined_pipeline'] = results
        return results
    
    def generate_performance_report(self, output_file: str = 'performance_report.json') -> None:
        """
        Generate comprehensive performance report
        
        Args:
            output_file: Output JSON file path
        """
        logging.info(f"Generating performance report: {output_file}")
        
        # Compile all results
        report = {
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'system_info': {
                'numpy_version': np.__version__,
                'python_version': sys.version,
                'platform': sys.platform
            },
            'benchmarks': self.results
        }
        
        # Save to JSON
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        print(f"Performance report saved to: {output_file}")
        
        # Print summary
        print(f"\n=== Performance Summary ===")
        
        if 'fft_performance' in self.results:
            fft_results = self.results['fft_performance']
            max_throughput = max(fft_results['throughput_msps'])
            print(f"FFT Performance:")
            print(f"  Maximum throughput: {max_throughput:.1f} MSPS")
            print(f"  Best FFT size: {fft_results['fft_sizes'][np.argmax(fft_results['throughput_msps'])]}")
        
        if 'ddc_performance' in self.results:
            ddc_results = self.results['ddc_performance']
            max_throughput = max(ddc_results['throughput_msps'])
            avg_compression = np.mean([config['sample_rate'] / config['bandwidth'] for config in ddc_results['configurations']])
            print(f"DDC Performance:")
            print(f"  Maximum throughput: {max_throughput:.1f} MSPS")
            print(f"  Average compression ratio: {avg_compression:.0f}:1")
        
        if 'combined_pipeline' in self.results:
            pipeline_results = self.results['combined_pipeline']
            avg_factor = np.mean(pipeline_results['real_time_factor'])
            print(f"Combined Pipeline:")
            print(f"  Average real-time factor: {avg_factor:.2f}x")
            print(f"  Status: {'Real-time capable' if avg_factor < 1.0 else 'Requires optimization'}")
    
    def create_performance_plots(self, output_dir: str = 'performance_plots') -> None:
        """
        Create performance visualization plots
        
        Args:
            output_dir: Output directory for plots
        """
        logging.info(f"Creating performance plots in directory: {output_dir}")
        
        os.makedirs(output_dir, exist_ok=True)
        
        # FFT Performance Plot
        if 'fft_performance' in self.results:
            self._plot_fft_performance(output_dir)
        
        # DDC Performance Plot
        if 'ddc_performance' in self.results:
            self._plot_ddc_performance(output_dir)
        
        print(f"Plots saved to: {output_dir}")
    
    def _plot_fft_performance(self, output_dir: str) -> None:
        """Plot FFT performance results"""
        fft_results = self.results['fft_performance']
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(14, 10))
        fig.suptitle('FFT Performance Analysis', fontsize=16)
        
        # Throughput vs FFT Size
        ax1.plot(fft_results['fft_sizes'], fft_results['throughput_msps'], 'bo-')
        ax1.set_xlabel('FFT Size')
        ax1.set_ylabel('Throughput (MSPS)')
        ax1.set_title('FFT Throughput vs Size')
        ax1.grid(True)
        
        # Computation Time vs FFT Size
        ax2.plot(fft_results['fft_sizes'], np.array(fft_results['computation_times'])*1000, 'ro-')
        ax2.set_xlabel('FFT Size')
        ax2.set_ylabel('Computation Time (ms)')
        ax2.set_title('FFT Computation Time vs Size')
        ax2.grid(True)
        
        # Memory Usage
        memory_mb = [mem / (1024*1024) for mem in fft_results['memory_usage']]
        ax3.bar(range(len(fft_results['fft_sizes'])), memory_mb, color='green')
        ax3.set_xlabel('FFT Size Index')
        ax3.set_ylabel('Memory Usage (MB)')
        ax3.set_title('FFT Memory Usage')
        ax3.grid(True)
        
        # Throughput vs. Computation Time
        ax4.plot(fft_results['throughput_msps'], np.array(fft_results['computation_times'])*1000, 'go-')
        ax4.set_xlabel('Throughput (MSPS)')
        ax4.set_ylabel('Computation Time (ms)')
        ax4.set_title('Throughput vs. Computation Time')
        ax4.grid(True)
        
        plt.tight_layout(rect=[0, 0.03, 1, 0.95])
        plt.savefig(os.path.join(output_dir, 'fft_performance.png'), dpi=150, bbox_inches='tight')
        plt.close()
    
    def _plot_ddc_performance(self, output_dir: str) -> None:
        """Plot DDC performance results"""
        ddc_results = self.results['ddc_performance']
        
        fig = plt.figure(figsize=(14, 15))
        fig.suptitle('DDC Performance Analysis', fontsize=16)
        gs = fig.add_gridspec(3, 2)

        ax1 = fig.add_subplot(gs[0, 0])
        ax2 = fig.add_subplot(gs[0, 1])
        ax3 = fig.add_subplot(gs[1, 0])
        ax4 = fig.add_subplot(gs[1, 1])
        ax5 = fig.add_subplot(gs[2, :])
        
        # Throughput vs Sample Rate
        sample_rates = [config['sample_rate']/1e6 for config in ddc_results['configurations']]
        ax1.scatter(sample_rates, ddc_results['throughput_msps'], c='blue', s=100)
        ax1.set_xlabel('Sample Rate (MSPS)')
        ax1.set_ylabel('Throughput (MSPS)')
        ax1.set_title('DDC Throughput vs Sample Rate')
        ax1.grid(True)
        
        # Processing Time Distribution
        processing_times_ms = [t*1000 for t in ddc_results['processing_times']]
        ax2.bar(range(len(ddc_results['configurations'])), processing_times_ms, color='red')
        ax2.set_xlabel('Configuration')
        ax2.set_ylabel('Processing Time (ms)')
        ax2.set_title('DDC Processing Time by Configuration')
        ax2.grid(True)
        
        # Decimation Factor Distribution
        ax3.bar(range(len(ddc_results['configurations'])), ddc_results['decimation_factors'], color='green')
        ax3.set_xlabel('Configuration')
        ax3.set_ylabel('Decimation Factor')
        ax3.set_title('Decimation Factors')
        ax3.grid(True)
        
        # Bandwidth Utilization
        ax4.bar(range(len(ddc_results['configurations'])), ddc_results['output_bw_utilization'], color='orange')
        ax4.set_xlabel('Configuration')
        ax4.set_ylabel('Bandwidth Utilization')
        ax4.set_title('Filter Bandwidth Utilization')
        ax4.grid(True)
        
        # Filter Performance
        for i, config in enumerate(ddc_results['configurations']):
            ddc = DigitalDownconverter(config['sample_rate'], config['center_freq'], config['bandwidth'])
            freqs, mag_response = ddc.get_frequency_response(1024)
            ax5.plot(freqs / 1e6, mag_response, label=f"Config {i+1}")

        ax5.set_xlabel('Frequency (MHz)')
        ax5.set_ylabel('Magnitude (dB)')
        ax5.set_title('Filter Frequency Response')
        ax5.legend()
        ax5.grid(True)

        plt.tight_layout(rect=[0, 0.03, 1, 0.95])
        plt.savefig(os.path.join(output_dir, 'ddc_performance.png'), dpi=150, bbox_inches='tight')
        plt.close()


def run_comprehensive_benchmark(output_file: str, output_dir: str):
    """Run comprehensive performance benchmark suite"""
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
    logging.info("Starting Wideband SDR Performance Benchmark Suite")
    
    # Initialize benchmark
    benchmark = PerformanceBenchmark()
    
    # Generate test signals
    benchmark.generate_test_signals()
    
    # Run FFT performance tests
    benchmark.benchmark_fft_performance('single_tone')
    benchmark.benchmark_fft_performance('multi_tone')
    benchmark.benchmark_fft_performance('wideband_noise')
    
    # Run DDC performance tests
    benchmark.benchmark_ddc_performance()
    
    # Run combined pipeline tests
    benchmark.benchmark_combined_pipeline()
    
    # Generate report
    benchmark.generate_performance_report(output_file)
    
    # Create plots
    benchmark.create_performance_plots(output_dir)
    
    logging.info("Benchmark Complete")
    
    return benchmark


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run comprehensive performance benchmark suite for Wideband SDR")
    parser.add_argument('--output-file', type=str, default='fft_ddc_performance_report.json',
                        help='Output file for performance report')
    parser.add_argument('--output-dir', type=str, default='performance_plots',
                        help='Output directory for performance plots')
    args = parser.parse_args()

    # Run comprehensive benchmark
    results = run_comprehensive_benchmark(output_file=args.output_file, output_dir=args.output_dir)
    
    print("\nBenchmark completed successfully!")
    print(f"Check '{args.output_file}' for detailed results.")
    print(f"Check '{args.output_dir}/' directory for visualization charts.")
