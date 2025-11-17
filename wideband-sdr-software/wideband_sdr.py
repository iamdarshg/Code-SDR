#!/usr/bin/env python3
"""
Wideband SDR Python Driver
Complete Python driver for the 1 MHz - 10 GHz Wideband SDR

Created: November 2025
Version: 1.0.0
License: MIT

Features:
- libusb device discovery and initialization
- Command interface (frequency, gain, sample rate)
- Asynchronous sample streaming
- Callback or queue-based sample delivery
- Thread-safe operation
- Multi-device support

Example usage:
    from wideband_sdr import WidebandSDR

    sdr = WidebandSDR()
    sdr.set_frequency(100_000_000)  # 100 MHz
    sdr.set_sample_rate(10_000)     # 10 MSPS
    sdr.set_gain(30)                 # 30 dB
    sdr.start_stream()
    # ... process samples ...
    sdr.stop_stream()
    sdr.close()
"""

import sys
import time
import threading
import queue
import logging
from typing import Optional, Callable, List, Tuple, Any
from dataclasses import dataclass
from enum import Enum
import struct
import numpy as np

try:
    import usb.core
    import usb.util
except ImportError:
    print("PyUSB not found. Install with: pip install pyusb")
    sys.exit(1)

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class SDRError(Exception):
    """Base exception for SDR operations"""
    pass


class SDRTimeoutError(SDRError):
    """Timeout error for SDR operations"""
    pass


class SDRConnectionError(SDRError):
    """Connection error for SDR device"""
    pass


class SDRStreamError(SDRError):
    """Streaming error for SDR operations"""
    pass


class WidebandSDRError(SDRError):
    """Wideband SDR specific error"""
    pass


class StreamCallback:
    """Callback interface for sample streaming"""
    
    def __init__(self, callback: Callable[[np.ndarray], None]):
        self.callback = callback
    
    def __call__(self, samples: np.ndarray):
        """Process incoming samples"""
        try:
            self.callback(samples)
        except Exception as e:
            logger.error(f"Callback error: {e}")


@dataclass
class SDRStats:
    """SDR statistics structure"""
    samples_processed: int = 0
    packets_dropped: int = 0
    overrun_count: int = 0
    bandwidth_utilization: float = 0.0
    average_sample_rate: float = 0.0
    current_frequency: int = 0
    current_sample_rate: int = 0
    current_gain: int = 0


