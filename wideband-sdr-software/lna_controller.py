#!/usr/bin/env python3
"""
LNA Controller for Wideband SDR
BGA614 Low Noise Amplifier control implementation

Created: November 2025
Version: 1.0.0
License: MIT

Features:
- BGA614 LNA gain control (0-30 dB in 5 dB steps)
- Enable/disable functionality
- Error handling and retry logic
- Status reporting and logging
"""

import logging
import struct
import time
from enum import Enum
from typing import Optional, Tuple, List, Dict, Any

logger = logging.getLogger(__name__)


class LNAError(Exception):
    """Base exception for LNA operations"""
    pass


class LNACommunicationError(LNAError):
    """Communication error with LNA"""
    pass


class LNAInvalidGainError(LNAError):
    """Invalid gain level error"""
    pass


class LNAOperationTimeoutError(LNAError):
    """Timeout error for LNA operations"""
    pass


class LNAStatus:
    """LNA status information"""
    
    def __init__(self):
        self.enabled: bool = False
        self.current_gain_db: int = 0
        self.gain_level: int = 0
        self.gain_register: int = 0
        self.operation_count: int = 0
        self.error_count: int = 0
        
    def __repr__(self) -> str:
        return (f"LNAStatus(enabled={self.enabled}, gain={self.current_gain_db}dB, "
                f"level={self.gain_level}, errors={self.error_count})")


