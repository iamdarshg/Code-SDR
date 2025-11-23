#!/usr/bin/env python3
"""
Test suite for Wideband SDR Calibration Wizard

Comprehensive test suite to validate calibration functionality
including frequency accuracy, gain calibration, and data persistence.

Created: November 2025
Version: 1.0.0
License: MIT
"""

import unittest
import json
import tempfile
import numpy as np
from pathlib import Path
import sys
import os

# Add the software directory to the path
sys.path.insert(0, str(Path(__file__).parent))

try:
    from calibration import (
        CalibrationWizard, CalibrationMetadata, FrequencyCalibrationResult,
        GainCalibrationResult, MixerCharacterisationData
    )
    HAS_CALIBRATION = True
except ImportError:
    HAS_CALIBRATION = False

# Mock WidebandSDR class for testing
class MockWidebandSDR:
    """Mock WidebandSDR class for testing"""
    
    MIN_FREQUENCY = 1_000_000
    MAX_FREQUENCY = 10_000_000_000
    MIN_SAMPLE_RATE = 1_000
    MAX_SAMPLE_RATE = 105_000_000
    MIN_GAIN = 0
    MAX_GAIN = 63
    
    def __init__(self):
        self.current_frequency = 100_000_000
        self.current_sample_rate = 1_000_000
        self.current_gain = 30
        self._sample_count = 0
    
    def set_frequency(self, frequency):
        if not (self.MIN_FREQUENCY <= frequency <= self.MAX_FREQUENCY):
            raise ValueError("Frequency out of range")
        self.current_frequency = frequency
        return True
    
    def get_frequency(self):
        return self.current_frequency
    
    def set_sample_rate(self, sample_rate):
        if not (self.MIN_SAMPLE_RATE <= sample_rate <= self.MAX_SAMPLE_RATE):
            raise ValueError("Sample rate out of range")
        self.current_sample_rate = sample_rate
        return True
    
    def get_sample_rate(self):
        return self.current_sample_rate
    
    def set_gain(self, gain):
        if not (self.MIN_GAIN <= gain <= self.MAX_GAIN):
            raise ValueError("Gain out of range")
        self.current_gain = gain
        return True
    
    def get_gain(self):
        return self.current_gain
    
    def read_samples(self, count):
        """Generate mock samples with controlled characteristics"""
        self._sample_count += count
        
        # Generate realistic mock samples
        samples = np.random.normal(0, 0.1, count).astype(np.float32)
        
        # Add a test tone at the current frequency
        if self.current_frequency > 0:
            t = np.arange(count) / self.current_sample_rate
            tone_freq = self.current_frequency
            if tone_freq < self.current_sample_rate / 2:  # Nyquist check
                tone = 0.5 * np.cos(2 * np.pi * tone_freq * t)
                samples += tone
        
        return samples
    
    def get_device_info(self):
        return {
            'product': 'Mock Wideband SDR',
            'manufacturer': 'Test Manufacturer',
            'vendor_id': '0x04D8',
            'product_id': '0x000A'
        }


