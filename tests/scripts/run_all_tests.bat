echo.

set MODULES=adc_interface nco_generator cic_decimator async_fifo ethernet_mac fft_processor hamming_window rp2040_interface udp_ip_stack adaptive_gain_scaler average_power_detector
echo Modules to test: %MODULES%
echo.

REM Initialize counters
set /a PASSED=0
set /a FAILED=0
set /a TIMEOUTS=0
set /a COMPILE_ERRORS=0
set /a SIM_ERRORS=0
set /a CRASHES=0

REM Set timeout values (in seconds)
set /a SIM_TIMEOUT=60

set TOTAL_MODULES=0

for %%M in (%MODULES%) do (
    set /a TOTAL_MODULES+=1
    echo =======================================
    echo Testing Module: %%M (Timeout: %SIM_TIMEOUT%s)
    echo =======================================

    REM Check if files exist
    if exist "verilog\%%M.v" if exist "verilog\%%M_tb.v" (
        echo Running compilation...
        REM Simple compile and run
        iverilog -g2012 -o "tests/sim_output/%%M.vvp" -I verilog verilog\%%M.v verilog\%%M_tb.v > tests/sim_output/%%M_compile.log 2>&1

        if !errorLevel! EQU 0 (
            echo Compilation successful. Starting simulation...

            REM Run simulation with timeout mechanism using simplified approach
            echo Starting simulation (timeout: %SIM_TIMEOUT% seconds)...
            REM Run vvp and then wait, kill if still running
            start /B vvp tests/sim_output/%%M.vvp > tests/sim_output/%%M_sim.log 2>&1
            REM Wait for timeout period
            timeout /t %SIM_TIMEOUT% /nobreak >nul
            REM Force kill any lingering vvp processes (always kill after timeout period)
            taskkill /f /im vvp.exe >nul 2>&1

            REM Check if simulation completed normally by looking for finish signal
            set "SIM_FINISHED=0"
            if exist tests/sim_output/%%M_sim.log (
                findstr /c:"$finish called" tests/sim_output/%%M_sim.log >nul
                if !errorlevel! EQU 0 (
                    set "SIM_FINISHED=1"
                )
            )

            REM Check for timeout
            if !SIM_FINISHED! EQU 0 (
                echo Simulation timed out after %SIM_TIMEOUT% seconds. Capturing state...
                REM Add timeout marker to log
                echo TIMEOUT: Simulation exceeded %SIM_TIMEOUT% second limit >> tests/sim_output/%%M_sim.log
                set "RESULT=FAILED"
                set "REASON=simulation timed out"
                set "ERROR_TYPE=1"
                set /a TIMEOUTS+=1
            ) else (
                REM Check for crash indicators (segmentation fault, etc.)
                findstr /c:"Segmentation fault" /c:"Access violation" /c:"Exception" tests/sim_output/%%M_sim.log >nul 2>&1
                if !errorLevel! EQU 0 (
                    echo Simulation crashed. Core dump captured in log...
                    set "RESULT=FAILED"
                    set "REASON=simulation crashed"
                    set "ERROR_TYPE=2"
                    set /a CRASHES+=1
                    REM Capture system information
                    echo CRASH_INFO: Date=!date! Time=!time! >> tests/sim_output/%%M_crash.log
                    systeminfo >> tests/sim_output/%%M_crash.log 2>&1
                ) else (
                    set "ERROR_TYPE=0"
                )
            )

            if !ERROR_TYPE! EQU 0 (
                findstr "TEST PASSED" tests/sim_output/%%M_sim.log >nul
                if !errorLevel! EQU 0 (
                    set "RESULT=PASSED"
                    set "REASON=test completed successfully"
                ) else (
                    findstr "TEST FAILED" tests/sim_output/%%M_sim.log >nul
                    if !errorLevel! EQU 0 (
                        set "RESULT=FAILED"
                        set "REASON=logic reports error"
                    ) else (
                        set "RESULT=FAILED"
                        set "REASON=no pass/fail indication found in log"
                        set /a SIM_ERRORS+=1
                    )
                )
            )

            echo   RESULT: !RESULT! (%%M !REASON!)

            if !RESULT! EQU FAILED (
                set /a FAILED+=1
                if !REASON! EQU compilation error set /a COMPILE_ERRORS+=1
            ) else (
                set /a PASSED+=1
            )
        ) else (
            echo   RESULT: FAILED (%%M compilation error)
            set /a COMPILE_ERRORS+=1
            set /a FAILED+=1
        )

    )
    echo.
)

