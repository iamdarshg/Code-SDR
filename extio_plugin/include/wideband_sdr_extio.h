/*
 * Wideband SDR ExtIO Plugin Implementation Header
 * C++ wrapper for integrating WidebandSDR Python driver with ExtIO interface
 */

#pragma once

#include "extio.h"
#include <windows.h>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

// Forward declarations for Python integration
namespace py {
    class module;
    class object;
}

class WidebandSDRExtIO {
private:
    // Device state
    std::string deviceName;
    std::string deviceType;
    bool isInitialized;
    bool isStreaming;
    bool deviceConnected;
    
    // Configuration parameters
    long currentFrequency;
    long currentSampleRate;
    int currentGain;
    int currentMode;
    
    // Hardware limits
    long minFrequency;
    long maxFrequency;
    long minSampleRate;
    long maxSampleRate;
    int minGain;
    int maxGain;
    
    // Device information
    EXT_DEVICEINFO deviceInfo;
    
    // Buffer management
    static constexpr size_t BUFFER_SIZE = 16384;  // 16K samples
    static constexpr size_t MAX_BUFFER_COUNT = 4;
    std::vector<std::vector<short>> sampleBuffers;
    std::atomic<size_t> writeBufferIndex;
    std::atomic<size_t> readBufferIndex;
    std::atomic<size_t> samplesAvailable;
    
    // Threading
    std::thread streamingThread;
    std::mutex bufferMutex;
    std::condition_variable bufferCondition;
    std::atomic<bool> stopStreaming;
    
    // Python integration
    void* pythonHandle;
    void* widebandSDRModule;
    void* widebandSDRClass;
    void* sdrInstance;
    
    // Statistics
    struct {
        std::atomic<uint64_t> samplesReceived;
        std::atomic<uint64_t> samplesProcessed;
        std::atomic<uint32_t> bufferOverruns;
        std::atomic<uint32_t> usbErrors;
        uint64_t lastSampleTime;
    } stats;
    
public:
    /**
     * Constructor
     */
    WidebandSDRExtIO();
    
    /**
     * Destructor
     */
    ~WidebandSDRExtIO();
    
    /**
     * Initialize the plugin
     * @return true on success, false on failure
     */
    bool Initialize();
    
    /**
     * Cleanup resources
     */
    void Cleanup();
    
    /**
     * Open device connection
     * @param deviceIndex Index of device to open (0-based)
     * @return true on success, false on failure
     */
    bool OpenDevice(int deviceIndex = 0);
    
    /**
     * Close device connection
     */
    void CloseDevice();
    
    /**
     * Start hardware operation
     * @param frequency Frequency in Hz
     * @return 0 on success, -1 on error
     */
    int StartHardware(long frequency);
    
    /**
     * Stop hardware operation
     */
    void StopHardware();
    
    /**
     * Set frequency
     * @param frequency Frequency in Hz
     * @return 0 on success, -1 on error
     */
    int SetFrequency(long frequency);
    
    /**
     * Get current frequency
     * @return Current frequency in Hz
     */
    long GetFrequency() const { return currentFrequency; }
    
    /**
     * Set sample rate
     * @param rate Sample rate in Hz
     * @return Actual sample rate set
     */
    long SetSampleRate(long rate);
    
    /**
     * Get current sample rate
     * @return Current sample rate in Hz
     */
    long GetSampleRate() const { return currentSampleRate; }
    
    /**
     * Set gain
     * @param gain Gain value
     * @return 0 on success, -1 on error
     */
    int SetGain(int gain);
    
    /**
     * Get current gain
     * @return Current gain
     */
    int GetGain() const { return currentGain; }
    
    /**
     * Get device information
     * @param deviceIndex Device index
     * @param info Pointer to device info structure
     * @return 0 on success, -1 on error
     */
    int GetDeviceInfo(int deviceIndex, EXT_DEVICEINFO* info);
    
    /**
     * Enumerate available devices
     * @return Number of devices found
     */
    int EnumerateDevices();
    
    /**
     * Get hardware status
     * @return Status code
     */
    int GetHardwareStatus();
    
