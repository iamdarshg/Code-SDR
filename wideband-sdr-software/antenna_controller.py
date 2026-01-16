#!/usr/bin/env python3
"""
Antenna Controller for Wideband SDR
Multi-antenna switching and diversity control implementation

Created: November 2025
Version: 1.0.0
License: MIT

Features:
- Primary/Secondary antenna selection
- Combined antenna mode
- Diversity reception with automatic switching
- Signal strength monitoring
- Status tracking and error recovery
"""

import logging
import struct
import time
from enum import IntEnum
from typing import Optional, Tuple, List, Dict, Any, Union

logger = logging.getLogger(__name__)


class AntennaError(Exception):
    """Base exception for antenna operations"""
    pass


class AntennaCommunicationError(AntennaError):
    """Communication error with antenna controller"""
    pass


class AntennaInvalidModeError(AntennaError):
    """Invalid antenna mode error"""
    pass


class AntennaOperationTimeoutError(AntennaError):
    """Timeout error for antenna operations"""
    pass


class AntennaMode(IntEnum):
    """Antenna selection modes"""
    PRIMARY = 0
    SECONDARY = 1
    COMBINED = 2
    DIVERSITY = 3


class DiversityStatus:
    """Diversity reception status information"""
    
    def __init__(self):
        self.auto_selection_enabled: bool = False
        self.primary_signal_strength: int = 0
        self.secondary_signal_strength: int = 0
        self.switch_threshold: int = 100
        self.switch_hysteresis: int = 10
        self.switch_count: int = 0
        self.last_switch_time: float = 0.0
        self.current_best_antenna: AntennaMode = AntennaMode.PRIMARY
        
    def __repr__(self) -> str:
        return (f"DiversityStatus(auto={self.auto_selection_enabled}, "
                f"primary={self.primary_signal_strength}, "
                f"secondary={self.secondary_signal_strength}, "
                f"switches={self.switch_count})")


class AntennaStatus:
    """Comprehensive antenna status information"""
    
    def __init__(self):
        self.enabled: bool = False
        self.current_mode: AntennaMode = AntennaMode.PRIMARY
        self.control_register: int = 0
        self.diversity: DiversityStatus = DiversityStatus()
        self.operation_count: int = 0
        self.error_count: int = 0
        
    def __repr__(self) -> str:
        return (f"AntennaStatus(enabled={self.enabled}, "
                f"mode={self.current_mode.name}, "
                f"diversity={self.diversity}, "
                f"errors={self.error_count})")


