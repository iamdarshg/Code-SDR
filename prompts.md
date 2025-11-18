# Wideband SDR Development Prompts

**Complete structured development roadmap for Wideband SDR (1 MHz - 10 GHz). All prompts designed for single-day completion with clear deliverables.**

---

# 🔵 PHASE 0: CORE FEATURES (9 Days)

## 🎯 PROMPT 0a: GNU Radio OOT Module - Day 1

**Objective:** Create a complete GNU Radio OOT module structure and implement the core source block that provides real-time I/Q samples from the Wideband SDR device. This is a focused day-long task.

**Scope:** Project setup, build system configuration, and core source block implementation.

**Deliverables by EOD:**
- [ ] Complete `gr-wideband_sdr/` directory structure created
- [ ] CMakeLists.txt configured for build
- [ ] Source block C++ headers and implementation stubs
- [ ] GRC XML block definition for source block
- [ ] Module compiles successfully
- [ ] Module can be imported in Python

**Implementation Requirements:**

### 1. GNU Radio OOT Module Structure

Create comprehensive GNU Radio integration with complete OOT module structure:

**Directory Structure:**
```
gr-wideband_sdr/
├── apps/                    # Standalone applications
│   ├── wideband_sdr_fft_display.py
│   └── wideband_sdr_waterfall.py
├── cmake/                   # Build configuration
│   ├── Modules/
│   └── config.cmake
├── docs/                    # GNU Radio documentation
├── grc/                     # GNU Radio Companion blocks
│   ├── wideband_sdr_source.xml
│   ├── wideband_sdr_sink.xml
│   └── wideband_sdr_tuner.xml
├── include/                 # C++ headers
│   └── gnuradio/
│       └── wideband_sdr/
│           ├── api.h
│           ├── source.h
│           ├── sink.h
│           └── tuner.h
├── lib/                     # C++ implementation
│   ├── CMakeLists.txt
│   ├── source_impl.cc
│   ├── source_impl.h
│   ├── sink_impl.cc
│   └── sink_impl.h
├── python/                  # Python bindings
│   ├── CMakeLists.txt
│   ├── bindings.cc
│   └── __init__.py
├── swig/                    # SWIG interface files
│   ├── wideband_sdr.i
│   └── source.i
├── CMakeLists.txt          # Main build configuration
├── setup.py                # Python setup
└── README.md               # OOT module documentation
```

**GNU Radio Source Block Implementation (`lib/source_impl.cc`):**
```cpp
namespace gr {
  namespace wideband_sdr {
    
    class source_impl : public source {
    private:
      std::shared_ptr<WidebandSDR> d_sdr;
      uint32_t d_frequency;
      uint32_t d_sample_rate;
      uint8_t d_gain;
      std::vector<uint8_t> d_buffer;
      
    public:
      source_impl(uint32_t sample_rate);
      ~source_impl();
      
      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
      
      void set_frequency(uint32_t frequency);
      void set_sample_rate(uint32_t sample_rate);
      void set_gain(uint8_t gain);
      
      uint32_t get_frequency() const;
      uint32_t get_sample_rate() const;
      uint8_t get_gain() const;
    };
    
  } // namespace wideband_sdr
} // namespace gr
```

**GNU Radio Companion (GRC) XML Block Definition:**
```xml
<?xml version="1.0"?>
<block>
  <name>Wideband SDR Source</name>
  <key>wideband_sdr_source</key>
  <category>Wideband SDR</category>
  <import>import gnuradio.wideband_sdr as wideband_sdr</import>
  <make>wideband_sdr.source($sample_rate)</make>
  
  <param>
    <name>Sample Rate</name>
    <key>sample_rate</key>
    <value>1e6</value>
    <type>eng_float</type>
  </param>
  
  <param>
    <name>Frequency</name>
    <key>frequency</key>
    <value>100e6</value>
    <type>eng_float</type>
  </param>
  
  <param>
    <name>Gain</name>
    <key>gain</key>
    <value>30</value>
    <type>int</type>
  </param>
  
  <sink>
    <name>in</name>
    <type>complex</type>
  </sink>
  
  <doc>
    Wideband SDR Source Block for GNU Radio
    
    Provides real-time I/Q samples from the Wideband SDR device.
    Supports frequencies 1 MHz to 10 GHz with adjustable gain and sample rate.
  </doc>
</block>
```

### 2. SDR# and HDSDR ExtIO Plugin Implementation

**Windows ExtIO Plugin Structure (`extio_plugin/`):**
```cpp
// extio.h - Standard ExtIO interface
#pragma once

typedef struct {
    long srate;           // Sample rate
    long tunefreq;        // Tuning frequency
    long iffreq;          // IF frequency
    char modulation[12];  // Modulation type
    long bandwidth;       // Bandwidth
    int modenum;          // Mode number
} EXT_RADIOINIT;

// Main ExtIO callback
long __stdcall InitHW(char *hwtype, char *drv, EXT_RADIOINIT *init);
void __stdcall CloseHW(void);
int __stdcall StartHW(long freq);
void __stdcall StopHW(void);
long __stdcall SetHWLO(long LOfreq);
long __stdcall GetHWLO(void);
int __stdcall GetStatus(void);
void __stdcall ShowGUI(void);
long __stdcall GetTune(void);
int __stdcall GetMode(void);
int __stdcall GetGain(void);
```

**Plugin Implementation:**
```cpp
#include "extio.h"
#include "wideband_sdr.h"

static WidebandSDR* g_sdr = nullptr;
static bool g_streaming = false;

long __stdcall InitHW(char *hwtype, char *drv, EXT_RADIOINIT *init)
{
    try {
        g_sdr = new WidebandSDR();
        g_sdr->open();
        
        // Return capabilities
        init->srate = 10e6;      // Default sample rate: 10 MSPS
        init->tunefreq = 100e6;  // Default frequency: 100 MHz
        init->iffreq = 0;
        init->bandwidth = 5e6;
        strcpy(init->modulation, "WB");
        
        return 0;  // Success
    } catch (const std::exception& e) {
        return -1; // Error
    }
}

int __stdcall StartHW(long freq)
{
    if (!g_sdr) return -1;
    
    try {
        g_sdr->set_frequency(freq);
        g_sdr->start_stream();
        g_streaming = true;
        return 0;
    } catch (const std::exception& e) {
        return -1;
    }
}

void __stdcall StopHW(void)
{
    if (g_sdr && g_streaming) {
        g_sdr->stop_stream();
        g_streaming = false;
    }
}
```

### 3. Advanced DC Offset Correction and Calibration

**Firmware DC Offset Correction (`firmware/dc_offset.h`):**
```c
// DC offset correction structure
typedef struct {
    int32_t dc_offset_i;
    int32_t dc_offset_q;
    int32_t averaging_factor;
    uint32_t correction_samples;
} dc_offset_correction_t;

// DC offset correction functions
bool adc_estimate_dc_offset(dc_offset_correction_t *dc_corr);
void adc_apply_dc_offset_correction(int16_t *samples, uint32_t count, 
                                     const dc_offset_correction_t *dc_corr);
bool adc_auto_dc_calibration(void);
```

**Python DC Offset Implementation:**
```python
class DCOffsetCorrector:
    def __init__(self, window_size=1024):
        self.window_size = window_size
        self.dc_offset_i = 0.0
        self.dc_offset_q = 0.0
        self.correction_factor = 0.1
        
    def estimate_dc_offset(self, samples):
        """Estimate DC offset from IQ samples"""
        if len(samples) < self.window_size:
            return
        
        # Separate I and Q components (assuming complex samples)
        i_component = np.real(samples)
        q_component = np.imag(samples)
        
        # Exponential moving average
        new_offset_i = np.mean(i_component)
        new_offset_q = np.mean(q_component)
        
        self.dc_offset_i = (1.0 - self.correction_factor) * self.dc_offset_i + \
                          self.correction_factor * new_offset_i
        self.dc_offset_q = (1.0 - self.correction_factor) * self.dc_offset_q + \
                          self.correction_factor * new_offset_q
    
    def apply_correction(self, samples):
        """Apply DC offset correction to samples"""
        corrected = samples.copy()
        corrected = (corrected - (self.dc_offset_i + 1j * self.dc_offset_q))
        return corrected
    
    def calibrate(self, num_samples=10000):
        """Calibrate DC offset at current frequency"""
        # Read samples with no signal
        samples = sdr.read_samples(num_samples)
        
        # Estimate DC offset
        for chunk in [samples[i:i+self.window_size] 
                      for i in range(0, len(samples), self.window_size)]:
            self.estimate_dc_offset(chunk)
        
        return self.dc_offset_i, self.dc_offset_q
```

### 4. FFT Acceleration and Digital Downconversion

**Firmware FFT Acceleration:**
```c
// dsPIC33 hardware FFT support
#include "libq.h"  // Microchip DSP library

typedef struct {
    complex16_t *fft_input;
    complex16_t *fft_output;
    uint32_t fft_size;
    uint32_t scaling_mode;
} fft_accelerator_t;

bool dsp_init_fft_accelerator(fft_accelerator_t *fft_accel, uint32_t size);
void dsp_compute_fft(fft_accelerator_t *fft_accel);
void dsp_compute_ifft(fft_accelerator_t *fft_accel);
void dsp_apply_window(complex16_t *data, uint32_t size, uint8_t window_type);
```

