@echo off
REM run_all_tests.bat - Code-SDR FPGA Test Runner
REM Tests all modules using cocotb and/or Icarus Verilog (iverilog)
REM Generates concise, readable output

setlocal enabledelayedexpansion

cd /d "%~dp0\..\.."

REM Initialize counters
set /a PASSED=0
set /a FAILED=0
set /a TIMEOUTS=0
set /a TOTAL_MODULES=0

REM Test timeout in seconds
set /a SIM_TIMEOUT=10

REM Module list (all with available testbenches)
set "MODULES=adc_interface nco_generator cic_decimator async_fifo ethernet_mac fft_processor hamming_window rp2040_interface udp_ip_stack adaptive_gain_scaler average_power_detector digital_downconverter dma_engine"
for %%M in (%MODULES%) do set /a TOTAL_MODULES+=1

echo.
echo ========================================
echo      Code-SDR FPGA Test Suite
echo ========================================
echo.
echo Testing %MODULES%
echo Timeout: %SIM_TIMEOUT% seconds
echo.

for %%M in (%MODULES%) do (
    echo =======================================
    echo Testing Module: %%M
    echo =======================================

    set "TEST_RESULT=UNKNOWN"

    REM Check if cocotb test exists
    if exist "tests\cocotb_tests\test_%%M.py" (
        echo Running cocotb test...
        python -m pytest tests\cocotb_tests\test_%%M.py -v --tb=no > tests\sim_output\%%M_cocotb.log 2^>^&1
        if !errorlevel! == 0 (
            echo   cocotb: PASS
            set "TEST_RESULT=PASS"
        ) else (
            echo   cocotb: FAIL
            set "TEST_RESULT=FAIL"
        )
    ) else (
        echo   cocotb: NOT AVAILABLE
    )

    REM Check if iverilog test exists
    if exist "verilog\%%M.v" if exist "verilog\%%M_tb.v" (
        echo Running iverilog test...

        REM Compile
        iverilog -g2012 -I verilog -o "tests\sim_output\%%M.vvp" -s %%M_tb verilog\*.v > "tests\sim_output\%%M_compile.log" 2>&1
        if exist "tests\sim_output\%%M.vvp" (
            echo   compilation: OK

            REM Run with timeout
            start /B vvp tests\sim_output\%%M.vvp > tests\sim_output\%%M_sim.log 2>&1
            timeout /t %SIM_TIMEOUT% /nobreak > nul
            taskkill /f /im vvp.exe > nul 2>&1

            REM Check for finish signal
            findstr "$finish called" tests\sim_output\%%M_sim.log > nul 2>&1
            if !errorlevel! == 0 (
                echo   iverilog: PASS
                if "!TEST_RESULT!"=="UNKNOWN" set "TEST_RESULT=PASS"
            ) else (
                echo   iverilog: TIMEOUT
                set "TEST_RESULT=FAIL"
                set /a TIMEOUTS+=1
            )
        ) else (
            echo   iverilog: COMPILE FAIL
            set "TEST_RESULT=FAIL"
        )
    ) else (
        echo   iverilog: NOT AVAILABLE
    )

    REM Count results
    if "!TEST_RESULT!"=="PASS" (
        set /a PASSED+=1
    ) else if "!TEST_RESULT!"=="FAIL" (
        set /a FAILED+=1
    )

    echo.
)

echo ========================================
echo              SUMMARY
echo ========================================
echo.
echo Modules tested: %PASSED%/%TOTAL_MODULES%
echo Failures: %FAILED%
echo Timeouts: %TIMEOUTS%
echo.
if %FAILED% EQU 0 if %TIMEOUTS% EQU 0 (
    echo OVERALL RESULT: ALL TESTS PASSED
) else (
    echo OVERALL RESULT: SOME TESTS FAILED
)
echo ========================================

exit
