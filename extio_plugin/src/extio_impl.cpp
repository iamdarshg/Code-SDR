/*
 * Wideband SDR ExtIO Plugin Implementation
 * Main implementation of ExtIO interface for HDSDR/SDR# compatibility
 */

#include "wideband_sdr_extio.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global plugin instance
WidebandSDRExtIO* g_pluginInstance = nullptr;

// ExtIO Plugin Implementation
class ExtIOPlugin {
private:
    std::unique_ptr<WidebandSDRExtIO> device;
    bool initialized;
    
public:
    ExtIOPlugin() : initialized(false) {
        g_pluginInstance = nullptr;
    }
    
    ~ExtIOPlugin() {
        Cleanup();
    }
    
    bool Initialize() {
        if (initialized) return true;
        
        device = std::make_unique<WidebandSDRExtIO>();
        if (!device->Initialize()) {
            return false;
        }
        
        g_pluginInstance = device.get();
        initialized = true;
        return true;
    }
    
    void Cleanup() {
        if (device && initialized) {
            device->Cleanup();
        }
        initialized = false;
        g_pluginInstance = nullptr;
    }
    
    WidebandSDRExtIO* GetDevice() {
        return initialized ? device.get() : nullptr;
    }
};

// Global plugin instance
static ExtIOPlugin g_plugin;

// =============================================================================
// ExtIO Interface Functions (C Interface for HDSDR/SDR#)
// =============================================================================

extern "C" {

/**
 * Initialize hardware and return device info
 * This is the main entry point called by HDSDR/SDR#
 */
long __stdcall InitHW(char *hwtype, char *drv, EXT_RADIOINIT *init) {
    if (!g_plugin.Initialize()) {
        return EXTIO_ERROR;
    }
    
    auto* device = g_plugin.GetDevice();
    if (!device || !device->OpenDevice(0)) {
        return EXTIO_ERROR;
    }
    
    // Populate initialization structure
    if (init) {
        memset(init, 0, sizeof(EXT_RADIOINIT));
        init->srate = device->GetSampleRate();
        init->tunefreq = device->GetFrequency();
        init->iffreq = 0;
        init->bandwidth = init->srate / 2;
        strcpy(init->modulation, "WB");
        init->modenum = 0;
    }
    
    // Populate device type strings
    if (hwtype) {
        strcpy(hwtype, WIDEBAND_SDR_DEVICE_NAME);
    }
    
    if (drv) {
        strcpy(drv, WIDEBAND_SDR_EXTIO_VERSION);
    }
    
    return EXTIO_SUCCESS;
}

/**
 * Close hardware connection
 */
void __stdcall CloseHW(void) {
    auto* device = g_plugin.GetDevice();
    if (device) {
        device->StopHardware();
        device->CloseDevice();
    }
}

/**
 * Start hardware operation at specified frequency
 */
int __stdcall StartHW(long freq) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return EXTIO_ERROR;
    }
    
    return device->StartHardware(freq);
}

/**
 * Stop hardware operation
 */
void __stdcall StopHW(void) {
    auto* device = g_plugin.GetDevice();
    if (device) {
        device->StopHardware();
    }
}

/**
 * Set local oscillator frequency
 */
long __stdcall SetHWLO(long LOfreq) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return 0;
    }
    
    device->SetFrequency(LOfreq);
    return device->GetFrequency();
}

/**
 * Get current local oscillator frequency
 */
long __stdcall GetHWLO(void) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return 0;
    }
    
    return device->GetLOFrequency();
}

/**
 * Get hardware status
 */
int __stdcall GetStatus(void) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return EXTIO_ERROR;
    }
    
    return device->GetHardwareStatus();
}

/**
 * Show hardware GUI (optional)
 */
void __stdcall ShowGUI(void) {
    // Optional: Show device control GUI
    // For now, no GUI implementation needed
}

/**
 * Hide hardware GUI (optional)
 */
void __stdcall HideGUI(void) {
    // Optional: Hide device control GUI
    // For now, no GUI implementation needed
}

/**
 * Get tuned frequency
 */
long __stdcall GetTune(void) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return 0;
    }
    
    return device->GetFrequency();
}

/**
 * Get current mode
 */
int __stdcall GetMode(void) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return 0;
    }
    
    return 0; // Default mode (AM)
}

/**
 * Get current gain
 */
int __stdcall GetGain(void) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return 0;
    }
    
    return device->GetGain();
}

