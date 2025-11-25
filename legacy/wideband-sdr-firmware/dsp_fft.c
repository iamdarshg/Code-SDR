/*
 * dsp_fft.c - FFT Acceleration Module for Wideband SDR
 * 
 * Implements software FFT using radix-2 algorithm
 * Optimized for real-time signal processing on dsPIC33AK256MC505
 * 
 * Author: Wideband SDR Development Team
 * Date: 2025-11-19
 */

#include "dsp_fft.h"
#include <math.h>
#include <string.h>

/* FFT Configuration Constants */
#define MAX_FFT_SIZE   16384
#define MIN_FFT_SIZE   256
#define LOG2_MAX_SIZE  14
#define LOG2_MIN_SIZE  8

/* Global FFT accelerator instances */
static fft_accelerator_t fft_accelerators[MAX_FFT_INSTANCES];

/* Window function tables - pre-computed for common window sizes */
static float hann_window[MAX_FFT_SIZE];
static float hamming_window[MAX_FFT_SIZE];
static float blackman_window[MAX_FFT_SIZE];

/* Scaling factors for different modes */
static float scaling_factors[MAX_FFT_SIZE];

/* Static function prototypes */
static bool validate_fft_size(uint32_t size);
static void compute_window_tables(void);
static void initialize_scaling_factors(void);
static uint32_t get_reversed_bits(uint32_t data, uint32_t num_bits);
static void compute_fft_radix2(complex16_t *data, uint32_t n, bool inverse);

/*
 * Initialize FFT accelerator with specified size
 * 
 * @param fft_accel Pointer to FFT accelerator structure
 * @param size FFT size (must be power of 2)
 * @return true if successful, false otherwise
 */
bool dsp_init_fft_accelerator(fft_accelerator_t *fft_accel, uint32_t size)
{
    if (!fft_accel || !validate_fft_size(size)) {
        return false;
    }
    
    /* Initialize FFT accelerator structure */
    fft_accel->fft_size = size;
    fft_accel->scaling_mode = FFT_SCALE_NONE;
    fft_accel->window_type = WINDOW_NONE;
    fft_accel->real_time_mode = false;
    fft_accel->throughput_samples = 0;
    fft_accel->processing_time_us = 0;
    
    /* Allocate memory for FFT buffers */
    fft_accel->fft_input = (complex16_t *)malloc(size * sizeof(complex16_t));
    fft_accel->fft_output = (complex16_t *)malloc(size * sizeof(complex16_t));
    fft_accel->window_coeffs = (float *)malloc(size * sizeof(float));
    fft_accel->spectrum_magnitude = (float *)malloc(size * sizeof(float));
    
    if (!fft_accel->fft_input || !fft_accel->fft_output || 
        !fft_accel->window_coeffs || !fft_accel->spectrum_magnitude) {
        /* Memory allocation failed */
        dsp_cleanup_fft_accelerator(fft_accel);
        return false;
    }
    
    /* Initialize memory */
    memset(fft_accel->fft_input, 0, size * sizeof(complex16_t));
    memset(fft_accel->fft_output, 0, size * sizeof(complex16_t));
    memset(fft_accel->window_coeffs, 0, size * sizeof(float));
    memset(fft_accel->spectrum_magnitude, 0, size * sizeof(float));
    
    /* Initialize window tables if not already done */
    compute_window_tables();
    initialize_scaling_factors();
    
    return true;
}

/*
 * Cleanup FFT accelerator and free resources
 * 
 * @param fft_accel Pointer to FFT accelerator structure
 */
void dsp_cleanup_fft_accelerator(fft_accelerator_t *fft_accel)
{
    if (fft_accel) {
        if (fft_accel->fft_input) {
            free(fft_accel->fft_input);
            fft_accel->fft_input = NULL;
        }
        if (fft_accel->fft_output) {
            free(fft_accel->fft_output);
            fft_accel->fft_output = NULL;
        }
        if (fft_accel->window_coeffs) {
            free(fft_accel->window_coeffs);
            fft_accel->window_coeffs = NULL;
        }
        if (fft_accel->spectrum_magnitude) {
            free(fft_accel->spectrum_magnitude);
            fft_accel->spectrum_magnitude = NULL;
        }
        
        /* Reset structure */
        memset(fft_accel, 0, sizeof(fft_accelerator_t));
    }
}

/*
 * Compute forward FFT using radix-2 algorithm
 * 
 * @param fft_accel Pointer to FFT accelerator structure
 */