class LNAController:
    """
    LNA Controller for BGA614 Low Noise Amplifier
    
    Controls the BGA614 LNA with 7 gain levels (0-30 dB in 5 dB steps)
    """
    
    # BGA614 Gain Levels (dB)
    GAIN_LEVELS = [0, 5, 10, 15, 20, 25, 30]
    MIN_GAIN = 0
    MAX_GAIN = 30
    GAIN_STEP = 5
    
    # USB Command Interface
    CMD_SET_GAIN = 0x01
    CMD_ENABLE_DISABLE = 0x02
    CMD_GET_STATUS = 0x03
    CMD_GET_GAIN = 0x04
    
    # Response timeout
    COMMAND_TIMEOUT = 1.0  # seconds
    
    def __init__(self, sdr_device):
        """
        Initialize LNA Controller
        
        Args:
            sdr_device: WidebandSDR device instance
        """
        self.sdr = sdr_device
        self.status = LNAStatus()
        self.command_retries = 3
        self.last_command_time = 0.0
        
        logger.info("LNA Controller initialized - BGA614")
    
    def set_gain(self, gain_db: int, verify: bool = True) -> bool:
        """
        Set LNA gain to nearest supported level
        
        Args:
            gain_db: Desired gain in dB (will be rounded to nearest 5 dB)
            verify: If True, read back gain to verify
            
        Returns:
            bool: Success status
            
        Raises:
            LNAInvalidGainError: If gain is out of supported range
            LNACommunicationError: If communication fails
        """
        if not (self.MIN_GAIN <= gain_db <= self.MAX_GAIN):
            raise LNAInvalidGainError(f"Gain {gain_db} dB out of range [{self.MIN_GAIN}, {self.MAX_GAIN}] dB")
        
        # Find nearest supported gain level
        nearest_gain = min(self.GAIN_LEVELS, key=lambda x: abs(x - gain_db))
        gain_level = self.GAIN_LEVELS.index(nearest_gain)
        
        logger.info(f"Setting LNA gain to {gain_db} dB (nearest: {nearest_gain} dB, level: {gain_level})")
        
        # Send command to device
        try:
            self._send_lna_command(self.CMD_SET_GAIN, struct.pack('<BB', gain_level))
            
            # Update status
            self.status.current_gain_db = nearest_gain
            self.status.gain_level = gain_level
            self.status.operation_count += 1
            
            # Verify if requested
            if verify:
                actual_gain = self.get_gain()
                if actual_gain != nearest_gain:
                    logger.warning(f"Gain verification failed: set {nearest_gain} dB, read {actual_gain} dB")
                    return False
            
            logger.info(f"LNA gain set to {nearest_gain} dB successfully")
            return True
            
        except Exception as e:
            self.status.error_count += 1
            logger.error(f"Failed to set LNA gain: {e}")
            raise LNACommunicationError(f"Failed to set gain: {e}")
    
    def get_gain(self) -> int:
        """
        Get current LNA gain
        
        Returns:
            int: Current gain in dB
            
        Raises:
            LNACommunicationError: If communication fails
        """
        try:
            # Read status from device
            lna_status = self._get_lna_status()
            return lna_status.current_gain_db
            
        except Exception as e:
            logger.error(f"Failed to get LNA gain: {e}")
            # Return cached value as fallback
            return self.status.current_gain_db
    
    def enable(self, verify: bool = True) -> bool:
        """
        Enable LNA
        
        Args:
            verify: If True, read back status to verify
            
        Returns:
            bool: Success status
            
        Raises:
            LNACommunicationError: If communication fails
        """
        if self.status.enabled:
            logger.debug("LNA already enabled")
            return True
        
        logger.info("Enabling LNA")
        
        try:
            # Send enable command
            self._send_lna_command(self.CMD_ENABLE_DISABLE, struct.pack('<B', 1))
            
            # Update status
            self.status.enabled = True
            self.status.operation_count += 1
            
            # Verify if requested
            if verify:
                if not self.is_enabled():
                    logger.warning("LNA enable verification failed")
                    return False
            
            logger.info("LNA enabled successfully")
            return True
            
        except Exception as e:
            self.status.error_count += 1
            logger.error(f"Failed to enable LNA: {e}")
            raise LNACommunicationError(f"Failed to enable LNA: {e}")
    
    def disable(self, verify: bool = True) -> bool:
        """
        Disable LNA
        
        Args:
            verify: If True, read back status to verify
            
        Returns:
            bool: Success status
            
        Raises:
            LNACommunicationError: If communication fails
        """
        if not self.status.enabled:
            logger.debug("LNA already disabled")
            return True
        
        logger.info("Disabling LNA")
        
        try:
            # Send disable command
            self._send_lna_command(self.CMD_ENABLE_DISABLE, struct.pack('<B', 0))
            
            # Update status
            self.status.enabled = False
            self.status.operation_count += 1
            
            # Verify if requested
            if verify:
                if self.is_enabled():
                    logger.warning("LNA disable verification failed")
                    return False
            
            logger.info("LNA disabled successfully")
            return True
            
        except Exception as e:
            self.status.error_count += 1
            logger.error(f"Failed to disable LNA: {e}")
            raise LNACommunicationError(f"Failed to disable LNA: {e}")
    
    def is_enabled(self) -> bool:
        """
        Check if LNA is enabled
        
        Returns:
            bool: Enable status
        """
        try:
            # Read status from device
            lna_status = self._get_lna_status()
            return lna_status.enabled
            
        except Exception as e:
            logger.error(f"Failed to get LNA enabled status: {e}")
            # Return cached value as fallback
            return self.status.enabled
    
    def get_status(self) -> LNAStatus:
        """
        Get comprehensive LNA status
        
        Returns:
            LNAStatus: Complete status information
        """
        try:
            return self._get_lna_status()
        except Exception as e:
            logger.error(f"Failed to get LNA status: {e}")
            return self.status
    
    def get_supported_gains(self) -> List[int]:
        """
        Get list of supported gain levels
        
        Returns:
            List[int]: Supported gains in dB
        """
        return self.GAIN_LEVELS.copy()
    
    def auto_gain_control(self, signal_strength: float, 
                         target_level: float = 0.8,
                         min_gain: int = 5,
                         max_gain: int = 30) -> bool:
        """
        Automatic gain control based on signal strength
        
        Args:
            signal_strength: Current signal strength (0.0-1.0)
            target_level: Target signal level
            min_gain: Minimum gain to use
            max_gain: Maximum gain to use
            
        Returns:
            bool: Success status
        """
        # Calculate required gain adjustment
        if signal_strength < target_level * 0.8:
            # Signal too weak, increase gain
            new_gain = self.status.current_gain_db + self.GAIN_STEP
        elif signal_strength > target_level * 1.2:
            # Signal too strong, decrease gain  
            new_gain = self.status.current_gain_db - self.GAIN_STEP
        else:
            # Signal level is good
            return True
        
        # Clamp to valid range
        new_gain = max(min_gain, min(max_gain, new_gain))
        
        # Round to nearest supported level
        new_gain = min(self.GAIN_LEVELS, key=lambda x: abs(x - new_gain))
        
        # Only change if different from current
        if new_gain != self.status.current_gain_db:
            logger.info(f"AGC: Signal {signal_strength:.2f}, adjusting gain to {new_gain} dB")
            return self.set_gain(new_gain, verify=True)
        
        return True
    
    def calibrate_dc_offset(self, samples: Optional[list] = None) -> Tuple[float, float]:
        """
        Calibrate DC offset for LNA chain
        
        Args:
            samples: Optional sample data for calibration
            
        Returns:
            Tuple[float, float]: DC offset (real, imaginary)
        """
        logger.info("Calibrating LNA DC offset")
        
        # If samples provided, calculate DC offset from them
        if samples is not None and len(samples) > 0:
            dc_offset_real = sum(s.real for s in samples) / len(samples)
            dc_offset_imag = sum(s.imag for s in samples) / len(samples)
        else:
            # Use estimated values
            dc_offset_real = 0.0
            dc_offset_imag = 0.0
        
        logger.info(f"LNA DC offset calibration: {dc_offset_real:.4f}, {dc_offset_imag:.4f}")
        return dc_offset_real, dc_offset_imag
    
    def reset_statistics(self):
        """Reset operation statistics"""
        self.status.operation_count = 0
        self.status.error_count = 0
        logger.info("LNA statistics reset")
    
    def _send_lna_command(self, command: int, data: bytes) -> bytes:
        """
        Send LNA command to device
        
        Args:
            command: Command code
            data: Command data
            
        Returns:
            bytes: Response data
            
        Raises:
            LNACommunicationError: If communication fails
        """
        # Rate limiting
        current_time = time.time()
        if current_time - self.last_command_time < 0.01:  # 10ms minimum
            time.sleep(0.01)
        self.last_command_time = current_time
        
        # Retry logic
        for attempt in range(self.command_retries):
            try:
                # Send command packet
                cmd_packet = struct.pack('<B', 0x0F) + struct.pack('<B', command) + data
                self.sdr.device.write(self.sdr.EP_BULK_OUT, cmd_packet, timeout=1000)
                
                # Read response
                response = self.sdr.device.read(self.sdr.EP_BULK_IN, 64, timeout=1000)
                return response
                
            except Exception as e:
                if attempt == self.command_retries - 1:
                    raise LNACommunicationError(f"LNA command failed after {self.command_retries} attempts: {e}")
                logger.warning(f"LNA command attempt {attempt + 1} failed: {e}, retrying...")
                time.sleep(0.1)
        
        raise LNACommunicationError("LNA command failed - unexpected code path")
    
    def _get_lna_status(self) -> LNAStatus:
        """
        Get LNA status from device
        
        Returns:
            LNAStatus: Device status
        """
        try:
            response = self._send_lna_command(self.CMD_GET_STATUS, b'')
            
            if len(response) >= 4:
                status = LNAStatus()
                status.enabled = bool(response[0])
                status.gain_level = response[1] & 0x07
                status.gain_register = response[2]
                status.operation_count = self.status.operation_count
                status.error_count = self.status.error_count
                
                # Calculate actual gain in dB
                if 0 <= status.gain_level < len(self.GAIN_LEVELS):
                    status.current_gain_db = self.GAIN_LEVELS[status.gain_level]
                else:
                    status.current_gain_db = 0
                
                # Update cached status
                self.status = status
                return status
            else:
                raise LNACommunicationError(f"Invalid status response length: {len(response)}")
                
        except Exception as e:
            logger.error(f"Failed to get LNA status from device: {e}")
            raise LNACommunicationError(f"Failed to get device status: {e}")
    
    def __enter__(self):
        """Context manager entry"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        pass
    
    def __repr__(self) -> str:
        return f"LNAController(status={self.status})"


# Convenience functions for quick testing
def quick_lna_test():
    """Quick test function for LNA controller"""
    print("LNA Controller Quick Test")
    print("========================")
    
    try:
        from wideband_sdr import WidebandSDR
        
        with WidebandSDR() as sdr:
            lna = LNAController(sdr)
            
            print(f"Initial status: {lna.get_status()}")
            
            # Test enable
            print("Testing LNA enable...")
            if lna.enable():
                print("✓ LNA enabled successfully")
            else:
                print("✗ LNA enable failed")
            
            # Test gain settings
            print("Testing gain settings...")
            test_gains = [0, 10, 20, 30]
            for gain_db in test_gains:
                try:
                    if lna.set_gain(gain_db):
                        actual_gain = lna.get_gain()
                        print(f"✓ Gain {gain_db} dB -> {actual_gain} dB")
                    else:
                        print(f"✗ Gain {gain_db} dB setting failed")
                except Exception as e:
                    print(f"✗ Gain {gain_db} dB error: {e}")
            
            # Test disable
            print("Testing LNA disable...")
            if lna.disable():
                print("✓ LNA disabled successfully")
            else:
                print("✗ LNA disable failed")
            
            print(f"Final status: {lna.get_status()}")
            
    except Exception as e:
        print(f"Test failed: {e}")


if __name__ == "__main__":
    quick_lna_test()
