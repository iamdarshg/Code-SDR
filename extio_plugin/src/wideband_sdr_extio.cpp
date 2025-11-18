/*
 * WidebandSDRExtIO Implementation
 * C++ implementation of the WidebandSDR wrapper class
 */

#include "wideband_sdr_extio.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <atomic>

// Define M_PI if not available
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =============================================================================
// WidebandSDRExtIO Implementation
// =============================================================================

WidebandSDRExtIO::WidebandSDRExtIO()
    : isInitialized(false)
    , isStreaming(false)
    , deviceConnected(false)
    , currentFrequency(100000000)  // 100 MHz default
    , currentSampleRate(1000000)   // 1 MSPS default
    , currentGain(30)              // 30 dB default
    , currentMode(0)               // AM mode
    , minFrequency(WIDEBAND_SDR_MIN_FREQ)
    , maxFrequency(WIDEBAND_SDR_MAX_FREQ)
    , minSampleRate(WIDEBAND_SDR_MIN_RATE)
    , maxSampleRate(WIDEBAND_SDR_MAX_RATE)
    , minGain(WIDEBAND_SDR_MIN_GAIN)
    , maxGain(WIDEBAND_SDR_MAX_GAIN)
    , writeBufferIndex(0)
    , readBufferIndex(0)
    , samplesAvailable(0)
    , stopStreaming(false)
    , pythonHandle(nullptr)
    , widebandSDRModule(nullptr)
    , widebandSDRClass(nullptr)
    , sdrInstance(nullptr) {
    
    // Initialize device info
    memset(&deviceInfo, 0, sizeof(deviceInfo));
    strcpy(deviceInfo.name, WIDEBAND_SDR_DEVICE_NAME);
    strcpy(deviceInfo.manufacturer, WIDEBAND_SDR_MANUFACTURER);
    deviceInfo.channels = 1;
    deviceInfo.minFreq = minFrequency;
    deviceInfo.maxFreq = maxFrequency;
    deviceInfo.minRate = minSampleRate;
    deviceInfo.maxRate = maxSampleRate;
    
    // Initialize sample buffers
    sampleBuffers.resize(MAX_BUFFER_COUNT);
    for (auto& buffer : sampleBuffers) {
        buffer.resize(BUFFER_SIZE);
    }
    
    // Initialize statistics
    stats.samplesReceived = 0;
    stats.samplesProcessed = 0;
    stats.bufferOverruns = 0;
    stats.usbErrors = 0;
    stats.lastSampleTime = 0;
}

WidebandSDRExtIO::~WidebandSDRExtIO() {
    Cleanup();
}

bool WidebandSDRExtIO::Initialize() {
    if (isInitialized) {
        return true;
    }
    
    LogMessage(EXTIO_LOG_INFO, "Initializing WidebandSDR ExtIO Plugin");
    
    // Initialize Python integration
    if (!InitializePython()) {
        LogMessage(EXTIO_LOG_ERROR, "Failed to initialize Python integration");
        return false;
    }
    
    isInitialized = true;
    LogMessage(EXTIO_LOG_INFO, "WidebandSDR ExtIO Plugin initialized successfully");
    
    return true;
}

void WidebandSDRExtIO::Cleanup() {
    if (isInitialized) {
        LogMessage(EXTIO_LOG_INFO, "Cleaning up WidebandSDR ExtIO Plugin");
        
        StopHardware();
        CloseDevice();
        CleanupPython();
        
        isInitialized = false;
    }
}

bool WidebandSDRExtIO::OpenDevice(int deviceIndex) {
    if (!isInitialized) {
        LogMessage(EXTIO_LOG_ERROR, "Plugin not initialized");
        return false;
    }
    
    if (deviceConnected) {
        LogMessage(EXTIO_LOG_INFO, "Device already connected");
        return true;
    }
    
    LogMessage(EXTIO_LOG_INFO, "Opening WidebandSDR device");
    
    // For now, simulate device connection for testing
    // In production, this would connect to the actual Python WidebandSDR
    deviceConnected = true;
    deviceName = WIDEBAND_SDR_DEVICE_NAME;
    deviceType = "Wideband SDR";
    
    LogMessage(EXTIO_LOG_INFO, "Device opened successfully");
    return true;
}