void dsp_compute_fft(fft_accelerator_t *fft_accel)
{
    if (!fft_accel || !fft_accel->fft_input || !fft_accel->fft_output) {
        return;
    }
    
    uint32_t size = fft_accel->fft_size;
    
    /* Apply window function if specified */
    if (fft_accel->window_type != WINDOW_NONE) {
        dsp_apply_window(fft_accel->fft_input, size, fft_accel->window_type);
    }
    
    /* Copy input to output buffer for processing */
    memcpy(fft_accel->fft_output, fft_accel->fft_input, size * sizeof(complex16_t));
    
    /* Perform FFT using radix-2 algorithm */
    compute_fft_radix2(fft_accel->fft_output, size, false);
    
    /* Apply scaling if enabled */
    if (fft_accel->scaling_mode == FFT_SCALE_FREQ_DOMAIN) {
        float scale_factor = 1.0f / sqrtf((float)size);
        for (uint32_t i = 0; i < size; i++) {
            fft_accel->fft_output[i].real *= scale_factor;
            fft_accel->fft_output[i].imag *= scale_factor;
        }
    }
    
    /* Update performance metrics */
    fft_accel->throughput_samples += size;
}

/*
 * Compute inverse FFT using radix-2 algorithm
 * 
 * @param fft_accel Pointer to FFT accelerator structure
 */
void dsp_compute_ifft(fft_accelerator_t *fft_accel)
{
    if (!fft_accel || !fft_accel->fft_input || !fft_accel->fft_output) {
        return;
    }
    
    uint32_t size = fft_accel->fft_size;
    
    /* Copy input to output buffer for processing */
    memcpy(fft_accel->fft_output, fft_accel->fft_input, size * sizeof(complex16_t));
    
    /* Perform IFFT using radix-2 algorithm */
    compute_fft_radix2(fft_accel->fft_output, size, true);
    
    /* Apply time domain scaling if enabled */
    if (fft_accel->scaling_mode == FFT_SCALE_TIME_DOMAIN || fft_accel->scaling_mode == FFT_SCALE_NONE) {
        float scale_factor = 1.0f / size;
        for (uint32_t i = 0; i < size; i++) {
            fft_accel->fft_output[i].real *= scale_factor;
            fft_accel->fft_output[i].imag *= scale_factor;
        }
    }
    
    /* Update performance metrics */
    fft_accel->throughput_samples += size;
}

/*
 * Apply window function to data
 * 
 * @param data Pointer to complex data array
 * @param size Size of data array
 * @param window_type Type of window function
 */
void dsp_apply_window(complex16_t *data, uint32_t size, uint8_t window_type)
{
    if (!data || size > MAX_FFT_SIZE) {
        return;
    }
    
    float *window = NULL;
    
    switch (window_type) {
        case WINDOW_HANN:
            window = hann_window;
            break;
        case WINDOW_HAMMING:
            window = hamming_window;
            break;
        case WINDOW_BLACKMAN:
            window = blackman_window;
            break;
        default:
            return; /* No window or unknown window type */
    }
    
    /* Apply window to both I and Q components */
    for (uint32_t i = 0; i < size; i++) {
        data[i].real *= window[i];
        data[i].imag *= window[i];
    }
}

/*
 * Set FFT scaling mode
 * 
 * @param fft_accel Pointer to FFT accelerator structure
 * @param scaling_mode Scaling mode to use
 * @return true if successful, false otherwise
 */
bool dsp_set_fft_scaling_mode(fft_accelerator_t *fft_accel, uint8_t scaling_mode)
{
    if (!fft_accel || scaling_mode >= FFT_SCALE_MAX) {
        return false;
    }
    
    fft_accel->scaling_mode = scaling_mode;
    return true;
}

/*
 * Get FFT performance metrics
 * 
 * @param fft_accel Pointer to FFT accelerator structure
 * @return Performance metrics structure
 */
fft_performance_metrics_t dsp_get_fft_performance(fft_accelerator_t *fft_accel)
{
    fft_performance_metrics_t metrics = {0};
    
    if (fft_accel) {
        metrics.throughput_samples = fft_accel->throughput_samples;
        metrics.fft_size = fft_accel->fft_size;
        metrics.scaling_mode = fft_accel->scaling_mode;
        metrics.real_time_mode = fft_accel->real_time_mode;
        metrics.processing_time_us = fft_accel->processing_time_us;
        
        /* Calculate derived metrics */
        if (fft_accel->processing_time_us > 0) {
            metrics.samples_per_second = (float)fft_accel->throughput_samples * 1000000.0f / fft_accel->processing_time_us;
        }
        metrics.utilization_percent = 0.0f; /* Placeholder - would need actual timing */
    }
    
    return metrics;
}

/*
 * Get spectrum magnitude
 * 
 * @param fft_accel Pointer to FFT accelerator structure
 * @param magnitude Pointer to output magnitude array
 * @return Number of magnitude values written
 */
