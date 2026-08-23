# Code-SDR V2 routing audit

## Release gate

The V2 PCB must not be described as fabrication-ready until all of the following are true:

- zero KiCad unconnected items;
- zero non-library DRC errors;
- zero schematic/PCB parity findings;
- the route-invariant validator passes;
- critical microwave nets remain on `F.Cu` without signal vias;
- fabrication outputs are regenerated from the exact validated board revision.

## Best clean checkpoint before finish sweep

| Metric | Value |
|---|---:|
| Freerouting residual opens | 33 |
| KiCad unconnected items | 80 |
| Non-library DRC errors | 1 |
| Stale library warnings | 3 |
| Schematic parity findings | 3 |

The 80 KiCad items are not 80 independent signal routes. They consist of:

- 46 duplicated/disconnected optional outer-layer `GND` pour island reports;
- 4 `CHASSIS_GND` shield-chain reports;
- 20 RF/IF route gaps;
- 6 power route gaps;
- 4 digital route gaps.

The single real DRC error is a `REF_100M_ADC` track passing too close to U42 pad 4. The finish sweep tests a 0.3333 mm rightward detour through KiCad DRC before accepting it.

## Finish-sweep method

The active workflow starts from the clean routed checkpoint and evaluates every modification independently. A candidate is retained only when it:

1. does not increase non-library DRC violations;
2. does not increase signal opens;
3. does not increase schematic parity findings; and
4. improves total opens, DRC, or parity.

The sweep tests:

- reconciliation with the project-local 0805 inductor footprint;
- the known `REF_100M_ADC` clearance detour;
- removal of disconnected `F.Cu` and `B.Cu` GND pours while preserving the continuous `In1.Cu` reference plane;
- a USB shield-chain connection with an interior stitching via;
- short residual routes on both `F.Cu` and `In2.Cu`, with RF-critical nets restricted to `F.Cu`.

## Rejected routing experiments

The following experiments were measured and rejected rather than merged:

| Experiment | Result |
|---|---|
| Older partial route | 283 opens, 100 violations |
| Six-strategy tournament | best: 95 KiCad opens, 230 violations |
| Ground postprocess | 114 opens, 9 violations |
| Targeted GND stitching | no change: 114 opens |
| DRC-filtered short gaps on old checkpoint | zero retained patches |
| Dense-escape tournament | best: 68 KiCad opens, 226 violations |
| Hard-tail continuation | no importable candidate |
| RF pre-routing | plateaued at 91 router opens; invalid out-of-board RF wires |
| Local power-cluster pours | no zones created; no change |
| v19 local power pours | no zones created; no change |
| Six-layer fallback | no importable candidate |

These results are retained as workflow artifacts and PR discussions. Only the final integration PR is eligible for merge.

## Legacy V1 status

The obsolete V1 project has been removed from the active hardware source. It must not be routed or fabricated as a substitute for V2.