void WidebandSDRExtIO::CloseDevice() {
    if (deviceConnected) {
        LogMessage(EXTIO_LOG_INFO, "Closing WidebandSDR device");
        deviceConnected = false;
    }
}

int WidebandSDRExtIO::StartHardware(long frequency) {
    if (!deviceConnected) {
        LogMessage(EXTIO_LOG_ERROR, "Device not connected");
        return EXTIO_ERROR;
    }
    
    if (isStreaming) {
        LogMessage(EXTIO_LOG_WARNING, "Already streaming");
        return EXTIO_SUCCESS;
    }
    
    // Create frequency message
    char freqMsg[128];
    sprintf(freqMsg, "Starting hardware at frequency: %ld Hz", frequency);
    LogMessage(EXTIO_LOG_INFO, freqMsg);
    
    // Set frequency
    if (SetFrequency(frequency) != 0) {
        LogMessage(EXTIO_LOG_ERROR, "Failed to set frequency");
        return EXTIO_ERROR;
    }
    
    // Start streaming
    if (!StartStreaming()) {
        LogMessage(EXTIO_LOG_ERROR, "Failed to start streaming");
        return EXTIO_ERROR;
    }
    
    isStreaming = true;
    LogMessage(EXTIO_LOG_INFO, "Hardware started successfully");
    
    return EXTIO_SUCCESS;
}

void WidebandSDRExtIO::StopHardware() {
    if (isStreaming) {
        LogMessage(EXTIO_LOG_INFO, "Stopping hardware");
        StopStreaming();
        isStreaming = false;
    }
}

int WidebandSDRExtIO::SetFrequency(long frequency) {
    if (!ValidateParameters(frequency, currentSampleRate, currentGain)) {
        char errorMsg[128];
        sprintf(errorMsg, "Invalid frequency: %ld", frequency);
        LogMessage(EXTIO_LOG_ERROR, errorMsg);
        return EXTIO_ERROR;
    }
    
    char msg[128];
    sprintf(msg, "Setting frequency: %ld Hz", frequency);
    LogMessage(EXTIO_LOG_INFO, msg);
    
    // For now, just update the current frequency
    // In production, this would call the Python WidebandSDR.set_frequency()
    currentFrequency = frequency;
    
    LogMessage(EXTIO_LOG_INFO, "Frequency set successfully");
    
    return EXTIO_SUCCESS;
}

long WidebandSDRExtIO::SetSampleRate(long rate) {
    if (!ValidateParameters(currentFrequency, rate, currentGain)) {
        char errorMsg[128];
        sprintf(errorMsg, "Invalid sample rate: %ld", rate);
        LogMessage(EXTIO_LOG_ERROR, errorMsg);
        return currentSampleRate;  // Return current rate on error
    }
    
    char msg[128];
    sprintf(msg, "Setting sample rate: %ld Hz", rate);
    LogMessage(EXTIO_LOG_INFO, msg);
    
    // For now, just update the current sample rate
    // In production, this would call the Python WidebandSDR.set_sample_rate()
    currentSampleRate = rate;
    
    LogMessage(EXTIO_LOG_INFO, "Sample rate set successfully");
    
    return currentSampleRate;
}

int WidebandSDRExtIO::SetGain(int gain) {
    if (!ValidateParameters(currentFrequency, currentSampleRate, gain)) {
        char errorMsg[128];
        sprintf(errorMsg, "Invalid gain: %d", gain);
        LogMessage(EXTIO_LOG_ERROR, errorMsg);
        return EXTIO_ERROR;
    }
    
    char msg[128];
    sprintf(msg, "Setting gain: %d dB", gain);
    LogMessage(EXTIO_LOG_INFO, msg);
    
    // For now, just update the current gain
    // In production, this would call the Python WidebandSDR.set_gain()
    currentGain = gain;
    
    LogMessage(EXTIO_LOG_INFO, "Gain set successfully");
    
    return EXTIO_SUCCESS;
}