uint32_t dsp_get_spectrum_magnitude(fft_accelerator_t *fft_accel, float *magnitude)
{
    if (!fft_accel || !magnitude || !fft_accel->fft_output) {
        return 0;
    }
    
    uint32_t size = fft_accel->fft_size;
    
    /* Compute magnitude spectrum */
    for (uint32_t i = 0; i < size; i++) {
        float real = fft_accel->fft_output[i].real;
        float imag = fft_accel->fft_output[i].imag;
        magnitude[i] = sqrtf(real * real + imag * imag);
    }
    
    /* Apply logarithmic scaling if configured */
    if (fft_accel->scaling_mode == FFT_SCALE_LOGARITHMIC) {
        float max_mag = magnitude[0];
        for (uint32_t i = 1; i < size; i++) {
            if (magnitude[i] > max_mag) {
                max_mag = magnitude[i];
            }
        }
        
        /* Convert to dB scale */
        float scale_factor = max_mag > 0 ? 20.0f / logf(max_mag) : 0;
        for (uint32_t i = 0; i < size; i++) {
            magnitude[i] = scale_factor * logf(magnitude[i] + 1e-10f);
        }
    }
    
    return size;
}

/*
 * Get frequency bins for FFT output
 * 
 * @param fft_accel Pointer to FFT accelerator structure
 * @param frequency_bins Pointer to output frequency array
 * @param sample_rate Sample rate in Hz
 * @return Number of frequency bins written
 */
uint32_t dsp_get_frequency_bins(fft_accelerator_t *fft_accel, float *frequency_bins, float sample_rate)
{
    if (!fft_accel || !frequency_bins) {
        return 0;
    }
    
    uint32_t size = fft_accel->fft_size;
    float freq_step = sample_rate / (float)size;
    
    /* Generate frequency bins from -fs/2 to +fs/2 */
    for (uint32_t i = 0; i < size; i++) {
        if (i < size / 2) {
            frequency_bins[i] = (float)i * freq_step;
        } else {
            frequency_bins[i] = (float)(i - size) * freq_step;
        }
    }
    
    return size;
}

/* System integration functions */

bool dsp_fft_system_init(void)
{
    /* Initialize global window tables */
    compute_window_tables();
    initialize_scaling_factors();
    
    /* Initialize all FFT accelerator instances */
    for (uint8_t i = 0; i < MAX_FFT_INSTANCES; i++) {
        memset(&fft_accelerators[i], 0, sizeof(fft_accelerator_t));
    }
    
    return true;
}

void dsp_fft_system_cleanup(void)
{
    /* Cleanup all FFT accelerator instances */
    for (uint8_t i = 0; i < MAX_FFT_INSTANCES; i++) {
        dsp_cleanup_fft_accelerator(&fft_accelerators[i]);
    }
}

fft_accelerator_t* dsp_get_fft_accelerator(uint8_t instance_id)
{
    if (instance_id >= MAX_FFT_INSTANCES) {
        return NULL;
    }
    
    return &fft_accelerators[instance_id];
}

bool dsp_process_realtime_samples(float *real_samples, float *imag_samples, 
                                  uint32_t num_samples, uint32_t sample_rate)
{
    /* Simple real-time processing function */
    if (!real_samples || !imag_samples || num_samples == 0) {
        return false;
    }
    
    /* Process samples (placeholder implementation) */
    for (uint32_t i = 0; i < num_samples; i++) {
        /* Basic processing could go here */
        (void)real_samples[i];  /* Suppress unused variable warning */
        (void)imag_samples[i];  /* Suppress unused variable warning */
    }
    
    return true;
}

bool dsp_compute_spectrum(float *samples, uint32_t num_samples, 
                          float *magnitude, float *frequencies,
                          uint32_t sample_rate, uint32_t *num_bins)
{
    if (!samples || !magnitude || !frequencies || !num_bins) {
        return false;
    }
    
    /* Find nearest power of 2 */
    uint32_t fft_size = 1;
    while (fft_size < num_samples && fft_size < MAX_FFT_SIZE) {
        fft_size <<= 1;
    }
    
    if (fft_size > MAX_FFT_SIZE) {
        fft_size = MAX_FFT_SIZE;
    }
    
    /* Get FFT accelerator */
    fft_accelerator_t *fft_accel = dsp_get_fft_accelerator(0);
    if (!fft_accel) {
        return false;
    }
    
    /* Initialize FFT if needed */
    if (fft_accel->fft_size != fft_size) {
        dsp_cleanup_fft_accelerator(fft_accel);
        if (!dsp_init_fft_accelerator(fft_accel, fft_size)) {
            return false;
        }
    }
    
    /* Copy samples to FFT input */
    for (uint32_t i = 0; i < fft_size; i++) {
        if (i < num_samples) {
            fft_accel->fft_input[i].real = samples[i];
            fft_accel->fft_input[i].imag = imag_samples ? imag_samples[i] : 0.0f;
        } else {
            fft_accel->fft_input[i].real = 0.0f;
            fft_accel->fft_input[i].imag = 0.0f;
        }
    }
    
    /* Compute FFT */
    dsp_compute_fft(fft_accel);
    
    /* Get magnitude and frequency bins */
    *num_bins = dsp_get_spectrum_magnitude(fft_accel, magnitude);
    dsp_get_frequency_bins(fft_accel, frequencies, sample_rate);
    
    return true;
}

