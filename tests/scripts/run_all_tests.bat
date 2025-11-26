@echo off
REM run_all_tests.bat - Simple FPGA Verilog test runner for Windows

setlocal EnableDelayedExpansion

REM Change to project root directory
cd /d "%~dp0\..\.."

echo ========================================
echo   SDR FPGA Test Suite
echo ========================================
echo.

set MODULES=adc_interface nco_generator cic_decimator async_fifo ethernet_mac fft_processor hamming_window rp2040_interface udp_ip_stack adaptive_gain_scaler average_power_detector
echo Modules to test: %MODULES%
echo.
for %%M in (%MODULES%) do (
    echo Testing: %%M

    REM Check if files exist
    if exist "verilog\%%M.v" if exist "verilog\%%M_tb.v" (
        echo Here we go...
        REM Simple compile and run
        iverilog -g2012 -o "tests\sim_output\%%M.vvp" -I verilog verilog\%%M.v verilog\%%M_tb.v > tests\sim_output\%%M_compile.log
        
        if !errorLevel! EQU 0 (
            REM Run simulation with timeout
            set TIMEOUT_SEC=300
            set "TIMED_OUT=0"
            REM Start simulation in background
            start /B vvp tests\sim_output\%%M.vvp >tests\sim_output\%%M_sim.log 2>&1
            REM Wait for timeout or completion
            timeout /t !TIMEOUT_SEC! /nobreak >nul 2>&1
            REM Check if vvp was still running (kill will succeed if still running)
            taskkill /F /IM vvp.exe >nul 2>&1
            if !errorlevel! EQU 0 set "TIMED_OUT=1"
            set "SIM_EXIT=!TIMED_OUT!"

            REM Check if simulation succeeded and test result
            if !SIM_EXIT! EQU 1 goto :timeout_%%M
            REM Check for TEST PASSED
            findstr "TEST PASSED" tests\sim_output\%%M_sim.log >nul
            if !errorLevel! EQU 0 goto :pass_%%M
            REM Check for TEST FAILED
            findstr "TEST FAILED" tests\sim_output\%%M_sim.log >nul
            if !errorLevel! EQU 0 goto :fail_%%M
            REM No pass/fail found
            echo   SIMULATION ERROR: %%M (no pass/fail indication found)
            goto :end_%%M
            :pass_%%M
            echo   PASSED: %%M
            goto :end_%%M
            :fail_%%M
            echo   FAILED: %%M (test logic reports failure)
            goto :end_%%M
            :timeout_%%M
            echo   TIMEOUT: %%M (simulation exceeded !TIMEOUT_SEC! seconds and was terminated)
            :end_%%M
        ) else (
            echo   COMPILE ERROR: %%M
        )
    
    )
    echo.
)

echo Done.
