# Code-SDR V2 hardware

This is the modular KiCad 9 receiver/control-board redesign. It covers
100 MHz-10 GHz with 40 MHz useful real bandwidth. It is explicitly not an ESC;
the ESC remains a separate external board.

## Project structure

- `Code-SDR-V2.kicad_sch` - hierarchical root schematic.
- `sheets/` - eight functional subsheets.
- `Code-SDR-V2.kicad_pcb` - 99 x 99 mm, four-layer PCB with the 49 RF50
  controlled-impedance nets routed.
- `Code-SDR-V2.kicad_pro` and `.kicad_dru` - project and enforced rules.
- `CodeSDR.pretty/` and `CodeSDR.kicad_sym` - project-local audited libraries.
- `tools/design_model.py` - single electrical component/net contract.
- `tools/generate_schematic.py` and `tools/generate_pcb.py` - deterministic CAD
  generation.
- `build/` - validation and intermediate output; final manufacturing outputs
  are exported separately for release.

The FPGA supply islands and configuration pulls implement the Lattice
CrossLink hardware checklist: used VCC, VCCGPLL, VCCAUX and each VCCIO bank
are locally filtered and bypassed, while the unused D-PHY supply groups are
intentionally unpowered. CRESET_B, CDONE and SPI_SS have 4.7 kΩ pull-ups to
VCCIO0 and the processor-driven SPI clock is source damped.

## Regeneration

```powershell
python hardware/v2/tools/generate_schematic.py
& "C:\Program Files\KiCad\9.0\bin\python.exe" hardware/v2/tools/generate_pcb.py
kicad-cli sch erc --format json -o hardware/v2/build/erc.json hardware/v2/Code-SDR-V2.kicad_sch
kicad-cli pcb drc --format json --all-track-errors -o hardware/v2/build/drc.json hardware/v2/Code-SDR-V2.kicad_pcb
```

Regeneration intentionally replaces the PCB with the clean placement baseline:
344 footprints, four copper layers, a solid In1 ground zone, and split In2
analog/digital power zones.
Do not run `generate_pcb.py` after beginning a manual route unless that route
is meant to be discarded.

## Route import and release

Freerouting must write its completed session to
`build/Code-SDR-V2-routed.ses`. Then run:

```powershell
java -Xmx2600m -jar freerouting-2.3.0.jar `
  -de hardware/v2/build/Code-SDR-V2.dsn `
  -do hardware/v2/build/Code-SDR-V2-routed.ses `
  -mp 1 -mt 0 -da -dct 1 --gui.enabled=false
python hardware/v2/tools/sanitize_route_session.py
& "C:\Program Files\KiCad\9.0\bin\python.exe" hardware/v2/tools/import_route.py
& "C:\Program Files\KiCad\9.0\bin\python.exe" hardware/v2/tools/export_route_checkpoint.py
& "C:\Program Files\KiCad\9.0\bin\python.exe" hardware/v2/tools/refill_zones.py
& "C:\Program Files\KiCad\9.0\bin\python.exe" hardware/v2/tools/add_rf_via_fences.py
& "C:\Program Files\KiCad\9.0\bin\python.exe" hardware/v2/tools/validate_route.py
powershell -ExecutionPolicy Bypass -File hardware/v2/tools/export_release.ps1
```

The actual four-layer routing contract is `F.Cu / solid In1 GND / split In2
power / B.Cu`. In2 is split between `+3V3_ANA` and `+3V3_DIG`; it is not a
ground plane. The 49 RF50 nets are routed as nominal controlled-impedance
traces on the outer layers using 0.23 mm geometry over the 0.13 mm outer
dielectric. F.Cu signals reference solid In1 GND. B.Cu controlled signals need
an explicitly reviewed return path because their adjacent In2 reference is a
split power plane. No signal trace may be placed on In1 or cross an In2 split
without a deliberate, documented return-path treatment.
Use one persisted pass at a time: import each normally completed session,
run DRC, export the routed board with `export_route_checkpoint.py`, and repeat
until KiCad reports no opens. `-mt 0` lets Freerouting select the available
worker-thread count. Add the fences and run length/skew validation only
after importing the completed clean-board route.

`import_route.py` also refreshes footprint hierarchy paths from the current
schematic before parity checking. Route-validation helpers must enforce the
four-layer contract above before manufacturing output is accepted.

## Non-CAD release gates

The specified 0.13 mm outer dielectric and nominal 0.23 mm RF trace are the
50-ohm impedance-design basis. Have the
selected fabricator field-solve RF and differential geometries using its actual
Dk, copper and solder mask. A first article must pass the VNA, PLL, NF,
linearity, image, thermal and full-range calibration plan in
[`../HARDWARE_VALIDATION.md`](../HARDWARE_VALIDATION.md).
