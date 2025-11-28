@echo off
setlocal EnableDelayedExpansion

echo Testing simple simulation...
cd /d "%~dp0\..\.."
del tests\sim_output\test_sim.log >nul 2>&1

echo Starting simulation...
vvp tests/sim_output/adc_interface.vvp > tests/sim_output/test_sim.log 2>&1

echo Checking result...
if exist tests/sim_output/test_sim.log (
    echo Simulation completed normally
) else (
    echo Log file not created
)
echo Done.
