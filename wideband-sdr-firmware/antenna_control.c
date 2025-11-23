/*
 * Wideband SDR - Antenna Control Module
 * Antenna switching and diversity control implementation
 * 
 * Created: November 2025
 * Version: 1.0.0
 * License: MIT
 * 
 * Features:
 * - Primary/Secondary antenna selection
 * - Combined antenna mode
 * - Diversity reception modes
 * - USB command interface
 * - Hardware control via GPIO
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

// Antenna Control Configuration
#define ANTENNA_SELECT_PIN  LATDbits.LATD3    // RD3 - Antenna Select
#define ANTENNA_SELECT_TRIS TRISDbits.TRISD3  // RD3 - Antenna Select Direction
#define ANTENNA_ENABLE_PIN  LATDbits.LATD4    // RD4 - Antenna Enable
#define ANTENNA_ENABLE_TRIS TRISDbits.TRISD4  // RD4 - Antenna Enable Direction

// Antenna Modes
typedef enum {
    ANTENNA_PRIMARY = 0,     // Primary antenna only
    ANTENNA_SECONDARY = 1,   // Secondary antenna only
    ANTENNA_COMBINED = 2,    // Both antennas combined
    ANTENNA_DIVERSITY = 3    // Diversity mode (automatic selection)
} antenna_mode_t;

// Diversity Control Structure
typedef struct {
    bool auto_selection;
    uint16_t signal_strength_primary;
    uint16_t signal_strength_secondary;
    uint16_t switch_threshold;
    uint16_t switch_hysteresis;
    uint32_t switch_counter;
    uint32_t last_switch_time;
} diversity_control_t;

// Antenna Control Structure
typedef struct {
    antenna_mode_t current_mode;
    bool enabled;
    diversity_control_t diversity;
    uint8_t control_register;
} antenna_control_t;

// Global Antenna Control State
static antenna_control_t antenna_state = {
    .current_mode = ANTENNA_PRIMARY,
    .enabled = false,
    .diversity = {
        .auto_selection = false,
        .signal_strength_primary = 0,
        .signal_strength_secondary = 0,
        .switch_threshold = 100,
        .switch_hysteresis = 10,
        .switch_counter = 0,
        .last_switch_time = 0
    },
    .control_register = 0
};

// Function Prototypes
bool antenna_init(void);
bool antenna_set_mode(antenna_mode_t mode);
antenna_mode_t antenna_get_mode(void);
bool antenna_enable(void);
bool antenna_disable(void);
bool antenna_is_enabled(void);
bool antenna_select_primary(void);
bool antenna_select_secondary(void);
bool antenna_select_combined(void);
bool antenna_set_diversity(bool enabled);

// Diversity Functions
bool antenna_diversity_update_signal_strength(uint16_t primary, uint16_t secondary);
void antenna_diversity_process(void);
bool antenna_diversity_should_switch(void);

// USB Command Handlers
bool antenna_process_usb_command(uint8_t subcommand, uint8_t *data, uint8_t length);
bool antenna_send_status(uint8_t *status_buffer, uint8_t max_length);

// Helper Functions
void antenna_update_hardware(void);
uint8_t antenna_mode_to_register(antenna_mode_t mode);
antenna_mode_t antenna_register_to_mode(uint8_t reg_value);

/*
 * Initialize Antenna Control System
 */
bool antenna_init(void) {
    // Configure antenna control pins as outputs
    ANTENNA_SELECT_TRIS = 0;   // Antenna Select pin as output
    ANTENNA_ENABLE_TRIS = 0;   // Antenna Enable pin as output
    
    // Initialize to disabled state
    ANTENNA_ENABLE_PIN = 0;    // Disable antenna switching
    ANTENNA_SELECT_PIN = 0;    // Default to primary
    
    // Reset state
    antenna_state.enabled = false;
    antenna_state.current_mode = ANTENNA_PRIMARY;
    antenna_state.control_register = 0;
    
    // Initialize diversity control
    antenna_state.diversity.auto_selection = false;
    antenna_state.diversity.signal_strength_primary = 0;
    antenna_state.diversity.signal_strength_secondary = 0;
    antenna_state.diversity.switch_counter = 0;
    antenna_state.diversity.last_switch_time = 0;
    
    printf("Antenna Control initialized - Multi-antenna support\n\r");
    return true;
}

/*
 * Set Antenna Mode
 */
bool antenna_set_mode(antenna_mode_t mode) {
    // Validate mode
    if (mode > ANTENNA_DIVERSITY) {
        printf("Antenna Error: Invalid mode %d\n\r", mode);
        return false;
    }
    
    // Update state
    antenna_state.current_mode = mode;
    antenna_state.control_register = antenna_mode_to_register(mode);
    
    // Update hardware
    antenna_update_hardware();
    
    const char* mode_names[] = {"Primary", "Secondary", "Combined", "Diversity"};
    printf("Antenna mode set to %s (register: 0x%02X)\n\r", 
           mode_names[mode], antenna_state.control_register);
    
    return true;
}

