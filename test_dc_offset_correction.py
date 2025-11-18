#!/usr/bin/env python3
"""
Wideband SDR - DC Offset Correction Integration Tests

Tests DC offset correction functionality to validate >20dB improvement
and auto-calibration routines as specified in prompt 0c.

Author: Wideband SDR Development Team
Created: November 2025
Version: 1.0.0
License: MIT
"""

import numpy as np
import matplotlib.pyplot as plt
import time
import sys
import os

# Add the software directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'wideband-sdr-software'))

from dc_offset_corrector import DCOffsetCorrector, DCOffsetMode, ContinuousDCCalibrator


class MockSDR:
    """Mock SDR device for testing DC offset correction"""
    
    def __init__(self, dc_offset_i=0.1, dc_offset_q=-0.05, noise_level=0.01):
        self.dc_offset_i = dc_offset_i
        self.dc_offset_q = dc_offset_q
        self.noise_level = noise_level
        self.sample_count = 0
        
    def read_samples(self, count):
        """Generate mock I/Q samples with DC offset"""
        np.random.seed(42)  # For reproducible tests
        
        # Generate noise
        noise_i = np.random.normal(0, self.noise_level, count)
        noise_q = np.random.normal(0, self.noise_level, count)
        
        # Add DC offset
        i_samples = noise_i + self.dc_offset_i
        q_samples = noise_q + self.dc_offset_q
        
        # Create complex samples
        samples = i_samples + 1j * q_samples
        
        self.sample_count += count
        return samples


