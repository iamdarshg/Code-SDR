cmake_minimum_required(VERSION 3.8.2)
project(gr-wideband_sdr VERSION 1.0.0 LANGUAGES CXX C)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic handling of Qt5
if (CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Allow testing and coverage
enable_testing()

# Find Boost
find_package(Boost 1.53 REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
  message(STATUS "Boost not found. Building without tests.")
endif()

# Find GNU Radio
find_package(Gnuradio 3.8 REQUIRED)
if(NOT Gnuradio_FOUND)
  message(FATAL_ERROR "GNU Radio core not found. Please install GNU Radio.")
endif()

# Find PyBind11
find_package(pybind11 REQUIRED)

# Add pybind11 for Python bindings
set(pybind11_FOUND TRUE)
include_directories(${pybind11_INCLUDE_DIRS})

# Find Python
find_package(PythonLibs REQUIRED)
if(NOT PythonLibs_FOUND)
  message(FATAL_ERROR "Python development headers not found. Please install python3-dev.")
endif()

# Find NumPy (for Python bindings)
find_package(NumPy REQUIRED)

# Set up library
add_subdirectory(lib)
add_subdirectory(python)
add_subdirectory(grc)
add_subdirectory(apps)
add_subdirectory(docs)

# Install GRC XML files
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/grc/wideband_sdr_source.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/grc/wideband_sdr_sink.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/grc/wideband_sdr_tuner.xml
    DESTINATION share/gnuradio/grc/blocks
)

# Create configuration headers
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gnuradio/wideband_sdr/api.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/gnuradio/wideband_sdr/api.h
)

# Install configuration headers
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/include/gnuradio/wideband_sdr/api.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gnuradio/wideband_sdr/source.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gnuradio/wideband_sdr/sink.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gnuradio/wideband_sdr/tuner.h
    DESTINATION include/gnuradio/wideband_sdr
)

# Package information
set(CPACK_PACKAGE_NAME "gr-wideband-sdr")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GNU Radio Out Of Tree Module for Wideband SDR")
set(CPACK_PACKAGE_CONTACT "darsh.gupta@example.com")
set(CPACK_PACKAGE_VENDOR "Wideband SDR Project")

include(CPack)
message(STATUS "GNU Radio Wideband SDR OOT Module Build Configuration Complete")
