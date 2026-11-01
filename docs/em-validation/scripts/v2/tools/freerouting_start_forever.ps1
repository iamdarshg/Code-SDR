<#
.SYNOPSIS
Starts one protected Freerouting pass in the background with no time limit.

The PCB remains untouched until the Python driver's acceptance gates pass.
Do not run this while another recorded Freerouting process is active.
#>
[CmdletBinding()]
param(
    [switch]$Restart
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$workRoot = Join-Path $root 'build\one-pass-freerouting'
New-Item -ItemType Directory -Force -Path $workRoot | Out-Null

$latest = Get-ChildItem -LiteralPath $workRoot -Directory -ErrorAction SilentlyContinue |
    Sort-Object Name -Descending | Select-Object -First 1
if ($latest) {
    $statePath = Join-Path $latest.FullName 'run-state.json'
    if (Test-Path -LiteralPath $statePath) {
        $state = Get-Content -Raw -LiteralPath $statePath | ConvertFrom-Json
        if ($state.pid -and (Get-Process -Id $state.pid -ErrorAction SilentlyContinue)) {
            throw "Freerouting is already running (PID $($state.pid)) in $($latest.FullName). Use freerouting_monitor.py instead."
        }
    }
    if (-not $Restart -and -not (Test-Path -LiteralPath (Join-Path $latest.FullName 'result.json'))) {
        throw "The previous run has no final result. Use freerouting_continue.ps1 to inspect it, or explicitly use -Restart to start a new pass."
    }
}

$stdout = Join-Path $workRoot 'background-latest.stdout.log'
$stderr = Join-Path $workRoot 'background-latest.stderr.log'
$arguments = @(
    'hardware/v2/tools/run_one_pass_freerouting.py',
    '--board', 'hardware/v2/Code-SDR-V2.kicad_pcb',
    '--project', 'hardware/v2/Code-SDR-V2.kicad_pro',
    '--work-root', 'hardware/v2/build/one-pass-freerouting',
    '--download-official-router'
)
$process = Start-Process -FilePath 'python' -ArgumentList $arguments -WorkingDirectory (Get-Location) `
    -RedirectStandardOutput $stdout -RedirectStandardError $stderr -WindowStyle Hidden -PassThru

$run = $null
for ($attempt = 0; $attempt -lt 40 -and -not $run; $attempt++) {
    Start-Sleep -Milliseconds 500
    $run = Get-ChildItem -LiteralPath $workRoot -Directory | Sort-Object Name -Descending | Select-Object -First 1
    if ($run -and -not (Test-Path -LiteralPath (Join-Path $run.FullName 'candidate\candidate.kicad_pcb'))) {
        $run = $null
    }
}
if (-not $run) {
    throw "The background driver did not create a run directory. Inspect $stderr"
}
@{
    pid = $process.Id
    state = 'running'
    started_at = (Get-Date).ToString('o')
    command = 'one protected Freerouting pass, no timeout'
} | ConvertTo-Json | Set-Content -Encoding utf8 -LiteralPath (Join-Path $run.FullName 'run-state.json')

Write-Output "Started Freerouting PID $($process.Id) in $($run.FullName)"
Write-Output "Check: python hardware/v2/tools/freerouting_monitor.py --run '$($run.FullName)'"