class WidebandSDR:
    """
    Wideband SDR Python Driver
    
    Provides high-level interface to the dsPIC33AK256MC505 based SDR
    """
    
    # USB Configuration
    USB_VENDOR_ID = 0x04D8      # Microchip
    USB_PRODUCT_ID = 0x000A     # Wideband SDR
    USB_CONFIGURATION = 1
    USB_INTERFACE = 0
    
    # USB Endpoints
    EP_BULK_IN = 1              # Bulk IN endpoint for sample data
    EP_BULK_OUT = 2             # Bulk OUT endpoint for commands
    EP_INTERRUPT_IN = 3         # Interrupt IN endpoint for status
    
    # Command Definitions
    CMD_SET_FREQUENCY = 0x01
    CMD_SET_SAMPLE_RATE = 0x02
    CMD_SET_GAIN = 0x03
    CMD_START_STREAM = 0x04
    CMD_STOP_STREAM = 0x05
    CMD_GET_STATUS = 0x06
    CMD_CALIBRATE = 0x07
    CMD_SET_POWER = 0x08
    
    # Supported Parameters
    MIN_FREQUENCY = 1_000_000    # 1 MHz
    MAX_FREQUENCY = 10_000_000_000  # 10 GHz
    MIN_SAMPLE_RATE = 1_000      # 1 kSPS
    MAX_SAMPLE_RATE = 105_000_000  # 105 MSPS
    MIN_GAIN = 0
    MAX_GAIN = 63
    
    def __init__(self, device_index: int = 0):
        """
        Initialize Wideband SDR
        
        Args:
            device_index: Device index (0 for first device)
        """
        self.device_index = device_index
        self.device = None
        self.is_open = False
        self.streaming_active = False
        self.stream_thread = None
        self.sample_queue = queue.Queue(maxsize=100)
        self.callback = None
        self.stats = SDRStats()
        self.lock = threading.Lock()
        
        # Default configuration
        self.current_frequency = 100_000_000  # 100 MHz
        self.current_sample_rate = 1_000_000  # 1 MSPS
        self.current_gain = 30  # 30 dB
        
        # Buffer configuration
        self.buffer_size = 16384  # 16K samples
        self.sample_format = np.int16  # 16-bit samples from 10-bit ADC
        
    def open(self) -> bool:
        """
        Open connection to SDR device
        
        Returns:
            bool: Success status
            
        Raises:
            SDRConnectionError: If device cannot be opened
        """
        try:
            # Find USB device
            devices = usb.core.find(
                idVendor=self.USB_VENDOR_ID,
                idProduct=self.USB_PRODUCT_ID,
                find_all=True
            )
            
            if not devices:
                raise SDRConnectionError("Wideband SDR device not found")
            
            if isinstance(devices, list):
                if self.device_index >= len(devices):
                    raise SDRConnectionError(f"Device index {self.device_index} not available")
                self.device = devices[self.device_index]
            else:
                self.device = devices
            
            # Detach kernel driver if needed
            try:
                self.device.detach_kernel_driver(0)
            except (usb.core.USBError, NotImplementedError):
                pass  # Driver not attached or not supported
            
            # Set configuration
            self.device.set_configuration()
            
            # Get configuration and interface
            cfg = self.device.get_active_configuration()
            intf = cfg[(0, 0)]
            
            # Claim interface
            usb.util.claim_interface(self.device, self.USB_INTERFACE)
            
            self.is_open = True
            logger.info(f"Wideband SDR device opened (index {self.device_index})")
            
            # Initialize device with default settings
            self._initialize_device()
            
            return True
            
        except usb.core.USBError as e:
            raise SDRConnectionError(f"USB error: {e}")
        except Exception as e:
            raise SDRConnectionError(f"Failed to open device: {e}")
    
    def close(self):
        """Close connection to SDR device"""
        if self.is_open:
            self.stop_stream()
            
            try:
                usb.util.release_interface(self.device, self.USB_INTERFACE)
                self.device = None
                self.is_open = False
                logger.info("Wideband SDR device closed")
            except usb.core.USBError as e:
                logger.error(f"Error closing device: {e}")
    
    def set_frequency(self, frequency: int) -> bool:
        """
        Set SDR frequency
        
        Args:
            frequency: Frequency in Hz
            
        Returns:
            bool: Success status
            
        Raises:
            WidebandSDRError: If frequency is out of range
        """
        if not self.is_open:
            raise WidebandSDRError("Device not open")
        
        if not (self.MIN_FREQUENCY <= frequency <= self.MAX_FREQUENCY):
            raise WidebandSDRError(f"Frequency {frequency} out of range [{self.MIN_FREQUENCY}, {self.MAX_FREQUENCY}]")
        
        try:
            # Send frequency command
            cmd = struct.pack('<BI', self.CMD_SET_FREQUENCY, frequency)
            self.device.write(self.EP_BULK_OUT, cmd, timeout=1000)
            
            self.current_frequency = frequency
            logger.info(f"Frequency set to {frequency/1e6:.2f} MHz")
            return True
            
        except usb.core.USBError as e:
            raise WidebandSDRError(f"Failed to set frequency: {e}")
    
    def get_frequency(self) -> int:
        """
        Get current frequency
        
        Returns:
            int: Current frequency in Hz
        """
        return self.current_frequency
    
    def set_sample_rate(self, sample_rate: int) -> bool:
        """
        Set sample rate
        
        Args:
            sample_rate: Sample rate in samples per second
            
        Returns:
            bool: Success status
            
        Raises:
            WidebandSDRError: If sample rate is out of range
        """
        if not self.is_open:
            raise WidebandSDRError("Device not open")
        
        if not (self.MIN_SAMPLE_RATE <= sample_rate <= self.MAX_SAMPLE_RATE):
            raise WidebandSDRError(f"Sample rate {sample_rate} out of range [{self.MIN_SAMPLE_RATE}, {self.MAX_SAMPLE_RATE}]")
        
        try:
            # Send sample rate command
            cmd = struct.pack('<BI', self.CMD_SET_SAMPLE_RATE, sample_rate)
            self.device.write(self.EP_BULK_OUT, cmd, timeout=1000)
            
            self.current_sample_rate = sample_rate
            logger.info(f"Sample rate set to {sample_rate/1e6:.2f} MSPS")
            return True
            
        except usb.core.USBError as e:
            raise WidebandSDRError(f"Failed to set sample rate: {e}")
    
    def get_sample_rate(self) -> int:
        """
        Get current sample rate
        
        Returns:
            int: Current sample rate in samples per second
        """
        return self.current_sample_rate
    
    def set_gain(self, gain: int) -> bool:
        """
        Set gain
        
        Args:
            gain: Gain in dB (0-63)
            
        Returns:
            bool: Success status
            
        Raises:
            WidebandSDRError: If gain is out of range
        """
        if not self.is_open:
            raise WidebandSDRError("Device not open")
        
        if not (self.MIN_GAIN <= gain <= self.MAX_GAIN):
            raise WidebandSDRError(f"Gain {gain} out of range [{self.MIN_GAIN}, {self.MAX_GAIN}]")
        
        try:
            # Send gain command
            cmd = struct.pack('<BH', self.CMD_SET_GAIN, gain)
            self.device.write(self.EP_BULK_OUT, cmd, timeout=1000)
            
            self.current_gain = gain
            logger.info(f"Gain set to {gain} dB")
            return True
            
        except usb.core.USBError as e:
            raise WidebandSDRError(f"Failed to set gain: {e}")
    
    def get_gain(self) -> int:
        """
        Get current gain
        
        Returns:
            int: Current gain in dB
        """
        return self.current_gain
    
    def start_stream(self, callback: Optional[Callable[[np.ndarray], None]] = None) -> bool:
        """
        Start sample streaming
        
        Args:
            callback: Optional callback function for samples
            
        Returns:
            bool: Success status
            
        Raises:
            SDRStreamError: If streaming cannot be started
        """
        if not self.is_open:
            raise SDRStreamError("Device not open")
        
        if self.streaming_active:
            logger.warning("Streaming already active")
            return True
        
        # Set callback
        if callback:
            self.callback = StreamCallback(callback)
        
        # Start streaming command
        try:
            cmd = struct.pack('<B', self.CMD_START_STREAM)
            self.device.write(self.EP_BULK_OUT, cmd, timeout=1000)
            
            self.streaming_active = True
            
            # Start streaming thread
            self.stream_thread = threading.Thread(
                target=self._streaming_thread,
                daemon=True,
                name="SDRStream"
            )
            self.stream_thread.start()
            
            logger.info("Sample streaming started")
            return True
            
        except usb.core.USBError as e:
            raise SDRStreamError(f"Failed to start streaming: {e}")
    
    def stop_stream(self) -> bool:
        """
        Stop sample streaming
        
        Returns:
            bool: Success status
        """
        if not self.streaming_active:
            logger.warning("Streaming not active")
            return True
        
        try:
            # Stop streaming command
            cmd = struct.pack('<B', self.CMD_STOP_STREAM)
            self.device.write(self.EP_BULK_OUT, cmd, timeout=1000)
            
            self.streaming_active = False
            
            # Wait for stream thread to finish
            if self.stream_thread and self.stream_thread.is_alive():
                self.stream_thread.join(timeout=2.0)
            
            # Clear queues
            while not self.sample_queue.empty():
                try:
                    self.sample_queue.get_nowait()
                except queue.Empty:
                    break
            
            logger.info("Sample streaming stopped")
            return True
            
        except usb.core.USBError as e:
            logger.error(f"Failed to stop streaming: {e}")
            return False
    
    def read_samples(self, count: int = 1024, timeout: float = 1.0) -> Optional[np.ndarray]:
        """
        Read samples from queue (blocking)
        
        Args:
            count: Number of samples to read
            timeout: Timeout in seconds
            
        Returns:
            np.ndarray: Array of samples or None if timeout
        """
        samples = []
        start_time = time.time()
        
        while len(samples) < count:
            if time.time() - start_time > timeout:
                logger.warning(f"Timeout reading {count} samples (got {len(samples)})")
                break
            
            try:
                chunk = self.sample_queue.get(timeout=0.1)
                samples.extend(chunk)
            except queue.Empty:
                continue
        
        if samples:
            result = np.array(samples[:count], dtype=self.sample_format)
            self.stats.samples_processed += len(result)
            return result
        
        return None
    
    def get_statistics(self) -> SDRStats:
        """
        Get SDR statistics
        
        Returns:
            SDRStats: Current statistics
        """
        with self.lock:
            # Update live statistics
            if self.streaming_active:
                try:
                    # Get status from device
                    cmd = struct.pack('<B', self.CMD_GET_STATUS)
                    self.device.write(self.EP_BULK_OUT, cmd, timeout=100)
                    
                    status_data = self.device.read(self.EP_BULK_IN, 64, timeout=100)
                    
                    # Parse status response
                    if len(status_data) >= 16:
                        freq, rate, gain, sample_count = struct.unpack('<IIII', status_data[:16])
                        self.stats.current_frequency = freq
                        self.stats.current_sample_rate = rate
                        self.stats.current_gain = gain
                        
                except usb.core.USBError:
                    pass  # Ignore read errors for statistics
            
            return self.stats
    
    def calibrate(self) -> bool:
        """
        Perform device calibration
        
        Returns:
            bool: Success status
        """
        if not self.is_open:
            raise WidebandSDRError("Device not open")
        
        try:
            cmd = struct.pack('<B', self.CMD_CALIBRATE)
            self.device.write(self.EP_BULK_OUT, cmd, timeout=5000)  # 5 second timeout for calibration
            logger.info("Device calibration initiated")
            return True
            
        except usb.core.USBError as e:
            raise WidebandSDRError(f"Calibration failed: {e}")
    
    def get_device_info(self) -> dict:
        """
        Get device information
        
        Returns:
            dict: Device information dictionary
        """
        if not self.device:
            return {}
        
        info = {
            'vendor_id': f"0x{self.device.idVendor:04x}",
            'product_id': f"0x{self.device.idProduct:04x}",
            'manufacturer': usb.util.get_string(self.device, 256, 1),
            'product': usb.util.get_string(self.device, 256, 2),
            'serial_number': usb.util.get_string(self.device, 256, 3),
            'configuration': self.USB_CONFIGURATION,
            'interface': self.USB_INTERFACE,
        }
        
        return info
    
    def reset_statistics(self):
        """Reset statistics counters"""
        with self.lock:
            self.stats = SDRStats(
                current_frequency=self.current_frequency,
                current_sample_rate=self.current_sample_rate,
                current_gain=self.current_gain
            )
    
    def _initialize_device(self):
        """Initialize device with default settings"""
        # Set default frequency
        self.set_frequency(self.current_frequency)
        
        # Set default sample rate
        self.set_sample_rate(self.current_sample_rate)
        
        # Set default gain
        self.set_gain(self.current_gain)
        
        logger.info("Device initialized with default settings")
    
    def _streaming_thread(self):
        """Background thread for streaming samples"""
        logger.info("Streaming thread started")
        
        try:
            while self.streaming_active:
                try:
                    # Read sample data from bulk IN endpoint
                    data = self.device.read(
                        self.EP_BULK_IN,
                        self.buffer_size * 2,  # 2 bytes per sample
                        timeout=100
                    )
                    
                    if data:
                        # Convert to numpy array
                        samples = np.frombuffer(data, dtype=np.uint16)
                        samples = samples.astype(self.sample_format)
                        
                        # Apply callback if set
                        if self.callback:
                            try:
                                self.callback(samples)
                            except Exception as e:
                                logger.error(f"Callback error: {e}")
                        
                        # Put samples in queue
                        try:
                            self.sample_queue.put_nowait(samples)
                        except queue.Full:
                            # Queue full, drop samples and update stats
                            self.stats.packets_dropped += 1
                            logger.warning("Sample queue full, dropping samples")
                        
                except usb.core.USBError as e:
                    if self.streaming_active:
                        logger.error(f"USB read error during streaming: {e}")
                        self.stats.overrun_count += 1
                    break
                except Exception as e:
                    logger.error(f"Streaming thread error: {e}")
                    break
        
        except Exception as e:
            logger.error(f"Streaming thread fatal error: {e}")
        
        finally:
            self.streaming_active = False
            logger.info("Streaming thread stopped")
    
    def __enter__(self):
        """Context manager entry"""
        self.open()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        self.close()
    
    def __del__(self):
        """Destructor"""
        self.close()


