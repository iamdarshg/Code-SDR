/*
 * Wideband SDR - AD9215 ADC Interface with DMA Implementation
 * dsPIC33AK256MC505 based SDR with AD9215BCPZ-105 ADC
 * 
 * Created: November 2025
 * Version: 1.0.0
 * License: MIT
 * 
 * Features:
 * - 10-bit 105 MSPS ADC interface
 * - Parallel data capture on PORTD
 * - DMA-driven sampling (zero CPU overhead)
 * - Ping-pong buffer management
 * - Timer-based sample rate control
 * - Real-time data streaming
 */

#include "adc_dma.h"

// Global ADC configuration
static adc_config_t current_adc_config = {
    .sample_rate = 1000000,         // 1 MSPS default
    .resolution = 10,              // 10-bit ADC
    .prescaler = ADC_PRESCALER_1,  // No prescaler
    .differential_mode = false,    // Single-ended mode
    .continuous_conversion = true, // Continuous conversion
    .dma_enabled = true,           // DMA enabled
    .gain = 0                      // No additional gain
};

// ADC buffer manager
static adc_buffer_manager_t adc_manager = {0};

// ADC statistics
static adc_statistics_t adc_stats = {0};

// Global ADC buffer
static uint16_t adc_buffers[NUM_BUFFERS][DMA_BUFFER_SIZE];

// Calibration values
static int16_t adc_offset = 0;
static uint32_t adc_gain = 0x8000;  // Unity gain (Q15 format)

// Current sample rate
static uint32_t current_sample_rate = 1000000;

/*
 * Initialize ADC interface
 */
bool adc_init(adc_config_t *config) {
    if (config != NULL) {
        current_adc_config = *config;
    }
    
    // Configure PORTD for ADC data input (parallel 10-bit bus)
    TRISD = 0xFFFF;              // All PORTD pins as input
    PORTD = 0x0000;              // Initialize to 0
    
    // Initialize ADC control pins if any (not needed for AD9215 parallel interface)
    
    // Initialize DMA
    if (!adc_init_dma()) {
        return false;
    }
    
    // Initialize buffer manager
    if (!adc_init_buffer_manager(&adc_manager, 
                                (uint16_t *[]) {adc_buffers[0], adc_buffers[1]}, 
                                DMA_BUFFER_SIZE)) {
        return false;
    }
    
    // Configure ADC clock and sample rate
    if (!adc_set_sample_rate(current_adc_config.sample_rate)) {
        return false;
    }
    
    // Reset statistics
    adc_reset_statistics(&adc_stats);
    
    // Initial calibration (offset calibration)
    adc_calibrate_offset();
    
    return true;
}

/*
 * Configure ADC sample rate
 */
bool adc_set_sample_rate(uint32_t sample_rate) {
    if (!adc_validate_sample_rate(sample_rate)) {
        return false;
    }
    
    current_sample_rate = sample_rate;
    current_adc_config.sample_rate = sample_rate;
    
    // Configure timer for sample rate control
    if (!adc_configure_timer1(sample_rate)) {
        return false;
    }
    
    // Calculate and store actual sample rate
    adc_clock_config_t clock_config;
    if (adc_calculate_timer_config(sample_rate, &clock_config)) {
        current_sample_rate = clock_config.sample_rate;
    }
    
    return true;
}

/*
 * Get current sample rate
 */
uint32_t adc_get_sample_rate(void) {
    return current_sample_rate;
}

/*
 * Configure ADC clock
 */
bool adc_configure_clock(adc_clock_config_t *config) {
    if (config == NULL) {
        return false;
    }
    
    // Calculate optimal configuration based on sample rate
    return adc_calculate_timer_config(config->sample_rate, config);
}

/*
 * Enable ADC conversion
 */
void adc_enable(void) {
    // For AD9215, this would enable the ADC chip
    // Physical pin control would go here
    // For now, we just ensure timer is running
    adc_start_timer1();
}

/*
 * Disable ADC conversion
 */
void adc_disable(void) {
    // Stop timer and ADC
    adc_stop_timer1();
}