class DCCorrectionTestSuite:
    """Test suite for DC offset correction functionality"""
    
    def __init__(self):
        self.passed_tests = 0
        self.failed_tests = 0
        self.test_results = []
        
    def log_test_result(self, test_name, passed, message=""):
        """Log test result"""
        if passed:
            self.passed_tests += 1
            status = "PASS"
        else:
            self.failed_tests += 1
            status = "FAIL"
        
        result = f"[{status}] {test_name}"
        if message:
            result += f": {message}"
        
        self.test_results.append(result)
        print(result)
        
        return passed
    
    def test_basic_functionality(self):
        """Test basic DC offset correction functionality"""
        print("\n=== Testing Basic Functionality ===")
        
        # Initialize corrector
        corrector = DCOffsetCorrector(window_size=512, alpha=0.1)
        
        # Test with known DC offset
        dc_offset_i, dc_offset_q = 0.2, -0.15
        mock_sdr = MockSDR(dc_offset_i, dc_offset_q)
        
        # Generate samples and estimate DC offset
        samples = mock_sdr.read_samples(2000)
        estimated_i, estimated_q = corrector.estimate_dc_offset(samples)
        
        # Check if estimation is reasonably accurate
        i_error = abs(estimated_i - dc_offset_i)
        q_error = abs(estimated_q - dc_offset_q)
        
        # Allow for some tolerance due to noise
        i_accurate = i_error < 0.05
        q_accurate = q_error < 0.05
        
        self.log_test_result(
            "Basic DC Offset Estimation",
            i_accurate and q_accurate,
            f"I error: {i_error:.3f}, Q error: {q_error:.3f}"
        )
        
        # Test correction application
        corrected_samples = corrector.apply_correction(samples)
        
        # Check if corrected samples have reduced DC offset
        corrected_dc_i = np.mean(np.real(corrected_samples))
        corrected_dc_q = np.mean(np.imag(corrected_samples))
        
        improvement_i = abs(dc_offset_i) / (abs(corrected_dc_i) + 1e-10)
        improvement_q = abs(dc_offset_q) / (abs(corrected_dc_q) + 1e-10)
        
        improvement_db = 20 * np.log10(min(improvement_i, improvement_q))
        
        self.log_test_result(
            "DC Offset Correction Application",
            improvement_db > 10,  # At least 10dB improvement expected
            f"Improvement: {improvement_db:.2f} dB"
        )
        
        return improvement_db > 20  # Return true if >20dB improvement achieved
    
    def test_calibration_routine(self):
        """Test automatic calibration routine"""
        print("\n=== Testing Calibration Routine ===")
        
        corrector = DCOffsetCorrector(window_size=1024, alpha=0.1)
        mock_sdr = MockSDR(0.15, -0.08)
        
        # Check initial state
        initially_calibrated = corrector.is_calibrated()
        self.log_test_result(
            "Initial Calibration State",
            not initially_calibrated,
            f"Calibrated: {initially_calibrated}"
        )
        
        # Perform calibration
        cal_i, cal_q = corrector.calibrate(mock_sdr, num_samples=5000)
        
        # Check if calibrated after procedure
        calibrated_after = corrector.is_calibrated()
        self.log_test_result(
            "Calibration Completion",
            calibrated_after,
            f"Calibrated: {calibrated_after}"
        )
        
        # Test with corrected samples
        samples = mock_sdr.read_samples(1000)
        corrected_samples = corrector.apply_correction(samples)
        
        corrected_dc_i = np.mean(np.real(corrected_samples))
        corrected_dc_q = np.mean(np.imag(corrected_samples))
        
        # Calculate improvement
        original_offset = max(abs(mock_sdr.dc_offset_i), abs(mock_sdr.dc_offset_q))
        corrected_offset = max(abs(corrected_dc_i), abs(corrected_dc_q))
        
        if corrected_offset > 0:
            improvement_db = 20 * np.log10(original_offset / corrected_offset)
        else:
            improvement_db = float('inf')
        
        self.log_test_result(
            "Calibration Improvement",
            improvement_db > 20,
            f"Improvement: {improvement_db:.2f} dB"
        )
        
        return improvement_db > 20
    
    def test_real_time_correction(self):
        """Test real-time correction with streaming"""
        print("\n=== Testing Real-time Correction ===")
        
        corrector = DCOffsetCorrector(window_size=512, alpha=0.05)
        corrector.enable_performance_monitoring(True)
        
        mock_sdr = MockSDR(0.1, -0.05)
        
        # Simulate streaming with multiple chunks
        chunk_size = 256
        num_chunks = 20
        start_time = time.time()
        
        total_samples_processed = 0
        corrections_applied = 0
        
        for chunk_idx in range(num_chunks):
            # Generate samples
            samples = mock_sdr.read_samples(chunk_size)
            
            # Apply real-time correction
            corrected_samples, offsets = corrector.process_samples_batch(samples, correct=True)
            
            total_samples_processed += len(samples)
            corrections_applied += 1
            
            # Check that correction is being applied
            if chunk_idx > 10:  # After some chunks, should be calibrated
                corrected_dc = np.mean(np.real(corrected_samples))
                if abs(corrected_dc) < 0.01:  # Small residual offset
                    break
        
        elapsed_time = time.time() - start_time
        performance = corrector.get_performance_metrics()
        
        # Check performance metrics
        samples_per_sec = performance['samples_per_second']
        real_time_factor = samples_per_sec / 1000000  # Assuming 1 MSPS
        
        self.log_test_result(
            "Real-time Performance",
            real_time_factor > 0.5,  # Should handle at least 50% of 1 MSPS
            f"Samples/sec: {samples_per_sec:.0f}, RT factor: {real_time_factor:.2f}"
        )
        
        self.log_test_result(
            "Continuous Correction",
            corrections_applied == num_chunks,
            f"Chunks processed: {corrections_applied}/{num_chunks}"
        )
        
        return real_time_factor > 0.5
    
    def test_improvement_validation(self):
        """Test >20dB improvement validation"""
        print("\n=== Testing >20dB Improvement Validation ===")
        
        # Test with various DC offset levels
        test_offsets = [
            (0.3, 0.2),   # Large offset
            (0.15, 0.1),  # Medium offset
            (0.05, 0.03), # Small offset
        ]
        
        improvements = []
        
        for dc_i, dc_q in test_offsets:
            corrector = DCOffsetCorrector(window_size=2048, alpha=0.1)
            mock_sdr = MockSDR(dc_i, dc_q, noise_level=0.01)
            
            # Calibrate
            corrector.calibrate(mock_sdr, num_samples=10000)
            
            # Test with new samples
            test_samples = mock_sdr.read_samples(2000)
            corrected_samples = corrector.apply_correction(test_samples)
            
            # Calculate improvement
            original_dc = max(abs(dc_i), abs(dc_q))
            corrected_dc_i = np.mean(np.real(corrected_samples))
            corrected_dc_q = np.mean(np.imag(corrected_samples))
            corrected_dc = max(abs(corrected_dc_i), abs(corrected_dc_q))
            
            if corrected_dc > 0:
                improvement_db = 20 * np.log10(original_dc / corrected_dc)
            else:
                improvement_db = float('inf')
            
            improvements.append(improvement_db)
            
            self.log_test_result(
                f"Improvement Test (offset={dc_i:.2f},{dc_q:.2f})",
                improvement_db > 20,
                f"Improvement: {improvement_db:.2f} dB"
            )
        
        # Check if all tests pass
        all_passed = all(imp > 20 for imp in improvements)
        avg_improvement = np.mean(improvements)
        
        self.log_test_result(
            "Overall Improvement Validation",
            all_passed,
            f"Average improvement: {avg_improvement:.2f} dB"
        )
        
        return all_passed
    
    def test_mode_switching(self):
        """Test different correction modes"""
        print("\n=== Testing Mode Switching ===")
        
        corrector = DCOffsetCorrector(mode=DCOffsetMode.DISABLED)
        mock_sdr = MockSDR(0.2, -0.1)
        
        samples = mock_sdr.read_samples(1000)
        
        # Test disabled mode
        disabled_result = corrector.apply_correction(samples)
        disabled_changed = not np.array_equal(samples, disabled_result)
        
        self.log_test_result(
            "Disabled Mode",
            not disabled_changed,
            "Samples unchanged in disabled mode"
        )
        
        # Test software mode
        corrector.set_mode(DCOffsetMode.SOFTWARE)
        corrector.calibrate(mock_sdr, num_samples=5000)
        
        software_result = corrector.apply_correction(samples)
        software_changed = not np.array_equal(samples, software_result)
        
        self.log_test_result(
            "Software Mode",
            software_changed,
            "Samples corrected in software mode"
        )
        
        return True
    
    def test_statistics_monitoring(self):
        """Test statistics and monitoring functionality"""
        print("\n=== Testing Statistics Monitoring ===")
        
        corrector = DCOffsetCorrector()
        corrector.enable_performance_monitoring(True)
        
        mock_sdr = MockSDR(0.1, -0.05)
        
        # Process some samples
        for _ in range(5):
            samples = mock_sdr.read_samples(512)
            corrector.process_samples_batch(samples)
        
        # Check statistics
        stats = corrector.get_statistics()
        performance = corrector.get_performance_metrics()
        
        stats_valid = (
            stats.total_samples > 0 and
            stats.average_offset_i != 0 and
            stats.average_offset_q != 0
        )
        
        performance_valid = (
            performance['samples_processed'] > 0 and
            performance['samples_per_second'] > 0
        )
        
        self.log_test_result(
            "Statistics Collection",
            stats_valid,
            f"Total samples: {stats.total_samples}"
        )
        
        self.log_test_result(
            "Performance Monitoring",
            performance_valid,
            f"Rate: {performance['samples_per_second']:.0f} samples/sec"
        )
        
        return stats_valid and performance_valid
    
    def generate_test_report(self):
        """Generate comprehensive test report"""
        print("\n" + "="*60)
        print("DC OFFSET CORRECTION TEST REPORT")
        print("="*60)
        
        print(f"\nTest Summary:")
        print(f"  Passed: {self.passed_tests}")
        print(f"  Failed: {self.failed_tests}")
        print(f"  Success Rate: {self.passed_tests/(self.passed_tests+self.failed_tests)*100:.1f}%")
        
        print(f"\nDetailed Results:")
        for result in self.test_results:
            print(f"  {result}")
        
        overall_success = self.failed_tests == 0
        
        print(f"\nOverall Result: {'PASS' if overall_success else 'FAIL'}")
        print(f"Prompt 0c Requirements: {'MET' if overall_success else 'NOT MET'}")
        
        return overall_success
    
    def run_all_tests(self):
        """Run complete test suite"""
        print("Starting DC Offset Correction Tests")
        print("Testing Prompt 0c Requirements:")
        print("  - Firmware DC offset correction implemented")
        print("  - Python DCOffsetCorrector class working")
        print("  - Auto-calibration routine functional")
        print("  - DC offset reduction >20dB validated")
        print("  - Real-time correction integrated with streaming")
        print("-" * 60)
        
        try:
            # Run all test categories
            self.test_basic_functionality()
            self.test_calibration_routine()
            self.test_real_time_correction()
            self.test_improvement_validation()
            self.test_mode_switching()
            self.test_statistics_monitoring()
            
            # Generate final report
            success = self.generate_test_report()
            
            return success
            
        except Exception as e:
            print(f"\nERROR: Test suite failed with exception: {e}")
            import traceback
            traceback.print_exc()
            return False


