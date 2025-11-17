/*
 * Wideband SDR - AD9215 ADC Interface with DMA
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

#ifndef ADC_DMA_H
#define ADC_DMA_H

#include <stdint.h>
#include <stdbool.h>

// ADC Configuration
#define ADC_RESOLUTION           10          // 10-bit ADC
#define ADC_MAX_SAMPLE_RATE      105000000   // 105 MSPS maximum
#define ADC_MIN_SAMPLE_RATE      1000        // 1 kSPS minimum
#define ADC_REFERENCE_VOLTAGE    3300        // 3.3V reference (mV)

// DMA Configuration
#define DMA_BUFFER_SIZE          16384       // 16 KB buffers
#define NUM_BUFFERS              2           // Ping-pong configuration
#define DMA_TRANSFER_SIZE        2           // 16-bit transfers

// ADC Channel Definitions
#define ADC_CHANNEL_0            0
#define ADC_CHANNEL_1            1
#define ADC_CHANNEL_2            2
#define ADC_CHANNEL_3            3
#define ADC_MAX_CHANNELS         4

// Sample Rate Prescalers
typedef enum {
    ADC_PRESCALER_1 = 0b00,     // 1:1 prescaler
    ADC_PRESCALER_2 = 0b01,     // 1:2 prescaler
    ADC_PRESCALER_4 = 0b10,     // 1:4 prescaler
    ADC_PRESCALER_8 = 0b11      // 1:8 prescaler
} adc_prescaler_t;

// ADC Clock Configuration
typedef struct {
    uint32_t system_freq;       // System frequency in Hz
    uint32_t sample_rate;       // Desired sample rate in Sps
    adc_prescaler_t prescaler;  // Clock prescaler selection
    uint16_t timer_period;      // Timer period register value
    uint8_t adcs_value;         // ADC conversion clock divider
} adc_clock_config_t;

// ADC Buffer Management
typedef struct {
    uint16_t *buffer[NUM_BUFFERS];  // Pointer to DMA buffers
    uint16_t current_buffer;         // Current active buffer
    uint16_t *write_ptr;             // Current write pointer
    uint32_t samples_collected;      // Total samples collected
    bool buffer_overflow;           // Buffer overflow flag
    bool streaming_active;          // Streaming status
} adc_buffer_manager_t;

// ADC Statistics
typedef struct {
    uint32_t total_samples;         // Total samples processed
    uint32_t dropped_samples;       // Samples dropped due to overflow
    uint32_t overrun_count;         // Buffer overrun count
    uint32_t average_sample_rate;   // Average sample rate achieved
    uint32_t max_sample_rate;       // Maximum sustainable sample rate
} adc_statistics_t;

// ADC Configuration Structure
typedef struct {
    uint32_t sample_rate;           // Sample rate in Sps
    uint8_t resolution;             // ADC resolution (fixed at 10 bits)
    adc_prescaler_t prescaler;      // Clock prescaler
    bool differential_mode;         // Single-ended vs differential
    bool continuous_conversion;     // Continuous vs single-shot
    bool dma_enabled;              // DMA enabled
    uint16_t gain;                 // Gain setting (0-63 dB)
} adc_config_t;

// Function Prototypes

// Initialization and Configuration
/**
 * @brief Initialize ADC interface
 * @param config Pointer to ADC configuration
 * @return bool Success status
 */
bool adc_init(adc_config_t *config);

/**
 * @brief Configure ADC sample rate
 * @param sample_rate Desired sample rate in Sps
 * @return bool Success status
 */
bool adc_set_sample_rate(uint32_t sample_rate);

/**
 * @brief Get current sample rate
 * @return uint32_t Current sample rate in Sps
 */
uint32_t adc_get_sample_rate(void);

/**
 * @brief Configure ADC clock
 * @param config Clock configuration structure
 * @return bool Success status
 */
bool adc_configure_clock(adc_clock_config_t *config);

/**
 * @brief Enable ADC conversion
 */