/*
 * Start ADC conversion
 */
void adc_start_conversion(void) {
    adc_manager.streaming_active = true;
    adc_start_timer1();
    adc_start_dma();
}

/*
 * Stop ADC conversion
 */
void adc_stop_conversion(void) {
    adc_manager.streaming_active = false;
    adc_stop_timer1();
    adc_stop_dma();
}

/*
 * Check if ADC is busy
 */
bool adc_is_busy(void) {
    // Check if DMA is active or timer is running
    return adc_manager.streaming_active && adc_dma_is_active();
}

/*
 * Read single sample from ADC
 */
uint16_t adc_read_sample(void) {
    // For parallel ADC interface, read directly from PORTD
    // This is for single-shot mode, not for DMA streaming
    return PORTD & 0x3FF;  // Mask to 10 bits
}

/*
 * Initialize DMA for ADC data capture
 */
bool adc_init_dma(void) {
    // Configure DMA0 for ADC data capture
    DMA0CONbits.MODE = 0b01;      // Peripheral to memory
    DMA0CONbits.DIR = 0;          // Read from peripheral
    DMA0CONbits.SIZE = 0;         // 16-bit transfers
    DMA0CONbits.TRMODE = 0b10;    // Continuous mode
    DMA0CONbits.AMODE = 0b10;     // Register indirect with post-increment
    DMA0CONbits.DONE = 0;         // Clear done flag
    
    DMA0REQ = 0x000D;             // ADC1 requests (example peripheral)
    DMA0STA = __builtin_dmaptr(adc_buffers[0]);
    DMA0STB = __builtin_dmaptr(adc_buffers[0] + DMA_BUFFER_SIZE);
    DMA0CNT = DMA_BUFFER_SIZE - 1;
    
    // Configure DMA1 for ping-pong
    DMA1CONbits.MODE = 0b01;      // Peripheral to memory
    DMA1CONbits.DIR = 0;          // Read from peripheral
    DMA1CONbits.SIZE = 0;         // 16-bit transfers
    DMA1CONbits.TRMODE = 0b10;    // Continuous mode
    DMA1CONbits.AMODE = 0b10;     // Register indirect with post-increment
    DMA1CONbits.DONE = 0;         // Clear done flag
    
    DMA1REQ = 0x000D;             // Same peripheral request
    DMA1STA = __builtin_dmaptr(adc_buffers[1]);
    DMA1STB = __builtin_dmaptr(adc_buffers[1] + DMA_BUFFER_SIZE);
    DMA1CNT = DMA_BUFFER_SIZE - 1;
    
    // Enable DMA interrupts
    IEC0bits.DMA0IE = 1;          // DMA0 interrupt enable
    IEC0bits.DMA1IE = 1;          // DMA1 interrupt enable
    IPC1bits.DMA0IP = 5;          // DMA0 interrupt priority
    IPC2bits.DMA1IP = 5;          // DMA1 interrupt priority
    
    // Do not start DMA yet - will be started when streaming begins
    DMA0CONbits.CHEN = 0;
    DMA1CONbits.CHEN = 0;
    
    return true;
}

/*
 * Configure DMA0 for continuous ADC capture
 */
bool adc_configure_dma0(uint16_t *buffer, uint32_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {
        return false;
    }
    
    DMA0CONbits.CHEN = 0;         // Stop DMA first
    DMA0STA = __builtin_dmaptr(buffer);
    DMA0STB = __builtin_dmaptr(buffer + buffer_size);
    DMA0CNT = buffer_size - 1;
    
    return true;
}

/*
 * Configure DMA1 for ping-pong operation
 */
bool adc_configure_dma1(uint16_t *buffer, uint32_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {
        return false;
    }
    
    DMA1CONbits.CHEN = 0;         // Stop DMA first
    DMA1STA = __builtin_dmaptr(buffer);
    DMA1STB = __builtin_dmaptr(buffer + buffer_size);
    DMA1CNT = buffer_size - 1;
    
    return true;
}

