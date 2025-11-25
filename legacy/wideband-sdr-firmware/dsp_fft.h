/*
 * dsp_fft.h - FFT Acceleration Module Header for Wideband SDR
 * 
 * Header file for hardware-accelerated FFT using Microchip libq DSP library
 * Optimized for real-time signal processing on dsPIC33AK256MC505
 * 
 * Author: Wideband SDR Development Team
 * Date: 2025-11-19
 */

#ifndef DSP_FFT_H
#define DSP_FFT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* FFT Configuration Constants */
#define MAX_FFT_INSTANCES    8
#define MAX_FFT_SIZE         16384
#define MIN_FFT_SIZE         256

/* Window function definitions */
#define WINDOW_NONE          0
#define WINDOW_HANN          1
#define WINDOW_HAMMING       2
#define WINDOW_BLACKMAN      3

/* FFT Scaling modes */
#define FFT_SCALE_NONE       0
#define FFT_SCALE_TIME_DOMAIN    1
#define FFT_SCALE_FREQ_DOMAIN    2
#define FFT_SCALE_LOGARITHMIC    3
#define FFT_SCALE_MAX            4

/* Data type definitions */
typedef struct {
    float real;
    float imag;
} complex16_t;

typedef struct {
    float32_t real;
    float32_t imag;
} complex32_t;

/* FFT Accelerator Structure */
typedef struct {
    complex16_t *fft_input;
    complex16_t *fft_output;
    float16_t *window_coeffs;
    float16_t *spectrum_magnitude;
    uint32_t fft_size;
    uint8_t scaling_mode;
    uint8_t window_type;
    bool real_time_mode;
    uint32_t throughput_samples;
    uint32_t processing_time_us;
} fft_accelerator_t;

/* Performance metrics structure */
typedef struct {
    uint32_t throughput_samples;
    uint32_t fft_size;
    uint8_t scaling_mode;
    bool real_time_mode;
    uint32_t processing_time_us;
    float samples_per_second;
    float utilization_percent;
} fft_performance_metrics_t;

/* Function Prototypes */

/* Core FFT functions */
bool dsp_init_fft_accelerator(fft_accelerator_t *fft_accel, uint32_t size);
void dsp_cleanup_fft_accelerator(fft_accelerator_t *fft_accel);
void dsp_compute_fft(fft_accelerator_t *fft_accel);
void dsp_compute_ifft(fft_accelerator_t *fft_accel);

/* Window and scaling functions */
void dsp_apply_window(complex16_t *data, uint32_t size, uint8_t window_type);
bool dsp_set_fft_scaling_mode(fft_accelerator_t *fft_accel, uint8_t scaling_mode);

/* Performance and utilities */
fft_performance_metrics_t dsp_get_fft_performance(fft_accelerator_t *fft_accel);
uint32_t dsp_get_spectrum_magnitude(fft_accelerator_t *fft_accel, float16_t *magnitude);
uint32_t dsp_get_frequency_bins(fft_accelerator_t *fft_accel, float32_t *frequency_bins, float32_t sample_rate);

/* Utility functions for the Wideband SDR system integration */
bool dsp_fft_system_init(void);
void dsp_fft_system_cleanup(void);
fft_accelerator_t* dsp_get_fft_accelerator(uint8_t instance_id);
bool dsp_process_realtime_samples(float32_t *real_samples, float32_t *imag_samples, 
                                  uint32_t num_samples, uint32_t sample_rate);
bool dsp_compute_spectrum(float32_t *samples, uint32_t num_samples, 
                          float32_t *magnitude, float32_t *frequencies,
                          uint32_t sample_rate, uint32_t *num_bins);

#endif /* DSP_FFT_H */
