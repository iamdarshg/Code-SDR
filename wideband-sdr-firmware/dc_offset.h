/*
 * Wideband SDR - DC Offset Correction Module
 * dsPIC33AK256MC505 based SDR firmware
 * 
 * Created: November 2025
 * Version: 1.0.0
 * License: MIT
 * 
 * Features:
 * - Real-time DC offset estimation and correction
 * - Exponential moving average tracking
 * - Automatic calibration routines
 * - Hardware and software correction modes
 * - Multi-channel support
 */

#ifndef DC_OFFSET_H
#define DC_OFFSET_H

#include <stdint.h>
#include <stdbool.h>

// DC Offset Correction Configuration
#define DC_OFFSET_MAX_CHANNELS    4           // Maximum ADC channels
#define DC_OFFSET_BUFFER_SIZE     2048        // Sample buffer for estimation
#define DC_OFFSET_WINDOW_SIZE     1024        // Moving average window
#define DC_OFFSET_DEFAULT_ALPHA   100         // Smoothing factor (0-1000)
#define DC_OFFSET_CALIBRATION_TIME  5000      // Calibration time in ms

// DC Offset Correction Types
typedef enum {
    DC_OFFSET_MODE_DISABLED = 0,    // No correction
    DC_OFFSET_MODE_SOFTWARE = 1,    // Software correction
    DC_OFFSET_MODE_HARDWARE = 2,    // Hardware correction
    DC_OFFSET_MODE_AUTO = 3         // Automatic mode selection
} dc_offset_mode_t;

// DC Offset Correction Structure
typedef struct {
    int32_t dc_offset_i;            // I channel DC offset
    int32_t dc_offset_q;            // Q channel DC offset
    int32_t alpha;                  // Exponential moving average factor
    uint32_t sample_count;          // Samples processed for estimation
    bool calibrated;                // Calibration status
    dc_offset_mode_t mode;          // Current correction mode
    uint8_t channel;                // ADC channel number
} dc_offset_correction_t;

// DC Offset Statistics
typedef struct {
    uint32_t total_samples;         // Total samples processed
    uint32_t calibration_samples;   // Samples used for calibration
    int32_t max_offset_i;           // Maximum I offset observed
    int32_t min_offset_i;           // Minimum I offset observed
    int32_t max_offset_q;           // Maximum Q offset observed
    int32_t min_offset_q;           // Minimum Q offset observed
    float average_offset_i;         // Average I offset
    float average_offset_q;         // Average Q offset
    bool calibration_required;      // Flag indicating calibration needed
} dc_offset_statistics_t;

// Firmware Integration Structure
typedef struct {
    dc_offset_correction_t channels[DC_OFFSET_MAX_CHANNELS];
    dc_offset_statistics_t statistics;
    uint8_t active_channels;        // Number of active channels
    bool auto_calibration_enabled;  // Auto-calibration flag
    uint32_t last_calibration_time; // Timestamp of last calibration
} dc_offset_system_t;

// Function Prototypes

// Initialization and Configuration
/**
 * @brief Initialize DC offset correction system
 * @param system Pointer to DC offset system structure
 * @param active_channels Number of active ADC channels
 * @return bool Success status
 */
bool dc_offset_init(dc_offset_system_t *system, uint8_t active_channels);

/**
 * @brief Configure DC offset correction for specific channel
 * @param correction Pointer to correction structure
 * @param channel ADC channel number
 * @param alpha Smoothing factor (0-1000)
 * @param mode Correction mode
 * @return bool Success status
 */
bool dc_offset_configure_channel(dc_offset_correction_t *correction, 
                               uint8_t channel, uint32_t alpha, 
                               dc_offset_mode_t mode);

/**
 * @brief Set correction mode for all channels
 * @param system Pointer to DC offset system
 * @param mode Correction mode
 */
void dc_offset_set_mode(dc_offset_system_t *system, dc_offset_mode_t mode);

// DC Offset Estimation
/**
 * @brief Estimate DC offset from sample buffer using exponential moving average
 * @param correction Pointer to correction structure
 * @param samples Pointer to sample buffer (interleaved I/Q)
 * @param sample_count Number of samples in buffer
 * @return bool Success status
 */
bool dc_offset_estimate(dc_offset_correction_t *correction, 
                       int16_t *samples, uint32_t sample_count);

/**
 * @brief Update DC offset with single sample
 * @param correction Pointer to correction structure
 * @param i_sample I channel sample
 * @param q_sample Q channel sample
 */
void dc_offset_update_sample(dc_offset_correction_t *correction, 
                           int16_t i_sample, int16_t q_sample);

/**
 * @brief Reset DC offset estimation
 * @param correction Pointer to correction structure
 */
void dc_offset_reset(dc_offset_correction_t *correction);

/**
 * @brief Get current DC offset values
 * @param correction Pointer to correction structure
 * @param offset_i Pointer to store I offset
 * @param offset_q Pointer to store Q offset
 */
void dc_offset_get_values(dc_offset_correction_t *correction, 
                         int32_t *offset_i, int32_t *offset_q);

// DC Offset Correction Application
/**
 * @brief Apply DC offset correction to sample
 * @param correction Pointer to correction structure
 * @param i_sample I channel sample
 * @param q_sample Q channel sample
 * @param corrected_i Pointer to corrected I sample
 * @param corrected_q Pointer to corrected Q sample
 */
void dc_offset_apply_correction(dc_offset_correction_t *correction,
                              int16_t i_sample, int16_t q_sample,
                              int16_t *corrected_i, int16_t *corrected_q);

/**
 * @brief Apply correction to buffer of samples
 * @param correction Pointer to correction structure
 * @param input_samples Input sample buffer (interleaved I/Q)
 * @param output_samples Output sample buffer (interleaved I/Q)
 * @param sample_count Number of samples
 */
