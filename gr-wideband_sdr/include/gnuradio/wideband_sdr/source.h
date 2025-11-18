/*
 * Copyright (C) 2025 Wideband SDR Project
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_WIDEBAND_SDR_SOURCE_H
#define INCLUDED_WIDEBAND_SDR_SOURCE_H

#include <gnuradio/sync_block.h>
#include "wideband_sdr/api.h"

namespace gr {
  namespace wideband_sdr {

    /*!
     * \brief <b>Wideband SDR Source Block</b>
     * \ingroup wideband_sdr
     *
     * \details
     * The Wideband SDR source block provides real-time I/Q samples from the
     * Wideband SDR device. It supports frequencies from 1 MHz to 10 GHz
     * with adjustable sample rates and gain control.
     *
     * This block connects to the SDR hardware via USB and streams raw
     * I/Q samples to the GNU Radio flowgraph.
     */
    class WIDEBAND_SDR_API source : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<source> sptr;

      /*!
       * \brief Create a Wideband SDR source block
       * \param sample_rate Sample rate in samples per second (default: 1 MSPS)
       * \param frequency Center frequency in Hz (default: 100 MHz)
       * \param gain RF gain in dB (default: 30 dB)
       * \return Shared pointer to source block
       */
      static sptr make(double sample_rate = 1e6, double frequency = 100e6, int gain = 30);

      // Virtual destructor
      virtual ~source();

      /*!
       * \brief Set the center frequency
       * \param frequency Center frequency in Hz
       * \return True if successful
       */
      virtual bool set_frequency(double frequency) = 0;

      /*!
       * \brief Get the current center frequency
       * \return Current frequency in Hz
       */
      virtual double get_frequency() const = 0;

      /*!
       * \brief Set the sample rate
       * \param sample_rate Sample rate in samples per second
       * \return True if successful
       */
      virtual bool set_sample_rate(double sample_rate) = 0;

      /*!
       * \brief Get the current sample rate
       * \return Current sample rate in samples per second
       */
      virtual double get_sample_rate() const = 0;

      /*!
       * \brief Set the RF gain
       * \param gain Gain in dB (0-63 dB)
       * \return True if successful
       */
      virtual bool set_gain(int gain) = 0;

      /*!
       * \brief Get the current RF gain
       * \return Current gain in dB
       */
      virtual int get_gain() const = 0;

      /*!
       * \brief Start the SDR device streaming
       * \return True if successful
       */
      virtual bool start_streaming() = 0;

      /*!
       * \brief Stop the SDR device streaming
       * \return True if successful
       */
      virtual bool stop_streaming() = 0;

      /*!
       * \brief Check if the device is streaming
       * \return True if streaming
       */
      virtual bool is_streaming() const = 0;

      /*!
       * \brief Get device information
       * \return Dictionary with device information
       */
      virtual std::map<std::string, std::string> get_device_info() const = 0;

      /*!
       * \brief Get streaming statistics
       * \return Dictionary with statistics
       */
      virtual std::map<std::string, uint32_t> get_statistics() const = 0;
    };

  } // namespace wideband_sdr
} // namespace gr

#endif /* INCLUDED_WIDEBAND_SDR_SOURCE_H */
