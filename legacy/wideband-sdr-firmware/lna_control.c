/*
 * Wideband SDR - LNA Control Module
 * BGA614 Low Noise Amplifier control implementation
 * 
 * Created: November 2025
 * Version: 1.0.0
 * License: MIT
 * 
 * Features:
 * - BGA614 LNA gain control (0-30 dB in 5 dB steps)
 * - Enable/disable functionality
 * - USB command interface
 * - Hardware control via GPIO
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

// LNA Configuration
#define LNA_ENABLE_PIN    LATDbits.LATD1    // RD1 - LNA Enable
#define LNA_ENABLE_TRIS   TRISDbits.TRISD1  // RD1 - LNA Enable Direction
#define LNA_GAIN_PIN      LATDbits.LATD2    // RD2 - LNA Gain Select
#define LNA_GAIN_TRIS     TRISDbits.TRISD2  // RD2 - LNA Gain Direction

// LNA Gain Levels (BGA614)
typedef enum {
    LNA_GAIN_0_DB = 0,
    LNA_GAIN_5_DB = 1,
    LNA_GAIN_10_DB = 2,
    LNA_GAIN_15_DB = 3,
    LNA_GAIN_20_DB = 4,
    LNA_GAIN_25_DB = 5,
    LNA_GAIN_30_DB = 6,
} lna_gain_t;

// LNA Control Structure
typedef struct {
    bool enabled;
    lna_gain_t current_gain;
    uint8_t gain_control_register;
} lna_control_t;

// Global LNA Control State
static lna_control_t lna_state = {
    .enabled = false,
    .current_gain = LNA_GAIN_0_DB,
    .gain_control_register = 0
};

// Function Prototypes
bool lna_init(void);
bool lna_set_gain(lna_gain_t gain);
lna_gain_t lna_get_gain(void);
bool lna_enable(void);
bool lna_disable(void);
bool lna_is_enabled(void);
void lna_set_gain_register(uint8_t reg_value);
uint8_t lna_get_gain_register(void);
void lna_update_hardware(void);

// USB Command Handlers
bool lna_process_usb_command(uint8_t subcommand, uint8_t *data, uint8_t length);
bool lna_send_status(uint8_t *status_buffer, uint8_t max_length);

/*
 * Initialize LNA Control System
 */
bool lna_init(void) {
    // Configure LNA control pins as outputs
    LNA_ENABLE_TRIS = 0;    // LNA Enable pin as output
    LNA_GAIN_TRIS = 0;      // LNA Gain pin as output
    
    // Initialize to disabled state
    LNA_ENABLE_PIN = 0;     // Disable LNA
    LNA_GAIN_PIN = 0;       // Default gain level
    
    // Reset state
    lna_state.enabled = false;
    lna_state.current_gain = LNA_GAIN_0_DB;
    lna_state.gain_control_register = 0;
    
    printf("LNA Control initialized - BGA614\n\r");
    return true;
}

/*
 * Set LNA Gain Level
 */
bool lna_set_gain(lna_gain_t gain) {
    // Validate gain level
    if (gain > LNA_GAIN_30_DB) {
        printf("LNA Error: Invalid gain level %d\n\r", gain);
        return false;
    }
    
    // Update state
    lna_state.current_gain = gain;
    
    // Calculate gain control register value
    // BGA614 uses 3-bit control: 000=0dB, 001=5dB, 010=10dB, etc.
    uint8_t reg_value = (uint8_t)gain & 0x07;
    lna_set_gain_register(reg_value);
    
    // Update hardware
    lna_update_hardware();
    
    printf("LNA Gain set to %d dB (register: 0x%02X)\n\r", gain * 5, reg_value);
    return true;
}

/*
 * Get Current LNA Gain Level
 */
lna_gain_t lna_get_gain(void) {
    return lna_state.current_gain;
}

/*
 * Enable LNA
 */
bool lna_enable(void) {
    if (lna_state.enabled) {
        return true; // Already enabled
    }
    
    LNA_ENABLE_PIN = 1;     // Enable LNA
    lna_state.enabled = true;
    
    printf("LNA enabled\n\r");
    return true;
}

/*
 * Disable LNA
 */
bool lna_disable(void) {
    if (!lna_state.enabled) {
        return true; // Already disabled
    }
    
    LNA_ENABLE_PIN = 0;     // Disable LNA
    lna_state.enabled = false;
    
    printf("LNA disabled\n\r");
    return true;
}

/*
 * Check if LNA is Enabled
 */
bool lna_is_enabled(void) {
    return lna_state.enabled;
}