void dc_offset_apply_buffer_correction(dc_offset_correction_t *correction,
                                     int16_t *input_samples,
                                     int16_t *output_samples,
                                     uint32_t sample_count);

// Calibration Functions
/**
 * @brief Perform automatic DC offset calibration
 * @param system Pointer to DC offset system
 * @param duration_ms Calibration duration in milliseconds
 * @return bool Calibration success
 */
bool dc_offset_auto_calibrate(dc_offset_system_t *system, uint32_t duration_ms);

/**
 * @brief Calibrate specific channel
 * @param correction Pointer to correction structure
 * @param samples Pointer to calibration sample buffer
 * @param sample_count Number of samples for calibration
 * @return bool Calibration success
 */
bool dc_offset_calibrate_channel(dc_offset_correction_t *correction,
                               int16_t *samples, uint32_t sample_count);

/**
 * @brief Start continuous calibration mode
 * @param system Pointer to DC offset system
 * @param interval_ms Calibration interval in milliseconds
 */
void dc_offset_start_continuous_calibration(dc_offset_system_t *system, 
                                          uint32_t interval_ms);

/**
 * @brief Stop continuous calibration mode
 * @param system Pointer to DC offset system
 */
void dc_offset_stop_continuous_calibration(dc_offset_system_t *system);

// Hardware Integration
/**
 * @brief Configure hardware DC offset correction
 * @param correction Pointer to correction structure
 * @param enable_hardware Enable hardware correction
 * @return bool Success status
 */
bool dc_offset_configure_hardware(dc_offset_correction_t *correction, 
                                bool enable_hardware);

/**
 * @brief Apply hardware DC offset correction via DAC
 * @param correction Pointer to correction structure
 * @return bool Success status
 */
bool dc_offset_apply_hardware_correction(dc_offset_correction_t *correction);

/**
 * @brief Read hardware DC offset DAC values
 * @param correction Pointer to correction structure
 * @param dac_i Pointer to store I channel DAC value
 * @param dac_q Pointer to store Q channel DAC value
 */
void dc_offset_read_hardware_values(dc_offset_correction_t *correction,
                                   uint16_t *dac_i, uint16_t *dac_q);

// Statistics and Monitoring
/**
 * @brief Reset DC offset statistics
 * @param stats Pointer to statistics structure
 */
void dc_offset_reset_statistics(dc_offset_statistics_t *stats);

/**
 * @brief Update DC offset statistics
 * @param stats Pointer to statistics structure
 * @param offset_i Current I offset
 * @param offset_q Current Q offset
 */
void dc_offset_update_statistics(dc_offset_statistics_t *stats,
                                int32_t offset_i, int32_t offset_q);

/**
 * @brief Get DC offset statistics
 * @param system Pointer to DC offset system
 * @param stats Pointer to statistics structure to fill
 */
void dc_offset_get_statistics(dc_offset_system_t *system, 
                            dc_offset_statistics_t *stats);

/**
 * @brief Check if calibration is required
 * @param system Pointer to DC offset system
 * @return bool True if calibration required
 */
bool dc_offset_calibration_required(dc_offset_system_t *system);

/**
 * @brief Calculate DC offset correction effectiveness
 * @param before_offset DC offset before correction
 * @param after_offset DC offset after correction
 * @return float Improvement in dB
 */
float dc_offset_calculate_improvement(int32_t before_offset, int32_t after_offset);

// Performance Monitoring
/**
 * @brief Get DC offset processing performance metrics
 * @param system Pointer to DC offset system
 * @param samples_per_sec Samples processed per second
 * @param cpu_usage_percent CPU usage percentage
 */
void dc_offset_get_performance_metrics(dc_offset_system_t *system,
                                     uint32_t *samples_per_sec,
                                     uint8_t *cpu_usage_percent);

/**
 * @brief Enable/disable performance monitoring
 * @param system Pointer to DC offset system
 * @param enable Enable performance monitoring
 */
void dc_offset_enable_performance_monitoring(dc_offset_system_t *system, 
                                           bool enable);

// Integration with ADC System
/**
 * @brief Initialize DC offset integration with ADC DMA
 * @param system Pointer to DC offset system
 * @return bool Success status
 */
bool dc_offset_init_adc_integration(dc_offset_system_t *system);

/**
 * @brief Process ADC buffer with DC offset correction
 * @param system Pointer to DC offset system
 * @param adc_buffer ADC buffer with interleaved samples
 * @param corrected_buffer Buffer for corrected samples
 * @param sample_count Number of samples
 * @return uint32_t Number of samples processed
 */
uint32_t dc_offset_process_adc_buffer(dc_offset_system_t *system,
                                    int16_t *adc_buffer,
                                    int16_t *corrected_buffer,
                                    uint32_t sample_count);

// Utility Functions
/**
 * @brief Convert DC offset to human-readable format
 * @param offset DC offset value
 * @return uint32_t Offset in microvolts (scaled)
 */
uint32_t dc_offset_to_microvolts(int32_t offset);

/**
 * @brief Validate DC offset correction parameters
 * @param alpha Smoothing factor
 * @param mode Correction mode
 * @return bool True if parameters are valid
 */
bool dc_offset_validate_parameters(uint32_t alpha, dc_offset_mode_t mode);

/**
 * @brief Get DC offset correction version information
 * @return const char* Version string
 */
const char* dc_offset_get_version(void);

/**
 * @brief Perform self-test of DC offset correction system
 * @param system Pointer to DC offset system
 * @return bool True if self-test passed
 */
bool dc_offset_self_test(dc_offset_system_t *system);

#endif /* DC_OFFSET_H */