/**
 * Get hardware type and supported modes
 */
void __stdcall GetHWType(char *hardwareType, int *supportedModes) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        if (hardwareType) {
            strcpy(hardwareType, "Unknown");
        }
        if (supportedModes) {
            *supportedModes = 0;
        }
        return;
    }
    
    device->GetHWType(hardwareType, supportedModes);
}

/**
 * Get current sample rate
 */
long __stdcall GetHWSR(void) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return 0;
    }
    
    return device->GetSampleRate();
}

/**
 * Get extended status information
 */
long __stdcall GetStatus(int what_status, long value, char *units, int *dims) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return 0;
    }
    
    return device->GetStatus(what_status, value, units, dims);
}

} // extern "C"

// =============================================================================
// Plugin Management Functions
// =============================================================================

extern "C" {

/**
 * Get plugin version
 */
const char* __stdcall GetExtIOVersion(void) {
    return WIDEBAND_SDR_EXTIO_VERSION;
}

/**
 * Get plugin capabilities
 */
unsigned int __stdcall GetExtIOCapabilities(void) {
    return WidebandSDRExtIO::GetCapabilities();
}

/**
 * Initialize plugin with device index
 */
int __stdcall InitDevice(int deviceIndex) {
    if (!g_plugin.Initialize()) {
        return EXTIO_ERROR;
    }
    
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return EXTIO_ERROR;
    }
    
    return device->OpenDevice(deviceIndex) ? EXTIO_SUCCESS : EXTIO_ERROR;
}

/**
 * Get number of available devices
 */
int __stdcall GetDeviceCount(void) {
    if (!g_plugin.Initialize()) {
        return 0;
    }
    
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return 0;
    }
    
    return device->EnumerateDevices();
}

/**
 * Get device information
 */
int __stdcall GetDeviceInfo(int deviceIndex, EXT_DEVICEINFO *deviceInfo) {
    if (!g_plugin.Initialize()) {
        return EXTIO_ERROR;
    }
    
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return EXTIO_ERROR;
    }
    
    return device->GetDeviceInfo(deviceIndex, deviceInfo);
}

/**
 * Start/Stop streaming samples
 */
int __stdcall SetStreaming(int enable) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return EXTIO_ERROR;
    }
    
    if (enable) {
        return device->StartHardware(device->GetFrequency()) == 0 ? EXTIO_SUCCESS : EXTIO_ERROR;
    } else {
        device->StopHardware();
        return EXTIO_SUCCESS;
    }
}

/**
 * Get streaming status
 */
int __stdcall GetStreaming(void) {
    auto* device = g_plugin.GetDevice();
    if (!device) {
        return 0;
    }
    
    return device->IsStreaming() ? 1 : 0;
}

} // extern "C"

// =============================================================================
// DLL Entry Point and Export Management
// =============================================================================

/**
 * DLL main entry point
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            // Initialize plugin
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            // Cleanup plugin
            g_plugin.Cleanup();
            break;
    }
    return TRUE;
}

// =============================================================================
// Utility Functions Implementation
// =============================================================================

namespace ExtIOUtils {

std::string FrequencyToString(long frequency) {
    char buffer[32];
    
    if (frequency >= 1000000000) {
        snprintf(buffer, sizeof(buffer), "%.2f GHz", frequency / 1e9);
    } else if (frequency >= 1000000) {
        snprintf(buffer, sizeof(buffer), "%.2f MHz", frequency / 1e6);
    } else if (frequency >= 1000) {
        snprintf(buffer, sizeof(buffer), "%.2f kHz", frequency / 1e3);
    } else {
        snprintf(buffer, sizeof(buffer), "%ld Hz", frequency);
    }
    
    return std::string(buffer);
}

std::string SampleRateToString(long rate) {
    char buffer[32];
    
    if (rate >= 1000000) {
        snprintf(buffer, sizeof(buffer), "%.2f MSPS", rate / 1e6);
    } else if (rate >= 1000) {
        snprintf(buffer, sizeof(buffer), "%.2f kSPS", rate / 1e3);
    } else {
        snprintf(buffer, sizeof(buffer), "%ld SPS", rate);
    }
    
    return std::string(buffer);
}

std::string FormatError(int errorCode, const std::string& context) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "ExtIO Error %d in %s", errorCode, context.c_str());
    return std::string(buffer);
}

} // namespace ExtIOUtils
