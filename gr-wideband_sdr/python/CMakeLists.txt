find_package(pybind11 REQUIRED)

# List the Python bindings components
list(APPEND gr-wideband_sdr_python_files
    bindings.cc
    __init__.py
)

# Create the Python module using PyBind11
pybind11_module(wideband_sdr ${gr-wideband_sdr_python_files})

# Set module properties
set_target_properties(wideband_sdr PROPERTIES PREFIX "")
set_target_properties(wideband_sdr PROPERTIES SUFFIX ".so")

# Add dependencies to the Python module
target_link_libraries(wideband_sdr
    gnuradio-wideband_sdr
    ${PYTHON_LIBRARIES}
    ${NumPy_INCLUDE_DIRS}
)

# Include directories for the Python module
target_include_directories(wideband_sdr
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC ${Gnuradio_INCLUDE_DIRS}
    PUBLIC ${NumPy_INCLUDE_DIRS}
)

# Install Python module
install(TARGETS wideband_sdr
    DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/gnuradio
)

# Install Python files
install(FILES
    __init__.py
    bindings.cc
    DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/gnuradio
)

# Enable Python testing if pytest is available
find_package(pytest)
if(pytest_FOUND)
    enable_testing()
    add_test(NAME pybind11_unit_tests
             COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_bindings.py)
endif()

message(STATUS "Python bindings configured successfully")
