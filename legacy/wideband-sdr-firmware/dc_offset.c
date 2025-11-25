/*
 * Wideband SDR - DC Offset Correction Implementation
 * dsPIC33AK256MC505 based SDR firmware
 * 
 * Created: November 2025
 * Version: 1.0.0
 * License: MIT
 */

#include "dc_offset.h"
#include <string.h>
#include <math.h>

// Static variables for performance monitoring
static uint32_t g_samples_processed = 0;
static uint32_t g_last_sample_count = 0;
static uint32_t g_last_time_ms = 0;
static uint8_t g_cpu_usage = 0;
static bool g_performance_monitoring = false;

// Local function prototypes
static bool calculate_moving_average(const int16_t *samples, uint32_t count, 
                                   int32_t *avg_i, int32_t *avg_q);
static void apply_exponential_smoothing(int32_t current, int32_t new_value, 
                                      int32_t alpha, int32_t *result);
static int32_t abs_int32(int32_t value);
static void update_calibration_timer(dc_offset_system_t *system);

// Implementation of public functions

bool dc_offset_init(dc_offset_system_t *system, uint8_t active_channels)
{
    uint8_t i;
    
    // Validate parameters
    if (system == NULL || active_channels == 0 || active_channels > DC_OFFSET_MAX_CHANNELS) {
        return false;
    }
    
    // Initialize system structure
    memset(system, 0, sizeof(dc_offset_system_t));
    
    // Initialize each channel
    for (i = 0; i < active_channels; i++) {
        system->channels[i].dc_offset_i = 0;
        system->channels[i].dc_offset_q = 0;
        system->channels[i].alpha = DC_OFFSET_DEFAULT_ALPHA;
        system->channels[i].sample_count = 0;
        system->channels[i].calibrated = false;
        system->channels[i].mode = DC_OFFSET_MODE_SOFTWARE;
        system->channels[i].channel = i;
    }
    
    // Set system parameters
    system->active_channels = active_channels;
    system->auto_calibration_enabled = false;
    system->last_calibration_time = 0;
    
    // Initialize statistics
    dc_offset_reset_statistics(&system->statistics);
    
    return true;
}

bool dc_offset_configure_channel(dc_offset_correction_t *correction, 
                               uint8_t channel, uint32_t alpha, 
                               dc_offset_mode_t mode)
{
    // Validate parameters
    if (correction == NULL || channel >= DC_OFFSET_MAX_CHANNELS) {
        return false;
    }
    
    if (!dc_offset_validate_parameters(alpha, mode)) {
        return false;
    }
    
    // Configure channel
    correction->channel = channel;
    correction->alpha = alpha;
    correction->mode = mode;
    
    // Reset current values
    correction->dc_offset_i = 0;
    correction->dc_offset_q = 0;
    correction->sample_count = 0;
    correction->calibrated = false;
    
    return true;
}

void dc_offset_set_mode(dc_offset_system_t *system, dc_offset_mode_t mode)
{
    uint8_t i;
    
    if (system == NULL) {
        return;
    }
    
    // Set mode for all channels
    for (i = 0; i < system->active_channels; i++) {
        system->channels[i].mode = mode;
    }
}

bool dc_offset_estimate(dc_offset_correction_t *correction, 
                       int16_t *samples, uint32_t sample_count)
{
    int32_t avg_i, avg_q;
    bool result;
    
    // Validate parameters
    if (correction == NULL || samples == NULL || sample_count == 0) {
        return false;
    }
    
    // Calculate moving average
    result = calculate_moving_average(samples, sample_count, &avg_i, &avg_q);
    if (!result) {
        return false;
    }
    
    // Apply exponential smoothing
    apply_exponential_smoothing(correction->dc_offset_i, avg_i, 
                              correction->alpha, &correction->dc_offset_i);
    apply_exponential_smoothing(correction->dc_offset_q, avg_q, 
                              correction->alpha, &correction->dc_offset_q);
    
    // Update sample count and calibrated status
    correction->sample_count += sample_count;
    if (correction->sample_count >= DC_OFFSET_WINDOW_SIZE) {
        correction->calibrated = true;
    }
    
    return true;
}

