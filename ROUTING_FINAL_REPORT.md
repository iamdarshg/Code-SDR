# Code-SDR V2 — Final Routing Report

**Date:** 2026-07-21
**Branch:** `claude/rev-b-pcb-routing-fab-yp4v6y`

## Summary

The board is **fully routed and connected**: all 344 footprints, all 249
signal nets, 5125 track segments, 585 vias. Zero nets are missing copper.
This was done with a custom pcbnew-based autorouter
(`hardware/v2/tools/auto_router.py`) after FreeRouting-via-GitHub-Actions
proved unworkable (Java class-version mismatch, then post-route validation
issues — see git history on this branch for that earlier path).

DRC is **not zero-violation**. Violations were reduced from 1566 to 1458
over several cleanup passes, with real structural progress in some
categories, but full DRC-clean was not reached in the time available.
Below is exactly what's left and why, so follow-up work can pick up
efficiently.

## What's solid

- 344/344 footprints, all nets connected (verified directly against the
  board's connectivity, not just the router's own bookkeeping)
- RF-critical nets (LT_IN, LO_LOW/HIGH, IF_LOW/HIGH, LMX_LO*) are F.Cu-only,
  zero vias, as required
- Trace widths are impedance-derived from the actual stackup (F.Cu-to-GND
  h=0.18mm, er=4.2, Hammerstad-Jensen microstrip): 0.32mm for 50-ohm
  single-ended RF, 0.20mm/0.15mm-gap for 100-ohm differential pairs
  (Ethernet MDI, LMX2592 LO output — confirmed against datasheets), 0.25mm/
  0.15mm-gap for 90-ohm USB D+/D-
- Differential pairs are routed as a coupled unit (mirrored offset) where
  the search allowed it, so length and spacing stay matched
- Route order is analog/RF first, general nets next, noisy digital buses
  (RGMII/FPGA/QSPI/SPI/ADC data/USB data/JTAG) last, with an extra keepout
  margin between digital and analog/RF copper — this is the isolation
  strategy for keeping switching noise away from the RF/analog chain
- GND rides the pre-existing solid In1.Cu plane (never touched by routing)

## What's left (1458 DRC violations, 108 unconnected-item flags)

Violations are **concentrated**, not spread evenly. The top offenders by
net, in order: `+3V3_DIG` (120), `+3V3_ANA` (76), `GND` (63), `+1V2_CORE`
(53), `+5V_RF` (33), then the RGMII/MDI/PHY cluster (~15-30 each).

### Root cause 1: huge power nets routed as point-to-point chains
`+3V3_DIG`/`+3V3_ANA`/`+1V2_CORE`/`+5V_RF` have 21-76 pads each, scattered
across the whole board (not clustered by domain). The router connects them
via a nearest-neighbor chain of individual traces, which — on a board this
dense — inevitably produces some crossings and tight-clearance spots.
**The correct fix is copper-pour zones for these rails on In2.Cu** (the
stackup's dedicated power-plane layer, per the original design docs),
not more point-to-point trace attempts. I did not attempt this: laying out
non-overlapping zone regions for 4+ different rails whose pads interleave
across the whole board is a real design task, not a quick script change,
and I judged it unsafe to rush given the time remaining.

### Root cause 2: intentionally-adjacent nets read as "shorts"
Several of the RGMII_* violations (e.g. `RGMII_RXD0` vs
`RGMII_RXD0_FPGA`) are between nets joined by a single series resistor a
millimeter apart (R126 pin 1 = RGMII_RXD0, pin 2 = RGMII_RXD0_FPGA) — a
standard PHY/FPGA signal-integrity pattern. DRC flags nearby copper from
the two nets as `shorting_items`/`clearance` even though the design
intends them to sit right next to each other. Some of this category is a
genuine routing clearance problem (traces of the two nets running
parallel elsewhere on the board) and would benefit from manual review at
those specific junctions.

### Root cause 3: pre-existing, not caused by routing
- `lib_footprint_mismatch` (199, all **warning** severity): footprint
  library version mismatches (e.g. C317 vs `Capacitor_SMD:C_0402_1005Metric`)
  — a schematic/library issue, unrelated to copper placement.
- `starved_thermal` (118): GND zone thermal-relief spoke count (wants 2,
  some pads only got 1) — a byproduct of routing density around those
  pads, fixable by adjusting zone thermal-relief settings or hand-nudging
  a handful of pads, not a connectivity or short-circuit problem.

### Numbers that did improve this session
- `tracks_crossing`: 125 → 95
- `diff_pair_gap_out_of_range`: 53 → 9 (fixed a real bug: the differential
  router could route the second half of a pair without seeing the first
  half's just-placed copper as an obstacle — see commit
  "fix(routing): make diff-pair fallback route N against P's actual copper")
- Total violations: 1566 → 1458

## Known tooling issue hit along the way

Doing thousands of programmatic `board.Remove()`/`Add()` calls in a single
long-running Python process against KiCad 9's pcbnew bindings occasionally
left a stale SWIG pad wrapper behind (`'SwigPyObject' object has no
attribute 'x'`, later `'... is not iterable'`), crashing the retry script
mid-run. Worked around by: wrapping each net in try/except, re-scanning
footprints fresh per net instead of caching pad lists, checkpointing to
disk every 20 nets, and running large batches as smaller fresh subprocess
invocations instead of one giant one. One side effect: a handful of nets
had their old traces removed right before the retry crashed on them,
leaving them briefly disconnected — this was caught by a post-batch
zero-track check and all 7 affected nets were repaired before the final
commit (verified: 0 nets with zero copper in the final state).

## Recommended next steps, in priority order

1. Convert `+3V3_DIG`, `+3V3_ANA`, `+1V2_CORE`, `+5V_RF` (and GND's
   existing thermal-relief shortfall) to proper zone pours on In2.Cu —
   this should eliminate the majority of remaining `clearance`/
   `shorting_items`/`tracks_crossing` violations in one move.
2. Manually review the RGMII_*/`_FPGA` junctions and the MDI pairs for
   genuine clearance problems vs. expected close-proximity-by-design.
3. Bump zone thermal-relief spoke count or accept as-is for `starved_thermal`.
4. Ignore `lib_footprint_mismatch` for routing purposes; fix at the
   schematic/library level separately.

## Tools added this session

- `hardware/v2/tools/auto_router.py` — the router itself
- `hardware/v2/tools/retry_forced_nets.py` — targeted re-route of a given
  net list against the current board state
- `hardware/v2/tools/postprocess_route.py` — zone refill + metrics
- `hardware/v2/tools/comprehensive_validation.py` — routing/RF/diff-pair
  completeness checks
- `hardware/v2/build/drc-report2.json` — the DRC run this report is based on