**Python Digital Downconversion:**
```python
class DigitalDownconverter:
    def __init__(self, sample_rate, center_freq, bandwidth):
        self.sample_rate = sample_rate
        self.center_freq = center_freq
        self.bandwidth = bandwidth
        
        # Design low-pass filter
        nyquist = sample_rate / 2
        normalized_cutoff = (bandwidth / 2) / nyquist
        self.filter_coeffs = signal.firwin(101, normalized_cutoff)
        
        # Pre-compute NCO (Numerically Controlled Oscillator)
        self.nco_freq = center_freq / sample_rate
    
    def apply_ddc(self, samples):
        """Apply digital downconversion"""
        # NCO (frequency shift)
        t = np.arange(len(samples)) / self.sample_rate
        nco = np.exp(-2j * np.pi * self.nco_freq * t)
        downconverted = samples * nco
        
        # Low-pass filtering
        filtered = signal.lfilter(self.filter_coeffs, 1, downconverted)
        
        # Decimation
        decimation_factor = self.sample_rate // (self.bandwidth * 2)
        decimated = filtered[::decimation_factor]
        
        return decimated
```

### 5. Recording and Playback Functionality

**Python Recording/Playback Module (`wideband_sdr/recording.py`):**
```python
import struct
from dataclasses import dataclass
from typing import Optional
import json

@dataclass
class RecordingMetadata:
    """Metadata for SDR recordings"""
    sample_rate: int
    frequency: int
    gain: int
    timestamp: float
    duration: float
    format: str  # 'complex64', 'complex128', 'int16', 'uint16'
    description: str = ""
    
    def to_dict(self):
        return self.__dict__
    
    @classmethod
    def from_dict(cls, data):
        return cls(**data)

class SDRRecorder:
    """Record SDR samples to file"""
    
    def __init__(self, filename: str, metadata: RecordingMetadata):
        self.filename = filename
        self.metadata = metadata
        self.file = None
        self.sample_count = 0
        self.start_time = None
        
    def start_recording(self):
        """Start recording to file"""
        # Write header with metadata
        with open(f"{self.filename}.json", 'w') as f:
            json.dump(self.metadata.to_dict(), f, indent=2)
        
        self.file = open(self.filename, 'wb')
        self.start_time = time.time()
        
    def write_samples(self, samples: np.ndarray):
        """Write samples to file"""
        if self.file is None:
            raise RuntimeError("Recording not started")
        
        # Write samples in binary format
        if self.metadata.format == 'complex64':
            data = samples.astype(np.complex64).tobytes()
        elif self.metadata.format == 'complex128':
            data = samples.astype(np.complex128).tobytes()
        else:
            data = samples.tobytes()
        
        self.file.write(data)
        self.sample_count += len(samples)
    
    def stop_recording(self):
        """Stop recording and close file"""
        if self.file:
            self.file.close()
            
            # Update metadata with actual duration
            actual_duration = time.time() - self.start_time
            self.metadata.duration = actual_duration
            
            with open(f"{self.filename}.json", 'w') as f:
                json.dump(self.metadata.to_dict(), f, indent=2)

class SDRPlayback:
    """Replay SDR recordings"""
    
    def __init__(self, filename: str):
        self.filename = filename
        self.metadata = None
        self._load_metadata()
        
    def _load_metadata(self):
        """Load metadata from JSON file"""
        metadata_file = f"{self.filename}.json"
        with open(metadata_file, 'r') as f:
            data = json.load(f)
            self.metadata = RecordingMetadata.from_dict(data)
    
    def read_samples(self, count: int) -> Optional[np.ndarray]:
        """Read samples from file"""
        dtype = np.complex64 if self.metadata.format == 'complex64' else np.complex128
        
        with open(self.filename, 'rb') as f:
            data = f.read(count * dtype().nbytes)
            if not data:
                return None
            
            samples = np.frombuffer(data, dtype=dtype)
            return samples
    
    def get_sample_count(self) -> int:
        """Get total number of samples in file"""
        import os
        file_size = os.path.getsize(self.filename)
        dtype = np.complex64 if self.metadata.format == 'complex64' else np.complex128
        return file_size // dtype().nbytes
```

**Recording Example:**
```python
# Record 10 seconds of data
sdr = WidebandSDR()
sdr.set_frequency(95_500_000)  # 95.5 MHz
sdr.set_sample_rate(2_000_000)  # 2 MSPS

metadata = RecordingMetadata(
    sample_rate=2_000_000,
    frequency=95_500_000,
    gain=30,
    timestamp=time.time(),
    duration=10.0,
    format='complex64',
    description='FM Radio Band Recording'
)

recorder = SDRRecorder('radio_recording.iq', metadata)
recorder.start_recording()

sdr.start_stream()
start_time = time.time()

def record_callback(samples):
    recorder.write_samples(samples)

sdr.start_stream(callback=record_callback)
time.sleep(10.0)
sdr.stop_stream()
recorder.stop_recording()

# Playback the recording
playback = SDRPlayback('radio_recording.iq')
print(f"Recording: {playback.metadata.duration}s")
print(f"Total Samples: {playback.get_sample_count()}")

samples = playback.read_samples(2048)
```

### 6. Web-Based Control Interface

**Python Flask Web Interface (`wideband_sdr/web_interface.py`):**
```python
from flask import Flask, render_template, jsonify, request
from flask_cors import CORS
import json
import threading

app = Flask(__name__)
CORS(app)

class WebControlInterface:
    def __init__(self, sdr: WidebandSDR):
        self.sdr = sdr
        self.app = app
        self._setup_routes()
    
    def _setup_routes(self):
        """Setup Flask routes"""
        
        @self.app.route('/api/device/info', methods=['GET'])
        def get_device_info():
            """Get device information"""
            info = self.sdr.get_device_info()
            return jsonify(info)
        
        @self.app.route('/api/device/status', methods=['GET'])
        def get_device_status():
            """Get device status"""
            stats = self.sdr.get_statistics()
            return jsonify({
                'frequency': self.sdr.get_frequency(),
                'sample_rate': self.sdr.get_sample_rate(),
                'gain': self.sdr.get_gain(),
                'samples_processed': stats.samples_processed,
                'packets_dropped': stats.packets_dropped,
                'streaming': self.sdr.streaming_active
            })
        
        @self.app.route('/api/device/set_frequency', methods=['POST'])
        def set_frequency():
            """Set device frequency"""
            data = request.json
            freq = data.get('frequency')
            try:
                self.sdr.set_frequency(freq)
                return jsonify({'success': True, 'frequency': freq})
            except Exception as e:
                return jsonify({'success': False, 'error': str(e)}), 400
        
        @self.app.route('/api/device/set_gain', methods=['POST'])
        def set_gain():
            """Set device gain"""
            data = request.json
            gain = data.get('gain')
            try:
                self.sdr.set_gain(gain)
                return jsonify({'success': True, 'gain': gain})
            except Exception as e:
                return jsonify({'success': False, 'error': str(e)}), 400
        
        @self.app.route('/api/spectrum', methods=['GET'])
        def get_spectrum():
            """Get real-time spectrum"""
            samples = self.sdr.read_samples(1024)
            if samples is not None:
                fft = np.fft.fftshift(np.fft.fft(samples))
                magnitude_db = 20 * np.log10(np.abs(fft) + 1e-10)
                freqs = np.fft.fftshift(np.fft.fftfreq(len(samples)))
                
                return jsonify({
                    'freqs': freqs.tolist(),
                    'magnitude_db': magnitude_db.tolist()
                })
            return jsonify({'error': 'No data'}), 400
    
    def run(self, host='0.0.0.0', port=5000, debug=False):
        """Run Flask web server"""
        self.app.run(host=host, port=port, debug=debug)
```

**HTML Interface (`web/index.html`):**
```html
<!DOCTYPE html>
<html>
<head>
    <title>Wideband SDR Control</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .container { max-width: 1200px; margin: 0 auto; }
        .control-panel { background: #f5f5f5; padding: 20px; border-radius: 5px; }
        .spectrum-display { margin-top: 20px; }
        input[type="range"] { width: 300px; }
        button { padding: 10px 20px; background: #0066cc; color: white; border: none; border-radius: 3px; cursor: pointer; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Wideband SDR Control Interface</h1>
        
        <div class="control-panel">
            <h2>Device Control</h2>
            
            <label>Frequency (MHz):</label>
            <input type="number" id="frequency" value="100" min="1" max="10000" step="1">
            <button onclick="setFrequency()">Set Frequency</button>
            
            <label style="margin-left: 30px;">Gain (dB):</label>
            <input type="range" id="gain" min="0" max="63" value="30" 
                   oninput="document.getElementById('gainValue').textContent = this.value">
            <span id="gainValue">30</span>
            <button onclick="setGain()">Set Gain</button>
        </div>
        
        <div id="spectrumPlot" class="spectrum-display"></div>
    </div>
    
    <script>
        async function setFrequency() {
            const freq = parseFloat(document.getElementById('frequency').value) * 1e6;
            const response = await fetch('/api/device/set_frequency', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ frequency: freq })
            });
            const data = await response.json();
            if (data.success) alert('Frequency set to ' + freq/1e6 + ' MHz');
        }
        
        async function setGain() {
            const gain = parseInt(document.getElementById('gain').value);
            const response = await fetch('/api/device/set_gain', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ gain: gain })
            });
            const data = await response.json();
            if (data.success) alert('Gain set to ' + gain + ' dB');
        }
        
        async function updateSpectrum() {
            const response = await fetch('/api/spectrum');
            const data = await response.json();
            
            const trace = {
                x: data.freqs,
                y: data.magnitude_db,
                type: 'scatter',
                mode: 'lines'
            };
            
            Plotly.newPlot('spectrumPlot', [trace], {
                title: 'Real-Time Spectrum',
                xaxis: { title: 'Frequency (normalized)' },
                yaxis: { title: 'Magnitude (dB)' }
            });
        }
        
        // Update spectrum every 500ms
        setInterval(updateSpectrum, 500);
    </script>
</body>
</html>
```