/*
 * Get Current Antenna Mode
 */
antenna_mode_t antenna_get_mode(void) {
    return antenna_state.current_mode;
}

/*
 * Enable Antenna Control
 */
bool antenna_enable(void) {
    if (antenna_state.enabled) {
        return true; // Already enabled
    }
    
    ANTENNA_ENABLE_PIN = 1;    // Enable antenna switching
    antenna_state.enabled = true;
    
    printf("Antenna control enabled\n\r");
    return true;
}

/*
 * Disable Antenna Control
 */
bool antenna_disable(void) {
    if (!antenna_state.enabled) {
        return true; // Already disabled
    }
    
    ANTENNA_ENABLE_PIN = 0;    // Disable antenna switching
    antenna_state.enabled = false;
    
    printf("Antenna control disabled\n\r");
    return true;
}

/*
 * Check if Antenna Control is Enabled
 */
bool antenna_is_enabled(void) {
    return antenna_state.enabled;
}

/*
 * Select Primary Antenna
 */
bool antenna_select_primary(void) {
    return antenna_set_mode(ANTENNA_PRIMARY);
}

/*
 * Select Secondary Antenna
 */
bool antenna_select_secondary(void) {
    return antenna_set_mode(ANTENNA_SECONDARY);
}

/*
 * Select Combined Antenna Mode
 */
bool antenna_select_combined(void) {
    return antenna_set_mode(ANTENNA_COMBINED);
}

/*
 * Set Diversity Mode
 */
bool antenna_set_diversity(bool enabled) {
    antenna_state.diversity.auto_selection = enabled;
    
    if (enabled) {
        return antenna_set_mode(ANTENNA_DIVERSITY);
    } else {
        return antenna_set_mode(ANTENNA_PRIMARY); // Default back to primary
    }
}

/*
 * Update Signal Strength for Diversity
 */
bool antenna_diversity_update_signal_strength(uint16_t primary, uint16_t secondary) {
    antenna_state.diversity.signal_strength_primary = primary;
    antenna_state.diversity.signal_strength_secondary = secondary;
    
    return true;
}

/*
 * Process Diversity Selection
 */
void antenna_diversity_process(void) {
    if (!antenna_state.enabled || 
        antenna_state.current_mode != ANTENNA_DIVERSITY ||
        !antenna_state.diversity.auto_selection) {
        return;
    }
    
    uint16_t primary = antenna_state.diversity.signal_strength_primary;
    uint16_t secondary = antenna_state.diversity.signal_strength_secondary;
    uint16_t threshold = antenna_state.diversity.switch_threshold;
    uint16_t hysteresis = antenna_state.diversity.switch_hysteresis;
    
    // Determine which antenna has better signal
    antenna_mode_t current_selection = ANTENNA_PRIMARY;
    bool should_switch = false;
    
    if (primary > secondary) {
        current_selection = ANTENNA_PRIMARY;
        should_switch = (antenna_state.current_mode == ANTENNA_SECONDARY) &&
                       (primary > (secondary + hysteresis));
    } else {
        current_selection = ANTENNA_SECONDARY;
        should_switch = (antenna_state.current_mode == ANTENNA_PRIMARY) &&
                       (secondary > (primary + hysteresis));
    }
    
    // Only switch if signal difference exceeds threshold
    if (should_switch && abs((int)primary - (int)secondary) > threshold) {
        antenna_state.diversity.switch_counter++;
        antenna_state.diversity.last_switch_time = (uint32_t)time(NULL);
        
        antenna_set_mode(current_selection);
        
        printf("Diversity: Switched to %s antenna (Primary: %d, Secondary: %d)\n\r",
               current_selection == ANTENNA_PRIMARY ? "Primary" : "Secondary",
               primary, secondary);
    }
}

/*
 * Process Antenna USB Commands
 * Command Format: 0x10 (Antenna), subcommand, data...
 */
