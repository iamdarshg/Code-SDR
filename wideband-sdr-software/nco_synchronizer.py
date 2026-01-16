"""
nco_synchronizer.py - NCO Phase Synchronizer Module for Wideband SDR

Provides a stateful NCO (Numerically Controlled Oscillator) for efficient,
phase-continuous signal generation.

Author: Wideband SDR Development Team
Date: 2025-11-20
"""

import numpy as np

class NCOSynchronizer:
    """
    NCO Phase Synchronizer for improved frequency accuracy and performance.

    This class maintains the phase of the NCO between calls, ensuring a
    continuous waveform when generating signals in chunks.
    """

    def __init__(self, sample_rate: float, frequency: float, phase_offset: float = 0.0):
        """
        Initialize NCO Synchronizer.

        Args:
            sample_rate: Sample rate in Hz.
            frequency: Target frequency in Hz.
            phase_offset: Initial phase offset in radians.
        """
        self.sample_rate = sample_rate
        self.frequency = frequency
        self.phase_offset = phase_offset
        self.phase_increment = 2 * np.pi * frequency / sample_rate
        self.phase = phase_offset

    def generate_nco_samples(self, num_samples: int) -> np.ndarray:
        """
        Generate synchronized NCO samples.

        Args:
            num_samples: Number of samples to generate.

        Returns:
            Complex NCO samples as a NumPy array.
        """
        # Generate a sequence of phase values for the given number of samples
        phases = self.phase + self.phase_increment * np.arange(num_samples)

        # Update the internal phase for the next call, wrapping around 2*pi
        # to prevent the phase from growing indefinitely, which could lead to
        # floating-point precision issues over very long runs.
        self.phase = (self.phase + self.phase_increment * num_samples) % (2 * np.pi)

        return np.exp(1j * phases)

    def update_frequency(self, frequency: float) -> None:
        """
        Update the NCO frequency dynamically.

        Args:
            frequency: New frequency in Hz.
        """
        self.frequency = frequency
        self.phase_increment = 2 * np.pi * frequency / self.sample_rate