void adc_enable(void);

/**
 * @brief Disable ADC conversion
 */
void adc_disable(void);

/**
 * @brief Start ADC conversion
 */
void adc_start_conversion(void);

/**
 * @brief Stop ADC conversion
 */
void adc_stop_conversion(void);

/**
 * @brief Check if ADC is busy
 * @return bool Busy status
 */
bool adc_is_busy(void);

/**
 * @brief Read single sample from ADC
 * @return uint16_t ADC sample value
 */
uint16_t adc_read_sample(void);

// DMA Configuration and Management
/**
 * @brief Initialize DMA for ADC data capture
 * @return bool Success status
 */
bool adc_init_dma(void);

/**
 * @brief Configure DMA0 for continuous ADC capture
 * @param buffer Pointer to buffer memory
 * @param buffer_size Size of buffer in samples
 * @return bool Success status
 */
bool adc_configure_dma0(uint16_t *buffer, uint32_t buffer_size);

/**
 * @brief Configure DMA1 for ping-pong operation
 * @param buffer Pointer to buffer memory
 * @param buffer_size Size of buffer in samples
 * @return bool Success status
 */
bool adc_configure_dma1(uint16_t *buffer, uint32_t buffer_size);

/**
 * @brief Start DMA transfer
 * @return bool Success status
 */
bool adc_start_dma(void);

/**
 * @brief Stop DMA transfer
 */
void adc_stop_dma(void);

/**
 * @brief Check DMA transfer status
 * @return bool DMA active status
 */
bool adc_dma_is_active(void);

/**
 * @brief Get current DMA buffer
 * @return uint16_t* Pointer to current buffer
 */
uint16_t* adc_get_current_buffer(void);

/**
 * @brief Switch to next DMA buffer
 * @return bool Success status
 */
bool adc_switch_buffer(void);

// Buffer Management
/**
 * @brief Initialize ADC buffer manager
 * @param manager Pointer to buffer manager structure
 * @param buffers Array of buffer pointers
 * @param buffer_size Size of each buffer in samples
 * @return bool Success status
 */
bool adc_init_buffer_manager(adc_buffer_manager_t *manager, 
                           uint16_t *buffers[], uint32_t buffer_size);

/**
 * @brief Reset buffer manager
 * @param manager Pointer to buffer manager structure
 */
void adc_reset_buffer_manager(adc_buffer_manager_t *manager);

/**
 * @brief Get samples from buffer
 * @param manager Pointer to buffer manager
 * @param samples Pointer to output sample array
 * @param max_samples Maximum number of samples to retrieve
 * @return uint32_t Number of samples retrieved
 */
uint32_t adc_get_samples(adc_buffer_manager_t *manager, 
                        uint16_t *samples, uint32_t max_samples);

/**
 * @brief Check buffer status
 * @param manager Pointer to buffer manager
 * @return bool Buffer has data available
 */
bool adc_buffer_has_data(adc_buffer_manager_t *manager);

/**
 * @brief Clear buffer overflow flag
 * @param manager Pointer to buffer manager
 */
void adc_clear_buffer_overflow(adc_buffer_manager_t *manager);

// Statistics and Monitoring
/**
 * @brief Reset ADC statistics
 * @param stats Pointer to statistics structure
 */
void adc_reset_statistics(adc_statistics_t *stats);

/**
 * @brief Update ADC statistics
 * @param stats Pointer to statistics structure
 * @param samples_processed Number of samples just processed
 */
void adc_update_statistics(adc_statistics_t *stats, uint32_t samples_processed);

/**
 * @brief Get ADC statistics
 * @param stats Pointer to statistics structure to fill
 */
void adc_get_statistics(adc_statistics_t *stats);

/**
 * @brief Calculate actual sample rate from timing
 * @param samples_processed Number of samples processed
 * @param time_elapsed_us Time elapsed in microseconds
 * @return uint32_t Actual sample rate in Sps
 */