    /**
     * Get extended status information
     * @param what Status type
     * @param value Status value
     * @param units Units string buffer
     * @param dims Dimension array
     * @return Status value
     */
    long GetStatus(int what, long value, char* units, int* dims);
    
    /**
     * Set local oscillator frequency
     * @param loFreq LO frequency in Hz
     * @return Current LO frequency
     */
    long SetLOFrequency(long loFreq);
    
    /**
     * Get current LO frequency
     * @return Current LO frequency in Hz
     */
    long GetLOFrequency() const { return currentFrequency; }
    
    /**
     * Get hardware type information
     * @param hardwareType Buffer for hardware type
     * @param supportedModes Pointer to supported modes bitmask
     */
    void GetHWType(char* hardwareType, int* supportedModes);
    
    /**
     * Check if device is connected
     * @return true if connected, false otherwise
     */
    bool IsConnected() const { return deviceConnected; }
    
    /**
     * Check if streaming is active
     * @return true if streaming, false otherwise
     */
    bool IsStreaming() const { return isStreaming; }
    
    /**
     * Get plugin version
     * @return Version string
     */
    static const char* GetVersion();
    
    /**
     * Get plugin capabilities
     * @return Capability bitmask
     */
    static unsigned int GetCapabilities();
    
private:
    /**
     * Initialize Python integration
     * @return true on success, false on failure
     */
    bool InitializePython();
    
    /**
     * Cleanup Python integration
     */
    void CleanupPython();
    
    /**
     * Start streaming thread
     * @return true on success, false on failure
     */
    bool StartStreaming();
    
    /**
     * Stop streaming thread
     */
    void StopStreaming();
    
    /**
     * Streaming thread function
     */
    void StreamingThread();
    
    /**
     * Python callback for samples
     * @param samples Sample data pointer
     * @param count Number of samples
     */
    void OnSamples(const short* samples, size_t count);
    
    /**
     * Read samples from device
     * @param buffer Buffer to store samples
     * @param count Number of samples to read
     * @return Number of samples read, 0 if no data, -1 on error
     */
    int ReadSamples(short* buffer, size_t count);
    
    /**
     * Log message
     * @param level Log level
     * @param message Message to log
     */
    void LogMessage(int level, const char* message);
    
    /**
     * Check device limits
     * @param frequency Frequency to check
     * @param sampleRate Sample rate to check
     * @param gain Gain to check
     * @return true if all parameters are valid
     */
    bool ValidateParameters(long frequency, long sampleRate, int gain);
};

// Global plugin instance
extern WidebandSDRExtIO* g_pluginInstance;

// Utility functions
namespace ExtIOUtils {
    /**
     * Convert frequency to string
     * @param frequency Frequency in Hz
     * @return Formatted frequency string
     */
    std::string FrequencyToString(long frequency);
    
    /**
     * Convert sample rate to string
     * @param rate Sample rate in Hz
     * @return Formatted sample rate string
     */
    std::string SampleRateToString(long rate);
    
    /**
     * Format error message
     * @param errorCode Error code
     * @param context Context information
     * @return Formatted error message
     */
    std::string FormatError(int errorCode, const std::string& context);
}

// Constants
#define WIDEBAND_SDR_EXTIO_VERSION "1.0.0"
#define WIDEBAND_SDR_DEVICE_NAME "Wideband SDR (1 MHz - 10 GHz)"
#define WIDEBAND_SDR_MANUFACTURER "Wideband SDR Project"

#define WIDEBAND_SDR_MIN_FREQ 1000000      // 1 MHz
#define WIDEBAND_SDR_MAX_FREQ 10000000000  // 10 GHz
#define WIDEBAND_SDR_MIN_RATE 1000         // 1 kSPS
#define WIDEBAND_SDR_MAX_RATE 105000000    // 105 MSPS
#define WIDEBAND_SDR_MIN_GAIN 0
#define WIDEBAND_SDR_MAX_GAIN 63

// Log levels
#define EXTIO_LOG_ERROR   0
#define EXTIO_LOG_WARNING 1
#define EXTIO_LOG_INFO    2
#define EXTIO_LOG_DEBUG   3
