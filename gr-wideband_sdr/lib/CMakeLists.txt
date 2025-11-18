include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

# List the library components
list(APPEND libwideband_sdr_sources
    source_impl.cc
    source_impl.h
    sink_impl.cc
    sink_impl.h
)

# Create the shared library
add_library(gnuradio-wideband_sdr SHARED ${libwideband_sdr_sources})

# Set library properties
set_target_properties(gnuradio-wideband_sdr PROPERTIES DEFINE_SYMBOL "gnuradio_wideband_sdr_EXPORTS")

# Add dependencies
target_link_libraries(gnuradio-wideband_sdr
    gnuradio::gnuradio-core
    gnuradio::gnuradio-runtime
    gnuradio::gnuradio-blocks
    ${Boost_LIBRARIES}
)

# Include directories for the library
target_include_directories(gnuradio-wideband_sdr
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC ${Gnuradio_INCLUDE_DIRS}
)

# Install library
install(TARGETS gnuradio-wideband_sdr
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