uint32_t adc_calculate_sample_rate(uint32_t samples_processed, 
                                  uint32_t time_elapsed_us);

// Timer Configuration for Sample Rate Control
/**
 * @brief Configure Timer1 for ADC conversion control
 * @param sample_rate Desired sample rate in Sps
 * @return bool Success status
 */
bool adc_configure_timer1(uint32_t sample_rate);

/**
 * @brief Start Timer1 for ADC control
 */
void adc_start_timer1(void);

/**
 * @brief Stop Timer1 for ADC control
 */
void adc_stop_timer1(void);

/**
 * @brief Get Timer1 current value
 * @return uint16_t Timer1 current value
 */
uint16_t adc_get_timer1_value(void);

/**
 * @brief Set Timer1 period
 * @param period Timer period value
 */
void adc_set_timer1_period(uint16_t period);

// Interrupt Handling
/**
 * @brief ADC interrupt service routine
 */
void __attribute__((interrupt, no_auto_psv)) _ADCInterrupt(void);

/**
 * @brief DMA0 interrupt service routine (Buffer 1 complete)
 */
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void);

/**
 * @brief DMA1 interrupt service routine (Buffer 2 complete)
 */
void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void);

/**
 * @brief Timer1 interrupt service routine (ADC trigger)
 */
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void);

// Utility Functions
/**
 * @brief Convert ADC raw value to voltage
 * @param adc_value Raw ADC value (0-1023)
 * @return uint32_t Voltage in millivolts
 */
uint32_t adc_raw_to_voltage(uint16_t adc_value);

/**
 * @brief Convert voltage to ADC raw value
 * @param voltage Voltage in millivolts
 * @return uint16_t ADC raw value
 */
uint16_t adc_voltage_to_raw(uint32_t voltage);

/**
 * @brief Calculate dBm from ADC sample
 * @param sample ADC sample value
 * @param reference_voltage Reference voltage in mV
 * @param input_impedance Input impedance in ohms
 * @return int32_t Power in dBm (scaled by 100)
 */
int32_t adc_sample_to_dbm(uint16_t sample, uint32_t reference_voltage, 
                         uint32_t input_impedance);

/**
 * @brief Validate sample rate range
 * @param sample_rate Sample rate to validate
 * @return bool True if within range
 */
bool adc_validate_sample_rate(uint32_t sample_rate);

/**
 * @brief Calculate optimal timer configuration
 * @param sample_rate Desired sample rate
 * @param config Pointer to clock configuration structure
 * @return bool Success status
 */
bool adc_calculate_timer_config(uint32_t sample_rate, adc_clock_config_t *config);

// Calibration and Offset Correction
/**
 * @brief Perform ADC offset calibration
 * @return bool Calibration success
 */
bool adc_calibrate_offset(void);

/**
 * @brief Apply offset correction to sample
 * @param sample Raw ADC sample
 * @return int16_t Offset-corrected sample
 */
int16_t adc_apply_offset_correction(uint16_t sample);

/**
 * @brief Set ADC offset value
 * @param offset Offset value to apply
 */
void adc_set_offset(int16_t offset);

/**
 * @brief Perform gain calibration
 * @return bool Calibration success
 */
bool adc_calibrate_gain(void);

/**
 * @brief Apply gain correction to sample
 * @param sample ADC sample value
 * @return uint16_t Gain-corrected sample
 */
uint16_t adc_apply_gain_correction(uint16_t sample);

/**
 * @brief Set ADC gain value
 * @param gain Gain correction factor (fixed-point Q15)
 */
void adc_set_gain(uint32_t gain);

// Power Management
/**
 * @brief Put ADC in low power mode
 */
void adc_enter_low_power_mode(void);

/**
 * @brief Wake ADC from low power mode
 */
void adc_exit_low_power_mode(void);

/**
 * @brief Configure ADC power management
 * @param low_power Enable low power mode
 */
void adc_configure_power_management(bool low_power);

#endif /* ADC_DMA_H */