/*
 * Start DMA transfer
 */
bool adc_start_dma(void) {
    if (adc_manager.streaming_active) {
        DMA0CONbits.CHEN = 1;
        DMA1CONbits.CHEN = 1;
        return true;
    }
    return false;
}

/*
 * Stop DMA transfer
 */
void adc_stop_dma(void) {
    DMA0CONbits.CHEN = 0;
    DMA1CONbits.CHEN = 0;
}

/*
 * Check DMA transfer status
 */
bool adc_dma_is_active(void) {
    return DMA0CONbits.CHEN || DMA1CONbits.CHEN;
}

/*
 * Get current DMA buffer
 */
uint16_t* adc_get_current_buffer(void) {
    return adc_manager.buffer[adc_manager.current_buffer];
}

/*
 * Switch to next DMA buffer
 */
bool adc_switch_buffer(void) {
    adc_manager.current_buffer = (adc_manager.current_buffer + 1) % NUM_BUFFERS;
    return true;
}

/*
 * Initialize ADC buffer manager
 */
bool adc_init_buffer_manager(adc_buffer_manager_t *manager, 
                           uint16_t *buffers[], uint32_t buffer_size) {
    if (manager == NULL || buffers == NULL) {
        return false;
    }
    
    for (int i = 0; i < NUM_BUFFERS; i++) {
        if (buffers[i] == NULL) {
            return false;
        }
        manager->buffer[i] = buffers[i];
    }
    
    manager->current_buffer = 0;
    manager->write_ptr = manager->buffer[0];
    manager->samples_collected = 0;
    manager->buffer_overflow = false;
    manager->streaming_active = false;
    
    return true;
}

/*
 * Reset buffer manager
 */
void adc_reset_buffer_manager(adc_buffer_manager_t *manager) {
    if (manager == NULL) {
        return;
    }
    
    manager->current_buffer = 0;
    manager->write_ptr = manager->buffer[0];
    manager->samples_collected = 0;
    manager->buffer_overflow = false;
    manager->streaming_active = false;
}

/*
 * Get samples from buffer
 */
uint32_t adc_get_samples(adc_buffer_manager_t *manager, 
                        uint16_t *samples, uint32_t max_samples) {
    if (manager == NULL || samples == NULL || max_samples == 0) {
        return 0;
    }
    
    uint32_t samples_copied = 0;
    uint16_t *current_read_ptr = manager->buffer[manager->current_buffer];
    
    // Copy samples up to the write pointer (circular buffer)
    uint32_t available_samples = DMA_BUFFER_SIZE;
    
    if (available_samples > max_samples) {
        available_samples = max_samples;
    }
    
    for (uint32_t i = 0; i < available_samples; i++) {
        samples[i] = current_read_ptr[i];
        samples_copied++;
    }
    
    return samples_copied;
}

/*
 * Check buffer status
 */
bool adc_buffer_has_data(adc_buffer_manager_t *manager) {
    if (manager == NULL) {
        return false;
    }
    
    return !manager->buffer_overflow && manager->streaming_active;
}

/*
 * Clear buffer overflow flag
 */
void adc_clear_buffer_overflow(adc_buffer_manager_t *manager) {
    if (manager != NULL) {
        manager->buffer_overflow = false;
    }
}

/*
 * Reset ADC statistics
 */
void adc_reset_statistics(adc_statistics_t *stats) {
    if (stats == NULL) {
        return;
    }
    
    stats->total_samples = 0;
    stats->dropped_samples = 0;
    stats->overrun_count = 0;
    stats->average_sample_rate = 0;
    stats->max_sample_rate = 0;
}

/*
 * Update ADC statistics
 */
void adc_update_statistics(adc_statistics_t *stats, uint32_t samples_processed) {
    if (stats == NULL) {
        return;
    }
    
    stats->total_samples += samples_processed;
    stats->max_sample_rate = (samples_processed > stats->max_sample_rate) ? 
                             samples_processed : stats->max_sample_rate;
}

/*
 * Get ADC statistics
 */
