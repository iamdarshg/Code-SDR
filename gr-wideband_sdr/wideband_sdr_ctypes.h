#ifndef WIDEBAND_SDR_CTYPES_H
#define WIDEBAND_SDR_CTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Python ctypes bridge functions for Wideband SDR integration */

/* Device control functions */
int ctypes_open_device(int device_index);
void ctypes_close_device();
int ctypes_set_frequency(int frequency);
int ctypes_set_sample_rate(int sample_rate);
int ctypes_set_gain(int gain);

/* Device status functions */
int ctypes_get_frequency();
int ctypes_get_sample_rate();
int ctypes_get_gain();

/* Streaming functions */
int ctypes_start_streaming();
int ctypes_stop_streaming();
int ctypes_read_samples(float* buffer, int max_samples);

/* Information functions */
const char* ctypes_get_device_info_json();
const char* ctypes_get_statistics_json();

/* Utility functions */
int wideband_sdr_is_available();

#ifdef __cplusplus
}
#endif

#endif /* WIDEBAND_SDR_CTYPES_H */