bool antenna_process_usb_command(uint8_t subcommand, uint8_t *data, uint8_t length) {
    bool result = false;
    
    switch (subcommand) {
        case 0x01: // Set Mode
            if (length >= 1) {
                uint8_t mode = data[0] & 0x03; // Limit to 2 bits
                result = antenna_set_mode((antenna_mode_t)mode);
                printf("USB: Antenna Set Mode command received (mode: %d)\n\r", mode);
            }
            break;
            
        case 0x02: // Enable/Disable
            if (length >= 1) {
                if (data[0]) {
                    result = antenna_enable();
                } else {
                    result = antenna_disable();
                }
                printf("USB: Antenna %s command received\n\r", data[0] ? "Enable" : "Disable");
            }
            break;
            
        case 0x03: // Get Status
            result = true;
            printf("USB: Antenna Status command received\n\r");
            break;
            
        case 0x04: // Set Diversity
            if (length >= 1) {
                result = antenna_set_diversity(data[0] ? true : false);
                printf("USB: Antenna Diversity %s command received\n\r", 
                       data[0] ? "Enable" : "Disable");
            }
            break;
            
        case 0x05: // Update Signal Strength
            if (length >= 4) {
                uint16_t primary = (uint16_t)(data[0] | (data[1] << 8));
                uint16_t secondary = (uint16_t)(data[2] | (data[3] << 8));
                result = antenna_diversity_update_signal_strength(primary, secondary);
                printf("USB: Antenna Signal Strength updated (P: %d, S: %d)\n\r", 
                       primary, secondary);
            }
            break;
            
        default:
            printf("USB: Unknown Antenna subcommand 0x%02X\n\r", subcommand);
            break;
    }
    
    return result;
}

/*
 * Send Antenna Status to Host
 * Status Format: [enabled, current_mode, control_register, diversity_enabled, 
 *                switch_count, reserved...]
 */
bool antenna_send_status(uint8_t *status_buffer, uint8_t max_length) {
    if (max_length < 8) {
        return false;
    }
    
    status_buffer[0] = antenna_state.enabled ? 1 : 0;              // Enabled status
    status_buffer[1] = (uint8_t)antenna_state.current_mode;        // Current mode
    status_buffer[2] = antenna_state.control_register;             // Control register
    status_buffer[3] = antenna_state.diversity.auto_selection ? 1 : 0; // Diversity enabled
    status_buffer[4] = (uint8_t)(antenna_state.diversity.switch_counter & 0xFF);        // Switch count low
    status_buffer[5] = (uint8_t)((antenna_state.diversity.switch_counter >> 8) & 0xFF); // Switch count high
    status_buffer[6] = (uint8_t)(antenna_state.diversity.signal_strength_primary & 0xFF);      // Primary signal low
    status_buffer[7] = (uint8_t)((antenna_state.diversity.signal_strength_primary >> 8) & 0xFF); // Primary signal high
    
    return true;
}

/*
 * Update Hardware State
 */
void antenna_update_hardware(void) {
    // Update antenna selection pin based on current mode
    if (antenna_state.current_mode == ANTENNA_SECONDARY) {
        ANTENNA_SELECT_PIN = 1;
    } else {
        ANTENNA_SELECT_PIN = 0; // Primary, Combined, or Diversity default to primary
    }
}

/*
 * Convert Mode to Register Value
 */
uint8_t antenna_mode_to_register(antenna_mode_t mode) {
    return (uint8_t)mode & 0x03; // 2-bit mode encoding
}

/*
 * Convert Register Value to Mode
 */
antenna_mode_t antenna_register_to_mode(uint8_t reg_value) {
    return (antenna_mode_t)(reg_value & 0x03);
}

/*
 * Get Mode Name for Debugging
 */
const char* antenna_get_mode_name(antenna_mode_t mode) {
    switch (mode) {
        case ANTENNA_PRIMARY: return "Primary";
        case ANTENNA_SECONDARY: return "Secondary";
        case ANTENNA_COMBINED: return "Combined";
        case ANTENNA_DIVERSITY: return "Diversity";
        default: return "Unknown";
    }
}

/*
 * Antenna Self-Test Function
 */
bool antenna_self_test(void) {
    printf("Antenna Self-Test: Starting...\n\r");
    
    // Test disable
    antenna_disable();
    if (antenna_is_enabled()) {
        printf("Antenna Self-Test: FAIL - Could not disable antenna control\n\r");
        return false;
    }
    
    // Test enable
    antenna_enable();
    if (!antenna_is_enabled()) {
        printf("Antenna Self-Test: FAIL - Could not enable antenna control\n\r");
        return false;
    }
    
    // Test all modes
    antenna_mode_t test_modes[] = {
        ANTENNA_PRIMARY, ANTENNA_SECONDARY, 
        ANTENNA_COMBINED, ANTENNA_DIVERSITY
    };
    
    for (int i = 0; i < 4; i++) {
        if (!antenna_set_mode(test_modes[i])) {
            printf("Antenna Self-Test: FAIL - Could not set mode %d\n\r", test_modes[i]);
            return false;
        }
        if (antenna_get_mode() != test_modes[i]) {
            printf("Antenna Self-Test: FAIL - Mode mismatch\n\r");
            return false;
        }
    }
    
    // Test diversity signal strength update
    if (!antenna_diversity_update_signal_strength(100, 200)) {
        printf("Antenna Self-Test: FAIL - Could not update signal strength\n\r");
        return false;
    }
    
    printf("Antenna Self-Test: PASSED\n\r");
    return true;
}
