/*
 * Wideband SDR - ADF4351 PLL Driver Header
 * dsPIC33AK256MC505 based SDR with ADF4351 frequency synthesizer
 * 
 * Created: November 2025
 * Version: 1.0.0
 * License: MIT
 * 
 * Features:
 * - Fractional-N synthesis
 * - 35 MHz - 4.4 GHz frequency range
 * - Sub-Hz frequency resolution
 * - Automatic output divider selection
 * - Ultra-low phase noise
 */

#ifndef ADF4351_H
#define ADF4351_H

#include <stdint.h>
#include <stdbool.h>

// ADF4351 Register Definitions
#define ADF4351_REG0    0x00
#define ADF4351_REG1    0x01
#define ADF4351_REG2    0x02
#define ADF4351_REG3    0x03
#define ADF4351_REG4    0x04
#define ADF4351_REG5    0x05

// Frequency Range Definitions
#define ADF4351_MIN_FREQ    35000000UL      // 35 MHz
#define ADF4351_MAX_FREQ    4400000000UL    // 4.4 GHz
#define ADF4351_MIN_FREQ_DIV 1000000UL      // 1 MHz (with divide-by-64)
#define ADF4351_MAX_FREQ_EXT 10000000000UL  // 10 GHz (with external mixer)

// PLL Configuration Structure
typedef struct {
    uint32_t freq;           // Target frequency in Hz
    uint32_t ref_freq;       // Reference frequency in Hz
    uint16_t int_val;        // Integer value
    uint32_t frac_val;       // Fractional value
    uint16_t mod_val;        // Modulus value
    uint8_t div_select;      // Output divider selection (1-64)
    uint8_t output_power;    // Output power level (0-3)
    bool enable;             // PLL enable status
} adf4351_config_t;

// Register Structures
typedef union {
    struct {
        unsigned int FRAC    :12;   // Fractional value (bits 0-11)
        unsigned int INT     :16;   // Integer value (bits 12-27)
        unsigned int reserved :3;   // Reserved (bits 28-30)
        unsigned int LD      :1;    // Lock detect (bit 31)
    } bits;
    uint32_t value;
} adf4351_reg0_t;

typedef union {
    struct {
        unsigned int FRAC    :12;   // Fractional value (bits 0-11)
        unsigned int MOD     :12;   // Modulus value (bits 12-23)
        unsigned int PHASE   :12;   // Phase value (bits 24-35)
        unsigned int reserved :4;   // Reserved (bits 36-39)
    } bits;
    uint32_t value;
} adf4351_reg1_t;

typedef union {
    struct {
        unsigned int CR      :1;    // Charge pump current (bit 0)
        unsigned int CP3     :1;    // CP3 setting (bit 1)
        unsigned int LDP     :1;    // Lock detect precision (bit 2)
        unsigned int LDF     :1;    // Lock detect filter (bit 3)
        unsigned int CPD     :4;    // Charge pump direction (bits 4-7)
        unsigned int MUXOUT  :3;    // Multiplexer output (bits 8-10)
        unsigned int REF_DIV :10;   // Reference divider (bits 11-20)
        unsigned int REF_DOUBLER :1; // Reference doubler (bit 21)
        unsigned int R_A     :3;    // R counter (bits 22-24)
        unsigned int reserved :2;   // Reserved (bits 25-26)
        unsigned int LD      :1;    // Lock detect (bit 27)
        unsigned int MUXOUT_LD :4;  // MUXOUT lock detect (bits 28-31)
    } bits;
    uint32_t value;
} adf4351_reg2_t;

typedef union {
    struct {
        unsigned int CPG     :1;    // CP gain (bit 0)
        unsigned int CP3     :1;    // CP3 setting (bit 1)
        unsigned int reserved0 :1;  // Reserved (bit 2)
        unsigned int CSR     :1;    // Cycle slip reduction (bit 3)
        unsigned int reserved1 :8;  // Reserved (bits 4-11)
        unsigned int FE      :4;    // Fine economy (bits 12-15)
        unsigned int reserved2 :2;  // Reserved (bits 16-17)
        unsigned int F01     :2;    // Fine gain (bits 18-19)
        unsigned int VCODIVSEL :6;  // VCO divider select (bits 20-25)
        unsigned int VCOAUTO :1;    // VCO auto selection (bit 26)
        unsigned int MUXOUT  :3;    // Multiplexer output (bits 27-29)
        unsigned int reserved3 :1;  // Reserved (bit 30)
        unsigned int LD      :1;    // Lock detect (bit 31)
    } bits;
    uint32_t value;
} adf4351_reg3_t;

typedef union {
    struct {
        unsigned int OUTPOWER :2;   // Output power (bits 0-1)
        unsigned int OUTA_PD  :1;   // Output A power down (bit 2)
        unsigned int OUTB_PD  :1;   // Output B power down (bit 3)
        unsigned int RFENB    :1;   // RF enable (bit 4)
        unsigned int reserved0 :10; // Reserved (bits 5-14)
        unsigned int DIVSELECT:6;   // Divider select (bits 15-20)
        unsigned int reserved1 :3;  // Reserved (bits 21-23)
        unsigned int MUXOUT  :3;    // Multiplexer output (bits 24-26)
        unsigned int reserved2 :4;  // Reserved (bits 27-30)
        unsigned int LD      :1;    // Lock detect (bit 31)
    } bits;
    uint32_t value;
} adf4351_reg4_t;

