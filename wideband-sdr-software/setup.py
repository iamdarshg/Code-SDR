#!/usr/bin/env python3
"""
Wideband SDR Software Package Setup
Complete software package setup for the 1 MHz - 10 GHz Wideband SDR

Created: November 2025
Version: 1.0.0
License: MIT
"""

from setuptools import setup, find_packages
from pathlib import Path
import os

# Get version from version.py
def get_version():
    """Get version from version module"""
    version_file = Path(__file__).parent / "version.py"
    if version_file.exists():
        exec(open(version_file).read())
        return locals()['__version__']
    return "1.0.0"

# Get long description from README
def get_long_description():
    """Get long description from README file"""
    readme_file = Path(__file__).parent.parent / "WIDEBAND-SDR-COMPLETE-PACKAGE.md"
    if readme_file.exists():
        with open(readme_file, encoding='utf-8') as f:
            return f.read()
    return "Wideband SDR - 1 MHz to 10 GHz Software-Defined Radio"

# Package requirements
REQUIREMENTS = [
    "pyusb>=1.2.1",
    "numpy>=1.20.0",
    "scipy>=1.7.0",
    "matplotlib>=3.5.0",
    "psutil>=5.8.0",
]

# Optional dependencies for different features
OPTIONAL_REQUIREMENTS = {
    'examples': [
        "scipy>=1.7.0",
        "matplotlib>=3.5.0",
        "pyqt5>=5.15.0",  # For GUI examples
    ],
    'gnu_radio': [
        "gnuradio>=3.10.0",
        "gnuradio-companion",
    ],
    'sdr_sharp': [
        "pywin32; platform_system=='Windows'",
    ],
    'all': [
        "scipy>=1.7.0",
        "matplotlib>=3.5.0",
        "pyqt5>=5.15.0",
        "gnuradio>=3.10.0",
        "pywin32; platform_system=='Windows'",
    ]
}

# Development requirements
DEV_REQUIREMENTS = [
    "pytest>=6.2.0",
    "pytest-cov>=2.12.0",
    "black>=21.0.0",
    "flake8>=3.9.0",
    "mypy>=0.910",
    "sphinx>=4.0.0",
    "sphinx-rtd-theme>=0.5.0",
]

# Package information
PACKAGE_NAME = "wideband_sdr"
VERSION = get_version()
DESCRIPTION = "Python driver and software package for Wideband SDR (1 MHz - 10 GHz)"
LONG_DESCRIPTION = get_long_description()
LONG_DESCRIPTION_CONTENT_TYPE = "text/markdown"
AUTHOR = "Wideband SDR Project"
AUTHOR_EMAIL = "support@widebandsdr.org"
URL = "https://github.com/widebandsdr/wideband-sdr"
DOWNLOAD_URL = "https://github.com/widebandsdr/wideband-sdr/archive/v{}.tar.gz".format(VERSION)
PROJECT_URLS = {
    "Bug Tracker": "https://github.com/widebandsdr/wideband-sdr/issues",
    "Documentation": "https://widebandsdr.readthedocs.io/",
    "Source Code": "https://github.com/widebandsdr/wideband-sdr",
}
LICENSE = "MIT"
CLASSIFIERS = [
    "Development Status :: 5 - Production/Stable",
    "Intended Audience :: Science/Research",
    "Intended Audience :: Telecommunications Industry",
    "License :: OSI Approved :: MIT License",
    "Operating System :: OS Independent",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Topic :: Scientific/Engineering",
    "Topic :: Scientific/Engineering :: Physics",
    "Topic :: Communications",
    "Topic :: Multimedia :: Graphics :: Viewers :: Scientific/Medical",
    "Topic :: Software Development :: Libraries :: Python Modules",
]

# Platform-specific settings
PLATFORM_LINUX = "linux"
PLATFORM_WINDOWS = "win32"
PLATFORM_MACOS = "darwin"

# Command class for building firmware
class BuildFirmwareCommand:
    """Custom command to build firmware"""
    
    description = "Build firmware for dsPIC33AK256MC505"
    user_options = []
    
    def initialize_options(self):
        """Initialize options"""
        pass
    
    def finalize_options(self):
        """Finalize options"""
        pass
    
    def run(self):
        """Run firmware build"""
        import subprocess
        import sys
        import os
        
        firmware_dir = Path(__file__).parent.parent / "wideband-sdr-firmware"
        if firmware_dir.exists():
            print("Building firmware...")
            os.chdir(firmware_dir)
            try:
                subprocess.check_call(["make", "all"])
                print("Firmware build completed successfully!")
            except subprocess.CalledProcessError as e:
                print(f"Firmware build failed: {e}")
                sys.exit(1)
        else:
            print("Firmware directory not found, skipping firmware build")

