<#
.SYNOPSIS
Safely checks a currently running one-pass Freerouting job.

Freerouting cannot resume a killed headless route without a completed SES.
This command deliberately refuses to create a second pass; use the restart
script only when a new pass is explicitly intended.
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$workRoot = Join-Path $root 'build\one-pass-freerouting'
$run = Get-ChildItem -LiteralPath $workRoot -Directory | Sort-Object Name -Descending | Select-Object -First 1
if (-not $run) { throw "No Freerouting run exists below $workRoot" }

$statePath = Join-Path $run.FullName 'run-state.json'
if (Test-Path -LiteralPath $statePath) {
    $state = Get-Content -Raw -LiteralPath $statePath | ConvertFrom-Json
    if ($state.pid -and (Get-Process -Id $state.pid -ErrorAction SilentlyContinue)) {
        python hardware/v2/tools/freerouting_monitor.py --run $run.FullName
        exit 0
    }
}
if (Test-Path -LiteralPath (Join-Path $run.FullName 'result.json')) {
    python hardware/v2/tools/freerouting_monitor.py --run $run.FullName
    exit 0
}
throw "The latest router process is not running and has no completed result/SES to resume. Start a new pass only with freerouting_restart.ps1."