echo ========================================
echo         TEST SUMMARY
echo.
echo Total Modules Tested: %TOTAL_MODULES%
echo.
echo PASSED:  %PASSED%
echo FAILED:  %FAILED% (including timeouts: %TIMEOUTS%, crashes: %CRASHES%, compile errors: %COMPILE_ERRORS%, sim errors: %SIM_ERRORS%)
echo.
if %FAILED% EQU 0 (
    if %COMPILE_ERRORS% EQU 0 (
        if %TIMEOUTS% EQU 0 (
            if %CRASHES% EQU 0 (
                if %SIM_ERRORS% EQU 0 (
                    echo OVERALL RESULT: ALL TESTS PASSED!
                )
            )
        )
    )
)
if %FAILED% GTR 0 (
    echo OVERALL RESULT: SOME TESTS FAILED - Check logs for details
)
if %COMPILE_ERRORS% GTR 0 (
    echo OVERALL RESULT: COMPILATION ERRORS - Check compile logs
)
if %TIMEOUTS% GTR 0 (
    echo OVERALL RESULT: TIMEOUTS DETECTED - Check sim logs for state dumps
)
if %CRASHES% GTR 0 (
    echo OVERALL RESULT: CRASHES DETECTED - Check crash logs for details
)
echo.
echo ========================================
echo Test suite completed.
@echo off
REM run_all_tests.bat - FPGA Verilog test runner with timeout and error detection
REM Enhanced version with timeout protection and comprehensive error reporting

cd /d "%~dp0\..\.."

echo ========================================
echo   SDR FPGA Test Suite (Enhanced)
echo ========================================
echo.

REM Test modules in order
set MODULES=adc_interface nco_generator cic_decimator async_fifo ethernet_mac fft_processor hamming_window rp2040_interface udp_ip_stack adaptive_gain_scaler average_power_detector
echo Testing modules: %MODULES%
echo.

set /a PASSED=0
set /a FAILED=0
set /a TIMEOUTS=0
set /a CRASHES=0
set /a COMPILE_ERRORS=0

for %%M in (%MODULES%) do (
    echo =======================================
    echo Testing Module: %%M
    echo =======================================

    REM Check files exist
    if exist "verilog\%%M.v" if exist "verilog\%%M_tb.v" (
        echo Compiling %%M...

        REM Compile the module
        iverilog -g2012 -o "tests/sim_output/%%M.vvp" -I verilog verilog\%%M.v verilog\%%M_tb.v 2^>nul ^>nul
        if errorlevel 1 (
            echo   RESULT: FAILED - Compilation error
            set /a COMPILE_ERRORS+=1
            set /a FAILED+=1
        ) else (
            echo   Compilation successful. Running simulation...

            REM Run simulation with timeout protection
            start /B vvp "tests/sim_output/%%M.vvp" ^> "tests/sim_output/%%M_sim.log" 2^>^&1
            timeout /t 60 /nobreak ^>nul
            taskkill /f /im vvp.exe ^>nul 2^>^&1

            REM Analyze results
            if exist "tests/sim_output/%%M_sim.log" (
                REM Check for normal completion
                findstr "$finish called" "tests/sim_output/%%M_sim.log" ^>nul
                if errorlevel 1 (
                    REM Check for crashes in simulation log
                    findstr /c:"Segmentation fault" /c:"Access violation" /c:"Exception" /c:"core dumped" "tests/sim_output/%%M_sim.log" ^>nul
                    if not errorlevel 1 (
                        echo   RESULT: FAILED - Simulation crashed ^(core dump captured^)
                        echo CRASH_INFO: Module=%%M Date=%date% Time=%time% ^>^> "tests/sim_output/%%M_crash.log"
                        systeminfo ^>^> "tests/sim_output/%%M_crash.log" 2^>nul
                        set /a CRASHES+=1
                    ) else (
                        echo   RESULT: FAILED - Simulation timeout ^(60s limit exceeded^)
                        echo TIMEOUT: Module %%M exceeded 60 second simulation limit ^>^> "tests/sim_output/%%M_sim.log"
                        set /a TIMEOUTS+=1
                    )
                ) else (
                    REM Check test results
                    findstr "TEST PASSED" "tests/sim_output/%%M_sim.log" ^>nul
                    if not errorlevel 1 (
                        echo   RESULT: PASSED - Test completed successfully
                        set /a PASSED+=1
                    ) else (
                        findstr "TEST FAILED" "tests/sim_output/%%M_sim.log" ^>nul
                        if not errorlevel 1 (
                            echo   RESULT: FAILED - Logic error in test
                        ) else (
                            echo   RESULT: FAILED - No test result indication found
                        )
                        set /a FAILED+=1
                    )
                )
            ) else (
                echo   RESULT: FAILED - Simulation log not created
                set /a FAILED+=1
            )
        )
    ) else (
        echo   RESULT: FAILED - Testbench files missing
        set /a FAILED+=1
    )
    echo.
)

