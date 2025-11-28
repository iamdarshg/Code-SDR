@echo off
echo Testing manual compilation...
cd /d "%~dp0\..\.."
echo Current directory: %CD%
echo Checking files...
if exist "verilog\adc_interface.v" if exist "verilog\adc_interface_tb.v" (
    echo Files exist, compiling...
    iverilog -g2012 -o "tests/sim_output/adc_interface.vvp" -I verilog verilog/adc_interface.v verilog/adc_interface_tb.v > tests/sim_output/adc_interface_compile.log 2>&1
    echo Compilation exit code: %errorlevel%
) else (
    echo Files do not exist
)
echo Done.
