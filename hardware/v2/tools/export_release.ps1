param(
    [string]$ReleaseName = "Code-SDR-V2-final"
)

$ErrorActionPreference = "Stop"

function Invoke-KiCad {
    param(
        [Parameter(ValueFromRemainingArguments = $true)]
        [string[]]$Arguments
    )

    & kicad-cli @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "kicad-cli failed with exit code $LASTEXITCODE`: $($Arguments -join ' ')"
    }
}

$v2Root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot ".."))
$repoRoot = [IO.Path]::GetFullPath((Join-Path $v2Root "..\.."))
$releaseRoot = [IO.Path]::GetFullPath((Join-Path $v2Root "release"))
$packageRoot = [IO.Path]::GetFullPath((Join-Path $releaseRoot $ReleaseName))
$expectedPrefix = $releaseRoot.TrimEnd([IO.Path]::DirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
if (-not $packageRoot.StartsWith($expectedPrefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to replace a release directory outside $releaseRoot"
}
if (Test-Path -LiteralPath $packageRoot) {
    Remove-Item -LiteralPath $packageRoot -Recurse -Force
}

$gerberDir = Join-Path $packageRoot "manufacturing\gerbers"
$assemblyDir = Join-Path $packageRoot "manufacturing\assembly"
$docsDir = Join-Path $packageRoot "documents"
$cadDir = Join-Path $packageRoot "cad"
$analysisDir = Join-Path $packageRoot "analysis"
$validationDir = Join-Path $packageRoot "validation"
$renderDir = Join-Path $packageRoot "renders"
@($gerberDir, $assemblyDir, $docsDir, $cadDir, $analysisDir, $validationDir, $renderDir) |
    ForEach-Object { New-Item -ItemType Directory -Path $_ -Force | Out-Null }

$board = Join-Path $v2Root "Code-SDR-V2.kicad_pcb"
$schematic = Join-Path $v2Root "Code-SDR-V2.kicad_sch"

Invoke-KiCad sch erc --format json --severity-all --exit-code-violations `
    -o (Join-Path $validationDir "erc.json") $schematic
Invoke-KiCad pcb drc --format json --severity-all --all-track-errors `
    --schematic-parity --exit-code-violations --units mm `
    -o (Join-Path $validationDir "drc.json") $board

Invoke-KiCad sch export bom `
    --fields 'Reference,Value,Footprint,Datasheet,Description,${QUANTITY},${DNP}' `
    --labels 'References,Value,Footprint,Datasheet,Description,Quantity,DNP' `
    --group-by 'Value,Footprint,Datasheet' --exclude-dnp `
    -o (Join-Path $assemblyDir "Code-SDR-V2-BOM.csv") $schematic
Invoke-KiCad pcb export pos --format csv --units mm --side both --exclude-dnp `
    -o (Join-Path $assemblyDir "Code-SDR-V2-positions.csv") $board
Invoke-KiCad pcb export gerbers `
    -l 'F.Cu,In1.Cu,In2.Cu,B.Cu,F.Paste,F.SilkS,B.SilkS,F.Mask,B.Mask,Edge.Cuts' `
    --subtract-soldermask --precision 6 -o $gerberDir $board
Invoke-KiCad pcb export drill --format excellon --excellon-units mm `
    --excellon-separate-th --generate-map --map-format pdf `
    -o $gerberDir $board
Invoke-KiCad pcb export ipcd356 -o (Join-Path $gerberDir "Code-SDR-V2.ipc") $board

Invoke-KiCad sch export pdf --black-and-white `
    -o (Join-Path $docsDir "Code-SDR-V2-schematic.pdf") $schematic
Invoke-KiCad pcb export pdf --mode-single --black-and-white --include-border-title `
    --sketch-pads-on-fab-layers -l 'F.Fab,Edge.Cuts' `
    -o (Join-Path $assemblyDir "Code-SDR-V2-top-assembly.pdf") $board
Invoke-KiCad pcb export pdf --mode-single --black-and-white --include-border-title `
    --sketch-pads-on-fab-layers --mirror -l 'B.Fab,Edge.Cuts' `
    -o (Join-Path $assemblyDir "Code-SDR-V2-bottom-assembly.pdf") $board

Invoke-KiCad pcb render --side top --quality high --floor --perspective `
    --width 2400 --height 1500 `
    -o (Join-Path $renderDir "Code-SDR-V2-top.png") $board
Invoke-KiCad pcb render --side bottom --quality high --floor --perspective `
    --width 2400 --height 1500 `
    -o (Join-Path $renderDir "Code-SDR-V2-bottom.png") $board
Invoke-KiCad pcb export step --force --board-only --include-tracks --include-pads `
    --include-zones --include-inner-copper --include-silkscreen --include-soldermask `
    -o (Join-Path $cadDir "Code-SDR-V2-board.step") $board

Copy-Item -LiteralPath $board, $schematic,
    (Join-Path $v2Root "Code-SDR-V2.kicad_pro"),
    (Join-Path $v2Root "Code-SDR-V2.kicad_dru"),
    (Join-Path $v2Root "CodeSDR.kicad_sym"),
    (Join-Path $v2Root "fp-lib-table"),
    (Join-Path $v2Root "sym-lib-table"),
    (Join-Path $v2Root "README.md") -Destination $cadDir
Copy-Item -LiteralPath (Join-Path $v2Root "sheets") -Destination $cadDir -Recurse
Copy-Item -LiteralPath (Join-Path $v2Root "CodeSDR.pretty") -Destination $cadDir -Recurse
Copy-Item -LiteralPath (Join-Path $v2Root "tools") -Destination $cadDir -Recurse
$routeSession = Join-Path $v2Root "build\Code-SDR-V2-routed.ses"
if (Test-Path -LiteralPath $routeSession) {
    Copy-Item -LiteralPath $routeSession -Destination $cadDir
}
$toolCache = Join-Path $cadDir "tools\__pycache__"
if (Test-Path -LiteralPath $toolCache) {
    Remove-Item -LiteralPath $toolCache -Recurse -Force
}

Copy-Item -LiteralPath (Join-Path $repoRoot "hardware\HARDWARE_VALIDATION.md"),
    (Join-Path $repoRoot "hardware\recommendations.md"),
    (Join-Path $repoRoot "documentation\INTEGRATION_SPECIFICATIONS.md"),
    (Join-Path $repoRoot "documentation\FPGA_ARCHITECTURE.md") -Destination $docsDir
Copy-Item -LiteralPath (Join-Path $repoRoot "hardware\redesign\FREQUENCY_PLAN.csv"),
    (Join-Path $repoRoot "hardware\redesign\FILTER_RESPONSE.csv"),
    (Join-Path $repoRoot "hardware\redesign\NOISE_BUDGET.csv"),
    (Join-Path $repoRoot "hardware\redesign\RF_ANALYSIS.md"),
    (Join-Path $repoRoot "hardware\redesign\POWER_TREE.csv"),
    (Join-Path $repoRoot "hardware\redesign\COST_COMPARISON.csv"),
    (Join-Path $repoRoot "hardware\redesign\COST_SUMMARY.md"),
    (Join-Path $repoRoot "hardware\redesign\README.md"),
    (Join-Path $repoRoot "hardware\redesign\V1_BOM.csv"),
    (Join-Path $repoRoot "hardware\redesign\validate_plan.py"),
    (Join-Path $repoRoot "hardware\redesign\rf_analysis.py"),
    (Join-Path $repoRoot "hardware\redesign\cost_model.py") -Destination $analysisDir

$routeAudit = Join-Path $v2Root "ROUTING_AUDIT.md"
if (Test-Path -LiteralPath $routeAudit) {
    Copy-Item -LiteralPath $routeAudit -Destination $validationDir
}
$routeStatistics = Join-Path $v2Root "build\ROUTE_STATISTICS.csv"
if (Test-Path -LiteralPath $routeStatistics) {
    Copy-Item -LiteralPath $routeStatistics -Destination $validationDir
}
$routeValidation = Join-Path $v2Root "build\validate-route.log"
if (Test-Path -LiteralPath $routeValidation) {
    Copy-Item -LiteralPath $routeValidation -Destination $validationDir
}

$manifest = Get-ChildItem -LiteralPath $packageRoot -Recurse -File |
    Sort-Object FullName |
    ForEach-Object {
        $hash = Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName
        [pscustomobject]@{
            Path = [IO.Path]::GetRelativePath($packageRoot, $_.FullName)
            Bytes = $_.Length
            SHA256 = $hash.Hash
        }
    }
$manifest | Export-Csv -NoTypeInformation -Encoding utf8 `
    -LiteralPath (Join-Path $packageRoot "MANIFEST-SHA256.csv")

$zipPath = Join-Path $releaseRoot "$ReleaseName.zip"
if (Test-Path -LiteralPath $zipPath) {
    Remove-Item -LiteralPath $zipPath -Force
}
Compress-Archive -LiteralPath $packageRoot -DestinationPath $zipPath -CompressionLevel Optimal
Write-Output $zipPath