Create comprehensive GNU Radio integration with complete OOT module structure:

**Directory Structure:**
```
gr-wideband_sdr/
├── apps/                    # Standalone applications
├── cmake/                   # Build configuration
├── docs/                    # GNU Radio documentation
├── grc/                     # GNU Radio Companion blocks
│   └── wideband_sdr_source.xml
├── include/                 # C++ headers
│   └── gnuradio/
│       └── wideband_sdr/
│           ├── api.h
│           └── source.h
├── lib/                     # C++ implementation
│   ├── CMakeLists.txt
│   ├── source_impl.cc
│   └── source_impl.h
├── python/                  # Python bindings
├── swig/                    # SWIG interface files
├── CMakeLists.txt
├── setup.py
└── README.md
```

**Key Tasks:**
1. Create complete directory structure
2. Configure CMakeLists.txt for build system
3. Implement source_impl.cc with work() function
4. Create GRC XML block definition
5. Set up SWIG bindings for Python
6. Test compilation and import

**Time Estimate:** 6-8 hours

---

## Prompt 0b: ExtIO Plugin Implementation - Day 2

**Objective:** Develop Windows ExtIO plugins for SDR# and HDSDR compatibility. This is a focused day-long task.

**Scope:** ExtIO plugin architecture implementation, DLL creation, and plugin interface

**Deliverables by EOD:**
- [ ] ExtIO.dll compiled and functional
- [ ] Plugin loads in SDR# and HDSDR
- [ ] Frequency tuning works from UI
- [ ] Gain control functional
- [ ] Device enumeration working
- [ ] Error handling in place

**Implementation Requirements:**

### SDR# and HDSDR ExtIO Plugin

**Windows ExtIO Plugin Structure:**

Create `extio_plugin/` with the following structure:
```
extio_plugin/
├── CMakeLists.txt
├── src/
│   ├── extio.h              # Standard ExtIO interface
│   ├── extio_impl.cpp       # Main implementation
│   ├── wideband_sdr_extio.cpp
│   └── version_resource.rc  # Windows resource file
└── README.md
```

**Core ExtIO Interface:**
```cpp
// extio.h - Standard ExtIO interface definitions
typedef struct {
    long srate;           // Sample rate
    long tunefreq;        // Tuning frequency
    long iffreq;          // IF frequency
    char modulation[12];  // Modulation type
    long bandwidth;       // Bandwidth
    int modenum;          // Mode number
} EXT_RADIOINIT;

// Main ExtIO callback functions
long __stdcall InitHW(char *hwtype, char *drv, EXT_RADIOINIT *init);
void __stdcall CloseHW(void);
int __stdcall StartHW(long freq);
void __stdcall StopHW(void);
long __stdcall SetHWLO(long LOfreq);
long __stdcall GetHWLO(void);
int __stdcall GetStatus(void);
```

**Implementation Key Points:**
- DLL must export C functions with __stdcall calling convention
- Initialize USB device in InitHW()
- Handle frequency changes via SetHWLO()
- Implement status polling for streaming state
- Proper error handling and recovery

**Time Estimate:** 6-8 hours

---

## Prompt 0c: DC Offset Correction and Calibration - Day 3

**Objective:** Implement advanced DC offset correction in both firmware and Python, with automatic calibration routines.

**Scope:** DC offset estimation, real-time correction, and calibration procedures

**Deliverables by EOD:**
- [ ] Firmware DC offset correction implemented
- [ ] Python DCOffsetCorrector class working
- [ ] Auto-calibration routine functional
- [ ] DC offset reduction >20dB validated
- [ ] Real-time correction integrated with streaming

**Implementation Requirements:**

### DC Offset Correction Module

**Firmware Implementation (`firmware/dc_offset.c`):**
```c
typedef struct {
    int32_t dc_offset_i;
    int32_t dc_offset_q;
    int32_t averaging_factor;
    uint32_t correction_samples;
} dc_offset_correction_t;

bool adc_estimate_dc_offset(dc_offset_correction_t *dc_corr);
void adc_apply_dc_offset_correction(int16_t *samples, uint32_t count, 
                                     const dc_offset_correction_t *dc_corr);
bool adc_auto_dc_calibration(void);
```

**Python DC Offset Corrector:**
```python
class DCOffsetCorrector:
    """Real-time DC offset correction"""
    
    def __init__(self, window_size=1024):
        self.window_size = window_size
        self.dc_offset_i = 0.0
        self.dc_offset_q = 0.0
        self.correction_factor = 0.1
    
    def estimate_dc_offset(self, samples):
        """Exponential moving average DC estimation"""
        i_component = np.real(samples)
        q_component = np.imag(samples)
        
        new_offset_i = np.mean(i_component)
        new_offset_q = np.mean(q_component)
        
        self.dc_offset_i = (1.0 - self.correction_factor) * self.dc_offset_i + \
                          self.correction_factor * new_offset_i
        self.dc_offset_q = (1.0 - self.correction_factor) * self.dc_offset_q + \
                          self.correction_factor * new_offset_q
    
    def apply_correction(self, samples):
        """Apply DC offset correction"""
        return samples - (self.dc_offset_i + 1j * self.dc_offset_q)
    
    def calibrate(self, sdr, num_samples=10000):
        """Calibrate at current frequency"""
        samples = sdr.read_samples(num_samples)
        for chunk in np.array_split(samples, 10):
            self.estimate_dc_offset(chunk)
        return self.dc_offset_i, self.dc_offset_q
```

**Validation:**
- Measure DC offset before and after correction
- Verify >20dB improvement
- Test at multiple frequencies
- Verify correction updates in real-time

**Time Estimate:** 5-6 hours

---

## Prompt 0d: FFT Acceleration and Digital Downconversion - Day 4

**Objective:** Implement FFT acceleration and digital downconversion for high-performance signal processing.

**Scope:** Hardware FFT, digital downconversion filter design, and decimation

**Deliverables by EOD:**
- [ ] Firmware FFT accelerator module complete
- [ ] Python DDC class implemented
- [ ] NCO (Numerically Controlled Oscillator) functional
- [ ] Low-pass filter design working
- [ ] Decimation operational
- [ ] Performance benchmarks recorded

**Implementation Requirements:**

### FFT Acceleration Framework

**Firmware FFT (`firmware/dsp_fft.c`):**
```c
#include "libq.h"  // Microchip DSP library

typedef struct {
    complex16_t *fft_input;
    complex16_t *fft_output;
    uint32_t fft_size;
    uint32_t scaling_mode;
} fft_accelerator_t;

bool dsp_init_fft_accelerator(fft_accelerator_t *fft_accel, uint32_t size);
void dsp_compute_fft(fft_accelerator_t *fft_accel);
void dsp_compute_ifft(fft_accelerator_t *fft_accel);
void dsp_apply_window(complex16_t *data, uint32_t size, uint8_t window_type);
```

**Python Digital Downconverter:**
```python
class DigitalDownconverter:
    """Software DDC with filtering and decimation"""
    
    def __init__(self, sample_rate, center_freq, bandwidth):
        self.sample_rate = sample_rate
        self.center_freq = center_freq
        self.bandwidth = bandwidth
        
        # Design low-pass filter for bandwidth
        nyquist = sample_rate / 2
        normalized_cutoff = (bandwidth / 2) / nyquist
        self.filter_coeffs = signal.firwin(101, normalized_cutoff)
        
        # NCO frequency
        self.nco_freq = center_freq / sample_rate
    
    def apply_ddc(self, samples):
        """Apply complete DDC: NCO + filter + decimate"""
        # Frequency shift via NCO
        t = np.arange(len(samples)) / self.sample_rate
        nco = np.exp(-2j * np.pi * self.nco_freq * t)
        downconverted = samples * nco
        
        # Low-pass filtering
        filtered = signal.lfilter(self.filter_coeffs, 1, downconverted)
        
        # Decimation
        decimation_factor = self.sample_rate // (self.bandwidth * 2)
        decimated = filtered[::decimation_factor]
        
        return decimated
```

**Performance Validation:**
- Measure FFT computation time
- Verify frequency resolution
- Test DDC output quality
- Benchmark decimation efficiency

**Time Estimate:** 6-7 hours

---

## Prompt 0e: Recording and Playback System - Day 5

**Objective:** Implement comprehensive SDR sample recording and playback with metadata support.

**Scope:** File I/O, multiple format support, metadata management, playback functionality