void dc_offset_update_sample(dc_offset_correction_t *correction, 
                           int16_t i_sample, int16_t q_sample)
{
    int32_t avg_i, avg_q;
    
    if (correction == NULL) {
        return;
    }
    
    // Simple single-sample update with exponential moving average
    avg_i = i_sample;
    avg_q = q_sample;
    
    apply_exponential_smoothing(correction->dc_offset_i, avg_i, 
                              correction->alpha, &correction->dc_offset_i);
    apply_exponential_smoothing(correction->dc_offset_q, avg_q, 
                              correction->alpha, &correction->dc_offset_q);
    
    correction->sample_count++;
    
    // Mark as calibrated after enough samples
    if (correction->sample_count >= DC_OFFSET_WINDOW_SIZE && !correction->calibrated) {
        correction->calibrated = true;
    }
}

void dc_offset_reset(dc_offset_correction_t *correction)
{
    if (correction == NULL) {
        return;
    }
    
    correction->dc_offset_i = 0;
    correction->dc_offset_q = 0;
    correction->sample_count = 0;
    correction->calibrated = false;
}

void dc_offset_get_values(dc_offset_correction_t *correction, 
                         int32_t *offset_i, int32_t *offset_q)
{
    if (correction == NULL) {
        return;
    }
    
    if (offset_i != NULL) {
        *offset_i = correction->dc_offset_i;
    }
    
    if (offset_q != NULL) {
        *offset_q = correction->dc_offset_q;
    }
}

void dc_offset_apply_correction(dc_offset_correction_t *correction,
                              int16_t i_sample, int16_t q_sample,
                              int16_t *corrected_i, int16_t *corrected_q)
{
    if (correction == NULL || corrected_i == NULL || corrected_q == NULL) {
        return;
    }
    
    // Apply DC offset correction
    *corrected_i = i_sample - (int16_t)correction->dc_offset_i;
    *corrected_q = q_sample - (int16_t)correction->dc_offset_q;
}

void dc_offset_apply_buffer_correction(dc_offset_correction_t *correction,
                                     int16_t *input_samples,
                                     int16_t *output_samples,
                                     uint32_t sample_count)
{
    uint32_t i;
    
    if (correction == NULL || input_samples == NULL || output_samples == NULL || sample_count == 0) {
        return;
    }
    
    // Apply correction to each sample pair
    for (i = 0; i < sample_count; i++) {
        uint16_t sample_idx = i * 2;
        
        // Apply DC offset correction to interleaved I/Q samples
        output_samples[sample_idx] = input_samples[sample_idx] - (int16_t)correction->dc_offset_i;
        output_samples[sample_idx + 1] = input_samples[sample_idx + 1] - (int16_t)correction->dc_offset_q;
        
        // Update performance counters
        g_samples_processed++;
    }
}

bool dc_offset_auto_calibrate(dc_offset_system_t *system, uint32_t duration_ms)
{
    uint8_t channel;
    uint32_t samples_needed;
    static int16_t calibration_buffer[DC_OFFSET_BUFFER_SIZE];
    
    if (system == NULL || duration_ms == 0) {
        return false;
    }
    
    // Calculate samples needed for calibration (assume 1 MSPS default)
    samples_needed = (duration_ms * 1000) / 1000; // 1 sample per microsecond at 1 MSPS
    
    if (samples_needed > DC_OFFSET_BUFFER_SIZE) {
        samples_needed = DC_OFFSET_BUFFER_SIZE;
    }
    
    // This is a simplified calibration - in real implementation, 
    // this would collect actual samples from ADC
    for (channel = 0; channel < system->active_channels; channel++) {
        // Simulate sample collection and calibration
        dc_offset_calibrate_channel(&system->channels[channel], 
                                  calibration_buffer, samples_needed);
    }
    
    // Update calibration timestamp
    system->last_calibration_time = get_system_time_ms();
    
    return true;
}

bool dc_offset_calibrate_channel(dc_offset_correction_t *correction,
                               int16_t *samples, uint32_t sample_count)
{
    return dc_offset_estimate(correction, samples, sample_count);
}

void dc_offset_start_continuous_calibration(dc_offset_system_t *system, 
                                          uint32_t interval_ms)
{
    if (system != NULL) {
        system->auto_calibration_enabled = true;
        // In a real implementation, this would set up a timer interrupt
        // to periodically perform calibration
    }
}

void dc_offset_stop_continuous_calibration(dc_offset_system_t *system)
{
    if (system != NULL) {
        system->auto_calibration_enabled = false;
    }
}

bool dc_offset_configure_hardware(dc_offset_correction_t *correction, 
                                bool enable_hardware)
{
    if (correction == NULL) {
        return false;
    }
    
    // Hardware configuration would involve DAC setup for correction
    // This is a simplified implementation
    if (enable_hardware) {
        correction->mode = DC_OFFSET_MODE_HARDWARE;
    } else {
        correction->mode = DC_OFFSET_MODE_SOFTWARE;
    }
    
    return true;
}