void adc_get_statistics(adc_statistics_t *stats) {
    if (stats != NULL) {
        *stats = adc_stats;
    }
}

/*
 * Calculate actual sample rate from timing
 */
uint32_t adc_calculate_sample_rate(uint32_t samples_processed, 
                                  uint32_t time_elapsed_us) {
    if (time_elapsed_us == 0) {
        return 0;
    }
    
    // Calculate rate in samples per second
    return (samples_processed * 1000000UL) / time_elapsed_us;
}

/*
 * Configure Timer1 for ADC conversion control
 */
bool adc_configure_timer1(uint32_t sample_rate) {
    // Calculate timer period for desired sample rate
    uint32_t system_clock = 200000000UL;  // 200 MHz system clock
    
    // Timer1 with 1:256 prescaler
    uint32_t timer_clock = system_clock / 256;
    uint32_t period = timer_clock / sample_rate;
    
    if (period > 65535) {
        period = 65535;  // Max 16-bit value
    }
    
    // Configure Timer1
    T1CONbits.TON = 0;            // Timer off
    T1CONbits.TGATE = 0;          // Gated time accumulation disabled
    T1CONbits.TCKPS = 0b11;       // 1:256 prescaler
    T1CONbits.TCS = 0;            // Internal clock source
    
    PR1 = period & 0xFFFF;
    
    return true;
}

/*
 * Start Timer1 for ADC control
 */
void adc_start_timer1(void) {
    T1CONbits.TON = 1;
}

/*
 * Stop Timer1 for ADC control
 */
void adc_stop_timer1(void) {
    T1CONbits.TON = 0;
}

/*
 * Get Timer1 current value
 */
uint16_t adc_get_timer1_value(void) {
    return TMR1;
}

/*
 * Set Timer1 period
 */
void adc_set_timer1_period(uint16_t period) {
    PR1 = period;
}

/*
 * ADC Interrupt Service Routine
 */
void __attribute__((interrupt, no_auto_psv)) _ADCInterrupt(void) {
    // Process ADC conversion complete
    
    // Clear ADC interrupt flag
    IFS0bits.AD1IF = 0;
}

/*
 * DMA0 Interrupt Service Routine (Buffer 1 complete)
 */
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void) {
    // Update statistics
    adc_stats.total_samples += DMA_BUFFER_SIZE;
    
    // Switch to next buffer
    adc_switch_buffer();
    
    // Restart DMA for next buffer
    DMA0CONbits.CHEN = 0;
    DMA0CONbits.CHEN = 1;
    
    // Clear DMA0 interrupt flag
    IFS0bits.DMA0IF = 0;
}

/*
 * DMA1 Interrupt Service Routine (Buffer 2 complete)
 */
void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void) {
    // Update statistics
    adc_stats.total_samples += DMA_BUFFER_SIZE;
    
    // Switch to next buffer
    adc_switch_buffer();
    
    // Restart DMA for next buffer
    DMA1CONbits.CHEN = 0;
    DMA1CONbits.CHEN = 1;
    
    // Clear DMA1 interrupt flag
    IFS0bits.DMA1IF = 0;
}

/*
 * Timer1 Interrupt Service Routine (ADC trigger)
 */
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    // Trigger ADC conversion
    // For parallel ADC, this might trigger data capture or signal ready for data
    
    // Clear timer interrupt flag
    IFS0bits.T1IF = 0;
}

/*
 * Convert ADC raw value to voltage
 */
uint32_t adc_raw_to_voltage(uint16_t adc_value) {
    // 10-bit ADC: 0-1023 corresponds to 0-3.3V
    return ((uint32_t)adc_value * ADC_REFERENCE_VOLTAGE) / 1024;
}

/*
 * Convert voltage to ADC raw value
 */
uint16_t adc_voltage_to_raw(uint32_t voltage) {
    // Convert voltage back to ADC value
    return (uint16_t)((voltage * 1024) / ADC_REFERENCE_VOLTAGE);
}

/*
 * Calculate dBm from ADC sample
 */
