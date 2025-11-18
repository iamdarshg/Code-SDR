/*
 * Wideband SDR ExtIO Plugin Interface
 * Standard ExtIO interface for HDSDR and SDR# compatibility
 * 
 * This file defines the ExtIO plugin interface that HDSDR/SDR# applications
 * use to communicate with external hardware devices.
 */

#pragma once

#include <windows.h>

// ExtIO plugin version
#define EXTIO_VERSION 1

// ExtIO interface version structure
typedef struct {
    long srate;           // Sample rate (Hz)
    long tunefreq;        // Tuning frequency (Hz)
    long iffreq;          // IF frequency (Hz)
    char modulation[12];  // Modulation type
    long bandwidth;       // Bandwidth (Hz)
    int modenum;          // Mode number
} EXT_RADIOINIT;

// Status flags
#define EXTIO_SUCCESS     0
#define EXTIO_ERROR      -1
#define EXTIO_BUSY       -2
#define EXTIO_TIMEOUT    -3

// Device type information
typedef struct {
    char name[64];       // Device name
    char manufacturer[64]; // Manufacturer
    int channels;        // Number of channels
    long minFreq;        // Minimum frequency (Hz)
    long maxFreq;        // Maximum frequency (Hz)
    long minRate;        // Minimum sample rate
    long maxRate;        // Maximum sample rate
} EXT_DEVICEINFO;

// Forward declarations
class WidebandSDRExtIO;
class ExtIOPlugin;

// Plugin management functions
extern "C" {
    /**
     * Initialize hardware and return device info
     * @param hwtype Buffer for hardware type string
     * @param drv Buffer for driver info string
     * @param init Pointer to initialization structure
     * @return 0 on success, -1 on error
     */
    long __stdcall InitHW(char *hwtype, char *drv, EXT_RADIOINIT *init);

    /**
     * Close hardware connection
     */
    void __stdcall CloseHW(void);

    /**
     * Start hardware operation at specified frequency
     * @param freq Frequency in Hz
     * @return 0 on success, -1 on error
     */
    int __stdcall StartHW(long freq);

    /**
     * Stop hardware operation
     */
    void __stdcall StopHW(void);

    /**
     * Set local oscillator frequency
     * @param LOfreq LO frequency in Hz
     * @return Current LO frequency
     */
    long __stdcall SetHWLO(long LOfreq);

    /**
     * Get current local oscillator frequency
     * @return Current LO frequency in Hz
     */
    long __stdcall GetHWLO(void);

    /**
     * Get hardware status
     * @return Status code
     */
    int __stdcall GetStatus(void);

    /**
     * Show hardware GUI (optional)
     */
    void __stdcall ShowGUI(void);

    /**
     * Hide hardware GUI (optional)
     */
    void __stdcall HideGUI(void);

    /**
     * Get tuned frequency
     * @return Current tuned frequency in Hz
     */
    long __stdcall GetTune(void);

    /**
     * Get current mode
     * @return Current mode number
     */
    int __stdcall GetMode(void);

    /**
     * Get current gain
     * @return Current gain value
     */
    int __stdcall GetGain(void);

    /**
     * Get hardware type
     * @param[out] hardwareType Hardware type identifier
     * @param[out] supportedModes Bitmask of supported modes
     */
    void __stdcall GetHWType(char *hardwareType, int *supportedModes);

    /**
     * Set sample rate
     * @param rate Sample rate in Hz
     * @return Actual sample rate set
     */
    long __stdcall SetHWLO(long LOfreq);

    /**
     * Get current sample rate
     * @return Current sample rate in Hz
     */
    long __stdcall GetHWSR(void);

    /**
     * Called when HDSDR starts recording
     * @param szFileName Full path to output file
     * @param bytesToWrite Number of bytes to write
     * @return 0 on success, -1 on error
     */
    int __stdcall GetStatus(int what_status, long value, char *units, int *dims);

    /**
     * Get extended status information
     * @param what_status Status type
     * @param value Status value
     * @param units String buffer for units
     * @param dims Dimension array
     * @return Status value
     */
    long __stdcall GetStatus(int what_status, long value, char *units, int *dims);
}

// Convenience functions for plugin management
#ifdef __cplusplus
extern "C" {
#endif
    
    /**
     * Get plugin version
     * @return Plugin version string
     */
    const char* __stdcall GetExtIOVersion(void);

    /**
     * Get plugin capabilities
     * @return Capability bitmask
     */
    unsigned int __stdcall GetExtIOCapabilities(void);

    /**
     * Initialize plugin with device index
     * @param deviceIndex Index of device to use (0-based)
     * @return 0 on success, -1 on error
     */
    int __stdcall InitDevice(int deviceIndex);

    /**
     * Get number of available devices
     * @return Number of devices found
     */
    int __stdcall GetDeviceCount(void);

    /**
     * Get device information
     * @param deviceIndex Device index
     * @param deviceInfo Pointer to device info structure
     * @return 0 on success, -1 on error
     */
    int __stdcall GetDeviceInfo(int deviceIndex, EXT_DEVICEINFO *deviceInfo);

    /**
     * Start/Stop streaming samples
     * @param enable Enable (1) or disable (0) streaming
     * @return 0 on success, -1 on error
     */
    int __stdcall SetStreaming(int enable);

    /**
     * Get streaming status
     * @return 1 if streaming, 0 if stopped
     */
    int __stdcall GetStreaming(void);

#ifdef __cplusplus
}
#endif

// Constants for capabilities
#define EXTIO_CAP_SAMPLE_RATE     0x00000001
#define EXTIO_CAP_GAIN            0x00000002
#define EXTIO_CAP_FREQ_TUNE       0x00000004
#define EXTIO_CAP_DIRECT_SAMPLING 0x00000008
#define EXTIO_CAP_IQ_BUFFERING    0x00000010
#define EXTIO_CAP_THREAD_SAFE     0x00000020
#define EXTIO_CAP_MULTI_DEVICE    0x00000040

// Mode definitions (standard HDSDR modes)
#define MODE_AM     0
#define MODE_AMN    1
#define MODE_AMS    2
#define MODE_LSB    3
#define MODE_USB    4
#define MODE_FM     5
#define MODE_FMN    6
#define MODE_WFM    7
#define MODE_DIGL   8
#define MODE_DIGU   9

// Status types for GetStatus function
#define STATUS_SAMPLES_READY      0
#define STATUS_SAMPLES_DROPPED    1
#define STATUS_OVERRUN            2
#define STATUS_UNDERRUN           3
#define STATUS_BUFFER_SIZE        4
#define STATUS_SAMPLE_RATE        5
#define STATUS_FREQUENCY          6
#define STATUS_GAIN               7
#define STATUS_TEMPERATURE        8
#define STATUS_USB_UTILIZATION    9
