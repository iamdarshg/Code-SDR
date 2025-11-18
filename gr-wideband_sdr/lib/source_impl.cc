/*
 * Copyright (C) 2025 Wideband SDR Project
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/io_signature.h>
#include "source_impl.h"
#include <iostream>
#include <chrono>
#include <stdexcept>

namespace gr {
  namespace wideband_sdr {

    source::sptr source_impl::make(double sample_rate, double frequency, int gain)
    {
      return gnuradio::make_block_sptr<source_impl>(sample_rate, frequency, gain);
    }

    source_impl::source_impl(double sample_rate, double frequency, int gain)
      : gr::sync_block("wideband_sdr_source",
                      gr::io_signature::make(0, 0, 0),
                      gr::io_signature::make(1, 1, sizeof(std::complex<float>))),
                      d_sample_rate(static_cast<uint32_t>(sample_rate)),
                      d_frequency(static_cast<uint32_t>(frequency)),
                      d_gain(static_cast<uint8_t>(gain)),
                      d_streaming_active(false),
                      d_device_index(0),
                      d_device_handle(nullptr),
                      d_samples_processed(0),
                      d_packets_dropped(0),
                      d_dropped_bytes(0),
                      d_thread_running(false)
    {
      // Initialize buffer with reasonable size
      d_sample_buffer.resize(32768);  // 32K samples buffer
      
      // Signal that no data is ready initially
      d_data_ready = false;
      
      // Try to open the device (non-blocking for now)
      if (!open_device()) {
        std::cerr << "Warning: Could not open SDR device. Will retry when streaming starts." << std::endl;
      }
      
      // Configure device if opened
      if (d_device_handle != nullptr) {
        configure_device();
      }
    }

    source_impl::~source_impl()
    {
      // Stop streaming if active
      if (d_streaming_active) {
        stop_streaming();
      }
      
      // Close device
      close_device();
    }

    bool source_impl::open_device()
    {
      try {
        // Placeholder for device opening
        // In real implementation, this would use USB communication
        // to establish connection with the WidebandSDR device
        
        std::cout << "Opening Wideband SDR device (index " << d_device_index << ")..." << std::endl;
        
        // Simulate successful device opening
        d_device_handle = new int(1);  // Placeholder handle
        return true;
      }
      catch (const std::exception& e) {
        std::cerr << "Error opening device: " << e.what() << std::endl;
        d_device_handle = nullptr;
        return false;
      }
    }

    void source_impl::close_device()
    {
      if (d_device_handle != nullptr) {
        std::cout << "Closing Wideband SDR device..." << std::endl;
        delete static_cast<int*>(d_device_handle);
        d_device_handle = nullptr;
      }
    }

    bool source_impl::configure_device()
    {
      try {
        // Set frequency
        if (!set_frequency(d_frequency)) {
          std::cerr << "Failed to set frequency" << std::endl;
          return false;
        }
        
        // Set sample rate
        if (!set_sample_rate(d_sample_rate)) {
          std::cerr << "Failed to set sample rate" << std::endl;
          return false;
        }
        
        // Set gain
        if (!set_gain(d_gain)) {
          std::cerr << "Failed to set gain" << std::endl;
          return false;
        }
        
        std::cout << "Device configured: " << d_frequency/1e6 << " MHz, " 
                  << d_sample_rate/1e6 << " MSPS, Gain: " << static_cast<int>(d_gain) << " dB" << std::endl;
        
        return true;
      }
      catch (const std::exception& e) {
        std::cerr << "Error configuring device: " << e.what() << std::endl;
        return false;
      }
    }

    void source_impl::streaming_thread_function()
    {
      std::vector<std::complex<float>> temp_buffer(8192);  // 8K temp buffer
      
      while (d_thread_running && d_streaming_active) {
        try {
          // Read samples from device
          uint32_t samples_read = 0;
          bool success = read_device_samples(temp_buffer.data(), temp_buffer.size(), samples_read);
          
          if (success && samples_read > 0) {
            // Lock buffer for writing
            std::lock_guard<std::mutex> lock(d_mutex);
            
            // Copy to main buffer, handling wrap-around
            for (uint32_t i = 0; i < samples_read; i++) {
              d_sample_buffer[d_write_index] = temp_buffer[i];
              d_write_index = (d_write_index + 1) % d_sample_buffer.size();
              
              if (d_write_index == d_read_index) {
                // Buffer overflow - drop oldest sample
                d_read_index = (d_read_index + 1) % d_sample_buffer.size();
                d_packets_dropped++;
                d_dropped_bytes += sizeof(std::complex<float>);
              }
            }
            
            d_data_ready = true;
            d_cv.notify_one();
          }
          
          // Small delay to prevent excessive CPU usage
          std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        catch (const std::exception& e) {
          std::cerr << "Error in streaming thread: " << e.what() << std::endl;
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
      }
    }

    bool source_impl::read_device_samples(std::complex<float>* buffer, uint32_t max_samples, uint32_t& samples_read)
    {
      if (d_device_handle == nullptr) {
        // Generate simulated samples for testing
        samples_read = std::min(max_samples, 1024u);
        
        static float t = 0.0f;
        for (uint32_t i = 0; i < samples_read; i++) {
          float signal_freq = 1000.0f;  // 1 kHz test signal
          buffer[i] = std::complex<float>(
            0.1f * std::cos(2.0f * M_PI * signal_freq * (t + i) / 192000.0f),
            0.1f * std::sin(2.0f * M_PI * signal_freq * (t + i) / 192000.0f)
          );
        }
        
        t += samples_read / 192000.0f;
        return true;
      }
      
      // Real device reading would go here
      // This would involve USB communication with the device
      
      samples_read = 0;
      return false;  // Placeholder
    }

    int source_impl::work(int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items)
    {
      std::complex<float>* out = static_cast<std::complex<float>*>(output_items[0]);
      int samples_written = 0;
      
      // Check if streaming is active
      if (!d_streaming_active) {
        // Generate zeros when not streaming
        for (int i = 0; i < noutput_items; i++) {
          out[i] = std::complex<float>(0.0f, 0.0f);
        }
        return noutput_items;
      }
      
      // Wait for data if buffer is empty
      std::unique_lock<std::mutex> lock(d_mutex);
      if (!d_data_ready) {
        d_cv.wait(lock, [this] { return d_data_ready; });
      }
      
      // Read from buffer
      while (samples_written < noutput_items && d_data_ready) {
        if (d_read_index == d_write_index) {
          d_data_ready = false;
          break;
        }
        
        out[samples_written] = d_sample_buffer[d_read_index];
        d_read_index = (d_read_index + 1) % d_sample_buffer.size();
        samples_written++;
        d_samples_processed++;
      }
      
      lock.unlock();
      
      // Return number of samples produced
      return samples_written;
    }

    bool source_impl::set_frequency(double frequency)
    {
      uint32_t freq_hz = static_cast<uint32_t>(frequency);
      
      // Validate frequency range
      if (freq_hz < 1000000 || freq_hz > 10000000000ul) {  // 1 MHz to 10 GHz
        std::cerr << "Error: Frequency out of range (1 MHz - 10 GHz)" << std::endl;
        return false;
      }
      
      d_frequency = freq_hz;
      
      // If device is open, send frequency command
      if (d_device_handle != nullptr) {
        std::cout << "Setting frequency to " << freq_hz/1e6 << " MHz" << std::endl;
        // Real implementation would send USB command here
      }
      
      return true;
    }

    double source_impl::get_frequency() const
    {
      return static_cast<double>(d_frequency);
    }

    bool source_impl::set_sample_rate(double sample_rate)
    {
      uint32_t rate = static_cast<uint32_t>(sample_rate);
      
      // Validate sample rate range
      if (rate < 1000 || rate > 105000000) {  // 1 kSPS to 105 MSPS
        std::cerr << "Error: Sample rate out of range (1 kSPS - 105 MSPS)" << std::endl;
        return false;
      }
      
      d_sample_rate = rate;
      
      // If device is open, send sample rate command
      if (d_device_handle != nullptr) {
        std::cout << "Setting sample rate to " << rate/1e6 << " MSPS" << std::endl;
        // Real implementation would send USB command here
      }
      
      return true;
    }

    double source_impl::get_sample_rate() const
    {
      return static_cast<double>(d_sample_rate);
    }

    bool source_impl::set_gain(int gain)
    {
      if (gain < 0 || gain > 63) {
        std::cerr << "Error: Gain out of range (0-63 dB)" << std::endl;
        return false;
      }
      
      d_gain = static_cast<uint8_t>(gain);
      
      // If device is open, send gain command
      if (d_device_handle != nullptr) {
        std::cout << "Setting gain to " << gain << " dB" << std::endl;
        // Real implementation would send USB command here
      }
      
      return true;
    }

    int source_impl::get_gain() const
    {
      return static_cast<int>(d_gain);
    }

    bool source_impl::start_streaming()
    {
      if (d_streaming_active) {
        return true;  // Already streaming
      }
      
      // Open device if not already open
      if (d_device_handle == nullptr) {
        if (!open_device()) {
          std::cerr << "Failed to open device for streaming" << std::endl;
          return false;
        }
        
        // Configure device
        if (!configure_device()) {
          std::cerr << "Failed to configure device for streaming" << std::endl;
          close_device();
          return false;
        }
      }
      
      // Initialize buffer pointers
      d_read_index = 0;
      d_write_index = 0;
      d_data_ready = false;
      d_streaming_active = true;
      
      // Start streaming thread
      d_thread_running = true;
      d_streaming_thread = std::thread(&source_impl::streaming_thread_function, this);
      
      std::cout << "SDR streaming started" << std::endl;
      return true;
    }

    bool source_impl::stop_streaming()
    {
      if (!d_streaming_active) {
        return true;  // Already stopped
      }
      
      d_streaming_active = false;
      
      // Stop streaming thread
      d_thread_running = false;
      if (d_streaming_thread.joinable()) {
        d_streaming_thread.join();
      }
      
      std::cout << "SDR streaming stopped" << std::endl;
      return true;
    }

    bool source_impl::is_streaming() const
    {
      return d_streaming_active;
    }

    std::map<std::string, std::string> source_impl::get_device_info() const
    {
      std::map<std::string, std::string> info;
      info["device_type"] = "Wideband SDR";
      info["manufacturer"] = "Wideband SDR Project";
      info["model"] = "Wideband SDR V1.0";
      info["firmware_version"] = "1.0.0";
      info["hardware_version"] = "1.0";
      info["serial_number"] = "WBSDR0001";
      info["frequency_range"] = "1 MHz - 10 GHz";
      info["sample_rate_range"] = "1 kSPS - 105 MSPS";
      info["gain_range"] = "0 - 63 dB";
      info["usb_interface"] = "USB 2.0 High-Speed";
      info["resolution"] = "10 bits";
      return info;
    }

    std::map<std::string, uint32_t> source_impl::get_statistics() const
    {
      std::map<std::string, uint32_t> stats;
      stats["samples_processed"] = d_samples_processed;
      stats["packets_dropped"] = d_packets_dropped;
      stats["dropped_bytes"] = d_dropped_bytes;
      stats["buffer_utilization"] = get_buffer_utilization();
      return stats;
    }

    uint32_t source_impl::get_buffer_utilization()
    {
      std::lock_guard<std::mutex> lock(d_mutex);
      if (d_write_index >= d_read_index) {
        return d_write_index - d_read_index;
      } else {
        return d_sample_buffer.size() - d_read_index + d_write_index;
      }
    }

  } // namespace wideband_sdr
} // namespace gr
