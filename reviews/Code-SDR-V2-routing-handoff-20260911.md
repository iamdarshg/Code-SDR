# Code-SDR V2 routing handoff

Reviewed board: `hardware/v2/Code-SDR-V2.kicad_pcb` on branch `codex/finish-v2-pcb`, compared with `HEAD` (`8fa3ec2`). The working tree contains many unrelated routing candidates, reports, and an edited spreadsheet; only this board, the fresh DRC JSON, and this handoff are in scope for the checkpoint commit.

## Fresh KiCad evidence

KiCad CLI 9.0.7 was run on 2026-09-11 with `--severity-all --all-track-errors --schematic-parity`.

| Finding | Count | Status |
|---|---:|---|
| DRC violations | 395 | 3 error severity, 392 warning severity |
| Unconnected items | 0 | checkpoint acceptable |
| Schematic parity findings | 4 | all warnings |
| DRC error severity | 3 | remains open; checkpoint is not fabrication ready |
| DRC warning severity | 392 | remains open |

The three errors are localized clearance defects: two on F.Cu `RGMII_RX_DV_FPGA` beside C312 pad 2 GND at approximately (59.16, 65.66), and one on F.Cu `ADC_OR` beside C156 pad 1 `ADC_MODE` at approximately (67.22, 70.82). The two RGMII fragments are approximately 0.0974 mm from the required 0.1000 mm clearance; the ADC fragment is approximately 0.0864 mm. A minimal attempted RGMII detour increased other violations and was reverted. The ADC fragment is a short branch that should be removed or rerouted interactively only after confirming its intended endpoint.

The remaining warning groups are 199 library-footprint mismatch warnings, 116 dangling-via warnings, and 77 dangling-track warnings. The parity warnings identify missing PCB nets for the intentionally unconnected CT pad 5 on U5, U6, U7, and U8 (`unconnected-(U5-CT-Pad5)` through `unconnected-(U8-CT-Pad5)`). These require library/schematic intent review before release; they were not silently assigned nets.

Fresh report: `reviews/Code-SDR-V2-routing-check-20260911.json`.

## Differential pair screening

The following are all net-name pairs identifiable by the `_P`/`_N` convention. Lengths are the sum of PCB track segment Euclidean lengths for each net, including all copper layers; they are provisional because several pairs are open or contain dangling geometry. The shorter side and the measured mismatch are shown so manual matching can be performed at the actual endpoints.

| Pair | P length (mm) | N length (mm) | Shorter side | Mismatch (mm) |
|---|---:|---:|---|---:|
| AD8351_IN | 2.064 | 1.857 | N | 0.207 |
| AD8351_OUT | 5.150 | 1.827 | N | 3.324 |
| ADC_VIN | 45.742 | 41.993 | N | 3.749 |
| IF2_F1 | 5.395 | 4.016 | N | 1.378 |
| IF2_F2 | 5.208 | 5.704 | P | 0.496 |
| IF2_F3 | 7.067 | 11.918 | P | 4.851 |
| IF2_F4 | 13.330 | 14.255 | P | 0.925 |
| IF2_LPF | 10.412 | 6.694 | N | 3.717 |
| IF2 | 12.105 | 8.543 | N | 3.562 |
| IF_HIGH | 52.640 | 49.748 | N | 2.892 |
| IF_LOW | 12.563 | 12.663 | P | 0.100 |
| LO2 | 70.592 | 62.940 | N | 7.652 |
| LO_LOW | 34.750 | 34.950 | P | 0.200 |
| LT_IN | 9.345 | 9.058 | N | 0.287 |
| MDI_A | 35.440 | 42.524 | P | 7.084 |
| MDI_B | 36.527 | 35.780 | N | 0.748 |
| MDI_C | 20.876 | 18.888 | N | 1.988 |
| MDI_D | 16.365 | 18.169 | P | 1.804 |

Manual instruction: for every pair, first confirm both nets terminate at the intended pads and close the currently open/dangling sections. Then add length only to the shorter side, using symmetric impedance-controlled meanders in the same reference-plane corridor, while preserving pair spacing, via count, return path, and the board clearance rule. Re-measure after each pair and rerun DRC. Do not claim a pair is matched from these provisional totals.

## Other material physical and routing issues

* **Error / high:** the three clearance defects listed above. Move the offending short route fragments away from C312/C156 while retaining endpoint connectivity, or route them interactively with the required 0.100 mm clearance; rerun DRC.
* **Warning / high:** 116 dangling vias. Many are likely stitching or one-layer route remnants. Inspect each by UUID and remove only proven unused vias; preserve RF return vias, thermal arrays, and intentional layer transitions.
* **Warning / high:** 77 dangling tracks. Inspect endpoints and net intent; reconnect or remove only definite dead-end fragments. Do not bulk-delete by coordinate window.
* **Warning / medium:** 199 library-footprint mismatches. The 373 footprints are embedded board copies that differ from current KiCad libraries, often due to user transformations or local pad/graphics data. Compare against the schematic and approved footprint revision before updating libraries; do not use “Update from Library” blindly.
* **Warning / medium:** four CT pad parity mismatches on U5–U8 at the exact pad 5 locations reported by KiCad. Confirm whether these are no-connect pins in the source symbols and add explicit no-connect/net intent only if that is the approved schematic design.
* **Review / high:** the board diff from `HEAD` keeps 373 footprints but changes copper geometry from 10,932 to 11,515 segments and vias from 1,053 to 1,152. This is consistent with a routing checkpoint, while the edited spreadsheet and generated candidate tree are unrelated and excluded from the commit. Review the board diff before any release tag.
* **Review / high:** DRC does not validate impedance, differential skew, RF return paths, current capacity, thermal behavior, or fabricator stackup. These remain separate release gates.

## Footprint side and frequency optionality

All 373 footprints are on `F.Cu` (`F.Cu=373`, `B.Cu=0`; no footprint was moved to the back). This satisfies the requested all-front placement checkpoint.

The design documentation preserves optional continuous tuning from 100 MHz through 10 GHz, including the 10 MHz-to-10 GHz-capable balun/branch components and the optional high-band input path. The routing review did not remove or collapse that optionality. The 8–10 GHz D path, high-band LNA/cascade, switch mapping, LO/IF conversion, RF50 routing, and first-article/VNA checks still need explicit validation. DRC and these routing length totals do not prove 10 GHz operation; component ratings, stackup/field-solved impedance, launch quality, isolation, and measurements through 10 GHz remain required.

## Release statement

This is a routing checkpoint with zero unconnected items. It is not fabrication ready: three serious DRC clearance errors, 392 warnings, four parity warnings, provisional differential-pair measurements, and the independent RF/high-speed/power release gates remain.
