# Exact commands executed for the Code-SDR EM validation evidence area

All commands were executed on Windows (PowerShell 5.1+) on 2026-10-30.
Working directory where noted. Repo working copies were NOT modified;
all analysis ran on byte-identical copies inside this evidence area.

## 0. Evidence area setup

```
$ev="C:\Users\Darsh Gupta\engineering-portfolio-audit\evidence\code-sdr-em"
New-Item -ItemType Directory -Force -Path "$ev\scripts","$ev\outputs","$ev\plots","$ev\data" | Out-Null
$r="C:\Users\Darsh Gupta\engineering-portfolio-audit\repos\Code-SDR"
Copy-Item "$r\hardware\redesign" -Destination "$ev\scripts\redesign" -Recurse -Force
Copy-Item "$r\hardware\v2"        -Destination "$ev\scripts\v2" -Recurse -Force
Copy-Item "$r\hardware\HARDWARE_VALIDATION.md","$r\hardware\validate_hardware.ps1" -Destination "$ev\scripts" -Force
```

## 1. Input provenance hashing

```
Get-FileHash -Algorithm SHA256 (Get-ChildItem "$ev\scripts" -Recurse -File ...) 
  | Out-File "$ev\outputs\INPUT_FILES_SHA256.txt"
```

## 2. Existing repo analytical scripts re-run (in copies, byte-identical output)

```
cd "$ev\scripts\redesign"
python validate_plan.py > "$ev\outputs\validate_plan_run.txt" 2>&1
python rf_analysis.py  > "$ev\outputs\rf_analysis_run.txt" 2>&1
# verify byte-identical regeneration:
Get-FileHash "$ev\scripts\redesign\NOISE_BUDGET.csv" ; Get-FileHash "$r\hardware\redesign\NOISE_BUDGET.csv"
Get-FileHash "$ev\scripts\redesign\FILTER_RESPONSE.csv"; Get-FileHash "$r\hardware\redesign\FILTER_RESPONSE.csv"
Get-FileHash "$ev\scripts\redesign\RF_BUDGET_SCENARIOS.csv"; Get-FileHash "$r\hardware\redesign\RF_BUDGET_SCENARIOS.csv"
Get-FileHash "$ev\scripts\redesign\RF_ANALYSIS.md"; Get-FileHash "$r\hardware\redesign\RF_ANALYSIS.md"
```

## 3. KiCad-python pcbnew board audits (v2 committed board copy)

```
New-Item -ItemType Directory -Force -Path "$ev\scripts\v2\build" | Out-Null
cd "$ev\scripts\v2\tools"
& "C:\Program Files\KiCad\9.0\bin\python.exe" audit_rf50_routes.py > "$ev\outputs\audit_rf50_routes_run.txt" 2>&1
& "C:\Program Files\KiCad\9.0\bin\python.exe" validate_route.py   > "$ev\outputs\validate_route_run.txt" 2>&1
# board geometry / plane / zone scans:
& "C:\Program Files\KiCad\9.0\bin\python.exe" - <<EOF (rf50_net_geometry.json, plane_zones_scan.txt, in2_zones.txt)
```

## 4. Independent analytical EM checks (new scripts, this evidence area)

```
python "$ev\scripts\em_microstrip_quasistatic.py" > "$ev\outputs\em_microstrip_quasistatic_run.txt" 2>&1
& "C:\Program Files\KiCad\9.0\bin\python.exe" "$ev\scripts\em_return_path.py" > "$ev\outputs\em_return_path_run.txt" 2>&1
python "$ev\scripts\em_frequency_plan.py"  > "$ev\outputs\em_frequency_plan_run.txt" 2>&1
python "$ev\scripts\em_filter_recheck.py"  > "$ev\outputs\em_filter_recheck_run.txt" 2>&1
python "$ev\scripts\em_component_ranges.py"> "$ev\outputs\em_component_ranges_run.txt" 2>&1
python "$ev\scripts\em_plots.py"           > "$ev\outputs\em_plots_run.txt" 2>&1
```

## 5. kicad-cli DRC re-run (same committed board copy)

```
& "C:\Program Files\KiCad\9.0\bin\kicad-cli.exe" pcb drc --format json --all-track-errors `
   --exit-code-violations -o "$ev\outputs\drc_v2_recheck.json" "$ev\scripts\v2\Code-SDR-V2.kicad_pcb"
# exit code 5 (violations found): 391 violations, 0 unconnected
# classification: 199 lib_footprint_mismatch, 116 via_dangling, 76 track_dangling
```

## 6. Dev-board feasible analytical checks

```
$db="C:\Users\Darsh Gupta\engineering-portfolio-audit\repos\dev-board"
# RF launch geometry extract (CC1352 -> TR1 balun 0900PC15A0036):
& "C:\Program Files\KiCad\9.0\bin\python.exe" - (4 nets, 2.098 mm, 0.25 mm, Top, no vias)
# Release SHA-256 manifest verification (46/49 verify, 3 stale):
python - (hash verification script, output recorded below)
# Quasi-static electrical length of RF launches (analytical, system python)
# Board stackup/zone scan via pcbnew
```

## 7. Provenance of original repo (unmodified; dirty-state baseline recorded)

```
git -C "$r" rev-parse HEAD            # 4cdf7e0f4efd7b8ced1cb830cf73fb65e48e368a
git -C "$r" status --porcelain        # 66 pre-existing lines (prior agent work); NOT touched here
git -C "$db" rev-parse HEAD           # f9b02bf159f584cd5fc28696f905ed223af99435
git -C "$db" status --porcelain       # clean
```

## Fail-closed full-wave probe (documented, no solver exists)

```
Get-Command openEMS, csxcad, AppCSXCAD, feko, hfss, openems_cli  # none found
python -c "import openEMS"                                        # ModuleNotFoundError
# Octave 10.2 package scan: no openems package present
# Install-root scans (D:\, C:\Program Files, C:\Program Files (x86)): no EM solver
```