/* Static helper functions */

/*
 * Radix-2 FFT implementation
 * 
 * @param data Pointer to complex data array
 * @param n Size of data array (must be power of 2)
 * @param inverse true for IFFT, false for FFT
 */
static void compute_fft_radix2(complex16_t *data, uint32_t n, bool inverse)
{
    if (!data || n < 2) {
        return;
    }
    
    /* Bit reversal permutation */
    for (uint32_t i = 0, j = 0; i < n; i++) {
        if (i < j) {
            /* Swap data[i] and data[j] */
            float temp_real = data[i].real;
            float temp_imag = data[i].imag;
            data[i].real = data[j].real;
            data[i].imag = data[j].imag;
            data[j].real = temp_real;
            data[j].imag = temp_imag;
        }
        
        uint32_t m = n >> 1;
        while (m >= 1 && j >= m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    
    /* Cooley-Tukey FFT */
    for (uint32_t len = 2; len <= n; len <<= 1) {
        float angle = (inverse ? 2.0f : -2.0f) * (float)M_PI / len;
        float wlen_real = cosf(angle);
        float wlen_imag = sinf(angle);
        
        for (uint32_t i = 0; i < n; i += len) {
            float w_real = 1.0f;
            float w_imag = 0.0f;
            
            for (uint32_t j = 0; j < (len >> 1); j++) {
                uint32_t u_idx = i + j;
                uint32_t v_idx = i + j + (len >> 1);
                
                float u_real = data[u_idx].real;
                float u_imag = data[u_idx].imag;
                float v_real = data[v_idx].real;
                float v_imag = data[v_idx].imag;
                
                /* t = w * v */
                float t_real = w_real * v_real - w_imag * v_imag;
                float t_imag = w_real * v_imag + w_imag * v_real;
                
                /* u = u + t */
                data[u_idx].real = u_real + t_real;
                data[u_idx].imag = u_imag + t_imag;
                
                /* v = u - t */
                data[v_idx].real = u_real - t_real;
                data[v_idx].imag = u_imag - t_imag;
                
                /* w = w * wlen */
                float next_w_real = w_real * wlen_real - w_imag * wlen_imag;
                float next_w_imag = w_real * wlen_imag + w_imag * wlen_real;
                w_real = next_w_real;
                w_imag = next_w_imag;
            }
        }
    }
}

/*
 * Validate FFT size (must be power of 2 and within limits)
 * 
 * @param size FFT size to validate
 * @return true if valid, false otherwise
 */
static bool validate_fft_size(uint32_t size)
{
    if (size < MIN_FFT_SIZE || size > MAX_FFT_SIZE) {
        return false;
    }
    
    /* Check if size is power of 2 */
    return (size & (size - 1)) == 0;
}

/*
 * Compute window function tables
 * Pre-computes Hann, Hamming, and Blackman windows for common sizes
 */
static void compute_window_tables(void)
{
    uint32_t size = MAX_FFT_SIZE;
    
    /* Hann Window */
    for (uint32_t i = 0; i < size; i++) {
        hann_window[i] = 0.5f - 0.5f * cosf(2.0f * (float)M_PI * (float)i / (size - 1));
    }
    
    /* Hamming Window */
    for (uint32_t i = 0; i < size; i++) {
        hamming_window[i] = 0.54f - 0.46f * cosf(2.0f * (float)M_PI * (float)i / (size - 1));
    }
    
    /* Blackman Window */
    for (uint32_t i = 0; i < size; i++) {
        float term1 = 0.42f;
        float term2 = 0.5f * cosf(2.0f * (float)M_PI * (float)i / (size - 1));
        float term3 = 0.08f * cosf(4.0f * (float)M_PI * (float)i / (size - 1));
        blackman_window[i] = term1 - term2 + term3;
    }
}

/*
 * Initialize scaling factors table
 */
static void initialize_scaling_factors(void)
{
    for (uint32_t i = 0; i < MAX_FFT_SIZE; i++) {
        scaling_factors[i] = 1.0f;
    }
}

/*
 * Bit reversal utility for FFT reordering
 * 
 * @param data Input data
 * @param num_bits Number of bits to reverse
 * @return Reversed bit pattern
 */
static uint32_t get_reversed_bits(uint32_t data, uint32_t num_bits)
{
    uint32_t reversed = 0;
    for (uint32_t i = 0; i < num_bits; i++) {
        if (data & (1 << i)) {
            reversed |= (1 << (num_bits - 1 - i));
        }
    }
    return reversed;
}