**Deliverables by EOD:**
- [ ] SDRRecorder class fully functional
- [ ] Multiple format support (complex64, complex128, int16)
- [ ] Metadata JSON files generated correctly
- [ ] SDRPlayback class working
- [ ] Recording and playback tested end-to-end
- [ ] Example scripts working

**Implementation Requirements:**

### Recording/Playback Module

**Python Recording System (`wideband_sdr/recording.py`):**

```python
@dataclass
class RecordingMetadata:
    """Recording metadata"""
    sample_rate: int
    frequency: int
    gain: int
    timestamp: float
    duration: float
    format: str  # 'complex64', 'complex128', 'int16', 'uint16'
    description: str = ""
    
    def to_dict(self):
        return self.__dict__
    
    @classmethod
    def from_dict(cls, data):
        return cls(**data)

class SDRRecorder:
    """Record SDR samples to file with metadata"""
    
    def __init__(self, filename: str, metadata: RecordingMetadata):
        self.filename = filename
        self.metadata = metadata
        self.file = None
        self.sample_count = 0
        self.start_time = None
    
    def start_recording(self):
        """Initialize recording files"""
        # Write metadata JSON
        with open(f"{self.filename}.json", 'w') as f:
            json.dump(self.metadata.to_dict(), f, indent=2)
        
        # Open binary sample file
        self.file = open(self.filename, 'wb')
        self.start_time = time.time()
    
    def write_samples(self, samples: np.ndarray):
        """Write samples to binary file"""
        if self.file is None:
            raise RuntimeError("Recording not started")
        
        # Convert based on format
        if self.metadata.format == 'complex64':
            data = samples.astype(np.complex64).tobytes()
        elif self.metadata.format == 'complex128':
            data = samples.astype(np.complex128).tobytes()
        else:
            data = samples.tobytes()
        
        self.file.write(data)
        self.sample_count += len(samples)
    
    def stop_recording(self):
        """Finalize recording"""
        if self.file:
            self.file.close()
            self.metadata.duration = time.time() - self.start_time
            
            # Update metadata
            with open(f"{self.filename}.json", 'w') as f:
                json.dump(self.metadata.to_dict(), f, indent=2)

class SDRPlayback:
    """Replay recorded SDR samples"""
    
    def __init__(self, filename: str):
        self.filename = filename
        self.metadata = None
        self._load_metadata()
    
    def _load_metadata(self):
        """Load metadata from JSON"""
        with open(f"{self.filename}.json", 'r') as f:
            data = json.load(f)
            self.metadata = RecordingMetadata.from_dict(data)
    
    def read_samples(self, count: int) -> Optional[np.ndarray]:
        """Read samples from file"""
        dtype = np.complex64 if self.metadata.format == 'complex64' else np.complex128
        
        with open(self.filename, 'rb') as f:
            data = f.read(count * dtype().nbytes)
            if not data:
                return None
            return np.frombuffer(data, dtype=dtype)
    
    def get_sample_count(self) -> int:
        """Get total samples in file"""
        import os
        file_size = os.path.getsize(self.filename)
        dtype = np.complex64 if self.metadata.format == 'complex64' else np.complex128
        return file_size // dtype().nbytes
```

**Example Usage:**
```python
# Record 10 seconds
sdr = WidebandSDR()
sdr.set_frequency(95_500_000)
sdr.set_sample_rate(2_000_000)

metadata = RecordingMetadata(
    sample_rate=2_000_000, frequency=95_500_000, gain=30,
    timestamp=time.time(), duration=10.0, format='complex64',
    description='FM Radio Band Recording'
)

recorder = SDRRecorder('recording.iq', metadata)
recorder.start_recording()

def record_callback(samples):
    recorder.write_samples(samples)

sdr.start_stream(callback=record_callback)
time.sleep(10.0)
sdr.stop_stream()
recorder.stop_recording()

# Playback
playback = SDRPlayback('recording.iq')
print(f"Samples: {playback.get_sample_count()}")
samples = playback.read_samples(2048)
```

**Validation:**
- Record and playback cycle
- File size accuracy
- Metadata preservation
- Multiple format testing

**Time Estimate:** 5-6 hours

---

## Prompt 0f: Web-Based Control Interface - Day 6

**Objective:** Develop a Flask-based web interface for remote SDR control with real-time spectrum display.

**Scope:** Flask backend, REST API endpoints, HTML/JavaScript frontend, real-time updates

**Deliverables by EOD:**
- [ ] Flask application running on localhost:5000
- [ ] REST API endpoints functional (frequency, gain, status)
- [ ] Real-time spectrum display working
- [ ] HTML interface responsive
- [ ] Frequency and gain controls working
- [ ] WebSocket or polling-based updates

**Implementation Requirements:**

### Web Control Interface

**Flask Backend (`wideband_sdr/web_interface.py`):**

```python
from flask import Flask, render_template, jsonify, request
from flask_cors import CORS
import threading

app = Flask(__name__)
CORS(app)

class WebControlInterface:
    def __init__(self, sdr):
        self.sdr = sdr
        self.app = app
        self._setup_routes()
    
    def _setup_routes(self):
        """Configure all API endpoints"""
        
        @self.app.route('/api/device/info', methods=['GET'])
        def get_device_info():
            info = self.sdr.get_device_info()
            return jsonify(info)
        
        @self.app.route('/api/device/status', methods=['GET'])
        def get_device_status():
            stats = self.sdr.get_statistics()
            return jsonify({
                'frequency': self.sdr.get_frequency(),
                'sample_rate': self.sdr.get_sample_rate(),
                'gain': self.sdr.get_gain(),
                'samples_processed': stats.samples_processed,
                'packets_dropped': stats.packets_dropped,
                'streaming': self.sdr.streaming_active
            })
        
        @self.app.route('/api/device/set_frequency', methods=['POST'])
        def set_frequency():
            data = request.json
            freq = data.get('frequency')
            try:
                self.sdr.set_frequency(freq)
                return jsonify({'success': True, 'frequency': freq})
            except Exception as e:
                return jsonify({'success': False, 'error': str(e)}), 400
        
        @self.app.route('/api/device/set_gain', methods=['POST'])
        def set_gain():
            data = request.json
            gain = data.get('gain')
            try:
                self.sdr.set_gain(gain)
                return jsonify({'success': True, 'gain': gain})
            except Exception as e:
                return jsonify({'success': False, 'error': str(e)}), 400
        
        @self.app.route('/api/spectrum', methods=['GET'])
        def get_spectrum():
            """Get real-time spectrum"""
            samples = self.sdr.read_samples(1024)
            if samples is not None:
                fft = np.fft.fftshift(np.fft.fft(samples))
                magnitude_db = 20 * np.log10(np.abs(fft) + 1e-10)
                freqs = np.fft.fftshift(np.fft.fftfreq(len(samples), 
                                                       1/self.sdr.current_sample_rate))
                
                return jsonify({
                    'freqs': freqs.tolist(),
                    'magnitude_db': magnitude_db.tolist()
                })
            return jsonify({'error': 'No data'}), 400
    
    def run(self, host='0.0.0.0', port=5000):
        """Start Flask server"""
        self.app.run(host=host, port=port, debug=False, threaded=True)
```

**HTML Frontend (`web/index.html`):**

```html
<!DOCTYPE html>
<html>
<head>
    <title>Wideband SDR Control</title>
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .container { max-width: 1200px; margin: 0 auto; }
        .control-panel { background: #f5f5f5; padding: 20px; border-radius: 5px; }
        .spectrum-display { margin-top: 20px; height: 400px; }
        input[type="number"], input[type="range"] { margin: 10px; }
        button { padding: 10px 20px; background: #0066cc; color: white; 
                border: none; border-radius: 3px; cursor: pointer; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Wideband SDR Control</h1>
        
        <div class="control-panel">
            <label>Frequency (MHz):</label>
            <input type="number" id="frequency" value="100" min="1" max="10000">
            <button onclick="setFrequency()">Set</button>
            
            <label style="margin-left: 30px;">Gain (dB):</label>
            <input type="range" id="gain" min="0" max="63" value="30">
            <span id="gainValue">30</span>
            <button onclick="setGain()">Set</button>
        </div>
        
        <div id="spectrumPlot" class="spectrum-display"></div>
    </div>
    
    <script>
        async function setFrequency() {
            const freq = parseFloat(document.getElementById('frequency').value) * 1e6;
            const response = await fetch('/api/device/set_frequency', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ frequency: freq })
            });
            const data = await response.json();
            if (data.success) alert('Frequency set');
        }
        
        async function setGain() {
            const gain = parseInt(document.getElementById('gain').value);
            const response = await fetch('/api/device/set_gain', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ gain: gain })
            });
            const data = await response.json();
            if (data.success) alert('Gain set');
        }
        
        async function updateSpectrum() {
            const response = await fetch('/api/spectrum');
            const data = await response.json();
            
            const trace = {
                x: data.freqs, y: data.magnitude_db,
                type: 'scatter', mode: 'lines'
            };
            
            Plotly.newPlot('spectrumPlot', [trace], {
                title: 'Real-Time Spectrum',
                xaxis: { title: 'Frequency (normalized)' },
                yaxis: { title: 'Magnitude (dB)' }
            });
        }
        
        setInterval(updateSpectrum, 500);
    </script>
</body>
</html>
```

