#!/usr/bin/env python3
"""
Setup script for GNU Radio Wideband SDR OOT Module

This script provides installation commands for the Wideband SDR GNU Radio module.
"""

from setuptools import setup, find_packages
from setuptools.command.build_py import build_py
from distutils.command.build import build
import os
import sys
import subprocess

class BuildGRModule(build):
    """Custom build command for GNU Radio module"""
    
    def run(self):
        # Check for CMake
        if not self.distribution.get_command_obj('build').cmake_build:
            return
        
        build_dir = os.path.join(os.path.dirname(__file__), 'build')
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)
        
        # Run CMake if needed
        cmake_cache = os.path.join(build_dir, 'CMakeCache.txt')
        if not os.path.exists(cmake_cache):
            print("Running CMake configuration...")
            subprocess.run([
                'cmake', '..', 
                '-DCMAKE_BUILD_TYPE=Release',
                '-DPYTHON_EXECUTABLE=' + sys.executable
            ], cwd=build_dir, check=True)
        
        # Build the module
        print("Building Wideband SDR module...")
        subprocess.run(['make', '-j'], cwd=build_dir, check=True)
        
        # Install the module
        print("Installing Wideband SDR module...")
        subprocess.run(['make', 'install'], cwd=build_dir, check=True)
        
        build.run(self)

class BuildGRPython(build_py):
    """Custom Python build command for GNU Radio module"""
    
    def run(self):
        # Build C++ components first
        if hasattr(self.distribution, 'build_gr_module'):
            self.run_command('build_gr_module')
        
        # Then build Python components
        build_py.run(self)

# Read version from the module
def get_version():
    """Get version from python module"""
    version_file = os.path.join('python', '__init__.py')
    if os.path.exists(version_file):
        with open(version_file, 'r') as f:
            for line in f:
                if line.startswith('__version__'):
                    return line.split('=')[1].strip().strip('"\'')
    return "1.0.0"

# Read long description from README
def get_long_description():
    """Get long description from README"""
    readme_file = 'README.md'
    if os.path.exists(readme_file):
        with open(readme_file, 'r', encoding='utf-8') as f:
            return f.read()
    return "GNU Radio OOT Module for Wideband SDR"

setup(
    name='gr-wideband-sdr',
    version=get_version(),
    author='Wideband SDR Project',
    author_email='darsh.gupta@example.com',
    description='GNU Radio Out Of Tree Module for Wideband SDR',
    long_description=get_long_description(),
    long_description_content_type='text/markdown',
    url='https://github.com/iamdarshg/Code-SDR',
    packages=find_packages(),
    py_modules=['wideband_sdr'],
    python_requires='>=3.8',
    install_requires=[
        'numpy>=1.19.0',
        'pybind11>=2.6.0',
        'scipy>=1.7.0',
    ],
    extras_require={
        'development': [
            'cmake>=3.8.2',
            'boost-cpp>=1.71.0',
            'pytest>=6.0.0',
            'pytest-cov>=2.10.0',
        ],
        'docs': [
            'sphinx>=3.0.0',
            'sphinx-rtd-theme>=0.5.0',
        ],
    },
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'Intended Audience :: Telecommunications Industry',
        'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
        'Operating System :: OS Independent',
        'Programming Language :: C++',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Topic :: Communications',
        'Topic :: Scientific/Engineering',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ],
    keywords='sdr, gnuradio, software defined radio, wideband, rf, signal processing',
    project_urls={
        'Bug Reports': 'https://github.com/iamdarshg/Code-SDR/issues',
        'Source': 'https://github.com/iamdarshg/Code-SDR',
        'Documentation': 'https://github.com/iamdarshg/Code-SDR/wiki',
    },
    entry_points={
        'gnuradio.blocks': [
            'wideband_sdr_source = wideband_sdr.source:make',
        ],
    },
    # Custom commands
    cmdclass={
        'build': BuildGRModule,
        'build_py': BuildGRPython,
    },
    cmake_build=True,  # Enable CMake build
    cmake_args=[
        '-DCMAKE_BUILD_TYPE=Release',
        '-DPYTHON_EXECUTABLE=' + sys.executable,
    ],
    cmake_source_dir='.',
    cmake_build_dir='build',
    cmake_install_prefix='/usr/local',
)
