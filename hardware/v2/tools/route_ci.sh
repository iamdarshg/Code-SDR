#!/usr/bin/env bash
set -euo pipefail

sudo apt-get update
sudo apt-get install -y software-properties-common curl zip xvfb openjdk-21-jre-headless
sudo add-apt-repository --yes ppa:kicad/kicad-9.0-releases
sudo apt-get update
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y kicad
kicad-cli --version
python3 - <<'PY'
import pcbnew
print(pcbnew.GetBuildVersion())
PY
curl -fL --retry 4 --retry-delay 5 \
  -o /tmp/freerouting-2.2.4.jar \
  https://github.com/freerouting/freerouting/releases/download/v2.2.4/freerouting-2.2.4.jar

mkdir -p hardware/v2/build hardware/v2/release
board=hardware/v2/Code-SDR-V2.kicad_pcb
sch=hardware/v2/Code-SDR-V2.kicad_sch

# Preserve all library-table warnings as evidence, but fail only on electrical ERC errors.
kicad-cli sch erc --format json --severity-all \
  -o hardware/v2/build/erc-all.json "$sch" || true
kicad-cli sch erc --format json --severity-error --exit-code-violations \
  -o hardware/v2/build/erc-errors.json "$sch"
kicad-cli pcb drc --format json --severity-all --schematic-parity --units mm \
  -o hardware/v2/build/drc-pre-route.json "$board" || true

python3 hardware/redesign/validate_plan.py
python3 hardware/redesign/rf_analysis.py
python3 hardware/redesign/cost_model.py
python3 hardware/v2/tools/export_route_checkpoint.py --board "$board"
test -s hardware/v2/build/Code-SDR-V2.dsn

java -version
# Route only signals and rail nets; GND is served by the uninterrupted inner plane
# and outer pours prepared by export_route_checkpoint.py.
timeout 105m java -Xmx5200m -jar /tmp/freerouting-2.2.4.jar \
  -de hardware/v2/build/Code-SDR-V2.dsn \
  -do hardware/v2/build/Code-SDR-V2-routed.ses \
  -mp 80 -mt 0 -da -dct 2 --gui.enabled=false \
  2>&1 | tee hardware/v2/build/freerouting.log
test -s hardware/v2/build/Code-SDR-V2-routed.ses

python3 hardware/v2/tools/import_route.py
python3 hardware/v2/tools/add_rf_via_fences.py

set +e
kicad-cli pcb drc --format json --severity-all --all-track-errors \
  --schematic-parity --units mm \
  -o hardware/v2/build/drc-final.json "$board"
DRC_EXIT=$?
python3 hardware/v2/tools/validate_route.py \
  > hardware/v2/build/validate-route.log 2>&1
ROUTE_EXIT=$?
set -e

DRC_EXIT="$DRC_EXIT" ROUTE_EXIT="$ROUTE_EXIT" python3 - <<'PY'
import json, os
from collections import Counter
from pathlib import Path