# Command class for copying firmware
class CopyFirmwareCommand:
    """Custom command to copy firmware to package"""
    
    description = "Copy built firmware to package directory"
    user_options = []
    
    def initialize_options(self):
        """Initialize options"""
        pass
    
    def finalize_options(self):
        """Finalize options"""
        pass
    
    def run(self):
        """Copy firmware files"""
        import shutil
        from pathlib import Path
        
        source_dir = Path(__file__).parent.parent / "wideband-sdr-firmware" / "dist"
        dest_dir = Path(__file__).parent / "firmware"
        
        if source_dir.exists():
            print("Copying firmware to package...")
            if dest_dir.exists():
                shutil.rmtree(dest_dir)
            shutil.copytree(source_dir, dest_dir)
            print("Firmware copied successfully!")
        else:
            print("Firmware not found, skipping copy")

# Setup configuration
setup(
    name=PACKAGE_NAME,
    version=VERSION,
    description=DESCRIPTION,
    long_description=LONG_DESCRIPTION,
    long_description_content_type=LONG_DESCRIPTION_CONTENT_TYPE,
    author=AUTHOR,
    author_email=AUTHOR_EMAIL,
    url=URL,
    download_url=DOWNLOAD_URL,
    project_urls=PROJECT_URLS,
    license=LICENSE,
    classifiers=CLASSIFIERS,
    
    # Package structure
    packages=find_packages(),
    py_modules=["wideband_sdr"],
    
    # Package data
    package_data={
        "": ["*.md", "*.txt", "*.py", "*.c", "*.h"],
        "examples": ["*.py"],
        "firmware": ["*.hex", "*.bin"],
    },
    
    # Data files
    data_files=[
        ("share/doc/wideband-sdr", [
            "WIDEBAND-SDR-COMPLETE-PACKAGE.md",
            "LICENSE",
        ]),
        ("lib/systemd/system", [
            "wideband-sdr.service",
        ]),
        ("lib/udev/rules.d", [
            "99-wideband-sdr.rules",
        ]),
        ("bin", [
            "wideband-sdr-cli",
            "wideband-sdr-install",
        ]),
    ],
    
    # Entry points
    entry_points={
        "console_scripts": [
            "wideband-sdr=wideband_sdr.cli:main",
            "wideband-sdr-test=wideband_sdr.cli:test",
            "wideband-sdr-install=wideband_sdr.cli:install",
        ],
        "gui_scripts": [
            "wideband-sdr-gui=wideband_sdr.gui:main",
        ],
    },
    
    # Requirements
    install_requires=REQUIREMENTS,
    extras_require=OPTIONAL_REQUIREMENTS,
    python_requires=">=3.8",
    
    # Keywords
    keywords=[
        "sdr", "software-defined-radio", "rf", "radio", "wideband", 
        "frequency", "spectrum", "gnuradio", "rtl-sdr", "hackrf", 
        "microwave", "vhf", "uhf", "shf", "millimeter-wave"
    ],
    
    # Platforms
    platforms=["any"],
    
    # Command classes
    cmdclass={
        "build_firmware": BuildFirmwareCommand,
        "copy_firmware": CopyFirmwareCommand,
    },
    
    # Include directories
    include_package_data=True,
    
    # Zip safe
    zip_safe=False,
)

# Additional setup tasks
if __name__ == "__main__":
    # Custom commands can be run like:
    # python setup.py build_firmware
    # python setup.py copy_firmware
    # python setup.py sdist bdist_wheel
    
    print(f"Wideband SDR Package Setup v{VERSION}")
    print(f"Package name: {PACKAGE_NAME}")
    print(f"Python version: {os.sys.version}")
    print("\nAvailable commands:")
    print("  build_firmware  - Build firmware for dsPIC33AK256MC505")
    print("  copy_firmware   - Copy firmware to package directory")
    print("  sdist          - Create source distribution")
    print("  bdist_wheel    - Create wheel distribution")
    print("  install        - Install package")
    print("  develop        - Install in development mode")
    
    print("\nTo install with all optional dependencies:")
    print(f"  pip install {PACKAGE_NAME}[all]")
    
    print("\nTo install with specific features:")
    for feature in OPTIONAL_REQUIREMENTS:
        print(f"  pip install {PACKAGE_NAME}[{feature}]")