**Validation:**
- Server starts without errors
- All endpoints respond correctly
- UI updates in real-time
- Frequency and gain controls work

**Time Estimate:** 5-6 hours

---

## Prompt 0g: LNA and Antenna Control - Day 7

**Objective:** Implement low-noise amplifier (LNA) and antenna switching control for RF front-end management.

**Scope:** LNA gain control, antenna selection, diversity modes, firmware commands

**Deliverables by EOD:**
- [ ] LNA control firmware commands implemented
- [ ] LNAController Python class functional
- [ ] Antenna switching working
- [ ] Diversity mode selection functional
- [ ] Integration with main SDR driver
- [ ] Test suite passing

**Implementation Requirements:**

### LNA and Antenna Control

**Firmware LNA Control (`firmware/lna_control.c`):**

```c
typedef enum {
    LNA_GAIN_0_DB = 0,
    LNA_GAIN_5_DB = 1,
    LNA_GAIN_10_DB = 2,
    LNA_GAIN_15_DB = 3,
    LNA_GAIN_20_DB = 4,
    LNA_GAIN_25_DB = 5,
    LNA_GAIN_30_DB = 6,
} lna_gain_t;

bool lna_init(void);
bool lna_set_gain(lna_gain_t gain);
lna_gain_t lna_get_gain(void);
bool lna_enable(void);
bool lna_disable(void);
```

**Python LNA Controller:**

```python
class LNAController:
    """Control LNA (BGA614) gain and enable/disable"""
    
    LNA_GAIN_LEVELS = [0, 5, 10, 15, 20, 25, 30]  # dB
    
    def __init__(self, sdr):
        self.sdr = sdr
        self.enabled = True
        self.gain_level = 30
    
    def set_gain(self, gain_db: int) -> bool:
        """Set LNA to nearest supported gain level"""
        nearest = min(self.LNA_GAIN_LEVELS, 
                     key=lambda x: abs(x - gain_db))
        
        cmd = struct.pack('<BBB', 0x0F, 0x01, 
                         self.LNA_GAIN_LEVELS.index(nearest))
        
        try:
            self.sdr.device.write(self.sdr.EP_BULK_OUT, cmd)
            self.gain_level = nearest
            return True
        except usb.core.USBError:
            return False
    
    def enable(self) -> bool:
        """Enable LNA"""
        cmd = struct.pack('<BBB', 0x0F, 0x02, 0x01)
        try:
            self.sdr.device.write(self.sdr.EP_BULK_OUT, cmd)
            self.enabled = True
            return True
        except usb.core.USBError:
            return False
    
    def disable(self) -> bool:
        """Disable LNA"""
        cmd = struct.pack('<BBB', 0x0F, 0x02, 0x00)
        try:
            self.sdr.device.write(self.sdr.EP_BULK_OUT, cmd)
            self.enabled = False
            return True
        except usb.core.USBError:
            return False

class AntennaController:
    """Antenna switching and diversity control"""
    
    ANTENNA_PRIMARY = 0
    ANTENNA_SECONDARY = 1
    ANTENNA_COMBINED = 2
    
    def __init__(self, sdr):
        self.sdr = sdr
        self.current_antenna = self.ANTENNA_PRIMARY
    
    def select_antenna(self, antenna: int) -> bool:
        """Select active antenna"""
        if antenna not in [self.ANTENNA_PRIMARY, self.ANTENNA_SECONDARY, 
                          self.ANTENNA_COMBINED]:
            return False
        
        cmd = struct.pack('<BBB', 0x10, 0x01, antenna)
        try:
            self.sdr.device.write(self.sdr.EP_BULK_OUT, cmd)
            self.current_antenna = antenna
            return True
        except usb.core.USBError:
            return False
```

**Integration with Main Driver:**
```python
# In WidebandSDR class
def __init__(self, device_index: int = 0):
    # ... existing init code ...
    self.lna_controller = None
    self.antenna_controller = None

def open(self) -> bool:
    # ... existing open code ...
    self.lna_controller = LNAController(self)
    self.antenna_controller = AntennaController(self)
    return True
```

**Validation:**
- Gain levels set correctly
- Antenna switching works
- Enable/disable functional
- Integration tests passing

**Time Estimate:** 5-6 hours

---

## Prompt 0h: Calibration Wizard - Day 8

**Objective:** Develop comprehensive calibration wizard for frequency accuracy, gain, and signal quality.

**Scope:** Frequency calibration, gain calibration, DC offset calibration, calibration data management

**Deliverables by EOD:**
- [ ] CalibrationWizard class fully functional
- [ ] Frequency calibration working (<0.1ppm)
- [ ] Gain calibration implemented
- [ ] Calibration data persistence working
- [ ] Interactive wizard UI
- [ ] Validation tests passing

**Implementation Requirements:**

### Calibration System

**Python Calibration Wizard (`wideband_sdr/calibration.py`):**

```python
class CalibrationWizard:
    """Interactive calibration for SDR device"""
    
    def __init__(self, sdr):
        self.sdr = sdr
        self.calibration_data = {}
    
    def calibrate_frequency_accuracy(self, reference_freq: int) -> float:
        """Calibrate frequency against known reference signal"""
        print(f"Calibrating at {reference_freq/1e6:.2f} MHz...")
        
        self.sdr.set_frequency(reference_freq)
        time.sleep(1.0)  # PLL settling time
        
        # Collect samples
        samples = []
        for _ in range(10):
            chunk = self.sdr.read_samples(1024)
            if chunk is not None:
                samples.extend(chunk)
        
        samples = np.array(samples)
        
        # FFT to find peak
        fft = np.fft.fftshift(np.fft.fft(samples))
        frequencies = np.fft.fftshift(np.fft.fftfreq(
            len(samples), 1/self.sdr.current_sample_rate))
        
        peak_idx = np.argmax(np.abs(fft))
        measured_freq = frequencies[peak_idx]
        
        # Calculate error in ppm
        freq_error_ppm = ((measured_freq - reference_freq) / reference_freq) * 1e6
        
        print(f"Error: {freq_error_ppm:.2f} ppm")
        
        self.calibration_data['frequency_error_ppm'] = freq_error_ppm
        return freq_error_ppm
    
    def calibrate_gain(self, reference_power_dbm: float) -> dict:
        """Calibrate gain settings against known power level"""
        print("Calibrating gain...")
        
        gain_calibration = {}
        
        for gain_level in range(0, 64, 5):
            self.sdr.set_gain(gain_level)
            time.sleep(0.5)
            
            samples = self.sdr.read_samples(4096)
            if samples is not None:
                power_linear = np.mean(np.abs(samples)**2)
                power_dbm = 10 * np.log10(power_linear)
                
                gain_calibration[gain_level] = {
                    'measured_power_dbm': power_dbm,
                    'error_db': power_dbm - reference_power_dbm
                }
                
                print(f"  Gain {gain_level:2d}dB: {power_dbm:.1f} dBm")
        
        self.calibration_data['gain_calibration'] = gain_calibration
        return gain_calibration
    
    def save_calibration(self, filename: str):
        """Save calibration to JSON file"""
        with open(filename, 'w') as f:
            json.dump(self.calibration_data, f, indent=2)
        print(f"Calibration saved to {filename}")
    
    def load_calibration(self, filename: str):
        """Load calibration from JSON file"""
        with open(filename, 'r') as f:
            self.calibration_data = json.load(f)
        print(f"Calibration loaded from {filename}")
    
    def run_interactive_wizard(self):
        """Interactive calibration wizard"""
        print("\n=== Wideband SDR Calibration Wizard ===\n")
        
        print("1. Frequency Accuracy Calibration")
        print("   Requires: Known frequency reference signal")
        freq_ref = float(input("   Enter reference frequency (MHz): ")) * 1e6
        self.calibrate_frequency_accuracy(freq_ref)
        
        print("\n2. Gain Calibration")
        print("   Requires: Known amplitude signal")
        power_ref = float(input("   Enter reference power (dBm): "))
        self.calibrate_gain(power_ref)
        
        print("\n3. Save Results")
        filename = input("   Filename [calibration.json]: ") or "calibration.json"
        self.save_calibration(filename)
        
        print("\nCalibration complete!")
```

**Validation:**
- Frequency calibration accuracy <0.1 ppm
- Gain measurements consistent
- Data saved and loaded correctly
- Interactive wizard works smoothly

**Time Estimate:** 5-6 hours

---

## Prompt 0i: Self-Test and Diagnostics - Day 9

**Objective:** Implement comprehensive self-test and diagnostic framework for hardware validation.

**Scope:** USB connectivity testing, frequency range verification, sample rate testing, comprehensive diagnostics

**Deliverables by EOD:**
- [ ] DiagnosticsEngine class complete
- [ ] USB connectivity test working
- [ ] Frequency range tests passing
- [ ] Sample rate range tests passing
- [ ] Full diagnostic suite functional
- [ ] Test report generation

**Implementation Requirements:**

### Self-Test and Diagnostics

**Python Diagnostics Module (`wideband_sdr/diagnostics.py`):**