# Convenience functions for quick testing
def list_devices() -> List[dict]:
    """
    List all available Wideband SDR devices
    
    Returns:
        List[dict]: List of device information dictionaries
    """
    devices = []
    
    try:
        found_devices = usb.core.find(
            idVendor=WidebandSDR.USB_VENDOR_ID,
            idProduct=WidebandSDR.USB_PRODUCT_ID,
            find_all=True
        )
        
        if not found_devices:
            return devices
        
        if not isinstance(found_devices, list):
            found_devices = [found_devices]
        
        for i, device in enumerate(found_devices):
            try:
                info = {
                    'index': i,
                    'vendor_id': f"0x{device.idVendor:04x}",
                    'product_id': f"0x{device.idProduct:04x}",
                    'manufacturer': usb.util.get_string(device, 256, 1),
                    'product': usb.util.get_string(device, 256, 2),
                    'serial_number': usb.util.get_string(device, 256, 3),
                }
                devices.append(info)
            except usb.core.USBError:
                # Device might not be accessible
                continue
    
    except Exception as e:
        logger.error(f"Error listing devices: {e}")
    
    return devices


def quick_test():
    """Quick test function"""
    print("Wideband SDR Quick Test")
    print("=======================")
    
    # List devices
    devices = list_devices()
    if not devices:
        print("No Wideband SDR devices found!")
        return
    
    print(f"Found {len(devices)} device(s):")
    for device in devices:
        print(f"  Device {device['index']}: {device['product']}")
        print(f"    Manufacturer: {device['manufacturer']}")
        print(f"    Serial: {device['serial_number']}")
    
    # Test first device
    try:
        with WidebandSDR(device_index=0) as sdr:
            print(f"\nTesting device 0:")
            print(f"  Frequency: {sdr.get_frequency()/1e6:.2f} MHz")
            print(f"  Sample Rate: {sdr.get_sample_rate()/1e6:.2f} MSPS")
            print(f"  Gain: {sdr.get_gain()} dB")
            
            # Change frequency
            sdr.set_frequency(95_500_000)  # 95.5 MHz
            time.sleep(0.5)
            print(f"  New Frequency: {sdr.get_frequency()/1e6:.2f} MHz")
            
            # Get statistics
            stats = sdr.get_statistics()
            print(f"  Samples Processed: {stats.samples_processed}")
            
    except Exception as e:
        print(f"Test failed: {e}")


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        quick_test()
    else:
        print("Wideband SDR Python Driver")
        print("Run 'python wideband_sdr.py test' for quick test")
        print("See documentation for full usage examples")
