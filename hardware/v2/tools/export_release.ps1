param(
    [string]$ReleaseName = "final"
)

$ErrorActionPreference = "Stop"

function Invoke-KiCad {
    $KiCadArguments = @($args)
    & kicad-cli @KiCadArguments
    if ($LASTEXITCODE -ne 0) {
        throw "kicad-cli failed with exit code $LASTEXITCODE`: $($KiCadArguments -join ' ')"
    }
}

$v2Root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot ".."))
$repoRoot = [IO.Path]::GetFullPath((Join-Path $v2Root "..\.."))
$releaseRoot = [IO.Path]::GetFullPath((Join-Path $v2Root "release"))
$finalPackageRoot = [IO.Path]::GetFullPath((Join-Path $releaseRoot $ReleaseName))
$packageRoot = [IO.Path]::GetFullPath((Join-Path $releaseRoot ".$ReleaseName.staging"))
$expectedPrefix = $releaseRoot.TrimEnd([IO.Path]::DirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
if (-not $packageRoot.StartsWith($expectedPrefix, [StringComparison]::OrdinalIgnoreCase) -or
    -not $finalPackageRoot.StartsWith($expectedPrefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to replace a release directory outside $releaseRoot"
}
if (Test-Path -LiteralPath $packageRoot) {
    Remove-Item -LiteralPath $packageRoot -Recurse -Force
}
$locationPushed = $false
trap {
    $caughtError = $_
    if ($locationPushed) {
        Pop-Location
        $locationPushed = $false
    }
    if (Test-Path -LiteralPath $packageRoot) {
        Remove-Item -LiteralPath $packageRoot -Recurse -Force
    }
    throw $caughtError
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

# KiCad resolves project-local design rules and library tables from the current
# project directory. Without this, CLI DRC silently uses a different rule set.
Push-Location -LiteralPath $v2Root
$locationPushed = $true

Invoke-KiCad sch erc --format json --severity-all --exit-code-violations `
    -o (Join-Path $validationDir "erc.json") $schematic
Invoke-KiCad pcb drc --format json --severity-all --all-track-errors `
    --schematic-parity --units mm `
    -o (Join-Path $validationDir "drc.json") $board

$drcReport = Get-Content -Raw -LiteralPath (Join-Path $validationDir "drc.json") |
    ConvertFrom-Json
if ($drcReport.violations.Count -ne 0) {
    throw "Release DRC contains $($drcReport.violations.Count) violation(s)"
}
if ($drcReport.schematic_parity.Count -ne 0) {
    throw "Release DRC contains $($drcReport.schematic_parity.Count) schematic parity issue(s)"
}
if ($drcReport.unconnected_items.Count -ne 0) {
    throw "Release DRC contains $($drcReport.unconnected_items.Count) unconnected item(s)"
}

$kicadCli = (Get-Command kicad-cli -ErrorAction Stop).Source
$kicadPython = Join-Path (Split-Path -Parent $kicadCli) "python.exe"
& $kicadPython (Join-Path $PSScriptRoot "audit_rf50_routes.py")
if ($LASTEXITCODE -ne 0) {
    throw "RF50 connectivity/geometry audit failed with exit code $LASTEXITCODE"
}

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
    (Join-Path $repoRoot "documentation\FPGA_ARCHITECTURE.md"),
    (Join-Path $repoRoot "verilog\V2_HARDWARE_COMPATIBILITY.md") -Destination $docsDir
Copy-Item -LiteralPath (Join-Path $v2Root "FIRST_ARTICLE_VALIDATION.md"),
    (Join-Path $v2Root "FABRICATOR_IMPEDANCE_NOTE.md"),
    (Join-Path $v2Root "STACKUP_DRAWING.md") -Destination $docsDir
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
if (Test-Path -LiteralPath (Join-Path $v2Root "build\ROUTE_STATISTICS.csv")) {
    Copy-Item -LiteralPath (Join-Path $v2Root "build\ROUTE_STATISTICS.csv") -Destination $validationDir
}

$releaseNotice = @"
CODE-SDR V2 FIRST-ARTICLE FABRICATION PACKAGE

This package was generated only after KiCad reported zero DRC violations,
zero schematic-parity issues, and zero unconnected items.  The board house
must field-solve/tune the controlled-impedance geometries against its actual
laminate Dk, pressed dielectric thickness, finished copper, and solder mask.
First-article RF characterization remains mandatory after assembly.
"@
[IO.File]::WriteAllText(
    (Join-Path $packageRoot "README.txt"),
    $releaseNotice,
    [Text.UTF8Encoding]::new($false)
)

$manifest = Get-ChildItem -LiteralPath $packageRoot -Recurse -File |
    Sort-Object FullName |
    ForEach-Object {
        $hash = Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName
        [pscustomobject]@{
            Path = $_.FullName.Substring($packageRoot.Length + 1)
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
if (Test-Path -LiteralPath $finalPackageRoot) {
    Remove-Item -LiteralPath $finalPackageRoot -Recurse -Force
}
Move-Item -LiteralPath $packageRoot -Destination $finalPackageRoot
Compress-Archive -LiteralPath $finalPackageRoot -DestinationPath $zipPath -CompressionLevel Optimal
Pop-Location
$locationPushed = $false
Write-Output $zipPath
