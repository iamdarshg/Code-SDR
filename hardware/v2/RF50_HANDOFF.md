# Code-SDR V2 RF routing handoff

## Scope

The PCB contains the completed RF, LO, and IF controlled-impedance route set. Power, digital, ground stitching, and ordinary control nets remain intentionally unrouted for manual completion.

## Validated status

- Controlled-impedance target nets: **56**
- RF/LO/IF unconnected items: **0**
- RF shorts, track crossings, and ordinary clearance conflicts: **0**
- Schematic/PCB parity findings: **0**
- Edge-launch exceptions: **5**, at the four upper RF connectors and the low-band side connector

## HDI requirement

Fifteen via-in-pad locations use **0.20 mm finished diameter / 0.10 mm drill** escapes in seven dense LMX/LNA routes:

- `LMX_LO_HIGH_N_RAW`
- `LMX_LO_LOW_N_RAW`
- `RF_HA_LNA_OUT_RAW`
- `RF_HC_LNA_OUT_RAW`
- `RF_HD_LNA_OUT_RAW`
- `RF_HD_LNA2_IN`
- `RF_HIGH_SWITCHED`

These intentionally violate the project's ordinary 0.40/0.15 mm through-via limits. Use an HDI-capable fabricator with filled and capped via-in-pad processing, or manually re-place/reroute these seven nets before fabrication.

## Impedance warning

Routing width alone does not certify 50 ohms. Recalculate the track widths against the selected fabricator's actual dielectric thickness, copper weight, solder-mask model, and material Dk. Adjust the routed widths before ordering while preserving the route topology and clearances.

## Excluded control/status nets

The following names were initially grouped with RF by naming but are not controlled-impedance signal paths and remain for manual routing:

- `RF_HA_ENABLE`
- `RF_HB_ENABLE`
- `RF_HC_ENABLE`
- `RF_HD_ENABLE`
- `RF_HD2_ENABLE`
- `LMX_LOCK`

## Before fabrication

Finish all remaining airwires, add/verify return-path stitching, rerun complete-board DRC and schematic parity, inspect the RF reference planes, and verify impedance with the final stackup. This file is an RF-routing handoff, not a fabrication release.
