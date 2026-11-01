# Environment and tool versions — Code-SDR EM validation evidence area

Captured: 2026-10-30 (command output below)

## Toolchain

| Tool | Version | Source |
|---|---|---|
| System Python | 3.12.2 | `python --version` (C:\Program Files\Python312) |
| KiCad bundled Python | 3.11.5 | `C:\Program Files\KiCad\9.0\bin\python.exe` |
| KiCad / pcbnew | 9.0.7 | `kicad-cli version`; `pcbnew.GetBuildVersion()` |
| kicad-cli | 9.0.7 | `C:\Program Files\KiCad\9.0\bin\kicad-cli.exe version` |
| numpy (system py) | 2.2.6 | |
| scipy (system py) | 1.16.3 | |
| matplotlib (system py) | 3.9.2 | |
| scikit-rf (system py) | 1.9.0 | |
| numpy (KiCad py) | 2.3.5 | |
| scipy (KiCad py) | 1.17.1 | |
| Java (OpenJDK) | 25.0.3 | `java -version` |
| Octave | 10.2.0 (mingw64) | `D:\mcp\eng\octave\octave-10.2.0-w64-64\mingw64\bin\octave.exe` (not used for solve; start exceeded 120 s) |
| git | (repo clones) | repos/Code-SDR @ 4cdf7e0f4efd7b8ced1cb830cf73fb65e48e368a; repos/dev-board @ f9b02bf159f584cd5fc28696f905ed223af99435 |

## Full-wave / measurement tooling probe (FAIL-CLOSED results)

Probed command / paths and outcome:

| Probe | Result |
|---|---|
| `Get-Command openEMS` | NOT FOUND |
| `Get-Command csxcad`, `AppCSXCAD`, `openems_cli` | NOT FOUND |
| `Get-Command feko`, `hfss` (and variants) | NOT FOUND |
| `python -c "import openEMS"` | ModuleNotFoundError: No module named 'openEMS' |
| `python -c "import skrf"` | skrf 1.9.0 present (analytical/network modelling only; not a full-wave solver) |
| Octave package directory scan | **no openems / CSXCAD package installed** (package dirs sampled: audio, biosig, cfitsio, coder, communications, control, ... no openems) |
| Program files scan (D:\, C:\Program Files, C:\Program Files (x86)) for open/ems/csx | only OpenSCAD, OpenSSL, OpenXML, OpenOffice, Open Design Alliance — none are EM solvers |
| VNA / instruments | none connected; no measurement hardware referenced |

**Conclusion: no full-wave EM solver (openEMS/CSXCAD, HFSS, FEKO, matlab/octave EM, ngspice, Qucs, Sonnet, etc.) and no measurement hardware are available in this environment. All full-wave validation paths FAIL CLOSED. Any claim of full-wave validation or physical RF performance would be fabrication; none is made in this evidence area.**

## Available analytical tooling (what was used)

- System Python + numpy/scipy/matplotlib: quasi-static microstrip (Hammerstad-Jensen), ABCD network re-derivations, plots.
- KiCad Python (pcbnew) + kicad-cli: board geometry audits (RF50 nets, planes, zones, DRC re-run), geometry extraction.
- Repo-built analytical scripts (validate_plan.py, rf_analysis.py) re-run in copies.
- scikit-rf 1.9.0: present but NOT used to fabricate S-parameters from nothing; no measured Touchstone data exists on disk (checked).

## What would be needed for a real full-wave pass (documented for fail-closed)

- openEMS 0.0.35+ / CSXCAD built for Windows (or WSL), or a licensed HFSS/FEKO/similar.
- Imported geometry: KiCad PCB exported to STEP/3D or openEMS KiCad module (kicad_export) with stackup (F.Cu 0.035 / prepreg 0.13 Dk4.2 / In1 0.035 / core 1.2 Dk4.2 / In2 0.035 / prepreg 0.13 / B.Cu 0.035, ENIG, mask 0.010).
- Mesh + ports at J50/J60-J63 SMA launches and at chip pads; solve S-parameters 100 MHz-10 GHz.
- VNA/TDR measurement on first article per FIRST_ARTICLE_VALIDATION.md (reference plane calibration, Touchstone storage).

None of the above executed; this evidence area therefore contains ANALYTICAL / QUASI-STATIC results only.