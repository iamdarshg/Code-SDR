@echo off
REM run_all_tests.bat - Code-SDR FPGA Test Runner
REM This script now simply invokes the WSL-native shell script for testing.

setlocal

REM Define the project root directory within WSL
set "WSL_PROJECT_ROOT=/mnt/d/CodeProjects/Code-SDR"
set "WSL_SCRIPT_PATH=%WSL_PROJECT_ROOT%/tests/scripts/run_all_tests.sh"

echo Running FPGA test suite via WSL shell script...
wsl -d kali-linux -u darsh -- bash -c "cd %WSL_PROJECT_ROOT% && bash %WSL_SCRIPT_PATH%"

REM Propagate the exit code from the WSL script
if %errorlevel% NEQ 0 (
    echo.
    echo ========================================
    echo         OVERALL RESULT: FAILED
    echo ========================================
    exit /b %errorlevel%
) else (
    echo.
    echo ========================================
    echo         OVERALL RESULT: PASSED
    echo ========================================
    exit /b 0
)
