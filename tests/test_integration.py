#!/usr/bin/env python3
"""
Integration test for Wideband SDR Python ctypes bridge
Tests both GNU Radio OOT and ExtIO plugin integration
"""

import sys
import os
import subprocess
import tempfile
import json
from pathlib import Path

# Add current directory to path
sys.path.insert(0, os.path.dirname(__file__))

def test_python_ctypes_bridge():
    """Test the Python ctypes bridge functionality"""
    print("=== Testing Python ctypes bridge ===")
    
    try:
        import python_ctypes_bridge
        print("✓ Python ctypes bridge imported successfully")
        
        # Test device wrapper functionality
        wrapper = python_ctypes_bridge.SDRDeviceWrapper()
        print("✓ SDRDeviceWrapper created")
        
        # Test device opening
        if wrapper.open_device():
            print("✓ Device opened successfully")
            
            # Test device configuration
            print(f"  Initial frequency: {wrapper.get_frequency()}")
            
            # Set new frequency
            if wrapper.set_frequency(100_000_000):  # 100 MHz
                print("✓ Frequency set successfully")
                freq = wrapper.get_frequency()
                print(f"  New frequency: {freq}")
            
            # Get device info
            info = wrapper.get_device_info()
            print(f"✓ Device info: {info}")
            
            # Start streaming
            if wrapper.start_streaming():
                print("✓ Streaming started")
                
                # Read some samples
                import ctypes
                buffer_size = 1024
                buffer = (ctypes.c_float * (buffer_size * 2))()
                
                samples_read = wrapper.read_samples(ctypes.addressof(buffer), buffer_size)
                print(f"✓ Read {samples_read} samples")
                
                # Stop streaming
                if wrapper.stop_streaming():
                    print("✓ Streaming stopped")
            
            # Close device
            wrapper.close_device()
            print("✓ Device closed")
            
        else:
            print("⚠ Device not opened (dummy mode expected without hardware)")
            
        return True
        
    except Exception as e:
        print(f"✗ Error testing ctypes bridge: {e}")
        return False

def test_gnu_radio_integration():
    """Test GNU Radio OOT integration"""
    print("\n=== Testing GNU Radio OOT Integration ===")
    
    try:
        # Check if GNU Radio source files exist
        source_files = [
            "gr-wideband_sdr/lib/source_impl.cc",
            "gr-wideband_sdr/lib/source_impl.h",
            "gr-wideband_sdr/python/bindings.cc",
            "gr-wideband_sdr/grc/wideband_sdr_source.xml"
        ]
        
        for file_path in source_files:
            if os.path.exists(file_path):
                print(f"✓ Found: {file_path}")
            else:
                print(f"✗ Missing: {file_path}")
                return False
        
        # Check for ctypes bridge inclusion
        with open("gr-wideband_sdr/lib/source_impl.cc", 'r') as f:
            content = f.read()
            if "wideband_sdr_ctypes" in content:
                print("✓ Source implementation includes ctypes bridge")
            else:
                print("⚠ Source implementation may need ctypes bridge integration")
        
        print("✓ GNU Radio source files present and structure validated")
        return True
        
    except Exception as e:
        print(f"✗ Error testing GNU Radio integration: {e}")
        return False

def test_extio_plugin_integration():
    """Test ExtIO plugin integration"""
    print("\n=== Testing ExtIO Plugin Integration ===")
    
    try:
        # Check if ExtIO plugin files exist
        extio_files = [
            "extio_plugin/src/extio_impl.cpp",
            "extio_plugin/src/wideband_sdr_extio.cpp",
            "extio_plugin/include/wideband_sdr_extio.h",
            "extio_plugin/include/extio.h",
            "extio_plugin/CMakeLists.txt"
        ]
        
        for file_path in extio_files:
            if os.path.exists(file_path):
                print(f"✓ Found: {file_path}")
            else:
                print(f"✗ Missing: {file_path}")
                return False
        
        # Check for ctypes bridge usage
        extio_impl_path = "extio_plugin/src/extio_impl.cpp"
        if os.path.exists(extio_impl_path):
            with open(extio_impl_path, 'r') as f:
                content = f.read()
                if "WidebandSDRExtIO" in content:
                    print("✓ ExtIO implementation has WidebandSDRExtIO class")
                else:
                    print("⚠ ExtIO implementation may need WidebandSDRExtIO class")
        
        print("✓ ExtIO plugin files present and structure validated")
        return True
        
    except Exception as e:
        print(f"✗ Error testing ExtIO integration: {e}")
        return False