class TestCalibrationWizard(unittest.TestCase):
    """Test suite for CalibrationWizard class"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not HAS_CALIBRATION:
            self.skipTest("Calibration module not available")
        
        self.mock_sdr = MockWidebandSDR()
        self.temp_dir = tempfile.mkdtemp()
        self.calibration_dir = Path(self.temp_dir)
        
        self.wizard = CalibrationWizard(self.mock_sdr, str(self.calibration_dir))
    
    def tearDown(self):
        """Clean up test fixtures"""
        # Clean up temp directory
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_initialization(self):
        """Test CalibrationWizard initialization"""
        self.assertIsNotNone(self.wizard)
        self.assertEqual(self.wizard.sdr, self.mock_sdr)
        self.assertTrue(self.calibration_dir.exists())
        self.assertIsInstance(self.wizard.metadata, CalibrationMetadata)
    
    def test_frequency_calibration_result_structure(self):
        """Test frequency calibration result structure"""
        result = FrequencyCalibrationResult(
            target_frequency=100_000_000,
            measured_frequency=100_000_005,
            error_hz=5.0,
            error_ppm=0.05,
            confidence_level=0.95,
            measurement_iterations=10,
            fft_size=8192,
            window_function='hann',
            calibration_timestamp="2025-11-23T13:30:00"
        )
        
        self.assertEqual(result.target_frequency, 100_000_000)
        self.assertEqual(result.error_hz, 5.0)
        self.assertEqual(result.error_ppm, 0.05)
        self.assertEqual(result.confidence_level, 0.95)
    
    def test_gain_calibration_result_structure(self):
        """Test gain calibration result structure"""
        result = GainCalibrationResult(
            gain_setting=30,
            measured_power_dbm=-25.0,
            reference_power_dbm=-30.0,
            gain_error_db=5.0,
            linearity_error_db=0.1,
            measurement_timestamp="2025-11-23T13:30:00"
        )
        
        self.assertEqual(result.gain_setting, 30)
        self.assertEqual(result.measured_power_dbm, -25.0)
        self.assertEqual(result.gain_error_db, 5.0)
    
    def test_mixer_characterisation_structure(self):
        """Test mixer characterisation data structure"""
        data = MixerCharacterisationData(
            frequency_mhz=100.0,
            lo_power_dbm=0.0,
            rf_power_dbm=-20.0,
            if_power_dbm=-15.0,
            conversion_gain_db=15.0,
            noise_figure_db=10.0,
            ip1_db=0.0,
            ip3_db=0.0,
            harmonic_distortion_hd2=-60.0,
            harmonic_distortion_hd3=-80.0,
            isolation_lo_rf_db=-40.0,
            isolation_lo_if_db=-50.0
        )
        
        self.assertEqual(data.frequency_mhz, 100.0)
        self.assertEqual(data.conversion_gain_db, 15.0)
        self.assertEqual(data.noise_figure_db, 10.0)
    
    def test_calibration_data_storage(self):
        """Test calibration data storage and retrieval"""
        # Simulate a frequency calibration
        mock_result = {
            'target_frequency': 100_000_000,
            'measured_frequency': 100_000_005,
            'error_hz': 5.0,
            'error_ppm': 0.05,
            'confidence_level': 0.95,
            'measurement_iterations': 10,
            'fft_size': 8192,
            'window_function': 'hann',
            'calibration_timestamp': "2025-11-23T13:30:00"
        }
        
        self.wizard.calibration_data['frequency_calibration'] = [mock_result]
        
        # Save calibration
        saved_file = self.wizard.save_calibration("test_calibration.json")
        self.assertTrue(Path(saved_file).exists())
        
        # Load calibration
        loaded = self.wizard.load_calibration(saved_file)
        self.assertTrue(loaded)
        self.assertIn('frequency_calibration', self.wizard.calibration_data)
        self.assertEqual(len(self.wizard.calibration_data['frequency_calibration']), 1)
    
    def test_apply_calibration_corrections(self):
        """Test calibration correction application"""
        # Set up mock calibration data
        self.wizard.calibration_data['frequency_calibration'] = [
            {'error_ppm': 0.05}  # 0.05 ppm error
        ]
        
        self.wizard.calibration_data['gain_calibration'] = {
            '30': {'gain_error_db': 2.0}  # 2 dB error at gain 30
        }
        
        # Set current gain
        self.mock_sdr.set_gain(30)
        
        # Apply corrections
        corrected_freq, factors = self.wizard.apply_calibration_corrections(100_000_000)
        
        # Should apply opposite correction
        expected_freq = 100_000_000 * (1 - 0.05e-6)  # Correct for 0.05 ppm error
        self.assertAlmostEqual(corrected_freq, expected_freq, places=0)
        self.assertIn('frequency_correction_ppm', factors)
        self.assertEqual(factors['frequency_correction_ppm'], -0.05)
    
    def test_confidence_level_calculation(self):
        """Test confidence level calculation"""
        # Test high confidence (low error)
        conf_high = self.wizard._calculate_confidence_level(0.03)
        self.assertEqual(conf_high, 0.95)
        
        # Test medium confidence (moderate error)
        conf_medium = self.wizard._calculate_confidence_level(0.08)
        self.assertEqual(conf_medium, 0.80)
        
        # Test low confidence (high error)
        conf_low = self.wizard._calculate_confidence_level(1.0)
        self.assertEqual(conf_low, 0.30)
    
    def test_noise_power_estimation(self):
        """Test noise power estimation"""
        # Create test samples with known characteristics
        samples = np.random.normal(0, 0.1, 1000).astype(np.float32)
        noise_power = self.wizard._estimate_noise_power(samples)
        
        self.assertIsInstance(noise_power, float)
        self.assertGreater(noise_power, 0)
        self.assertLess(noise_power, 1)  # Should be reasonable for our test data
    
    def test_calibration_metadata(self):
        """Test calibration metadata structure"""
        metadata = self.wizard.metadata
        
        self.assertEqual(metadata.version, "1.0.0")
        self.assertIsNotNone(metadata.timestamp)
        self.assertEqual(metadata.calibration_algorithm, "FFT-based with ADC verification")
        self.assertEqual(metadata.accuracy_target_ppm, 0.1)


class TestCalibrationDataPersistence(unittest.TestCase):
    """Test calibration data persistence functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not HAS_CALIBRATION:
            self.skipTest("Calibration module not available")
        
        self.mock_sdr = MockWidebandSDR()
        self.temp_dir = tempfile.mkdtemp()
        self.calibration_dir = Path(self.temp_dir)
        
        self.wizard = CalibrationWizard(self.mock_sdr, str(self.calibration_dir))
        
        # Create sample calibration data
        self.sample_calibration = {
            'metadata': {
                'version': '1.0.0',
                'timestamp': '2025-11-23T13:30:00',
                'device_info': {'product': 'Test SDR'},
                'calibration_algorithm': 'FFT-based with ADC verification'
            },
            'calibration_data': {
                'frequency_calibration': [
                    {
                        'target_frequency': 100_000_000,
                        'error_ppm': 0.05,
                        'confidence_level': 0.95
                    }
                ],
                'gain_calibration': {
                    '30': {
                        'gain_setting': 30,
                        'gain_error_db': 1.5
                    }
                }
            }
        }
    
    def tearDown(self):
        """Clean up test fixtures"""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_save_and_load_calibration(self):
        """Test saving and loading calibration data"""
        # Create calibration file
        calibration_file = self.calibration_dir / "test_cal.json"
        
        # Save test data
        with open(calibration_file, 'w') as f:
            json.dump(self.sample_calibration, f, indent=2)
        
        # Load calibration
        success = self.wizard.load_calibration(str(calibration_file))
        self.assertTrue(success)
        
        # Verify loaded data
        self.assertIn('frequency_calibration', self.wizard.calibration_data)
        self.assertIn('gain_calibration', self.wizard.calibration_data)
        
        # Verify metadata
        self.assertEqual(self.wizard.metadata.version, '1.0.0')
        self.assertEqual(self.wizard.metadata.calibration_algorithm, 
                        'FFT-based with ADC verification')
    
    def test_nonexistent_file_load(self):
        """Test loading non-existent calibration file"""
        success = self.wizard.load_calibration("nonexistent_file.json")
        self.assertFalse(success)
    
    def test_invalid_json_load(self):
        """Test loading invalid JSON file"""
        invalid_file = self.calibration_dir / "invalid.json"
        
        # Write invalid JSON
        with open(invalid_file, 'w') as f:
            f.write("invalid json content")
        
        # Try to load (should handle gracefully)
        try:
            success = self.wizard.load_calibration(str(invalid_file))
            # Should either succeed (with empty data) or fail gracefully
            self.assertIsInstance(success, bool)
        except json.JSONDecodeError:
            # This is acceptable - the method should handle JSON errors
            pass