```python
class DiagnosticsEngine:
    """Comprehensive self-test and diagnostics"""
    
    def __init__(self, sdr):
        self.sdr = sdr
        self.test_results = {}
    
    def test_usb_connectivity(self) -> bool:
        """Test USB communication"""
        try:
            self.sdr.device.ctrl_transfer(
                bmRequestType=0x80, bRequest=0x06,
                wValue=0x0100, wIndex=0x0000,
                data_or_wLength=64
            )
            self.test_results['usb_connectivity'] = 'PASS'
            return True
        except Exception as e:
            self.test_results['usb_connectivity'] = f'FAIL: {e}'
            return False
    
    def test_frequency_range(self) -> dict:
        """Test frequency coverage"""
        test_frequencies = [1e6, 100e6, 500e6, 1e9, 5e9, 10e9]
        results = {}
        
        for freq in test_frequencies:
            try:
                self.sdr.set_frequency(freq)
                time.sleep(0.2)
                if self.sdr.get_frequency() == freq:
                    results[f'{freq/1e9:.1f}GHz'] = 'PASS'
                else:
                    results[f'{freq/1e9:.1f}GHz'] = 'FAIL'
            except Exception as e:
                results[f'{freq/1e9:.1f}GHz'] = f'ERROR: {e}'
        
        self.test_results['frequency_range'] = results
        return results
    
    def test_sample_rate_range(self) -> dict:
        """Test sample rate coverage"""
        test_rates = [1e3, 1e6, 10e6, 50e6, 105e6]
        results = {}
        
        for rate in test_rates:
            try:
                self.sdr.set_sample_rate(int(rate))
                time.sleep(0.2)
                if self.sdr.get_sample_rate() == rate:
                    results[f'{rate/1e6:.1f}MSPS'] = 'PASS'
                else:
                    results[f'{rate/1e6:.1f}MSPS'] = 'FAIL'
            except Exception as e:
                results[f'{rate/1e6:.1f}MSPS'] = f'ERROR: {e}'
        
        self.test_results['sample_rate_range'] = results
        return results
    
    def test_streaming(self, duration: float = 1.0) -> dict:
        """Test data streaming"""
        results = {}
        
        try:
            self.sdr.start_stream()
            start_time = time.time()
            sample_count = 0
            
            while time.time() - start_time < duration:
                samples = self.sdr.read_samples(1024, timeout=0.5)
                if samples is not None:
                    sample_count += len(samples)
            
            self.sdr.stop_stream()
            
            throughput_msps = (sample_count / duration) / 1e6
            results['streaming'] = 'PASS'
            results['throughput_msps'] = throughput_msps
            
        except Exception as e:
            results['streaming'] = f'FAIL: {e}'
        
        self.test_results['streaming'] = results
        return results
    
    def run_full_diagnostics(self) -> dict:
        """Run complete diagnostic suite"""
        print("\n=== Running Full Diagnostics ===\n")
        
        tests = [
            ('USB Connectivity', self.test_usb_connectivity),
            ('Frequency Range', self.test_frequency_range),
            ('Sample Rate Range', self.test_sample_rate_range),
            ('Streaming', self.test_streaming),
        ]
        
        for test_name, test_func in tests:
            print(f"  {test_name:.<40}", end=' ')
            try:
                test_func()
                print("✓")
            except Exception as e:
                print(f"✗ ({e})")
        
        print("\n=== Diagnostics Summary ===\n")
        for test_name, result in self.test_results.items():
            print(f"{test_name}: {result}\n")
        
        return self.test_results
    
    def generate_report(self, filename: str = "diagnostics_report.txt"):
        """Generate diagnostic report"""
        with open(filename, 'w') as f:
            f.write("=== Wideband SDR Diagnostics Report ===\n")
            f.write(f"Date: {time.strftime('%Y-%m-%d %H:%M:%S')}\n\n")
            
            for test_name, result in self.test_results.items():
                f.write(f"{test_name}:\n")
                if isinstance(result, dict):
                    for key, value in result.items():
                        f.write(f"  {key}: {value}\n")
                else:
                    f.write(f"  {result}\n")
                f.write("\n")
```

**Usage Example:**
```python
with WidebandSDR() as sdr:
    diags = DiagnosticsEngine(sdr)
    results = diags.run_full_diagnostics()
    diags.generate_report("diag_report.txt")
```

**Validation:**
- All tests execute without crashes
- Results accurately reflect device status
- Report generation working
- Pass criteria met for all tests

**Time Estimate:** 5-6 hours

---

## Prompt 1: Complete Windows Installer with Zadig Driver Automation

**Objective:** Enhance the existing Windows installer with comprehensive Zadig driver automation and automated device detection.

**Current State Analysis:**
- Basic installer exists in `install_windows.bat` (2,974 bytes)
- Currently handles Python dependencies and basic installation
- Needs enhancement for driver automation and comprehensive testing

**Implementation Requirements:**

### 1. Enhanced Windows Installer (`install_windows.bat`)
- **Zadig Integration:**
  - Download Zadig automatically if not present
  - Automated driver installation for WinUSB
  - Detect device by VID/PID (0x04D8/0x000A)
  - Handle driver conflicts gracefully
  - Add rollback capability if installation fails

- **Dependency Management:**
  - Check and install Python 3.8+ if missing
  - Verify pip and setuptools versions
  - Install/update required packages: `pyusb`, `numpy`, `scipy`, `matplotlib`
  - Download and install GNU Radio if selected
  - Download and install SDR# if selected

- **Device Detection Testing:**
  - Automatic detection of connected SDR devices
  - Test all USB endpoints
  - Verify firmware version compatibility
  - Generate detailed device report

- **Advanced Features:**
  - Silent install mode for enterprise deployment
  - Configuration backup/restore
  - Uninstall capability
  - Installation log file generation

### 2. Driver Management Module
Create `windows_driver_manager.py`:
```python
class WindowsDriverManager:
    def __init__(self):
        self.vendor_id = 0x04D8
        self.product_id = 0x000A
        self.driver_name = "WinUSB"
    
    def install_zadig(self, force=False):
        """Download and install Zadig automatically"""
    
    def install_driver(self, device_id=None):
        """Install driver using Zadig"""
    
    def check_device_status(self):
        """Check if device is properly installed"""
    
    def rollback_driver(self):
        """Rollback to previous driver"""
```

### 3. Test Suite (`test_windows_installer.py`)
- Unit tests for installer functions
- Integration tests with real hardware
- Mock device testing for CI/CD
- Performance benchmarking

### 4. Documentation Updates
- Update README.md with enhanced Windows installation steps
- Add troubleshooting guide for common Windows issues
- Create video tutorial for automated installer

**Deliverables:**
- Enhanced `install_windows.bat` with Zadig integration
- `windows_driver_manager.py` module
- `test_windows_installer.py` test suite
- Updated documentation with Windows-specific guides
- Installation verification script

---

## Prompt 2: Cross-Platform Testing and Comprehensive Validation

**Objective:** Create a robust cross-platform testing framework that validates all features across Windows, Linux, and macOS with automated hardware-in-the-loop testing.

**Implementation Requirements:**

### 1. Unified Test Framework (`test_framework/`)
```
test_framework/
├── __init__.py
├── base_test.py          # Base test class
├── hardware_interface.py # Hardware abstraction
├── test_runner.py       # Test execution engine
├── test_data/           # Test data files
├── fixtures/            # Test fixtures and mocks
└── reporters/           # Test result reporters
```

### 2. Platform Abstraction Layer
```python
class PlatformManager:
    def __init__(self):
        self.platform = self._detect_platform()
        self.device_interface = self._get_device_interface()
    
    def _detect_platform(self):
        """Detect Windows/Linux/macOS"""
    
    def setup_device_permissions(self):
        """Configure device access based on platform"""
    
    def run_benchmark(self, test_name):
        """Run platform-specific benchmarks"""
```

### 3. Hardware Testing Suite
Create `test_hardware.py`:
```python
class HardwareTestSuite:
    def test_usb_enumeration(self):
        """Test device detection and enumeration"""
    
    def test_frequency_accuracy(self):
        """Test PLL frequency accuracy across range"""
    
    def test_sample_rate_precision(self):
        """Test sample rate accuracy and stability"""
    
    def test_gain_control(self):
        """Test LNA gain control functionality"""
    
    def test_temperature_monitoring(self):
        """Test temperature sensor accuracy"""
    
    def test_power_management(self):
        """Test USB PD negotiation"""
    
    def test_self_diagnostic(self):
        """Test built-in self-test routines"""
```

### 4. Performance Testing
```python
class PerformanceTests:
    def test_streaming_throughput(self):
        """Test maximum sustainable sample rates"""
    
    def test_buffer_overflow_handling(self):
        """Test DMA buffer overflow protection"""
    
    def test_usb_bandwidth_utilization(self):
        """Test USB 2.0 bandwidth efficiency"""
    
    def test_cpu_usage(self):
        """Test CPU usage during various operations"""
    
    def test_memory_consumption(self):
        """Test memory usage patterns"""
```

### 5. Compatibility Testing
- **Python Version Testing:** Test on Python 3.8, 3.9, 3.10, 3.11
- **USB Controller Testing:** Test with different USB host controllers
- **Hardware Variation:** Test with different SDR hardware revisions
- **Real-time Constraints:** Test timing-critical operations