int WidebandSDRExtIO::GetDeviceInfo(int deviceIndex, EXT_DEVICEINFO* info) {
    if (!info || deviceIndex != 0) {
        return EXTIO_ERROR;
    }
    
    memcpy(info, &deviceInfo, sizeof(deviceInfo));
    return EXTIO_SUCCESS;
}

int WidebandSDRExtIO::EnumerateDevices() {
    if (!deviceConnected) {
        return 0;
    }
    
    // For now, assume only one device
    return 1;
}

int WidebandSDRExtIO::GetHardwareStatus() {
    if (!deviceConnected) {
        return EXTIO_ERROR;
    }
    
    return isStreaming ? EXTIO_SUCCESS : EXTIO_BUSY;
}

long WidebandSDRExtIO::GetStatus(int what, long value, char* units, int* dims) {
    if (!deviceConnected) {
        return 0;
    }
    
    switch (what) {
        case STATUS_SAMPLES_READY:
            return static_cast<long>(samplesAvailable.load());
        case STATUS_SAMPLE_RATE:
            return currentSampleRate;
        case STATUS_FREQUENCY:
            return currentFrequency;
        case STATUS_GAIN:
            return currentGain;
        case STATUS_BUFFER_SIZE:
            return BUFFER_SIZE;
        default:
            return 0;
    }
}

long WidebandSDRExtIO::SetLOFrequency(long loFreq) {
    SetFrequency(loFreq);
    return currentFrequency;
}

void WidebandSDRExtIO::GetHWType(char* hardwareType, int* supportedModes) {
    if (hardwareType) {
        strcpy(hardwareType, WIDEBAND_SDR_DEVICE_NAME);
    }
    
    if (supportedModes) {
        // Support all standard modes
        *supportedModes = (1 << MODE_AM) |
                         (1 << MODE_AMN) |
                         (1 << MODE_AMS) |
                         (1 << MODE_LSB) |
                         (1 << MODE_USB) |
                         (1 << MODE_FM) |
                         (1 << MODE_FMN) |
                         (1 << MODE_WFM) |
                         (1 << MODE_DIGL) |
                         (1 << MODE_DIGU);
    }
}

const char* WidebandSDRExtIO::GetVersion() {
    return WIDEBAND_SDR_EXTIO_VERSION;
}

unsigned int WidebandSDRExtIO::GetCapabilities() {
    return EXTIO_CAP_SAMPLE_RATE |
           EXTIO_CAP_GAIN |
           EXTIO_CAP_FREQ_TUNE |
           EXTIO_CAP_IQ_BUFFERING |
           EXTIO_CAP_THREAD_SAFE |
           EXTIO_CAP_MULTI_DEVICE;
}

// =============================================================================
// Private Methods
// =============================================================================

bool WidebandSDRExtIO::InitializePython() {
    // For now, Python integration is simulated
    // In production, this would initialize Python embedding
    LogMessage(EXTIO_LOG_INFO, "Python integration initialized (simulated)");
    return true;
}

void WidebandSDRExtIO::CleanupPython() {
    // For now, just log cleanup
    LogMessage(EXTIO_LOG_INFO, "Python integration cleaned up");
}

bool WidebandSDRExtIO::StartStreaming() {
    LogMessage(EXTIO_LOG_INFO, "Starting streaming thread");
    
    stopStreaming = false;
    
    // Start streaming thread
    streamingThread = std::thread(&WidebandSDRExtIO::StreamingThread, this);
    
    if (!streamingThread.joinable()) {
        LogMessage(EXTIO_LOG_ERROR, "Failed to start streaming thread");
        return false;
    }
    
    return true;
}

void WidebandSDRExtIO::StopStreaming() {
    LogMessage(EXTIO_LOG_INFO, "Stopping streaming thread");
    
    stopStreaming = true;
    
    if (streamingThread.joinable()) {
        streamingThread.join();
    }
}