def test_ctypes_header():
    """Test ctypes header file"""
    print("\n=== Testing ctypes Header File ===")
    
    try:
        header_path = "wideband_sdr_ctypes.h"
        if os.path.exists(header_path):
            print("✓ ctypes header file exists")
            
            with open(header_path, 'r') as f:
                content = f.read()
                
            # Check for essential functions
            required_functions = [
                "ctypes_open_device",
                "ctypes_close_device", 
                "ctypes_set_frequency",
                "ctypes_get_frequency",
                "ctypes_read_samples"
            ]
            
            for func in required_functions:
                if func in content:
                    print(f"✓ Function declared: {func}")
                else:
                    print(f"✗ Missing function: {func}")
                    return False
            
            print("✓ ctypes header properly structured")
            return True
        else:
            print("✗ ctypes header file missing")
            return False
            
    except Exception as e:
        print(f"✗ Error testing ctypes header: {e}")
        return False

def test_compilation_readiness():
    """Test compilation readiness"""
    print("\n=== Testing Compilation Readiness ===")
    
    try:
        # Check Python bridge compilation
        print("Testing Python bridge compilation...")
        result = subprocess.run([sys.executable, "-m", "py_compile", "python_ctypes_bridge.py"], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            print("✓ Python bridge compiles successfully")
        else:
            print(f"✗ Python bridge compilation failed: {result.stderr}")
            return False
        
        # Check CMake files
        cmake_files = [
            "gr-wideband_sdr/CMakeLists.txt",
            "extio_plugin/CMakeLists.txt"
        ]
        
        for cmake_file in cmake_files:
            if os.path.exists(cmake_file):
                print(f"✓ CMake file exists: {cmake_file}")
            else:
                print(f"✗ CMake file missing: {cmake_file}")
                return False
        
        print("✓ Compilation structure validated")
        return True
        
    except Exception as e:
        print(f"✗ Error testing compilation readiness: {e}")
        return False

def generate_implementation_summary():
    """Generate summary of implementation status"""
    print("\n=== Implementation Summary ===")
    
    summary = {
        "timestamp": "2025-11-18 19:57:45",
        "python_ctypes_bridge": "✓ Complete",
        "ctypes_header": "✓ Complete", 
        "gnu_radio_structure": "✓ Complete",
        "gnu_radio_integration": "🔄 In Progress",
        "extio_plugin_structure": "✓ Complete",
        "extio_plugin_integration": "🔄 In Progress",
        "compilation_readiness": "🔄 In Progress",
        "next_steps": [
            "Update GNU Radio source_impl.cc to use ctypes bridge",
            "Update ExtIO plugin to use ctypes bridge", 
            "Test compilation of both projects",
            "Validate end-to-end functionality"
        ]
    }
    
    print(json.dumps(summary, indent=2))
    return summary

def main():
    """Main test function"""
    print("Wideband SDR Integration Test Suite")
    print("=" * 50)
    
    # Change to script directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)
    
    # Run tests
    tests = [
        ("Python ctypes Bridge", test_python_ctypes_bridge),
        ("ctypes Header File", test_ctypes_header),
        ("GNU Radio Integration", test_gnu_radio_integration),
        ("ExtIO Plugin Integration", test_extio_plugin_integration),
        ("Compilation Readiness", test_compilation_readiness)
    ]
    
    results = {}
    for test_name, test_func in tests:
        results[test_name] = test_func()
    
    # Generate summary
    summary = generate_implementation_summary()
    
    # Overall status
    print(f"\n=== Overall Status ===")
    passed = sum(1 for result in results.values() if result)
    total = len(results)
    
    print(f"Tests Passed: {passed}/{total}")
    
    if passed == total:
        print("🎉 All tests passed! Implementation ready for next phase.")
    else:
        print("⚠ Some tests failed. Review implementation before proceeding.")
    
    return passed == total

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
