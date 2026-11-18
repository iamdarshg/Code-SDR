# Fab package for board sha256 `ffb28b7b912e…`: NOT FAB-READY (CS-G5 receipt, 2026-11-18)

This package is valid **only** for `hardware/v2/Code-SDR-V2.kicad_pcb` with sha256 `ffb28b7b912e77b0107ced50eed04e60f3d6088302d6970592a4662a4e694150`.
- Regenerate it with `tools/scripts/cs_g5_package.py` (audit workspace) or the commands in `MANIFEST_SHA256.json` (KiCad 9.0.7).
- It supersedes `board_fdef9e97/`.

## What changed since `board_fdef9e97`
1. **2ebef38:** the FPGA_AUX_2V5 track overhang at C323 was trimmed by 0.418 mm (CS-G1).
2. **This commit: all copper zones refilled.** Commit 2a2ab19 had removed 6 single-layer GND vias without refilling, which left 6 orphan clearance holes in the In2 power pours. The old Gerbers contained them. The refill adds 2.747 mm² of In2 copper (+3V3_DIG 2.286, +3V3_ANA 0.461) and touches nothing else:
   - IPC-D-356 netlist identical before and after;
   - DRC identical before and after;
   - the board diff consists only of fill-polygon coordinates.

## DRC receipt
**0 errors, 0 unconnected**, schematic parity 4 (U5–U8 `CT`, unconnected on both sides, waived). 215 warnings:
- 199 `lib_footprint_mismatch` (identity waiver);
- 16 dangling items (9 track, 7 via): 15 waived as load-bearing, plus the GRF2013_RFOUT_VDD stub, which is the owner's decision.

`drc_waivers/dangling_open2_20261118.md` records that some "load-bearing" waivers include dead overhangs past the pad they cross.

## Why it must not be ordered
- CS-G3: the LO_LOW_P/N pair crosses the In2 DIG/ANA split with no bridge capacitor.
- `validate_route` reports signal segments routed on the In1/In2 plane layers.
- 18 of 21 declared differential pairs exceed the repo's own skew limits (`docs/audit/diffpair-skew/`).
- The GRF2013 stub (about 16.5 mm of open stub on a broadband amplifier output) is undecided.