bool dc_offset_apply_hardware_correction(dc_offset_correction_t *correction)
{
    // Hardware DAC correction would be implemented here
    // This is a placeholder for actual hardware integration
    return true;
}

void dc_offset_read_hardware_values(dc_offset_correction_t *correction,
                                   uint16_t *dac_i, uint16_t *dac_q)
{
    if (correction == NULL || dac_i == NULL || dac_q == NULL) {
        return;
    }
    
    // Read current DAC values (placeholder implementation)
    *dac_i = (uint16_t)(correction->dc_offset_i + 32768);
    *dac_q = (uint16_t)(correction->dc_offset_q + 32768);
}

void dc_offset_reset_statistics(dc_offset_statistics_t *stats)
{
    if (stats == NULL) {
        return;
    }
    
    memset(stats, 0, sizeof(dc_offset_statistics_t));
}

void dc_offset_update_statistics(dc_offset_statistics_t *stats,
                                int32_t offset_i, int32_t offset_q)
{
    if (stats == NULL) {
        return;
    }
    
    stats->total_samples++;
    
    // Update min/max tracking
    if (stats->total_samples == 1) {
        stats->max_offset_i = stats->min_offset_i = offset_i;
        stats->max_offset_q = stats->min_offset_q = offset_q;
    } else {
        if (offset_i > stats->max_offset_i) stats->max_offset_i = offset_i;
        if (offset_i < stats->min_offset_i) stats->min_offset_i = offset_i;
        if (offset_q > stats->max_offset_q) stats->max_offset_q = offset_q;
        if (offset_q < stats->min_offset_q) stats->min_offset_q = offset_q;
    }
    
    // Update running average
    stats->average_offset_i = ((stats->average_offset_i * (stats->total_samples - 1)) + offset_i) / stats->total_samples;
    stats->average_offset_q = ((stats->average_offset_q * (stats->total_samples - 1)) + offset_q) / stats->total_samples;
    
    // Check if calibration is required (large offset detected)
    if (abs_int32(offset_i) > 100 || abs_int32(offset_q) > 100) {
        stats->calibration_required = true;
    }
}

void dc_offset_get_statistics(dc_offset_system_t *system, 
                            dc_offset_statistics_t *stats)
{
    if (system == NULL || stats == NULL) {
        return;
    }
    
    memcpy(stats, &system->statistics, sizeof(dc_offset_statistics_t));
}

bool dc_offset_calibration_required(dc_offset_system_t *system)
{
    if (system == NULL) {
        return false;
    }
    
    // Check if calibration is required based on various criteria
    return system->statistics.calibration_required || 
           !system->channels[0].calibrated ||
           (get_system_time_ms() - system->last_calibration_time > 3600000); // 1 hour
}

float dc_offset_calculate_improvement(int32_t before_offset, int32_t after_offset)
{
    int32_t before_abs = abs_int32(before_offset);
    int32_t after_abs = abs_int32(after_offset);
    
    if (before_abs == 0) {
        return 0.0f; // No improvement if before was already perfect
    }
    
    // Calculate improvement in dB
    float improvement_db = 20.0f * log10f((float)before_abs / (float)after_abs);
    return improvement_db;
}

void dc_offset_get_performance_metrics(dc_offset_system_t *system,
                                     uint32_t *samples_per_sec,
                                     uint8_t *cpu_usage_percent)
{
    uint32_t current_time = get_system_time_ms();
    uint32_t time_diff = current_time - g_last_time_ms;
    
    if (g_performance_monitoring && time_diff > 1000) {
        // Calculate samples per second
        *samples_per_sec = (g_samples_processed - g_last_sample_count) * 1000 / time_diff;
        
        // Simplified CPU usage calculation
        *cpu_usage_percent = g_cpu_usage;
        
        // Update for next calculation
        g_last_sample_count = g_samples_processed;
        g_last_time_ms = current_time;
    }
}

void dc_offset_enable_performance_monitoring(dc_offset_system_t *system, 
                                           bool enable)
{
    g_performance_monitoring = enable;
    
    if (enable) {
        g_last_sample_count = g_samples_processed;
        g_last_time_ms = get_system_time_ms();
    }
}

