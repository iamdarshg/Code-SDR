"""
GNU Radio Out Of Tree Module for Wideband SDR

This module provides GNU Radio blocks for interfacing with the Wideband SDR device.
The source block provides real-time I/Q samples from the SDR hardware.
"""

try:
    from .bindings import *
except ImportError:
    # If bindings are not available (during development), provide a fallback
    import warnings
    warnings.warn("GNU Radio Wideband SDR Python bindings not available. "
                  "This is normal during development.", RuntimeWarning)

# Module information
__version__ = "1.0.0"
__author__ = "Wideband SDR Project"
__email__ = "darsh.gupta@example.com"

# Export main symbols
try:
    from .source import source
except ImportError:
    pass  # Module might not be compiled yet

# For backward compatibility
from . import source as wideband_sdr_source

__all__ = [
    'source',
    'wideband_sdr_source',
]
