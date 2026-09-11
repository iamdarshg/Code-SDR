# Code-SDR V2 routing handoff

Reviewed board: `hardware/v2/Code-SDR-V2.kicad_pcb` on branch `main`. The working tree contains many unrelated routing candidates, reports, and an edited spreadsheet; only this board, the fresh DRC JSON, and this handoff are in scope for the checkpoint commit.

## Fresh KiCad evidence

KiCad CLI 9.0.7 was rerun on 2026-09-12 with `--severity-all --all-track-errors --schematic-parity` after the final localized clearance repair.

| Finding | Count | Status |
|---|---:|---|
| DRC violations | 391 | 0 error severity, 391 warning severity |
| Unconnected items | 0 | connectivity gate passes |
| Schematic parity findings | 4 | all warnings |
| DRC error severity | 0 | critical DRC error gate passes |
| DRC warning severity | 391 | remains open |

The three error-severity clearance defects are resolved. The `RGMII_RX_DV_FPGA` route now stays above C312 until it is clear of the grounded pad before turning down; its total routed length remains 36.755057 mm. The 0.060 mm `ADC_OR` fragment beside C156 was a dangling stub and was removed without changing the connected route. The repair changed only those two nets, added no vias, moved no footprints, and reduced dangling-track warnings from 77 to 76.

The remaining warning groups are 199 library-footprint mismatch warnings, 116 dangling-via warnings, and 76 dangling-track warnings. The parity warnings identify missing PCB nets for the intentionally unconnected CT pad 5 on U5, U6, U7, and U8 (`unconnected-(U5-CT-Pad5)` through `unconnected-(U8-CT-Pad5)`). These require library/schematic intent review before release; they were not silently assigned nets.

Fresh report: `reviews/Code-SDR-V2-routing-check-20260912.json`. Board SHA-256: `ECFF356E5D136C3BF1001F4C991DEAC22CDC8F19E0EA9AFE38C846D13464E2F6`.

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

* **Warning / high:** 116 dangling vias. Many are likely stitching or one-layer route remnants. Inspect each by UUID and remove only proven unused vias; preserve RF return vias, thermal arrays, and intentional layer transitions.
* **Warning / high:** 76 dangling tracks. Inspect endpoints and net intent; reconnect or remove only definite dead-end fragments. Do not bulk-delete by coordinate window.
* **Warning / medium:** 199 library-footprint mismatches. The 373 footprints are embedded board copies that differ from current KiCad libraries, often due to user transformations or local pad/graphics data. Compare against the schematic and approved footprint revision before updating libraries; do not use “Update from Library” blindly.
* **Warning / medium:** four CT pad parity mismatches on U5–U8 at the exact pad 5 locations reported by KiCad. Confirm whether these are no-connect pins in the source symbols and add explicit no-connect/net intent only if that is the approved schematic design.
* **Review / high:** the checkpoint contains 373 footprints, 11,510 routed segments, and 1,152 vias. The localized clearance repair removed five redundant route fragments, added no vias, and did not alter any differential-pair copper. Review the board diff before any release tag.
* **Review / high:** DRC does not validate impedance, differential skew, RF return paths, current capacity, thermal behavior, or fabricator stackup. These remain separate release gates.

## Footprint side and frequency optionality

All 373 footprints are on `F.Cu` (`F.Cu=373`, `B.Cu=0`; no footprint was moved to the back). This satisfies the requested all-front placement checkpoint.

The design documentation preserves optional continuous tuning from 100 MHz through 10 GHz, including the 10 MHz-to-10 GHz-capable balun/branch components and the optional high-band input path. The routing review did not remove or collapse that optionality. The 8–10 GHz D path, high-band LNA/cascade, switch mapping, LO/IF conversion, RF50 routing, and first-article/VNA checks still need explicit validation. DRC and these routing length totals do not prove 10 GHz operation; component ratings, stackup/field-solved impedance, launch quality, isolation, and measurements through 10 GHz remain required.

## Release statement

This routing checkpoint has zero unconnected items and zero error-severity DRC violations. It is not fabrication ready: 391 DRC warnings, four parity warnings, provisional differential-pair measurements, and the independent RF/high-speed/power release gates remain.
