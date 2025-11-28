@echo off
setlocal EnableDelayedExpansion

echo Testing simple timeout mechanism...
cd /d "%~dp0\..\.."

echo Starting background simulation...
taskkill /f /im vvp.exe >nul 2>&1
echo vvp tests/sim_output/adc_interface.vvp ^> tests/sim_output/test_sim.log 2^>^&1
start /B vvp tests/sim_output/adc_interface.vvp > tests/sim_output/test_sim.log 2>&1
timeout /t 5 /nobreak >nul
tasklist /fi "imagename eq vvp.exe" /nh >nul 2>&1
if !errorlevel! EQU 0 (
    REM still running, timeout occurred
    taskkill /f /im vvp.exe >nul 2>&1
    echo Simulation timed out
    echo Simulation did not complete
) else (
    REM completed within timeout
    echo Simulation completed within timeout
    if exist tests/sim_output/test_sim.log (
        type tests/sim_output/test_sim.log 2>nul | findstr /c:"$finish called" >nul
        if !errorlevel! EQU 0 (
            echo Simulation completed normally
        ) else (
            echo Simulation terminated early
        )
    ) else (
        echo Log file not created
    )
)

echo Done.