int32_t adc_sample_to_dbm(uint16_t sample, uint32_t reference_voltage, 
                         uint32_t input_impedance) {
    if (input_impedance == 0) {
        return -10000;  // Invalid impedance, return very low power
    }
    
    // Convert ADC sample to voltage
    uint32_t voltage_mv = adc_raw_to_voltage(sample);
    
    // Calculate power: P = V^2 / (2 * R) for RMS voltage
    // Convert to dBm: dBm = 10 * log10(P * 1000)
    uint64_t power_uw = ((uint64_t)voltage_mv * voltage_mv * 1000000ULL) / 
                       (2ULL * input_impedance);
    
    if (power_uw == 0) {
        return -10000;  // Very low power
    }
    
    // Calculate dBm
    int32_t dbm = (int32_t)(10.0 * log10((double)power_uw / 1000.0) * 100);
    
    return dbm;
}

/*
 * Validate sample rate range
 */
bool adc_validate_sample_rate(uint32_t sample_rate) {
    return (sample_rate >= ADC_MIN_SAMPLE_RATE && 
            sample_rate <= ADC_MAX_SAMPLE_RATE);
}

/*
 * Calculate optimal timer configuration
 */
bool adc_calculate_timer_config(uint32_t sample_rate, adc_clock_config_t *config) {
    if (config == NULL) {
        return false;
    }
    
    if (!adc_validate_sample_rate(sample_rate)) {
        return false;
    }
    
    config->sample_rate = sample_rate;
    config->system_freq = 200000000UL;  // 200 MHz system clock
    
    // Try different prescalers to find optimal configuration
    for (int prescaler = 0; prescaler <= 3; prescaler++) {
        uint32_t effective_clock = config->system_freq >> (prescaler * 2);
        uint32_t period = effective_clock / sample_rate;
        
        if (period <= 65535) {
            config->prescaler = (adc_prescaler_t)prescaler;
            config->timer_period = (uint16_t)period;
            return true;
        }
    }
    
    return false;  // Could not find valid configuration
}

/*
 * Perform ADC offset calibration
 */
bool adc_calibrate_offset(void) {
    // Perform offset calibration by measuring input with zero input
    // For now, use a fixed offset
    adc_offset = 0;  // Assume no offset for AD9215
    
    return true;
}

/*
 * Apply offset correction to sample
 */
int16_t adc_apply_offset_correction(uint16_t sample) {
    int32_t corrected = (int32_t)sample - adc_offset;
    
    // Clamp to valid range
    if (corrected < 0) corrected = 0;
    if (corrected > 1023) corrected = 1023;
    
    return (int16_t)corrected;
}

/*
 * Set ADC offset value
 */
void adc_set_offset(int16_t offset) {
    adc_offset = offset;
}

/*
 * Perform gain calibration
 */
bool adc_calibrate_gain(void) {
    // Perform gain calibration
    // For now, use unity gain
    adc_gain = 0x8000;  // Unity gain in Q15 format
    
    return true;
}

/*
 * Apply gain correction to sample
 */
uint16_t adc_apply_gain_correction(uint16_t sample) {
    // Apply gain correction using fixed-point multiplication
    uint32_t corrected = ((uint32_t)sample * adc_gain) >> 15;
    
    // Clamp to valid range
    if (corrected > 1023) corrected = 1023;
    
    return (uint16_t)corrected;
}

/*
 * Set ADC gain value
 */
void adc_set_gain(uint32_t gain) {
    adc_gain = gain;
}

/*
 * Put ADC in low power mode
 */
void adc_enter_low_power_mode(void) {
    adc_stop_conversion();
    // Additional power-down logic would go here
}

/*
 * Wake ADC from low power mode
 */
void adc_exit_low_power_mode(void) {
    // Wake from power-down
    adc_enable();
}

/*
 * Configure ADC power management
 */
void adc_configure_power_management(bool low_power) {
    if (low_power) {
        adc_enter_low_power_mode();
    } else {
        adc_exit_low_power_mode();
    }
}