### 6. Automated Test Execution
Create `run_tests.py`:
```python
def run_comprehensive_test_suite():
    """Run all tests across all platforms"""
    platforms = ['windows', 'linux', 'macos']
    test_categories = ['unit', 'integration', 'hardware', 'performance']
    
    for platform in platforms:
        for category in test_categories:
            run_test_category(platform, category)
    
    generate_combined_report()
```

### 7. Continuous Integration
**GitHub Actions Configuration:**
```yaml
name: Cross-Platform Testing
on: [push, pull_request]
jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        python-version: ['3.8', '3.9', '3.10', '3.11']
    
    steps:
    - uses: actions/checkout@v3
    - name: Set up Python
      uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        pip install -r requirements.txt
        pip install pytest pytest-cov
    
    - name: Run tests
      run: |
        pytest test_framework/ --cov=wideband_sdr
    
    - name: Upload coverage
      uses: codecov/codecov-action@v3
```

### 8. Test Data Management
- **Signal Generators:** Generate known test signals
- **Reference Measurements:** Store expected results
- **Calibration Data:** Test with various calibration states
- **Stress Testing:** Extreme condition test scenarios

### 9. Documentation and Reporting
- **Test Documentation:** Comprehensive test descriptions
- **Coverage Reports:** Code coverage analysis
- **Performance Reports:** Benchmark comparisons
- **Platform Comparison:** Cross-platform feature matrix

**Deliverables:**
- Complete `test_framework/` directory with all test components
- Cross-platform `run_tests.py` execution script
- GitHub Actions CI/CD configuration
- Comprehensive test documentation
- Platform-specific installation testing scripts

---

## Prompt 3: Performance Optimization and Advanced Tuning

**Objective:** Implement comprehensive performance optimizations across firmware and software, achieving maximum throughput and minimum latency while maintaining signal quality.

**Implementation Requirements:**

### 1. Firmware Performance Optimization

#### DMA Buffer Management Optimization
```c
// Enhanced DMA buffer management with double buffering
typedef struct {
    uint16_t *primary_buffer;
    uint16_t *secondary_buffer;
    uint32_t buffer_size;
    volatile uint32_t write_index;
    volatile uint32_t read_index;
    volatile bool buffer_ready;
    volatile uint32_t overflow_count;
} optimized_buffer_manager_t;

// Implement lock-free buffer switching
bool switch_buffer_optimized(void);
uint32_t get_buffer_utilization(void);
bool optimize_for_sample_rate(uint32_t sample_rate);
```

#### USB Transfer Optimization
- **Bulk Transfer Sizing:** Optimize USB packet sizes for maximum throughput
- **DMA-to-USB Direct:** Implement direct DMA-to-USB transfers
- **Compression Integration:** Hardware-accelerated compression
- **Parallel Processing:** Multi-threaded USB processing

#### ADC Interface Optimization
```c
// Optimize ADC sampling for different sample rates
bool optimize_adc_timing(uint32_t sample_rate);
void configure_optimal_prescaler(uint32_t sample_rate);
bool enable_adc_overdrive_mode(void);
```

### 2. Python Software Performance Optimization

#### Multi-threading and Async Processing
```python
import asyncio
import concurrent.futures
from multiprocessing import Pool

class OptimizedSDRProcessor:
    def __init__(self):
        self.thread_pool = ThreadPoolExecutor(max_workers=4)
        self.process_pool = ProcessPoolExecutor(max_workers=2)
        self.gpu_available = self._check_gpu_availability()
    
    async def process_samples_async(self, samples):
        """Async sample processing"""
    
    def parallel_fft_processing(self, samples):
        """Parallel FFT using multiple cores"""
    
    def gpu_accelerated_processing(self, samples):
        """GPU-accelerated processing using CuPy"""
```

#### NumPy/SciPy Optimization
- **Vectorized Operations:** Replace loops with NumPy vectorization
- **SIMD Instructions:** Utilize NumPy SIMD optimizations
- **Memory Layout:** Optimize array memory layout for cache efficiency
- **FFT Optimization:** Use optimized FFT libraries (FFTW, Intel MKL)

#### Performance Monitoring
```python
class PerformanceMonitor:
    def __init__(self):
        self.cpu_usage = psutil.cpu_percent(interval=1)
        self.memory_usage = psutil.virtual_memory()
        self.gpu_usage = self._get_gpu_usage()
    
    def start_profiling(self):
        """Start detailed performance profiling"""
    
    def get_bottleneck_analysis(self):
        """Identify performance bottlenecks"""
    
    def optimize_memory_layout(self):
        """Optimize memory usage patterns"""
```

### 3. Signal Processing Optimization

#### Digital Downconversion Optimization
```python
class OptimizedDDC:
    def __init__(self):
        self.use_gpu = self._check_gpu_capability()
        self.optimize_filters = self._create_optimized_filters()
    
    def cupy_digital_downconversion(self, samples, center_freq):
        """GPU-accelerated DDC using CuPy"""
    
    def optimized_iq_generation(self, samples):
        """Optimized I/Q signal generation"""
    
    def adaptive_filtering(self, samples, bandwidth):
        """Adaptive filtering based on signal characteristics"""
```

#### FFT Acceleration
```python
class FFTAccelerator:
    def __init__(self):
        self.fft_library = self._select_optimal_fft_library()
        self.gpu_fft = self._init_gpu_fft()
    
    def gpu_accelerated_fft(self, samples):
        """GPU-accelerated FFT processing"""
    
    def batch_fft_processing(self, batch_samples):
        """Process multiple FFTs in batch"""
    
    def real_time_fft_optimization(self, sample_rate):
        """Optimize FFT for real-time processing"""
```

### 4. Memory Management Optimization

#### Firmware Memory Optimization
- **Buffer Pool Management:** Implement efficient buffer allocation
- **DMA Memory Alignment:** Ensure optimal DMA memory alignment
- **Cache Optimization:** Optimize data structures for CPU cache efficiency
- **Memory Leak Prevention:** Comprehensive memory leak detection

#### Python Memory Optimization
```python
class MemoryOptimizer:
    def __init__(self):
        self.buffer_pool = BufferPool()
        self.memory_tracker = MemoryTracker()
    
    def optimize_array_allocations(self):
        """Optimize NumPy array memory usage"""
    
    def implement_memory_pool(self):
        """Implement custom memory pool for sample data"""
    
    def garbage_collection_tuning(self):
        """Optimize garbage collection for SDR workloads"""
```

### 5. Real-time Performance Optimization

#### Latency Optimization
```python
class LatencyOptimizer:
    def __init__(self):
        self.real_time_priority = self._set_realtime_priority()
        self.cpu_affinity = self._set_cpu_affinity()
    
    def minimize_processing_latency(self):
        """Minimize end-to-end processing latency"""
    
    def optimize_thread_scheduling(self):
        """Optimize thread scheduling for real-time performance"""
    
    def implement_priority_inheritance(self):
        """Implement priority inheritance for critical sections"""
```

#### Throughput Optimization
- **Pipeline Optimization:** Optimize processing pipelines
- **Parallel Processing:** Maximize parallel processing capabilities
- **Data Flow Optimization:** Optimize data flow patterns
- **Hardware Utilization:** Maximize hardware resource utilization

### 6. Performance Benchmarking Suite

#### Automated Benchmarking
```python
class PerformanceBenchmark:
    def benchmark_firmware_performance(self):
        """Benchmark firmware performance metrics"""
    
    def benchmark_software_performance(self):
        """Benchmark software performance metrics"""
    
    def benchmark_end_to_end_performance(self):
        """Benchmark complete system performance"""
    
    def generate_performance_report(self):
        """Generate detailed performance report"""
```

#### Performance Regression Testing
- **Baseline Comparisons:** Compare against performance baselines
- **Regression Detection:** Automated performance regression detection
- **Performance Monitoring:** Continuous performance monitoring
- **Optimization Tracking:** Track optimization effectiveness

**Deliverables:**
- Optimized firmware with enhanced DMA and USB performance
- High-performance Python software with GPU acceleration
- Comprehensive performance monitoring and benchmarking tools
- Performance optimization documentation and guidelines
- Automated performance regression testing framework

---

## Prompt 4: Documentation Updates and API Completion

**Objective:** Create comprehensive, professional-grade documentation covering all aspects of the Wideband SDR system with complete API documentation, tutorials, and support materials.

**Implementation Requirements:**

### 1. Complete API Documentation

#### Sphinx Documentation Setup
```
docs/
├── source/
│   ├── conf.py              # Sphinx configuration
│   ├── index.rst             # Main documentation index
│   ├── api/                  # API documentation
│   │   ├── firmware_api.rst
│   │   ├── python_api.rst
│   │   └── examples_api.rst
│   ├── tutorials/           # Step-by-step tutorials
│   │   ├── quick_start.rst
│   │   ├── advanced_usage.rst
│   │   ├── hardware_guide.rst
│   │   └── troubleshooting.rst
│   ├── reference/           # Reference documentation
│   │   ├── command_reference.rst
│   │   ├── configuration_reference.rst
│   │   └── file_formats.rst
│   └── development/         # Development documentation
│       ├── contributing.rst
│       ├── architecture.rst
│       └── testing_guide.rst
```

