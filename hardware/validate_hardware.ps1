[CmdletBinding()]
param(
    [string]$ProjectBase = "Wideband-Test-V1",
    [string]$OutputDirectory = "validation-output",
    [switch]$AllowViolations
)

$ErrorActionPreference = "Stop"
$hardwareRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$schematic = Join-Path $hardwareRoot "$ProjectBase.kicad_sch"
$board = Join-Path $hardwareRoot "$ProjectBase.kicad_pcb"
$outputRoot = Join-Path $hardwareRoot $OutputDirectory

if (-not (Test-Path -LiteralPath $schematic)) {
    throw "Schematic not found: $schematic"
}
if (-not (Test-Path -LiteralPath $board)) {
    throw "PCB not found: $board"
}

$kicad = Get-Command kicad-cli -ErrorAction SilentlyContinue
if (-not $kicad) {
    $fallback = "C:\Program Files\KiCad\9.0\bin\kicad-cli.exe"
    if (Test-Path -LiteralPath $fallback) {
        $kicad = Get-Item -LiteralPath $fallback
    } else {
        throw "KiCad CLI was not found. Install KiCad 9 or add kicad-cli to PATH."
    }
}

New-Item -ItemType Directory -Path $outputRoot -Force | Out-Null
$ercPath = Join-Path $outputRoot "erc.json"
$drcPath = Join-Path $outputRoot "drc.json"
$bomPath = Join-Path $outputRoot "bom.csv"

& $kicad.Source sch erc --output $ercPath --format json --severity-all $schematic
if ($LASTEXITCODE -ne 0) {
    throw "KiCad ERC command failed with exit code $LASTEXITCODE."
}

& $kicad.Source pcb drc --output $drcPath --format json --severity-all --schematic-parity $board
if ($LASTEXITCODE -ne 0) {
    throw "KiCad DRC command failed with exit code $LASTEXITCODE."
}

& $kicad.Source sch export bom --output $bomPath `
    --fields 'Reference,Value,Footprint,${QUANTITY},${DNP}' `
    --labels 'Refs,Value,Footprint,Qty,DNP' $schematic
if ($LASTEXITCODE -ne 0) {
    throw "KiCad BOM export failed with exit code $LASTEXITCODE."
}

$erc = Get-Content -LiteralPath $ercPath -Raw | ConvertFrom-Json
$drc = Get-Content -LiteralPath $drcPath -Raw | ConvertFrom-Json

$ercViolations = @($erc.sheets | ForEach-Object { $_.violations }).Count
$drcViolations = @($drc.violations).Count
$unconnectedItems = @($drc.unconnected_items).Count
$parityFindings = @($drc.schematic_parity).Count
$errorCount = @(
    $erc.sheets | ForEach-Object { $_.violations } | Where-Object severity -eq "error"
).Count + @(
    $drc.violations | Where-Object severity -eq "error"
).Count + @(
    $drc.unconnected_items | Where-Object severity -eq "error"
).Count + @(
    $drc.schematic_parity | Where-Object severity -eq "error"
).Count

[pscustomobject]@{
    Project = $ProjectBase
    KicadVersion = $drc.kicad_version
    ErcViolations = $ercViolations
    DrcViolations = $drcViolations
    UnconnectedItems = $unconnectedItems
    ParityFindings = $parityFindings
    ErrorSeverityTotal = $errorCount
    OutputDirectory = $outputRoot
} | Format-List

if (-not $AllowViolations -and (
    $ercViolations -gt 0 -or
    $drcViolations -gt 0 -or
    $unconnectedItems -gt 0 -or
    $parityFindings -gt 0
)) {
    Write-Error "Hardware validation failed. Release requires zero ERC, DRC, unconnected, and parity findings."
    exit 1
}

if ($AllowViolations -and (
    $ercViolations -gt 0 -or
    $drcViolations -gt 0 -or
    $unconnectedItems -gt 0 -or
    $parityFindings -gt 0
)) {
    Write-Warning "Hardware validation completed with violations because -AllowViolations was set."
    exit 0
}

Write-Host "Hardware validation passed."
