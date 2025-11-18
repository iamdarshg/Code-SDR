/*
 * Copyright (C) 2025 Wideband SDR Project
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// Include the source block header
#include "wideband_sdr/source.h"
#include "source_impl.h"

namespace py = pybind11;

// Python bindings for the Wideband SDR source block
PYBIND11_MODULE(wideband_sdr, m) {
    m.doc() = "GNU Radio Wideband SDR Python bindings";

    // Create the Python module
    py::module m_source = m.def_submodule("source", "Wideband SDR source block");

    // Define the source block class
    py::class_<gr::wideband_sdr::source, gr::wideband_sdr::source::sptr>(m_source, "source")
        .def("set_frequency", &gr::wideband_sdr::source::set_frequency, 
             "Set the center frequency (Hz)")
        .def("get_frequency", &gr::wideband_sdr::source::get_frequency,
             "Get the current frequency (Hz)")
        .def("set_sample_rate", &gr::wideband_sdr::source::set_sample_rate,
             "Set the sample rate (Hz)")
        .def("get_sample_rate", &gr::wideband_sdr::source::get_sample_rate,
             "Get the current sample rate (Hz)")
        .def("set_gain", &gr::wideband_sdr::source::set_gain,
             "Set the RF gain (dB)")
        .def("get_gain", &gr::wideband_sdr::source::get_gain,
             "Get the current gain (dB)")
        .def("start_streaming", &gr::wideband_sdr::source::start_streaming,
             "Start SDR device streaming")
        .def("stop_streaming", &gr::wideband_sdr::source::stop_streaming,
             "Stop SDR device streaming")
        .def("is_streaming", &gr::wideband_sdr::source::is_streaming,
             "Check if device is streaming")
        .def("get_device_info", &gr::wideband_sdr::source::get_device_info,
             "Get device information")
        .def("get_statistics", &gr::wideband_sdr::source::get_statistics,
             "Get streaming statistics");

    // Factory function
    m_source.def("make", &gr::wideband_sdr::source::make,
                 "Create a Wideband SDR source block",
                 py::arg("sample_rate") = 1e6,
                 py::arg("frequency") = 100e6,
                 py::arg("gain") = 30);

    // Convenience function for easy block creation
    m.def("source", &gr::wideband_sdr::source::make,
          "Create a Wideband SDR source block",
          py::arg("sample_rate") = 1e6,
          py::arg("frequency") = 100e6,
          py::arg("gain") = 30);

    // Module constants
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "Wideband SDR Project";
    m.attr("__email__") = "darsh.gupta@example.com";
}