#### Auto-generated API Documentation
```python
# Firmware API Documentation
def adf4351_set_frequency(freq):
    """
    Set ADF4351 PLL frequency
    
    Args:
        freq (uint32_t): Target frequency in Hz
        
    Returns:
        bool: Success status
        
    Example:
        >>> adf4351_set_frequency(100000000)  # 100 MHz
        True
        
    Notes:
        - Frequency range: 35 MHz - 10 GHz
        - Automatic output divider selection
        - Sub-Hz frequency resolution
    """

# Python API Documentation
def WidebandSDR.digital_downconversion(center_freq, bandwidth):
    """
    Apply digital downconversion and filtering
    
    Args:
        center_freq (int): Center frequency in Hz
        bandwidth (int): Bandwidth in Hz
        
    Returns:
        bool: Success status
        
    Example:
        >>> sdr = WidebandSDR()
        >>> sdr.digital_downconversion(95_500_000, 200_000)
        True
        
    See Also:
        - WidebandSDR.apply_fft_acceleration()
        - WidebandSDR.enhanced_waterfall()
    """
```

### 2. Comprehensive User Documentation

#### Quick Start Guide (`docs/tutorials/quick_start.rst`)
```rst
Quick Start Guide
=================

Hardware Setup
--------------

1. **Connect Hardware**
   
   - Connect Wideband SDR to USB port
   - Connect antenna to RF input
   - Power on device (if externally powered)

2. **Install Software**
   
   **Windows:**
   
   .. code-block:: batch
   
      install_windows.bat
   
   **Linux:**
   
   .. code-block:: bash
   
      sudo apt-get install python3-pip libusb-1.0-0-dev
      pip3 install pyusb numpy scipy
   
   **macOS:**
   
   .. code-block:: bash
   
      brew install python3 libusb
      pip3 install pyusb numpy scipy

3. **First Application**
   
   .. code-block:: python
   
      from wideband_sdr import WidebandSDR
      
      with WidebandSDR() as sdr:
          sdr.set_frequency(95_500_000)  # 95.5 MHz FM
          sdr.set_sample_rate(2_000_000)  # 2 MSPS
          samples = sdr.read_samples(1024)
          print(f"Received {len(samples)} samples")
```

#### Advanced Usage Guide (`docs/tutorials/advanced_usage.rst`)
```rst
Advanced Usage
==============

High-Performance Streaming
--------------------------

.. code-block:: python

   import numpy as np
   from scipy import signal
   
   def advanced_spectrum_analyzer():
       with WidebandSDR() as sdr:
           sdr.set_frequency(100_000_000)  # 100 MHz
           sdr.set_sample_rate(20_000_000)  # 20 MSPS
           sdr.enable_compression("delta")
           
           def spectrum_callback(samples):
               # Apply windowing
               windowed = samples * signal.windows.hann(len(samples))
               
               # Compute FFT
               fft = sdr.apply_fft_acceleration(windowed)
               
               # Convert to dB
               magnitude_db = 20 * np.log10(np.abs(fft))
               
               # Update display
               update_spectrum_display(magnitude_db)
           
           sdr.start_stream(spectrum_callback)
```

### 3. Hardware Documentation

#### Hardware Architecture Guide (`docs/reference/hardware_architecture.rst`)
```rst
Hardware Architecture
====================

Main Components
---------------

**U1: dsPIC33AK256MC505**
   - 200 MHz dsPIC architecture
   - 256 KB Flash, 512 KB SRAM
   - USB 2.0 High-Speed controller
   - 10-bit ADC interface

**U2: AD9215BCPZ-105**
   - 10-bit 105 MSPS ADC
   - Parallel data interface
   - 3.3V operation

**U3: ADF4351**
   - Wideband PLL/VCO
   - 35 MHz - 10 GHz range
   - Fractional-N synthesis
   - Sub-Hz resolution

**U4: BGA614H6327XTSA1**
   - Low Noise Amplifier
   - 0-2.4 GHz range
   - 17.5 dB typical gain
   - 2.5 dB noise figure
```

#### Schematic Documentation (`docs/hardware/schematics/`)
- Detailed schematics with component annotations
- PCB layout documentation
- Signal path descriptions
- Timing diagrams
- Component specifications

### 4. Software Architecture Documentation

#### Software Architecture Guide (`docs/development/architecture.rst`)
```rst
Software Architecture
====================

Firmware Architecture
---------------------

The firmware implements a layered architecture:

1. **Hardware Abstraction Layer (HAL)**
   - ADC interface management
   - PLL control interface
   - USB communication layer
   - GPIO control

2. **Device Drivers**
   - ADF4351 PLL driver
   - AD9215 ADC driver
   - LNA control driver
   - Temperature sensor driver

3. **Application Layer**
   - Command processing
   - Sample streaming
   - Calibration routines
   - Self-diagnostic functions

Python Software Architecture
----------------------------

The Python software follows a modular design:

1. **Core Driver (`wideband_sdr.py`)**
   - Device communication
   - Sample processing
   - Configuration management

2. **Signal Processing Module**
   - Digital downconversion
   - FFT acceleration
   - Spectrum analysis

3. **Application Examples**
   - Spectrum analyzer
   - Frequency scanner
   - Waterfall display
```

### 5. Configuration and File Format Documentation

#### Configuration Reference (`docs/reference/configuration.rst`)
```rst
Configuration Reference
======================

Device Configuration
--------------------

The SDR device supports the following configuration parameters:

**Frequency Settings**

- **Range:** 1 MHz - 10 GHz
- **Resolution:** < 1 Hz (with calibration)
- **Accuracy:** ±0.1 ppm (with TCXO)

**Sample Rate Settings**

- **Range:** 1 kSPS - 105 MSPS
- **Resolution:** Programmable
- **Accuracy:** ±0.01%

**Gain Settings**

- **LNA Gain:** 0 - 30 dB (BGA614)
- **ADC Gain:** Programmable in firmware
- **Step Size:** 1 dB

**File Formats**

**IQ Recording Format (.iq)**
   - Complex float32 format
   - Sample rate metadata
   - Frequency metadata
   - Timestamp information

**WAV Format (.wav)**
   - 16-bit signed integer
   - PCM encoding
   - Sample rate metadata
   - Channel information
```

### 6. Troubleshooting Guide

#### Comprehensive Troubleshooting (`docs/troubleshooting/`)
```rst
Troubleshooting Guide
====================

Common Issues
-------------

**Device Not Detected**

Windows:
1. Check Device Manager for "Unknown Device"
2. Run installer_windows.bat with administrator privileges
3. Install WinUSB driver using Zadig

Linux:
.. code-block:: bash

   # Check USB device
   lsusb | grep 04D8
   
   # Check permissions
   ls -l /dev/bus/usb/*/*
   
   # Reload udev rules
   sudo udevadm control --reload-rules

**Streaming Errors**

- Verify USB cable quality (use USB 2.0+ certified cable)
- Check USB port specification (avoid USB 1.1 ports)
- Monitor CPU usage during streaming
- Reduce sample rate if buffer overflow occurs

**Frequency Accuracy Issues**

- Run calibration routine
- Check temperature compensation
- Verify crystal oscillator frequency
- Update firmware to latest version
```

### 7. Developer Documentation

#### Contributing Guide (`docs/development/contributing.rst`)
```rst
Contributing Guidelines
======================

Development Setup
-----------------

1. **Clone Repository**
   
   .. code-block:: bash
   
      git clone https://github.com/iamdarshg/Code-SDR.git
      cd Code-SDR

2. **Firmware Development**
   
   - Install MPLAB X IDE
   - Install XC16 compiler
   - Use provided Makefile for building

3. **Python Development**
   
   .. code-block:: bash
   
      pip install -e .
      pip install pytest pytest-cov

Coding Standards
----------------

**C Coding Standards (Firmware)**

- Follow MISRA C guidelines
- Use meaningful variable names
- Comment complex algorithms
- Include error handling

**Python Coding Standards**

- Follow PEP 8 style guidelines
- Use type hints
- Include docstrings
- Write unit tests
```

### 8. Interactive Documentation

#### Jupyter Notebook Tutorials
Create `tutorials/` directory with interactive notebooks:
- `01_quick_start.ipynb` - Interactive quick start
- `02_signal_processing.ipynb` - Signal processing tutorials
- `03_advanced_features.ipynb` - Advanced feature tutorials
- `04_performance_optimization.ipynb` - Performance tuning

#### Online Documentation Website
- Host documentation on ReadTheDocs
- Include interactive examples
- Add search functionality
- Mobile-responsive design

**Deliverables:**
- Complete Sphinx documentation structure
- Auto-generated API documentation
- Comprehensive user tutorials
- Hardware and software architecture guides
- Troubleshooting and FAQ sections
- Interactive Jupyter notebook tutorials
- Online documentation website
- Contributing and development guides

---

## Summary

These five prompts provide comprehensive, detailed instructions for completing the remaining phases of the Wideband SDR development. Each prompt is self-contained and provides specific implementation details, code examples, and deliverable specifications. The prompts cover:

1. **Windows Installer Enhancement** - Complete Zadig integration and driver automation
2. **macOS Support** - Full macOS installation and native application support
3. **Cross-Platform Testing** - Comprehensive testing framework across all platforms
4. **Performance Optimization** - Firmware and software performance enhancements
5. **Documentation and API** - Complete professional documentation

Each prompt is designed to be implemented independently and contains sufficient detail for a developer to complete the phase successfully.