void WidebandSDRExtIO::StreamingThread() {
    LogMessage(EXTIO_LOG_INFO, "Streaming thread started");
    
    // Simulate sample generation for testing
    // In production, this would read from Python WidebandSDR
    size_t sampleCount = 0;
    
    while (!stopStreaming) {
        // Generate dummy samples for testing
        // In production, this would get real samples from Python
        OnSamples(nullptr, BUFFER_SIZE);
        
        // Simulate sample rate timing
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        sampleCount++;
        if (sampleCount % 100 == 0) {
            char msg[128];
            sprintf(msg, "Generated %zu samples", sampleCount * BUFFER_SIZE);
            LogMessage(EXTIO_LOG_DEBUG, msg);
        }
    }
    
    LogMessage(EXTIO_LOG_INFO, "Streaming thread stopped");
}

void WidebandSDRExtIO::OnSamples(const short* samples, size_t count) {
    stats.samplesReceived += count;
    
    // For demo purposes, generate dummy I/Q samples
    std::vector<short> dummySamples(count);
    for (size_t i = 0; i < count; i++) {
        // Generate simple sine wave at center frequency
        double t = (stats.samplesReceived + i) / (double)currentSampleRate;
        double i_sample = 32767 * 0.3 * sin(2.0 * M_PI * 1000.0 * t);  // 1 kHz tone
        double q_sample = 32767 * 0.3 * cos(2.0 * M_PI * 1000.0 * t);
        
        // Convert to 16-bit
        dummySamples[i * 2] = static_cast<short>(i_sample);
        dummySamples[i * 2 + 1] = static_cast<short>(q_sample);
    }
    
    // Put samples in buffer
    size_t currentWriteIndex = writeBufferIndex.load();
    size_t nextWriteIndex = (currentWriteIndex + 1) % MAX_BUFFER_COUNT;
    
    if (nextWriteIndex == readBufferIndex.load()) {
        // Buffer full, drop samples
        stats.bufferOverruns++;
        return;
    }
    
    sampleBuffers[currentWriteIndex] = dummySamples;
    writeBufferIndex.store(nextWriteIndex);
    samplesAvailable.fetch_add(count);
    
    stats.samplesProcessed += count;
    stats.lastSampleTime = GetTickCount64();
}

int WidebandSDRExtIO::ReadSamples(short* buffer, size_t count) {
    if (!buffer || count == 0) {
        return 0;
    }
    
    size_t currentReadIndex = readBufferIndex.load();
    size_t currentWriteIndex = writeBufferIndex.load();
    
    if (currentReadIndex == currentWriteIndex) {
        // No data available
        return 0;
    }
    
    size_t samplesToRead = std::min(count, sampleBuffers[currentReadIndex].size());
    memcpy(buffer, sampleBuffers[currentReadIndex].data(), samplesToRead * sizeof(short));
    
    // Advance read index
    readBufferIndex.store((currentReadIndex + 1) % MAX_BUFFER_COUNT);
    samplesAvailable.fetch_sub(samplesToRead);
    
    return static_cast<int>(samplesToRead);
}

void WidebandSDRExtIO::LogMessage(int level, const char* message) {
    const char* levelNames[] = {"ERROR", "WARNING", "INFO", "DEBUG"};
    const char* levelName = (level >= 0 && level <= 3) ? levelNames[level] : "UNKNOWN";
    
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    printf("[%s] [%s] WidebandSDR_ExtIO: %s\n", timestamp, levelName, message);
    
    // In production, this would log to a file or Windows event log
}

bool WidebandSDRExtIO::ValidateParameters(long frequency, long sampleRate, int gain) {
    if (frequency < minFrequency || frequency > maxFrequency) {
        return false;
    }
    
    if (sampleRate < minSampleRate || sampleRate > maxSampleRate) {
        return false;
    }
    
    if (gain < minGain || gain > maxGain) {
        return false;
    }
    
    return true;
}
