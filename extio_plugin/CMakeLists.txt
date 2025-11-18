cmake_minimum_required(VERSION 3.15)

# Project configuration
project(WidebandSDRExtIO
    VERSION 1.0.0
    DESCRIPTION "HDSDR/SDR# ExtIO Plugin for Wideband SDR"
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific settings
if(WIN32)
    # Windows specific settings
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
    
    # Visual Studio generator
    if(MSVC)
        add_compile_options(/W4 /WX)  # Enable warnings
    endif()
else()
    # Linux/macOS settings
    add_compile_options(-Wall -Wextra -Werror)
endif()

# Find required packages
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Python module path
set(PYTHON_SITE_PACKAGES
    "${CMAKE_INSTALL_PREFIX}/Lib/site-packages"
    CACHE PATH "Python site-packages directory"
)

# Source files
set(SOURCES
    src/extio_impl.cpp
    src/wideband_sdr_extio.cpp
    src/version_resource.rc
)

# Header files
set(HEADERS
    include/extio.h
    include/wideband_sdr_extio.h
)

# Library dependencies
set(LIBRARIES
    pybind11::embed
    pybind11::embed_headers
)

# Create shared library (DLL for Windows)
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# Set library properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    SUFFIX ".dll"
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Python_INCLUDE_DIRS}
)

# Compiler definitions for ExtIO plugin
target_compile_definitions(${PROJECT_NAME} PRIVATE
    EXTIO_EXPORTS
    WIDEBAND_SDR_EXPORTS
    _CRT_SECURE_NO_WARNINGS
)

# Install targets
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Copy Python driver to build directory
install(FILES 
    ${CMAKE_SOURCE_DIR}/../wideband-sdr-software/wideband_sdr.py
    DESTINATION bin
)

# Create test executable (for development/testing)
add_executable(extio_test
    test/extio_test.cpp
)

target_link_libraries(extio_test PRIVATE ${PROJECT_NAME})

# Testing (if enabled)
option(BUILD_TESTS "Build test applications" ON)

if(BUILD_TESTS)
    enable_testing()
    
    # Add simple test
    add_test(NAME basic_functionality COMMAND extio_test)
    
    # Custom test runner for device-specific tests
    add_test(NAME device_enumeration COMMAND extio_test --test-device-enum)
endif()

# Print configuration summary
message(STATUS "========================================")
message(STATUS "Wideband SDR ExtIO Plugin Configuration")
message(STATUS "========================================")
message(STATUS "Python version: ${Python_VERSION}")
message(STATUS "Python executable: ${Python_EXECUTABLE}")
message(STATUS "Python include dir: ${Python_INCLUDE_DIRS}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "========================================")
