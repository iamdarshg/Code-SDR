@echo off
REM run_all_tests.bat - Code-SDR FPGA Test Runner
REM Tests all modules using cocotb and/or Icarus Verilog (iverilog)
REM Generates concise, readable output

setlocal enabledelayedexpansion

REM Change directory to the project root within WSL
set "WSL_PROJECT_ROOT=/mnt/d/Code-SDR"

REM Set Python environment variables for cocotb (within WSL)
REM Python dependencies are managed within the WSL environment.
set "COCOTB_REDUCED_LOG_FMT=1"

REM Configure cocotb simulator and sources (within WSL)
set "COCOTB_SIM=verilator"
set "VERILOG_SOURCES_WSL=verilog/adaptive_gain_scaler.v verilog/adc_interface.v verilog/async_fifo.v verilog/average_power_detector.v verilog/cic_decimator.v verilog/clock_manager.v verilog/compensation_filter.v verilog/digital_downconverter.v verilog/dma_engine.v verilog/ethernet_mac.v verilog/fft_processor.v verilog/fpga_processing_pipeline.v verilog/hamming_window.v verilog/nco_generator.v verilog/rp2040_interface.v verilog/udp_ip_stack.v"
set "TOPLEVEL_LANG=verilog"

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
        set "TOPLEVEL=%%M"
        set "MODULE=test_%%M"
        set "COCOTB_OUT_DIR=tests\sim_output\%%M_out"
        set "SIM_BUILD=tests\sim_output\%%M_sim_build"
        md "tests\sim_output\%%M_out" 2>nul
        md "tests\sim_output\%%M_sim_build" 2>nul
        set "COCOTB_LOG_LEVEL=DEBUG"
        wsl -d kali-linux -u darsh -- bash -c "cd %WSL_PROJECT_ROOT% && /usr/bin/python3 -m pytest --sim=verilator --toplevel=%%M tests/cocotb_tests/test_%%M.py -v --tb=no" > tests\sim_output\%%M_cocotb.log 2^>^&1
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
        echo Running verilator test...

        REM Compile and Run with Verilator via WSL
        wsl -d kali-linux -u darsh -- bash -c "cd %WSL_PROJECT_ROOT% && mkdir -p tests/sim_output/%%M_verilator_build && verilator --cc --exe verilog/%%M_tb.v verilog/%%M.v --top-module %%M_tb --Mdir tests/sim_output/%%M_verilator_build --timing --Wno-WIDTHEXPAND > tests/sim_output/%%M_compile.log 2>&1"
        if !errorlevel! == 0 (
            echo   compilation: OK
            REM Build Verilator executable via WSL
            wsl -d kali-linux -u darsh -- bash -c "cd %WSL_PROJECT_ROOT% && make -C tests/sim_output/%%M_verilator_build -f V%%M_tb.mk > tests/sim_output/%%M_build.log 2>&1"
            if !errorlevel! == 0 (
                echo   build: OK
                REM Run with timeout via WSL
                wsl -d kali-linux -u darsh -- bash -c "cd %WSL_PROJECT_ROOT% && tests/sim_output/%%M_verilator_build/V%%M_tb > tests/sim_output/%%M_sim.log 2>&1 & pid=$! ; ( sleep %SIM_TIMEOUT% && kill $pid 2>/dev/null ) & wait $pid"
                
                REM Check for finish signal (still reading from Windows path)
                findstr "$finish called" tests\sim_output\%%M_sim.log > nul 2>&1
                if !errorlevel! == 0 (
                    echo   verilator: PASS
                    if "!TEST_RESULT!"=="UNKNOWN" set "TEST_RESULT=PASS"
                ) else (
                    echo   verilator: TIMEOUT
                    set "TEST_RESULT=FAIL"
                    set /a TIMEOUTS+=1
                )
            ) else (
                echo   verilator: BUILD FAIL
                set "TEST_RESULT=FAIL"
            )
        ) else (
            echo   verilator: COMPILE FAIL
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