data=json.loads(Path('hardware/v2/build/drc-final.json').read_text())
summary={
    'violations':len(data.get('violations',[])),
    'unconnected_items':len(data.get('unconnected_items',[])),
    'schematic_parity':len(data.get('schematic_parity',[])),
    'violation_types':dict(Counter(x.get('type','unknown') for x in data.get('violations',[]))),
    'drc_exit':int(os.environ['DRC_EXIT']),
    'route_exit':int(os.environ['ROUTE_EXIT']),
}
Path('hardware/v2/build/route-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
validation=Path('hardware/v2/build/validate-route.log').read_text(errors='replace')
ok=(summary['violations']==0 and summary['unconnected_items']==0 and
    summary['schematic_parity']==0 and summary['route_exit']==0)
Path('hardware/v2/build/route-ok').write_text('1\n' if ok else '0\n')
Path('hardware/v2/ROUTE_STATUS.md').write_text(f'''# Code-SDR V2 route status

- Result: **{'PASS' if ok else 'PARTIAL / REQUIRES CLEANUP'}**
- DRC violations: {summary['violations']}
- Unconnected items: {summary['unconnected_items']}
- Schematic/PCB parity findings: {summary['schematic_parity']}
- Route-invariant exit: {summary['route_exit']}

## Route-invariant output

```text
{validation.strip()}
```

The legacy `Wideband-Test-V1` board remains intentionally unrouted and marked
**DO NOT FABRICATE**. It is retained only as audit evidence; V2 is the corrected
receiver/control-board deliverable.
''')
print(json.dumps(summary,indent=2))
PY

if [[ "$(cat hardware/v2/build/route-ok)" != 1 ]]; then
  exit 0
fi

root=hardware/v2/release/Code-SDR-V2-final
rm -rf "$root" hardware/v2/release/Code-SDR-V2-final.zip
mkdir -p "$root"/{manufacturing/gerbers,manufacturing/assembly,documents,cad,analysis,validation,renders}
kicad-cli sch erc --format json --severity-error --exit-code-violations -o "$root/validation/erc-errors.json" "$sch"
kicad-cli pcb drc --format json --severity-all --all-track-errors --schematic-parity --exit-code-violations --units mm -o "$root/validation/drc.json" "$board"
kicad-cli sch export bom --fields 'Reference,Value,Footprint,Datasheet,Description,${QUANTITY},${DNP}' --labels 'References,Value,Footprint,Datasheet,Description,Quantity,DNP' --group-by 'Value,Footprint,Datasheet' --exclude-dnp -o "$root/manufacturing/assembly/Code-SDR-V2-BOM.csv" "$sch"
kicad-cli pcb export pos --format csv --units mm --side both --exclude-dnp -o "$root/manufacturing/assembly/Code-SDR-V2-positions.csv" "$board"
kicad-cli pcb export gerbers -l 'F.Cu,In1.Cu,In2.Cu,B.Cu,F.Paste,F.SilkS,B.SilkS,F.Mask,B.Mask,Edge.Cuts' --subtract-soldermask --precision 6 -o "$root/manufacturing/gerbers" "$board"
kicad-cli pcb export drill --format excellon --excellon-units mm --excellon-separate-th --generate-map --map-format pdf -o "$root/manufacturing/gerbers" "$board"
kicad-cli pcb export ipcd356 -o "$root/manufacturing/gerbers/Code-SDR-V2.ipc" "$board"
kicad-cli sch export pdf --black-and-white -o "$root/documents/Code-SDR-V2-schematic.pdf" "$sch"
kicad-cli pcb export pdf --mode-single --black-and-white --include-border-title --sketch-pads-on-fab-layers -l 'F.Fab,Edge.Cuts' -o "$root/manufacturing/assembly/Code-SDR-V2-top-assembly.pdf" "$board"
kicad-cli pcb export pdf --mode-single --black-and-white --include-border-title --sketch-pads-on-fab-layers --mirror -l 'B.Fab,Edge.Cuts' -o "$root/manufacturing/assembly/Code-SDR-V2-bottom-assembly.pdf" "$board"
xvfb-run -a kicad-cli pcb render --side top --quality basic --floor --perspective --width 2400 --height 1500 -o "$root/renders/Code-SDR-V2-top.png" "$board" || true
xvfb-run -a kicad-cli pcb render --side bottom --quality basic --floor --perspective --width 2400 --height 1500 -o "$root/renders/Code-SDR-V2-bottom.png" "$board" || true
kicad-cli pcb export step --force --board-only --include-tracks --include-pads --include-zones --include-inner-copper --include-silkscreen --include-soldermask -o "$root/cad/Code-SDR-V2-board.step" "$board" || true
cp hardware/v2/Code-SDR-V2.kicad_{pcb,sch,pro,dru} "$root/cad/"
cp hardware/v2/{CodeSDR.kicad_sym,fp-lib-table,sym-lib-table,README.md,ROUTE_STATUS.md} "$root/cad/"
cp -r hardware/v2/{sheets,CodeSDR.pretty,tools} "$root/cad/"
rm -rf "$root/cad/tools/__pycache__"
cp hardware/{HARDWARE_VALIDATION.md,reccomendations.md} "$root/documents/"
cp documentation/{INTEGRATION_SPECIFICATIONS.md,FPGA_ARCHITECTURE.md} "$root/documents/"
cp hardware/redesign/*.{csv,md,py} "$root/analysis/" 2>/dev/null || true
cp hardware/v2/build/{ROUTE_STATISTICS.csv,route-summary.json,validate-route.log,erc-all.json} "$root/validation/"
(cd hardware/v2/release && zip -qr Code-SDR-V2-final.zip Code-SDR-V2-final)
