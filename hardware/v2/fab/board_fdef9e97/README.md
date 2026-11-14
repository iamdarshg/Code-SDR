# Fab package for board sha256 `fdef9e97c288…` — NOT FAB-READY (CS-G5 receipt, 2026-11-15)

This package is valid **only** for `hardware/v2/Code-SDR-V2.kicad_pcb` with sha256
`fdef9e97c288d67f55aac2c4af2492c0bd4b44457da3d2adeed83f2f211a5b69` (branch `audit/code-sdr`).
If the board hash differs, this package is stale: regenerate it with the commands in
`MANIFEST_SHA256.json` (KiCad 9.0.7 `kicad-cli`).

Contents: 23 Gerber layers + job file, Excellon drill, `positions.pos` (mm), `drc_receipt.json`
(`--severity-all --schematic-parity`), and `MANIFEST_SHA256.json` (sha256 of every file, of the
board, and of the three DRC waiver files).

DRC receipt: **0 errors, 0 unconnected**, 216 warnings = 199 `lib_footprint_mismatch` (identity
waiver, pads identical) + 15 load-bearing dangling items (identity waiver) + **2 dangling items
still open** (GRF2013 RFOUT_VDD stub — owner call; FPGA_AUX_2V5 — inconclusive). Schematic parity:
4 items, the U5–U8 `CT` pins, unconnected on both sides (documented waiver).

**Why it must not be ordered:** CS-G3 (LO_LOW_P/N split) is unresolved, and
`validate_route` reports 1,890 signal segments on 84 nets routed on the In1/In2 plane layers;
the two open dangling items above are undecided. This receipt closes the *package/receipt/hash*
requirement of CS-G5 only.