echo ========================================
echo           TEST SUMMARY
echo ========================================
echo.
echo PASSED:  %PASSED%
echo FAILED:  %FAILED% ^(including timeouts: %TIMEOUTS%, crashes: %CRASHES%, compile errors: %COMPILE_ERRORS%^)
echo.

if %COMPILE_ERRORS% gtr 0 (
    echo COMPILATION ERRORS DETECTED - Check compile logs for details
)
if %TIMEOUTS% gtr 0 (
    echo TIMEOUTS DETECTED - Simulations exceeded 60s limit, check sim logs
)
if %CRASHES% gtr 0 (
    echo CRASHES DETECTED - Core dumps captured in crash logs
)

if %FAILED% equ 0 (
    echo OVERALL RESULT: ALL TESTS PASSED^^!
) else (
    echo OVERALL RESULT: SOME TESTS FAILED - Check logs for details
)

echo.
echo ========================================
echo Test suite completed.
echo ========================================
echo ========================================
echo.

set MODULES=adc_interface nco_generator cic_decimator async_fifo ethernet_mac fft_processor hamming_window rp2040_interface udp_ip_stack adaptive_gain_scaler average_power_detector
echo Modules to test: %MODULES%
echo.

REM Initialize counters
set /a PASSED=0
set /a FAILED=0
set /a TIMEOUTS=0
set /a COMPILE_ERRORS=0
set /a SIM_ERRORS=0
set /a CRASHES=0

REM Set timeout values (in seconds)
set /a SIM_TIMEOUT=60

set TOTAL_MODULES=0