def create_visualization():
    """Create visualization of DC offset correction performance"""
    print("\n=== Creating Visualization ===")
    
    # Initialize corrector
    corrector = DCOffsetCorrector(window_size=1024, alpha=0.1)
    mock_sdr = MockSDR(0.2, -0.15)
    
    # Process samples and track DC offset over time
    dc_offsets_i = []
    dc_offsets_q = []
    sample_indices = []
    
    chunk_size = 256
    num_chunks = 50
    
    for i in range(num_chunks):
        samples = mock_sdr.read_samples(chunk_size)
        
        # Estimate DC offset
        offset_i, offset_q = corrector.estimate_dc_offset(samples)
        
        # Apply correction
        corrected_samples = corrector.apply_correction(samples)
        
        # Track corrected DC offset
        corrected_dc_i = np.mean(np.real(corrected_samples))
        corrected_dc_q = np.mean(np.imag(corrected_samples))
        
        sample_indices.append(i * chunk_size)
        dc_offsets_i.append(corrected_dc_i)
        dc_offsets_q.append(corrected_dc_q)
    
    # Create plot
    plt.figure(figsize=(12, 8))
    
    plt.subplot(2, 1, 1)
    plt.plot(sample_indices, dc_offsets_i, 'b-', label='I Channel DC Offset')
    plt.plot(sample_indices, dc_offsets_q, 'r-', label='Q Channel DC Offset')
    plt.axhline(y=0, color='k', linestyle='--', alpha=0.5)
    plt.xlabel('Sample Index')
    plt.ylabel('DC Offset')
    plt.title('DC Offset Correction Over Time')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.subplot(2, 1, 2)
    # Show improvement over time
    original_offset = max(abs(mock_sdr.dc_offset_i), abs(mock_sdr.dc_offset_q))
    improvements = [20 * np.log10(original_offset / max(abs(i), abs(q))) 
                   for i, q in zip(dc_offsets_i, dc_offsets_q)]
    
    plt.plot(sample_indices, improvements, 'g-', label='Improvement (dB)')
    plt.axhline(y=20, color='r', linestyle='--', label='20 dB Target')
    plt.xlabel('Sample Index')
    plt.ylabel('Improvement (dB)')
    plt.title('DC Offset Correction Improvement')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('dc_offset_correction_test.png', dpi=150, bbox_inches='tight')
    plt.close()
    
    print("Visualization saved as 'dc_offset_correction_test.png'")


if __name__ == "__main__":
    # Run test suite
    test_suite = DCCorrectionTestSuite()
    success = test_suite.run_all_tests()
    
    # Create visualization
    try:
        create_visualization()
    except Exception as e:
        print(f"Visualization failed: {e}")
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)
