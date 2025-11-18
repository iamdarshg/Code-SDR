/*
 * Copyright (C) 2025 Wideband SDR Project
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_WIDEBAND_SDR_SOURCE_IMPL_H
#define INCLUDED_WIDEBAND_SDR_SOURCE_IMPL_H

#include <gnuradio/sync_block.h>
#include "wideband_sdr/source.h"
#include "wideband_sdr/api.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <complex>

namespace gr {
  namespace wideband_sdr {

    /*!
     * \brief Implementation class for Wideband SDR source block
     * \ingroup wideband_sdr
     *
     * \details
     * This class provides the implementation for the Wideband SDR source block,
     * handling device communication, streaming, and sample processing.
     */
    class source_impl : public source
    {
     private:
      // SDR Device parameters
      uint32_t d_sample_rate;
      uint32_t d_frequency;
      uint8_t d_gain;
      bool d_streaming_active;
      
      // Buffer management
      std::vector<std::complex<float>> d_sample_buffer;
      std::mutex d_mutex;
      std::condition_variable d_cv;
      bool d_data_ready;
      
      // Device control
      int d_device_index;
      void* d_device_handle;  // Placeholder for actual device handle
      
      // Statistics
      uint32_t d_samples_processed;
      uint32_t d_packets_dropped;
      uint32_t d_dropped_bytes;
      
      // Streaming thread
      std::thread d_streaming_thread;
      bool d_thread_running;
      
      // Private methods
      bool open_device();
      void close_device();
      bool configure_device();
      void streaming_thread_function();
      bool read_device_samples(std::complex<float>* buffer, uint32_t max_samples);
      uint32_t get_buffer_utilization();

     public:
      source_impl(double sample_rate, double frequency, int gain);
      ~source_impl() override;

      // GNU Radio block interface
      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items) override;

      // Device control methods
      bool set_frequency(double frequency) override;
      double get_frequency() const override;
      
      bool set_sample_rate(double sample_rate) override;
      double get_sample_rate() const override;
      
      bool set_gain(int gain) override;
      int get_gain() const override;
      
      bool start_streaming() override;
      bool stop_streaming() override;
      bool is_streaming() const override;
      
      // Device information
      std::map<std::string, std::string> get_device_info() const override;
      std::map<std::string, uint32_t> get_statistics() const override;
      
      // Static factory method
      static sptr make(double sample_rate, double frequency, int gain);
    };

  } // namespace wideband_sdr
} // namespace gr

#endif /* INCLUDED_WIDEBAND_SDR_SOURCE_IMPL_H */
