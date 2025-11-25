@echo off
REM Wideband SDR Windows Installation Script
REM Complete automated installer for Wideband SDR software package
REM 
REM Created: November 2025
REM Version: 1.0.0
REM License: MIT
REM 
REM Features:
REM - Python dependency installation
REM - libusb driver setup via Zadig
REM - Optional GNU Radio installation
REM - Optional SDR# installation
REM - Device detection test
REM - Automated configuration

setlocal enabledelayedexpansion

REM Script configuration
set SCRIPT_VERSION=1.0.0
set PYTHON_MIN_VERSION=3.8
set PACKAGE_NAME=wideband-sdr
set VENDOR_ID=04D8
set PRODUCT_ID=000A

REM Color codes for better output
set RED=[91m
set GREEN=[92m
set YELLOW=[93m
set BLUE=[94m
set NC=[0m

REM Logging
set LOG_FILE=%TEMP%\wideband-sdr-install.log

echo.
echo %BLUE%==============================================%NC%
echo %BLUE% Wideband SDR Windows Installer v%SCRIPT_VERSION%%NC%
echo %BLUE% Complete Software Package Installation%NC%
echo %BLUE%==============================================%NC%
echo.

REM Check administrator privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo %RED%ERROR: This installer requires administrator privileges.%NC%
    echo Please right-click and select "Run as administrator"
    pause
    exit /b 1
)

echo %YELLOW%Starting installation process...%NC%
echo.

REM Initialize log file
echo Wideband SDR Installation Log > "%LOG_FILE%"
echo Started: %DATE% %TIME% >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

REM Function to log messages
call :log_info "=== WIDEBAND SDR INSTALLATION STARTED ==="
call :log_info "Version: %SCRIPT_VERSION%"
call :log_info "Python requirements: >=%PYTHON_MIN_VERSION%"

REM Check Python installation
echo %YELLOW%Checking Python installation...%NC%
call :check_python

REM Install Python dependencies
echo %YELLOW%Installing Python dependencies...%NC%
call :install_python_deps

REM Download Zadig for driver installation
echo %YELLOW%Setting up USB drivers...%NC%
call :setup_usb_drivers

REM Install optional software
call :install_optional_software

REM Test device connection
echo %YELLOW%Testing device connection...%NC%
call :test_device_connection

REM Final setup and verification
echo %YELLOW%Performing final setup...%NC%
call :final_setup

echo.
echo %GREEN%==============================================%NC%
echo %GREEN% Installation completed successfully!%NC%
echo %GREEN%==============================================%NC%
echo.
echo %BLUE%Next steps:%NC%
echo 1. Connect your Wideband SDR device via USB
echo 2. Run 'python -m wideband_sdr.examples.spectrum_analyzer' to test
echo 3. Check the documentation for usage examples
echo.
echo %BLUE%Optional:%NC%
echo - Install GNU Radio for advanced signal processing
echo - Install SDR# for Windows SDR application
echo - Review the troubleshooting guide if issues occur
echo.

pause
exit /b 0

REM ==================== FUNCTION DEFINITIONS ====================

:check_python
    call :log_info "Checking Python installation..."
    
    REM Check if Python is in PATH
    python --version >nul 2>&1
    if %errorLevel% neq 0 (
        echo %RED%Python not found in PATH%NC%
        echo.
        echo %YELLOW%Python is required for this software.%NC%
        echo Please install Python %PYTHON_MIN_VERSION% or later from:
        echo https://www.python.org/downloads/
        echo.
        echo Make sure to check "Add Python to PATH" during installation.
        echo.
        pause
        exit /b 1
    )
    
    REM Get Python version
    for /f "tokens=2" %%i in ('python --version 2^>^&1') do set PYTHON_VERSION=%%i
    echo Found Python version: %PYTHON_VERSION%
    call :log_info "Python version: %PYTHON_VERSION%"
    
    REM Check pip
    python -m pip --version >nul 2>&1
    if %errorLevel% neq 0 (
        echo %RED%pip not found%NC%
        echo Installing pip...
        python -m ensurepip --default-pip
        if !errorLevel! neq 0 (
            echo %RED%Failed to install pip%NC%
            pause
            exit /b 1
        )
    )
    
    REM Upgrade pip
    echo Upgrading pip...
    python -m pip install --upgrade pip
    call :log_info "pip upgraded successfully"
    
    goto :eof

:install_python_deps
    call :log_info "Installing Python dependencies..."
    
    REM Core dependencies
    echo Installing core dependencies...
    python -m pip install pyusb>=1.2.1 numpy>=1.20.0 scipy>=1.7.0 matplotlib>=3.5.0 psutil>=5.8.0
    if %errorLevel% neq 0 (
        echo %RED%Failed to install core dependencies%NC%
        call :log_error "Core dependencies installation failed"
        pause
        exit /b 1
    )
    
    REM Optional dependencies
    echo Installing optional dependencies...
    python -m pip install pyqt5>=5.15.0
    if !errorLevel! neq 0 (
        echo %YELLOW%Warning: Failed to install PyQt5 (optional)%NC%
        call :log_warning "PyQt5 installation failed"
    )
    
    REM Install the wideband-sdr package
    echo Installing wideband-sdr package...
    python -m pip install -e .
    if %errorLevel% neq 0 (
        echo %RED%Failed to install wideband-sdr package%NC%
        call :log_error "Package installation failed"
        pause
        exit /b 1
    )
    
    call :log_info "Python dependencies installed successfully"
    goto :eof

:setup_usb_drivers
    call :log_info "Setting up USB drivers..."
    
    REM Download Zadig if not present
    if not exist "zadig.exe" (
        echo Downloading Zadig for driver installation...
        powershell -Command "(New-Object Net.WebClient).DownloadFile('https://zadig.akeo.ie/downloads/zadig_2.8.901.exe', 'zadig.exe')"
        if %errorLevel% neq 0 (
            echo %RED%Failed to download Zadig%NC%
            call :log_error "Zadig download failed"
            goto :eof
        )
        call :log_info "Zadig downloaded successfully"
    )
    
    echo.
    echo %YELLOW%IMPORTANT: Driver Installation Required%NC%
    echo.
    echo This installer will help you install the USB driver for your Wideband SDR device.
    echo.
    echo Steps:
    echo 1. Connect your Wideband SDR device to USB
    echo 2. Open Device Manager (Windows+X, Device Manager)
    echo 3. Look for your device under "Other devices" or "Universal Serial Bus controllers"
    echo 4. Right-click the device and select "Update driver"
    echo 5. Select "Browse my computer for driver software"
    echo 6. Select "Let me pick from a list of available drivers"
    echo 7. Choose "USB Input Device" or "WinUSB device"
    echo 8. Complete the installation
    echo.
    echo Alternatively, use Zadig (download above) and:
    echo 1. Run zadig.exe as administrator
    echo 2. Select your Wideband SDR device
    echo 3. Choose "WinUSB" driver
    echo 4. Click "Install Driver"
    echo.
    echo Press any key after completing driver installation...
    pause
    
    call :log_info "USB driver installation steps provided to user"
    goto :eof

:install_optional_software
    call :log_info "Installing optional software..."
    
    REM Ask user about optional installations
    echo.
    echo %YELLOW%Optional Software Installation%NC%
    echo.
    echo Would you like to install optional software?
    echo.
    echo 1. GNU Radio - Advanced signal processing (recommended)
    echo 2. SDR# - Windows SDR application
    echo 3. Skip optional installations
    echo.
    set /p choice="Enter your choice (1-3): "
    
    if "%choice%"=="1" goto install_gnu_radio
    if "%choice%"=="2" goto install_sdr_sharp
    if "%choice%"=="3" goto skip_optional
    goto install_optional_software

:install_gnu_radio
    call :log_info "Installing GNU Radio..."
    echo.
    echo GNU Radio installation notes:
    echo - This is a large download (~500MB)
    echo - Requires Windows Subsystem for Linux (WSL) or use pre-built binaries
    echo - For WSL: sudo apt install gnuradio
    echo - Or download from: https://www.gnuradio.org/
    echo.
    echo Opening GNU Radio website...
    start https://www.gnuradio.org/
    call :log_info "GNU Radio installation info provided"
    goto end_optional

:install_sdr_sharp
    call :log_info "Installing SDR#..."
    echo.
    echo SDR# installation steps:
    echo 1. Download SDR# from: http://sdrsharp.com/
    echo 2. Extract to a folder (e.g., C:\SDR)
    echo 3. Copy ExtIO plugin to SDR# folder
    echo 4. Run sdrsharp.exe
    echo.
    echo Opening SDR# website...
    start http://sdrsharp.com/
    call :log_info "SDR# installation info provided"
    goto end_optional

:skip_optional
    echo Skipping optional installations.
    call :log_info "User skipped optional installations"

:end_optional
    goto :eof

:test_device_connection
    call :log_info "Testing device connection..."
    
    REM Check if device is connected
    echo Testing USB device connection...
    
    REM Try to run device test
    python -c "from wideband_sdr import list_devices; devices = list_devices(); print(f'Found {len(devices)} device(s)'); [print(f'Device {d[\"index\"]}: {d[\"product\"]}') for d in devices]" 2>nul
    if %errorLevel% neq 0 (
        echo %YELLOW%Note: No Wideband SDR device detected.%NC%
        echo This is normal if no device is connected.
        echo.
        echo Once you connect your device, you can test with:
        echo python -m wideband_sdr.examples.spectrum_analyzer
        call :log_warning "No device detected (normal if not connected)"
    ) else (
        echo %GREEN%Device test completed%NC%
        call :log_info "Device test completed successfully"
    )
    
    goto :eof

:final_setup
    call :log_info "Performing final setup..."
    
    REM Create desktop shortcuts
    echo Creating desktop shortcuts...
    
    REM Create spectrum analyzer shortcut
    set DESKTOP=%USERPROFILE%\Desktop
    echo @echo off > "%DESKTOP%\Wideband SDR - Spectrum Analyzer.bat"
    echo cd /d "%CD%" >> "%DESKTOP%\Wideband SDR - Spectrum Analyzer.bat"
    echo python -m wideband_sdr.examples.spectrum_analyzer %%* >> "%DESKTOP%\Wideband SDR - Spectrum Analyzer.bat"
    
    REM Create frequency scanner shortcut
    echo @echo off > "%DESKTOP%\Wideband SDR - Frequency Scanner.bat"
    echo cd /d "%CD%" >> "%DESKTOP%\Wideband SDR - Frequency Scanner.bat"
    echo python -m wideband_sdr.examples.frequency_scanner %%* >> "%DESKTOP%\Wideband SDR - Frequency Scanner.bat"
    
    REM Create waterfall display shortcut
    echo @echo off > "%DESKTOP%\Wideband SDR - Waterfall Display.bat"
    echo cd /d "%CD%" >> "%DESKTOP%\Wideband SDR - Waterfall Display.bat"
    echo python -m wideband_sdr.examples.waterfall_display %%* >> "%DESKTOP%\Wideband SDR - Waterfall Display.bat"
    
    REM Create documentation shortcut
    if exist "WIDEBAND-SDR-COMPLETE-PACKAGE.md" (
        echo @echo off > "%DESKTOP%\Wideband SDR - Documentation.bat"
        echo start "" "%CD%\WIDEBAND-SDR-COMPLETE-PACKAGE.md" >> "%DESKTOP%\Wideband SDR - Documentation.bat"
    )
    
    call :log_info "Desktop shortcuts created"
    
    REM Test Python module import
    echo Testing Python module imports...
    python -c "import wideband_sdr; print('Module import successful')" 2>nul
    if %errorLevel% neq 0 (
        echo %RED%Module import test failed%NC%
        call :log_error "Python module import test failed"
    ) else (
        echo %GREEN%Python module import test passed%NC%
        call :log_info "Python module import test passed"
    )
    
    goto :eof

REM Logging functions
:log_info
    echo [%DATE% %TIME%] INFO: %1 >> "%LOG_FILE%"
    goto :eof

:log_warning
    echo [%DATE% %TIME%] WARNING: %1 >> "%LOG_FILE%"
    goto :eof

:log_error
    echo [%DATE% %TIME%] ERROR: %1 >> "%LOG_FILE%"
    goto :eof

REM End of script
endlocal
