# CS-G3 — LO_LOW_P/N split: measured, not resolved, not justifiable from geometry

Board: `hardware/v2/Code-SDR-V2.kicad_pcb` on `audit/code-sdr` (same file as the CS-G5 package `board_fdef9e97`).
Scripts (in this folder): `cs_g3_lo_low_geometry.py <board> <out.json>`, `cs_g3_return_caps.py <board> 24.8 45.17` (KiCad 9.0.7 pcbnew).
Raw: `lo_low_geometry.json` (per-0.25 mm samples of the In1/In2 fill under each trace), `return_path_caps.json`.

## What the pair is routed over

| item | value |
|---|---|
| trace width | 0.23 mm, both nets |
| layers | F.Cu breakout (P 3.9 mm, N 2.9 mm) → via → B.Cu run (P 30.9 mm, N 32.0 mm) → via → F.Cu |
| pair centre distance (same layer) | min 0.50, median 0.60, max 1.62 mm (edge gap ≈ 0.37 mm) |
| B.Cu → adjacent plane | In2.Cu, 0.13 mm dielectric |
| B.Cu → In1 (GND) | 0.13 + 0.035 + 1.2 mm = 1.37 mm, **behind In2** |
| In2 under the B.Cu run | **+3V3_DIG ≈ 55 %** (P 66 / N 77 samples), **+3V3_ANA ≈ 45 %** (P 50 / N 43 samples) |
| split crossing | y ≈ 45.05 → 45.30 mm at x = 24.5 (P) / 25.1 (N): a 0.25 mm gap between the DIG and ANA pours |

So the earlier EM report's statement is confirmed: the low-band mixer LO (~1.3–4.9 GHz) runs on B.Cu referenced to a **split power plane**, and crosses the split. Two further facts the earlier report did not state:

1. More than half the LO run is referenced to the **digital** 3.3 V plane, so the LO's return current shares copper with digital supply noise.
2. The board has **no capacitor between +3V3_DIG and +3V3_ANA anywhere** (0 found). The only way the return current can cross the split is plane → decoupling cap → GND → decoupling cap → plane. The nearest caps to the crossing point (24.8, 45.17) are:
   - +3V3_ANA–GND: C279 10 µF / C278 100 nF at **9.0 mm**, C277 at 9.6 mm
   - +3V3_DIG–GND: C340 100 nF at **19.2 mm**, C341 at 21.6 mm

   At 2.4 GHz, assuming FR-4 (εr ≈ 4.2, εeff ≈ 3.2, λg ≈ 70 mm), a 19 mm detour is about a quarter of a guided wavelength, which is not a small discontinuity.

## Does "it's a differential pair" justify it?

Only partly, and not enough to pass the gate:
- **Pure odd mode:** the opposite return currents under P and N can close laterally along the gap edge, over a loop about the 0.6 mm pair pitch wide. That is a small discontinuity, and it is the standard argument for letting differential pairs cross splits.
- **Coupling is weak:** at s/h ≈ 2.8 (0.37 mm gap over a 0.13 mm dielectric), each trace couples far more strongly to In2 than to its partner. Any imbalance therefore puts its return current in the plane. Sources of imbalance here: local length mismatch on the split layer (P 30.9 mm vs N 32.0 mm on B.Cu; overall pair skew is only 0.20 mm, 34.75 vs 34.95 mm, because F.Cu compensates, so the imbalance sits in the split region), via stagger at both ends, and an asymmetric crossing position.
- **Common mode** sees the full split, with the 9–19 mm cap detour above.

Separating these needs a field solve or a TDR on a built board. Neither is available here: the CS-G6 full-wave solver is blocked, with no openEMS/CSXCAD/HFSS. I am not putting a dB figure on it.

## Verdict: CS-G3 FAIL (unchanged), now quantified

The gate asks for the split to be "resolved or justified". It is neither. The geometry does not support a justification, because of the weak coupling, the 1.1 mm local length mismatch on the split layer and the absence of any bridge capacitor. Resolving it is a layout decision for the owner. The options, cheapest first:

1. **Stitching caps across the crossing.** Add two 0402 caps (e.g. 100 pF–1 nF, self-resonance above the LO band), each +3V3_DIG↔+3V3_ANA or each plane↔GND, within ~1 mm of (24.5–25.1, 45.2) on B.Cu. This shortens the return detour from 9–19 mm to about 1 mm. Cheapest fix, but it does not remove the DIG-plane reference.
2. **Re-pour In2 under the LO corridor.** Extend +3V3_ANA (or a GND island stitched to In1) under the whole B.Cu run, x ≈ 23–26, y ≈ 31–63. This removes both the crossing and the DIG reference.
3. **Move the run to F.Cu over solid In1 GND.** Right in principle, but it competes with the F.Cu RF corridor.

No board edit was made by this audit.