for %%M in (%MODULES%) do (
    set /a TOTAL_MODULES+=1
    echo =======================================
    echo Testing Module: %%M (Timeout: %SIM_TIMEOUT%s)
    echo =======================================

    REM Check if files exist
    if exist "verilog\%%M.v" if exist "verilog\%%M_tb.v" (
        echo Running compilation...
        REM Simple compile and run
        iverilog -g2012 -o "tests/sim_output/%%M.vvp" -I verilog verilog\%%M.v verilog\%%M_tb.v > tests/sim_output/%%M_compile.log 2>&1

        if !errorLevel! EQU 0 (
            echo Compilation successful. Starting simulation...

            REM Run simulation with timeout mechanism using simplified approach
            echo Starting simulation (timeout: %SIM_TIMEOUT% seconds)...
            REM Run vvp and then wait, kill if still running
            start /B vvp tests/sim_output/%%M.vvp > tests/sim_output/%%M_sim.log 2>&1
            REM Wait for timeout period
            timeout /t %SIM_TIMEOUT% /nobreak >nul
            REM Force kill any lingering vvp processes (always kill after timeout period)
            taskkill /f /im vvp.exe >nul 2>&1

            REM Check if simulation completed normally by looking for finish signal
            set "SIM_FINISHED=0"
            if exist tests/sim_output/%%M_sim.log (
                findstr /c:"$finish called" tests/sim_output/%%M_sim.log >nul
                if !errorlevel! EQU 0 (
                    set "SIM_FINISHED=1"
                )
            )

            REM Check for timeout
            if !SIM_FINISHED! EQU 0 (
                echo Simulation timed out after %SIM_TIMEOUT% seconds. Capturing state...
                REM Add timeout marker to log
                echo TIMEOUT: Simulation exceeded %SIM_TIMEOUT% second limit >> tests/sim_output/%%M_sim.log
                set "RESULT=FAILED"
                set "REASON=simulation timed out"
                set "ERROR_TYPE=1"
                set /a TIMEOUTS+=1
            ) else (
                REM Check for crash indicators (segmentation fault, etc.)
                findstr /c:"Segmentation fault" /c:"Access violation" /c:"Exception" tests/sim_output/%%M_sim.log >nul 2>&1
                if !errorLevel! EQU 0 (
                    echo Simulation crashed. Core dump captured in log...
                    set "RESULT=FAILED"
                    set "REASON=simulation crashed"
                    set "ERROR_TYPE=2"
                    set /a CRASHES+=1
                    REM Capture system information
                    echo CRASH_INFO: Date=!date! Time=!time! >> tests/sim_output/%%M_crash.log
                    systeminfo >> tests/sim_output/%%M_crash.log 2>&1
                ) else (
                    set "ERROR_TYPE=0"
                )
            )

            if !ERROR_TYPE! EQU 0 (
                findstr "TEST PASSED" tests/sim_output/%%M_sim.log >nul
                if !errorLevel! EQU 0 (
                    set "RESULT=PASSED"
                    set "REASON=test completed successfully"
                ) else (
                    findstr "TEST FAILED" tests/sim_output/%%M_sim.log >nul
                    if !errorLevel! EQU 0 (
                        set "RESULT=FAILED"
                        set "REASON=logic reports error"
                    ) else (
                        set "RESULT=FAILED"
                        set "REASON=no pass/fail indication found in log"
                        set /a SIM_ERRORS+=1
                    )
                )
            )

            echo   RESULT: !RESULT! (%%M !REASON!)

            if !RESULT! EQU FAILED (
                set /a FAILED+=1
                if !REASON! EQU compilation error set /a COMPILE_ERRORS+=1
            ) else (
                set /a PASSED+=1
            )
        ) else (
            echo   RESULT: FAILED (%%M compilation error)
            set /a COMPILE_ERRORS+=1
            set /a FAILED+=1
        )

    )
    echo.
)

echo ========================================
echo         TEST SUMMARY
echo ========================================
echo.
echo Total Modules Tested: %TOTAL_MODULES%
echo.
echo PASSED:  %PASSED%
echo FAILED:  %FAILED% (including timeouts: %TIMEOUTS%, crashes: %CRASHES%, compile errors: %COMPILE_ERRORS%, sim errors: %SIM_ERRORS%)
echo.
if %FAILED% EQU 0 (
    if %COMPILE_ERRORS% EQU 0 (
        if %TIMEOUTS% EQU 0 (
            if %CRASHES% EQU 0 (
                if %SIM_ERRORS% EQU 0 (
                    echo OVERALL RESULT: ALL TESTS PASSED!
                )
            )
        )
    )
)
if %FAILED% GTR 0 (
    echo OVERALL RESULT: SOME TESTS FAILED - Check logs for details
)
if %COMPILE_ERRORS% GTR 0 (
    echo OVERALL RESULT: COMPILATION ERRORS - Check compile logs
)
if %TIMEOUTS% GTR 0 (
    echo OVERALL RESULT: TIMEOUTS DETECTED - Check sim logs for state dumps
)
if %CRASHES% GTR 0 (
    echo OVERALL RESULT: CRASHES DETECTED - Check crash logs for details
)
echo.
echo ========================================
echo Test suite completed.