class AntennaController:
    """
    Antenna Controller for multi-antenna switching
    
    Controls antenna selection between primary, secondary, combined modes,
    and automatic diversity selection based on signal strength
    """
    
    # USB Command Interface
    CMD_SET_MODE = 0x01
    CMD_ENABLE_DISABLE = 0x02
    CMD_GET_STATUS = 0x03
    CMD_SET_DIVERSITY = 0x04
    CMD_UPDATE_SIGNAL_STRENGTH = 0x05
    
    # Response timeout
    COMMAND_TIMEOUT = 1.0  # seconds
    
    # Signal strength bounds
    MIN_SIGNAL_STRENGTH = 0
    MAX_SIGNAL_STRENGTH = 65535
    
    def __init__(self, sdr_device):
        """
        Initialize Antenna Controller
        
        Args:
            sdr_device: WidebandSDR device instance
        """
        self.sdr = sdr_device
        self.status = AntennaStatus()
        self.command_retries = 3
        self.last_command_time = 0.0
        self.diversity_update_interval = 1.0  # seconds
        
        logger.info("Antenna Controller initialized - Multi-antenna support")
    
    def set_mode(self, mode: Union[AntennaMode, int], verify: bool = True) -> bool:
        """
        Set antenna mode
        
        Args:
            mode: AntennaMode or integer mode (0=Primary, 1=Secondary, 2=Combined, 3=Diversity)
            verify: If True, read back mode to verify
            
        Returns:
            bool: Success status
            
        Raises:
            AntennaInvalidModeError: If mode is invalid
            AntennaCommunicationError: If communication fails
        """
        # Convert to AntennaMode if needed
        if isinstance(mode, int):
            try:
                mode = AntennaMode(mode)
            except ValueError:
                raise AntennaInvalidModeError(f"Invalid antenna mode: {mode}")
        
        # Validate mode
        if not isinstance(mode, AntennaMode):
            raise AntennaInvalidModeError(f"Invalid antenna mode type: {type(mode)}")
        
        logger.info(f"Setting antenna mode to {mode.name}")
        
        # Send command to device
        try:
            self._send_antenna_command(self.CMD_SET_MODE, struct.pack('<B', int(mode)))
            
            # Update status
            self.status.current_mode = mode
            self.status.control_register = int(mode) & 0x03
            self.status.operation_count += 1
            
            # Verify if requested
            if verify:
                actual_mode = self.get_mode()
                if actual_mode != mode:
                    logger.warning(f"Mode verification failed: set {mode.name}, read {actual_mode.name}")
                    return False
            
            logger.info(f"Antenna mode set to {mode.name} successfully")
            return True
            
        except Exception as e:
            self.status.error_count += 1
            logger.error(f"Failed to set antenna mode: {e}")
            raise AntennaCommunicationError(f"Failed to set mode: {e}")
    
    def get_mode(self) -> AntennaMode:
        """
        Get current antenna mode
        
        Returns:
            AntennaMode: Current mode
            
        Raises:
            AntennaCommunicationError: If communication fails
        """
        try:
            # Read status from device
            antenna_status = self._get_antenna_status()
            return antenna_status.current_mode
            
        except Exception as e:
            logger.error(f"Failed to get antenna mode: {e}")
            # Return cached value as fallback
            return self.status.current_mode
    
    def enable(self, verify: bool = True) -> bool:
        """
        Enable antenna control
        
        Args:
            verify: If True, read back status to verify
            
        Returns:
            bool: Success status
            
        Raises:
            AntennaCommunicationError: If communication fails
        """
        if self.status.enabled:
            logger.debug("Antenna control already enabled")
            return True
        
        logger.info("Enabling antenna control")
        
        try:
            # Send enable command
            self._send_antenna_command(self.CMD_ENABLE_DISABLE, struct.pack('<B', 1))
            
            # Update status
            self.status.enabled = True
            self.status.operation_count += 1
            
            # Verify if requested
            if verify:
                if not self.is_enabled():
                    logger.warning("Antenna enable verification failed")
                    return False
            
            logger.info("Antenna control enabled successfully")
            return True
            
        except Exception as e:
            self.status.error_count += 1
            logger.error(f"Failed to enable antenna control: {e}")
            raise AntennaCommunicationError(f"Failed to enable antenna control: {e}")
    
    def disable(self, verify: bool = True) -> bool:
        """
        Disable antenna control
        
        Args:
            verify: If True, read back status to verify
            
        Returns:
            bool: Success status
            
        Raises:
            AntennaCommunicationError: If communication fails
        """
        if not self.status.enabled:
            logger.debug("Antenna control already disabled")
            return True
        
        logger.info("Disabling antenna control")
        
        try:
            # Send disable command
            self._send_antenna_command(self.CMD_ENABLE_DISABLE, struct.pack('<B', 0))
            
            # Update status
            self.status.enabled = False
            self.status.operation_count += 1
            
            # Verify if requested
            if verify:
                if self.is_enabled():
                    logger.warning("Antenna disable verification failed")
                    return False
            
            logger.info("Antenna control disabled successfully")
            return True
            
        except Exception as e:
            self.status.error_count += 1
            logger.error(f"Failed to disable antenna control: {e}")
            raise AntennaCommunicationError(f"Failed to disable antenna control: {e}")
    
    def is_enabled(self) -> bool:
        """
        Check if antenna control is enabled
        
        Returns:
            bool: Enable status
        """
        try:
            # Read status from device
            antenna_status = self._get_antenna_status()
            return antenna_status.enabled
            
        except Exception as e:
            logger.error(f"Failed to get antenna enabled status: {e}")
            # Return cached value as fallback
            return self.status.enabled
    
    def select_primary(self, verify: bool = True) -> bool:
        """
        Select primary antenna
        
        Args:
            verify: If True, read back mode to verify
            
        Returns:
            bool: Success status
        """
        return self.set_mode(AntennaMode.PRIMARY, verify=verify)
    
    def select_secondary(self, verify: bool = True) -> bool:
        """
        Select secondary antenna
        
        Args:
            verify: If True, read back mode to verify
            
        Returns:
            bool: Success status
        """
        return self.set_mode(AntennaMode.SECONDARY, verify=verify)
    
    def select_combined(self, verify: bool = True) -> bool:
        """
        Select combined antenna mode (both antennas)
        
        Args:
            verify: If True, read back mode to verify
            
        Returns:
            bool: Success status
        """
        return self.set_mode(AntennaMode.COMBINED, verify=verify)
    
    def enable_diversity(self, verify: bool = True) -> bool:
        """
        Enable diversity mode with automatic antenna selection
        
        Args:
            verify: If True, read back mode to verify
            
        Returns:
            bool: Success status
        """
        # First enable diversity mode
        try:
            self._send_antenna_command(self.CMD_SET_DIVERSITY, struct.pack('<B', 1))
            self.status.diversity.auto_selection_enabled = True
        except Exception as e:
            logger.error(f"Failed to enable diversity mode: {e}")
            return False
        
        # Then set diversity mode
        return self.set_mode(AntennaMode.DIVERSITY, verify=verify)
    
    def disable_diversity(self, verify: bool = True) -> bool:
        """
        Disable diversity mode
        
        Args:
            verify: If True, read back mode to verify
            
        Returns:
            bool: Success status
        """
        # Disable diversity mode
        try:
            self._send_antenna_command(self.CMD_SET_DIVERSITY, struct.pack('<B', 0))
            self.status.diversity.auto_selection_enabled = False
        except Exception as e:
            logger.error(f"Failed to disable diversity mode: {e}")
            return False
        
        # Set back to primary mode
        return self.set_mode(AntennaMode.PRIMARY, verify=verify)
    
    def update_signal_strength(self, primary_strength: int, secondary_strength: int) -> bool:
        """
        Update signal strength for diversity processing
        
        Args:
            primary_strength: Primary antenna signal strength (0-65535)
            secondary_strength: Secondary antenna signal strength (0-65535)
            
        Returns:
            bool: Success status
        """
        # Validate input
        primary_strength = max(self.MIN_SIGNAL_STRENGTH, min(self.MAX_SIGNAL_STRENGTH, primary_strength))
        secondary_strength = max(self.MIN_SIGNAL_STRENGTH, min(self.MAX_SIGNAL_STRENGTH, secondary_strength))
        
        # Update internal status
        self.status.diversity.primary_signal_strength = primary_strength
        self.status.diversity.secondary_signal_strength = secondary_strength
        
        # Send to device for hardware diversity processing
        try:
            data = struct.pack('<HH', primary_strength, secondary_strength)
            self._send_antenna_command(self.CMD_UPDATE_SIGNAL_STRENGTH, data)
            
            # Process diversity if enabled
            if (self.status.current_mode == AntennaMode.DIVERSITY and 
                self.status.diversity.auto_selection_enabled):
                self._process_diversity()
            
            return True
            
        except Exception as e:
            self.status.error_count += 1
            logger.error(f"Failed to update signal strength: {e}")
            return False
    
    def get_status(self) -> AntennaStatus:
        """
        Get comprehensive antenna status
        
        Returns:
            AntennaStatus: Complete status information
        """
        try:
            return self._get_antenna_status()
        except Exception as e:
            logger.error(f"Failed to get antenna status: {e}")
            return self.status
    
    def get_supported_modes(self) -> List[AntennaMode]:
        """
        Get list of supported antenna modes
        
        Returns:
            List[AntennaMode]: Supported modes
        """
        return list(AntennaMode)
    
    def _process_diversity(self):
        """Process diversity selection logic"""
        primary = self.status.diversity.primary_signal_strength
        secondary = self.status.diversity.secondary_signal_strength
        threshold = self.status.diversity.switch_threshold
        hysteresis = self.status.diversity.switch_hysteresis
        
        # Determine which antenna has better signal
        if primary > secondary:
            # Primary is better
            if (self.status.current_mode == AntennaMode.SECONDARY and 
                primary > (secondary + hysteresis) and 
                abs(primary - secondary) > threshold):
                self.select_primary(verify=False)
                self._record_diversity_switch()
                
        elif secondary > primary:
            # Secondary is better
            if (self.status.current_mode == AntennaMode.PRIMARY and 
                secondary > (primary + hysteresis) and 
                abs(primary - secondary) > threshold):
                self.select_secondary(verify=False)
                self._record_diversity_switch()
    
    def _record_diversity_switch(self):
        """Record diversity switch event"""
        self.status.diversity.switch_count += 1
        self.status.diversity.last_switch_time = time.time()
        
        logger.debug(f"Diversity switch #{self.status.diversity.switch_count} to {self.status.current_mode.name}")
    
    def reset_statistics(self):
        """Reset operation statistics"""
        self.status.operation_count = 0
        self.status.error_count = 0
        self.status.diversity.switch_count = 0
        logger.info("Antenna statistics reset")
    
    def set_diversity_parameters(self, threshold: int = 100, hysteresis: int = 10):
        """
        Set diversity control parameters
        
        Args:
            threshold: Signal strength difference required for switching
            hysteresis: Hysteresis to prevent rapid switching
        """
        self.status.diversity.switch_threshold = max(0, min(65535, threshold))
        self.status.diversity.switch_hysteresis = max(0, min(32767, hysteresis))
        
        logger.info(f"Diversity parameters set: threshold={self.status.diversity.switch_threshold}, "
                   f"hysteresis={self.status.diversity.switch_hysteresis}")
    
    def _send_antenna_command(self, command: int, data: bytes) -> bytes:
        """
        Send antenna command to device
        
        Args:
            command: Command code
            data: Command data
            
        Returns:
            bytes: Response data
            
        Raises:
            AntennaCommunicationError: If communication fails
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
                cmd_packet = struct.pack('<B', 0x10) + struct.pack('<B', command) + data
                self.sdr.device.write(self.sdr.EP_BULK_OUT, cmd_packet, timeout=1000)
                
                # Read response
                response = self.sdr.device.read(self.sdr.EP_BULK_IN, 64, timeout=1000)
                return response
                
            except Exception as e:
                if attempt == self.command_retries - 1:
                    raise AntennaCommunicationError(f"Antenna command failed after {self.command_retries} attempts: {e}")
                logger.warning(f"Antenna command attempt {attempt + 1} failed: {e}, retrying...")
                time.sleep(0.1)
        
        raise AntennaCommunicationError("Antenna command failed - unexpected code path")
    
    def _get_antenna_status(self) -> AntennaStatus:
        """
        Get antenna status from device
        
        Returns:
            AntennaStatus: Device status
        """
        try:
            response = self._send_antenna_command(self.CMD_GET_STATUS, b'')
            
            if len(response) >= 8:
                status = AntennaStatus()
                status.enabled = bool(response[0])
                status.current_mode = AntennaMode(response[1] & 0x03)
                status.control_register = response[2]
                status.diversity.auto_selection_enabled = bool(response[3])
                status.diversity.switch_count = response[4] | (response[5] << 8)
                status.diversity.primary_signal_strength = response[6] | (response[7] << 8)
                status.diversity.secondary_signal_strength = 0  # Not in current response format
                status.operation_count = self.status.operation_count
                status.error_count = self.status.error_count
                
                # Update cached status
                self.status = status
                return status
            else:
                raise AntennaCommunicationError(f"Invalid status response length: {len(response)}")
                
        except Exception as e:
            logger.error(f"Failed to get antenna status from device: {e}")
            raise AntennaCommunicationError(f"Failed to get device status: {e}")
    
    def __enter__(self):
        """Context manager entry"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        pass
    
    def __repr__(self) -> str:
        return f"AntennaController(status={self.status})"


