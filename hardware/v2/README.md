# Code-SDR V2 hardware

This is the modular KiCad 9 receiver/control-board redesign. It covers
100 MHz-10 GHz with 40 MHz useful real bandwidth. It is explicitly not an ESC;
the ESC remains a separate external board.

## Project structure

- `Code-SDR-V2.kicad_sch` - hierarchical root schematic.
- `sheets/` - eight functional subsheets.
- `Code-SDR-V2.kicad_pcb` - four-layer routed PCB source.
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

Regeneration replaces the PCB, so do not run `generate_pcb.py` after importing
or hand-editing a route unless the route will be regenerated too.

## Route import and release

Freerouting must write its completed session to
`build/Code-SDR-V2-routed.ses`. Then run:

```powershell
java -Xmx2200m -jar freerouting-2.2.4.jar `
  -de hardware/v2/build/Code-SDR-V2.dsn `
  -do hardware/v2/build/Code-SDR-V2-routed.ses `
  -mp 10 -mt 0 -da --gui.enabled=false
& "C:\Program Files\KiCad\9.0\bin\python.exe" hardware/v2/tools/import_route.py
& "C:\Program Files\KiCad\9.0\bin\python.exe" hardware/v2/tools/add_rf_via_fences.py
& "C:\Program Files\KiCad\9.0\bin\python.exe" hardware/v2/tools/validate_route.py
powershell -ExecutionPolicy Bypass -File hardware/v2/tools/export_release.ps1
```

The exported DSN intentionally omits GND from track routing: the uninterrupted
In1 plane and filled F.Cu/B.Cu pours connect it without unnecessary stubs.
`-mt 0` disables post-route optimization; run length/skew validation and the
final DRC after importing the completed clean-board route.

`import_route.py` also refreshes footprint hierarchy paths from the current
schematic before parity checking. `validate_route.py` rejects signal routing
on the solid In1 ground plane and rejects vias or non-F.Cu segments in the
critical RF paths. It also checks USB, MDI and analog differential-pair skew,
ADC data-bus skew, and RGMII data/control skew.

## Non-CAD release gates

The specified 0.18 mm outer dielectric is the impedance-design basis. Have the
selected fabricator field-solve RF and differential geometries using its actual
Dk, copper and solder mask. A first article must pass the VNA, PLL, NF,
linearity, image, thermal and full-range calibration plan in
[`../HARDWARE_VALIDATION.md`](../HARDWARE_VALIDATION.md).