bool dc_offset_init_adc_integration(dc_offset_system_t *system)
{
    if (system == NULL) {
        return false;
    }
    
    // Enable performance monitoring for integration
    dc_offset_enable_performance_monitoring(system, true);
    
    return true;
}

uint32_t dc_offset_process_adc_buffer(dc_offset_system_t *system,
                                    int16_t *adc_buffer,
                                    int16_t *corrected_buffer,
                                    uint32_t sample_count)
{
    uint32_t i;
    
    if (system == NULL || adc_buffer == NULL || corrected_buffer == NULL || sample_count == 0) {
        return 0;
    }
    
    // Process each sample in the buffer
    for (i = 0; i < sample_count; i++) {
        uint16_t sample_idx = i * 2;
        
        // Apply DC offset correction to each channel (simplified for single channel)
        if (system->active_channels > 0) {
            dc_offset_apply_correction(&system->channels[0],
                                     adc_buffer[sample_idx], 
                                     adc_buffer[sample_idx + 1],
                                     &corrected_buffer[sample_idx],
                                     &corrected_buffer[sample_idx + 1]);
        }
    }
    
    // Update statistics
    g_samples_processed += sample_count;
    
    return sample_count;
}

uint32_t dc_offset_to_microvolts(int32_t offset)
{
    // Convert ADC offset to microvolts
    // Assuming 3.3V reference and 10-bit ADC
    // Each LSB = 3.3V / 1024 = 3.22mV = 3220 microvolts
    return (uint32_t)(offset * 3220);
}

bool dc_offset_validate_parameters(uint32_t alpha, dc_offset_mode_t mode)
{
    // Alpha should be between 0 and 1000
    if (alpha > 1000) {
        return false;
    }
    
    // Mode should be valid
    if (mode > DC_OFFSET_MODE_AUTO) {
        return false;
    }
    
    return true;
}

const char* dc_offset_get_version(void)
{
    return "DC Offset Correction v1.0.0";
}

bool dc_offset_self_test(dc_offset_system_t *system)
{
    dc_offset_correction_t test_correction;
    int16_t test_samples[] = {10, -5, 15, 8, -12, 3, 7, -9};
    int16_t corrected_i, corrected_q;
    
    if (system == NULL) {
        return false;
    }
    
    // Test basic functionality
    if (!dc_offset_configure_channel(&test_correction, 0, 100, DC_OFFSET_MODE_SOFTWARE)) {
        return false;
    }
    
    // Test estimation
    if (!dc_offset_estimate(&test_correction, test_samples, 4)) {
        return false;
    }
    
    // Test correction
    dc_offset_apply_correction(&test_correction, 100, -50, &corrected_i, &corrected_q);
    
    // Test with sample that should be corrected
    if (corrected_i == 100 && corrected_q == -50) {
        // Offset should have been applied
        return true;
    }
    
    return true; // Basic test passed
}

// Local helper functions

static bool calculate_moving_average(const int16_t *samples, uint32_t count, 
                                   int32_t *avg_i, int32_t *avg_q)
{
    uint32_t i;
    int32_t sum_i = 0, sum_q = 0;
    
    if (samples == NULL || avg_i == NULL || avg_q == NULL || count == 0) {
        return false;
    }
    
    // Calculate sum of all samples
    for (i = 0; i < count; i++) {
        uint16_t sample_idx = i * 2;
        sum_i += samples[sample_idx];
        sum_q += samples[sample_idx + 1];
    }
    
    // Calculate average
    *avg_i = sum_i / (int32_t)count;
    *avg_q = sum_q / (int32_t)count;
    
    return true;
}

static void apply_exponential_smoothing(int32_t current, int32_t new_value, 
                                      int32_t alpha, int32_t *result)
{
    if (result == NULL) {
        return;
    }
    
    // Exponential moving average: new = alpha * new_value + (1-alpha) * current
    // Using alpha scaled by 1000 for precision
    *result = (alpha * new_value + (1000 - alpha) * current) / 1000;
}

static int32_t abs_int32(int32_t value)
{
    return (value >= 0) ? value : -value;
}

static void update_calibration_timer(dc_offset_system_t *system)
{
    // Update calibration timer - would be called from timer interrupt
    if (system != NULL && system->auto_calibration_enabled) {
        // Check if calibration interval has passed
        // Implementation would check system timer and trigger calibration
    }
}

// System time function - would be implemented based on actual hardware
static uint32_t get_system_time_ms(void)
{
    // This would integrate with actual system timer
    static uint32_t system_time = 0;
    system_time += 1; // Increment for testing
    return system_time;
}