# Convenience functions for quick testing
def quick_antenna_test():
    """Quick test function for antenna controller"""
    print("Antenna Controller Quick Test")
    print("=============================")
    
    try:
        from wideband_sdr import WidebandSDR
        
        with WidebandSDR() as sdr:
            antenna = AntennaController(sdr)
            
            print(f"Initial status: {antenna.get_status()}")
            
            # Test enable
            print("Testing antenna control enable...")
            if antenna.enable():
                print("✓ Antenna control enabled successfully")
            else:
                print("✗ Antenna control enable failed")
            
            # Test modes
            print("Testing antenna modes...")
            test_modes = [AntennaMode.PRIMARY, AntennaMode.SECONDARY, 
                         AntennaMode.COMBINED, AntennaMode.DIVERSITY]
            
            for mode in test_modes:
                try:
                    if antenna.set_mode(mode):
                        actual_mode = antenna.get_mode()
                        print(f"✓ Mode {mode.name} -> {actual_mode.name}")
                    else:
                        print(f"✗ Mode {mode.name} setting failed")
                except Exception as e:
                    print(f"✗ Mode {mode.name} error: {e}")
            
            # Test diversity with signal strength
            print("Testing diversity with signal strength...")
            antenna.update_signal_strength(150, 100)
            time.sleep(1.0)
            antenna.update_signal_strength(80, 120)
            
            # Test disable
            print("Testing antenna control disable...")
            if antenna.disable():
                print("✓ Antenna control disabled successfully")
            else:
                print("✗ Antenna control disable failed")
            
            print(f"Final status: {antenna.get_status()}")
            
    except Exception as e:
        print(f"Test failed: {e}")


if __name__ == "__main__":
    quick_antenna_test()