typedef union {
    struct {
        unsigned int reserved0 :28; // Reserved (bits 0-27)
        unsigned int reserved1 :1;  // Reserved (bit 28)
        unsigned int LD        :1;  // Lock detect (bit 29)
        unsigned int MUXOUT    :3;  // Multiplexer output (bits 30-32)
    } bits;
    uint32_t value;
} adf4351_reg5_t;

// Function Prototypes

/**
 * @brief Initialize ADF4351 PLL
 * @param config Pointer to configuration structure
 * @return bool Success status
 */
bool adf4351_init(adf4351_config_t *config);

/**
 * @brief Set PLL frequency
 * @param freq Frequency in Hz (35 MHz - 4.4 GHz)
 * @return bool Success status
 */
bool adf4351_set_frequency(uint32_t freq);

/**
 * @brief Get current PLL frequency
 * @return uint32_t Current frequency in Hz
 */
uint32_t adf4351_get_frequency(void);

/**
 * @brief Enable/disable PLL output
 * @param enable Enable flag
 */
void adf4351_enable_output(bool enable);

/**
 * @brief Set output power level
 * @param power Power level (0-3, where 3 is highest)
 */
void adf4351_set_output_power(uint8_t power);

/**
 * @brief Check PLL lock status
 * @return bool Lock status
 */
bool adf4351_is_locked(void);

/**
 * @brief Configure PLL for specific frequency range
 * @param freq Target frequency
 * @return bool Success status
 */
bool adf4351_configure_frequency(uint32_t freq);

/**
 * @brief Calculate PLL parameters for given frequency
 * @param freq Target frequency in Hz
 * @param config Configuration structure to fill
 * @return bool Success status
 */
bool adf4351_calculate_params(uint32_t freq, adf4351_config_t *config);

/**
 * @brief Write single register to ADF4351
 * @param reg Register number (0-5)
 * @param data Register data
 */
void adf4351_write_register(uint8_t reg, uint32_t data);

/**
 * @brief Read register from ADF4351
 * @param reg Register number (0-5)
 * @return uint32_t Register data
 */
uint32_t adf4351_read_register(uint8_t reg);

/**
 * @brief Send complete register map to ADF4351
 * @param reg0 Register 0 data
 * @param reg1 Register 1 data
 * @param reg2 Register 2 data
 * @param reg3 Register 3 data
 * @param reg4 Register 4 data
 * @param reg5 Register 5 data
 */
void adf4351_write_all_registers(uint32_t reg0, uint32_t reg1, uint32_t reg2,
                                uint32_t reg3, uint32_t reg4, uint32_t reg5);

/**
 * @brief Set reference frequency
 * @param ref_freq Reference frequency in Hz
 */
void adf4351_set_reference_frequency(uint32_t ref_freq);

/**
 * @brief Get reference frequency
 * @return uint32_t Reference frequency in Hz
 */
uint32_t adf4351_get_reference_frequency(void);

/**
 * @brief Reset PLL to default state
 */
void adf4351_reset(void);

/**
 * @brief Power down PLL
 */
void adf4351_power_down(void);

/**
 * @brief Wake up PLL from power down
 */
void adf4351_wake_up(void);

/**
 * @brief Calibrate VCO
 * @return bool Calibration success
 */
bool adf4351_calibrate_vco(void);

/**
 * @brief Set VCO auto selection
 * @param auto_sel Auto selection flag
 */
void adf4351_set_vco_auto_selection(bool auto_sel);

/**
 * @brief Get VCO selection
 * @return uint8_t VCO divider selection
 */
uint8_t adf4351_get_vco_selection(void);

/**
 * @brief Calculate output frequency with divider
 * @param vco_freq VCO frequency
 * @param divider Output divider value
 * @return uint32_t Output frequency
 */
uint32_t adf4351_calculate_output_frequency(uint32_t vco_freq, uint8_t divider);

/**
 * @brief Find optimal output divider
 * @param freq Target frequency
 * @return uint8_t Optimal divider value (1,2,4,8,16,32,64)
 */
uint8_t adf4351_find_optimal_divider(uint32_t freq);

/**
 * @brief Validate frequency range
 * @param freq Frequency to validate
 * @return bool True if frequency is within range
 */
bool adf4351_validate_frequency(uint32_t freq);

/**
 * @brief Calculate phase frequency detector frequency
 * @param ref_freq Reference frequency
 * @param ref_div Reference divider
 * @param ref_doubler Reference doubler enabled
 * @return uint32_t PFD frequency
 */
uint32_t adf4351_calculate_pfd_frequency(uint32_t ref_freq, uint16_t ref_div, bool ref_doubler);

// SPI Communication Functions

/**
 * @brief Send byte via SPI
 * @param data Byte to send
 */
void adf4351_spi_send_byte(uint8_t data);

/**
 * @brief Send 32-bit word via SPI
 * @param word 32-bit word to send
 */
void adf4351_spi_send_word(uint32_t word);

/**
 * @brief Set SPI clock high
 */
void adf4351_spi_clock_high(void);

/**
 * @brief Set SPI clock low
 */
void adf4351_spi_clock_low(void);

/**
 * @brief Set SPI data high
 */
void adf4351_spi_data_high(void);

/**
 * @brief Set SPI data low
 */
void adf4351_spi_data_low(void);

/**
 * @brief Set SPI load enable high
 */
void adf4351_spi_load_enable_high(void);

/**
 * @brief Set SPI load enable low
 */
void adf4351_spi_load_enable_low(void);

/**
 * @brief SPI delay for timing
 * @param cycles Delay cycles
 */
void adf4351_spi_delay(uint8_t cycles);

#endif /* ADF4351_H */