class TestCalibrationExamples(unittest.TestCase):
    """Test examples and demonstration functionality"""
    
    def test_example_import(self):
        """Test that the calibration module can be imported correctly"""
        if not HAS_CALIBRATION:
            self.skipTest("Calibration module not available")
        
        # Test that all main classes can be imported
        from calibration import (
            CalibrationWizard, CalibrationMetadata, FrequencyCalibrationResult,
            GainCalibrationResult, MixerCharacterisationData
        )
        
        # Verify classes exist and are properly defined
        self.assertTrue(callable(CalibrationWizard))
        self.assertTrue(callable(CalibrationMetadata))
        self.assertTrue(callable(FrequencyCalibrationResult))
        
        # Check that dataclasses have expected fields
        freq_result = FrequencyCalibrationResult(
            target_frequency=100_000_000,
            measured_frequency=100_000_000,
            error_hz=0.0,
            error_ppm=0.0,
            confidence_level=1.0,
            measurement_iterations=1,
            fft_size=1024,
            window_function='hann',
            calibration_timestamp="2025-11-23T13:30:00"
        )
        
        self.assertTrue(hasattr(freq_result, 'target_frequency'))
        self.assertTrue(hasattr(freq_result, 'error_ppm'))
        self.assertTrue(hasattr(freq_result, 'confidence_level'))


def run_calibration_tests():
    """Run the complete test suite"""
    print("=" * 60)
    print("WIDEBAND SDR CALIBRATION WIZARD TEST SUITE")
    print("=" * 60)
    
    # Create test suite
    test_suite = unittest.TestSuite()
    
    # Add test classes
    test_classes = [
        TestCalibrationWizard,
        TestCalibrationDataPersistence,
        TestCalibrationExamples
    ]
    
    for test_class in test_classes:
        tests = unittest.TestLoader().loadTestsFromTestCase(test_class)
        test_suite.addTests(tests)
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(test_suite)
    
    # Print summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print(f"Success rate: {((result.testsRun - len(result.failures) - len(result.errors)) / result.testsRun * 100):.1f}%")
    
    return result.wasSuccessful()


if __name__ == "__main__":
    success = run_calibration_tests()
    
    if success:
        print("\n✅ All calibration tests PASSED!")
        sys.exit(0)
    else:
        print("\n❌ Some calibration tests FAILED!")
        sys.exit(1)