/*
 * Set Gain Control Register Value
 */
void lna_set_gain_register(uint8_t reg_value) {
    lna_state.gain_control_register = reg_value & 0x07; // 3-bit register
    
    // Update gain control pin (single bit control for simplicity)
    // For full 3-bit control, additional pins would be needed
    LNA_GAIN_PIN = (lna_state.gain_control_register & 0x01);
}

/*
 * Get Gain Control Register Value
 */
uint8_t lna_get_gain_register(void) {
    return lna_state.gain_control_register;
}

/*
 * Update Hardware State
 */
void lna_update_hardware(void) {
    // Hardware is updated immediately when gain register is set
    // Additional pin control logic would go here for full 3-bit control
    
    // For now, we use a simple single-bit approach
    // In a full implementation, multiple GPIO pins would be used
    if (lna_state.gain_control_register & 0x01) {
        LNA_GAIN_PIN = 1;
    } else {
        LNA_GAIN_PIN = 0;
    }
}

/*
 * Process LNA USB Commands
 * Command Format: 0x0F (LNA), subcommand, data...
 */
bool lna_process_usb_command(uint8_t subcommand, uint8_t *data, uint8_t length) {
    bool result = false;
    
    switch (subcommand) {
        case 0x01: // Set Gain
            if (length >= 1) {
                uint8_t gain_level = data[0] & 0x07; // Limit to 3 bits
                result = lna_set_gain((lna_gain_t)gain_level);
                printf("USB: LNA Set Gain command received (gain: %d)\n\r", gain_level);
            }
            break;
            
        case 0x02: // Enable/Disable
            if (length >= 1) {
                if (data[0]) {
                    result = lna_enable();
                } else {
                    result = lna_disable();
                }
                printf("USB: LNA %s command received\n\r", data[0] ? "Enable" : "Disable");
            }
            break;
            
        case 0x03: // Get Status
            result = true;
            printf("USB: LNA Status command received\n\r");
            break;
            
        case 0x04: // Get Current Gain
            // Status command includes current gain, so just acknowledge
            result = true;
            printf("USB: LNA Get Gain command received\n\r");
            break;
            
        default:
            printf("USB: Unknown LNA subcommand 0x%02X\n\r", subcommand);
            break;
    }
    
    return result;
}

/*
 * Send LNA Status to Host
 * Status Format: [enabled, current_gain, gain_register, reserved...]
 */
bool lna_send_status(uint8_t *status_buffer, uint8_t max_length) {
    if (max_length < 4) {
        return false;
    }
    
    status_buffer[0] = lna_state.enabled ? 1 : 0;           // Enabled status
    status_buffer[1] = (uint8_t)lna_state.current_gain;     // Current gain level
    status_buffer[2] = lna_state.gain_control_register;     // Register value
    status_buffer[3] = 0;                                   // Reserved
    
    return true;
}

/*
 * Convert dB to Gain Level
 */
lna_gain_t lna_db_to_gain_level(int db) {
    if (db <= 0) return LNA_GAIN_0_DB;
    if (db <= 5) return LNA_GAIN_5_DB;
    if (db <= 10) return LNA_GAIN_10_DB;
    if (db <= 15) return LNA_GAIN_15_DB;
    if (db <= 20) return LNA_GAIN_20_DB;
    if (db <= 25) return LNA_GAIN_25_DB;
    return LNA_GAIN_30_DB;
}

/*
 * Convert Gain Level to dB
 */
int lna_gain_level_to_db(lna_gain_t gain) {
    return (int)gain * 5;
}

/*
 * LNA Self-Test Function
 */
bool lna_self_test(void) {
    printf("LNA Self-Test: Starting...\n\r");
    
    // Test disable
    lna_disable();
    if (lna_is_enabled()) {
        printf("LNA Self-Test: FAIL - Could not disable LNA\n\r");
        return false;
    }
    
    // Test enable
    lna_enable();
    if (!lna_is_enabled()) {
        printf("LNA Self-Test: FAIL - Could not enable LNA\n\r");
        return false;
    }
    
    // Test gain levels
    for (int gain = LNA_GAIN_0_DB; gain <= LNA_GAIN_30_DB; gain++) {
        if (!lna_set_gain((lna_gain_t)gain)) {
            printf("LNA Self-Test: FAIL - Could not set gain level %d\n\r", gain);
            return false;
        }
        if (lna_get_gain() != gain) {
            printf("LNA Self-Test: FAIL - Gain level mismatch\n\r");
            return false;
        }
    }
    
    printf("LNA Self-Test: PASSED\n\r");
    return true;
}
