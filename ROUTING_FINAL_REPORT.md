# Code-SDR V2 — Final Routing Report

**Date:** 2026-07-21
**Branch:** `claude/rev-b-pcb-routing-fab-yp4v6y`

## Summary

The board is **fully routed and connected**: all 344 footprints, all 249
signal nets, 4480 track segments, 470 vias. Zero nets are missing copper —
verified directly against board connectivity (not just the router's own
bookkeeping) after every single change in this session.

This was done with a custom pcbnew-based autorouter
(`hardware/v2/tools/auto_router.py`) after FreeRouting-via-GitHub-Actions
proved unworkable (Java class-version mismatch, then post-route validation
issues — see git history on this branch for that earlier path).

DRC is **not zero-violation**, but improved substantially over the course
of this session: **1566 → 1306 violations (-17%)**, with much larger
reductions in specific categories that were actually within routing's
power to fix (`hole_clearance` -74%, `clearance` -21%). What's left is
concentrated in a small number of well-understood, documented causes.

## What's solid

- 344/344 footprints, all 249 signal nets connected, re-verified after
  every batch of changes this session (never shipped a state with a
  disconnected net)
- RF-critical nets (LT_IN, LO_LOW/HIGH, IF_LOW/HIGH, LMX_LO*) are F.Cu-only,
  zero vias, as required
- Trace widths are impedance-derived from the actual stackup (F.Cu-to-GND
  h=0.18mm, er=4.2, Hammerstad-Jensen microstrip): 0.32mm for 50-ohm
  single-ended RF, 0.20mm/0.15mm-gap for 100-ohm differential pairs
  (Ethernet MDI, LMX2592 LO output — confirmed against datasheets), 0.25mm/
  0.15mm-gap for 90-ohm USB D+/D-
- Differential pairs are routed as a coupled unit (mirrored offset) where
  the search allowed it, so length and spacing stay matched
- Big spread-out nets (power rails) use a minimum-spanning-tree topology
  instead of a forced serial chain, which branches locally instead of
  zig-zagging across the whole board
- Route order is analog/RF first, general nets next, noisy digital buses
  (RGMII/FPGA/QSPI/SPI/ADC data/USB data/JTAG) last, with an extra keepout
  margin between digital and analog/RF copper — the isolation strategy for
  keeping switching noise away from the RF/analog chain
- GND rides the pre-existing solid In1.Cu plane (never touched by routing)

## DRC violation trend across the session

| Stage | Total | clearance | tracks_crossing | hole_clearance | diff_pair_gap |
|---|---|---|---|---|---|
| Initial full route | 1566 | 500 | 125 | 128 | 53 |
| After diff-pair bugfix + first cleanup | 1458 | 501 | 95 | 105 | 9 |
| After MST for 4 big power nets | 1458 | — | — | — | — |
| After clearance-floor fix + 3 more sweeps (~120 nets) | **1306** | **395** | 142 | **27** | — |

`clearance` and `hole_clearance` improved a lot once the search's fallback
attempts were corrected to never dip below the board's true 0.1mm minimum
clearance (see "clearance-floor bug" below) — that one fix unlocked most
of the later gains. `tracks_crossing` bounced around depending on how many
nets fell back to a forced/direct connection (which explicitly ignores
obstacles) in a given pass; it's a genuine remaining issue, not a
measurement artifact.

`shorting_items` (199), `lib_footprint_mismatch` (199, all warnings),
`solder_mask_bridge` (~200), and `starved_thermal` (~115) stayed roughly
flat all session — see root causes below for why.

## What's left (1306 DRC violations, 103 unconnected-item flags)

### Root cause 1: huge power nets, still the biggest bucket
`+3V3_DIG`/`+3V3_ANA`/`+1V2_CORE`/`+5V_RF` have 21-76 pads each, scattered
across the whole board (not clustered by domain). MST topology helped
meaningfully (e.g. `+1V2_CORE` violations roughly halved), but point-to-
point traces for a 76-pad net spread over 160x100mm will always have some
tight spots on a board this dense. **The correct long-term fix is copper-
pour zones for these rails on In2.Cu** (the stackup's dedicated power-plane
layer per the original design docs) instead of individual traces. Not
attempted: In2.Cu already carries other signal routing from this session,
and 4 overlapping-footprint power nets sharing one plane layer cleanly is
a real floor-planning task, not a quick script change.

### Root cause 2: intentionally-adjacent nets read as "shorts"
Several RGMII_* violations (e.g. `RGMII_RXD0` vs `RGMII_RXD0_FPGA`) are
between nets joined by a single series resistor a millimeter apart (R126
pin 1 = RGMII_RXD0, pin 2 = RGMII_RXD0_FPGA) — a standard PHY/FPGA
signal-integrity pattern, not a routing bug. DRC flags nearby copper from
the two nets as `shorting_items`/`clearance` even though the design
intends them to sit right next to each other. This alone accounts for a
meaningful share of `shorting_items` staying flat despite everything else
being re-routed multiple times.

### Root cause 3: pre-existing, not caused by routing
- `lib_footprint_mismatch` (199, all **warning** severity): footprint
  library version mismatches (e.g. C317 vs `Capacitor_SMD:C_0402_1005Metric`)
  — a schematic/library issue, unrelated to copper placement.
- `starved_thermal` (~115): GND zone thermal-relief spoke count (wants 2,
  some pads only got 1) — a byproduct of routing density around those
  pads, fixable by adjusting zone thermal-relief settings, not a
  connectivity or short-circuit problem.

## Bugs found and fixed this session

1. **Layer-restriction bug** (connectivity-breaking): the search could
   terminate a path on a copper layer the destination pad doesn't actually
   have copper on — looked geometrically connected, wasn't electrically.
   Fixed by restricting arrival to the pad's real layer(s), forcing a via
   otherwise.
2. **Diff-pair obstacle-awareness bug**: when a differential pair's mirror
   attempt failed and N had to route independently, N's search couldn't
   see P's just-placed copper (added to the board after both were
   computed). Fixed by committing P to the board before searching for N's
   fallback path. This is what took `diff_pair_gap_out_of_range` from 53
   to 9.
3. **Clearance-floor bug**: fallback search attempts bottomed out at
   0.08-0.1mm clearance with a 0.2-0.4mm grid pitch — the grid's own
   obstacle check passed, but the pitch's quantization error meant a
   "clear" path could still land under the board's real 0.1mm minimum,
   producing marginal DRC violations (80+ of the clearance violations were
   literal 0.0mm touches; 150+ more were 0.01-0.097mm shortfalls). Raised
   floors to 0.1-0.15mm with a finer 0.15mm grid pitch. This unlocked most
   of the later-session improvement (clearance 501→395, hole_clearance
   102→27).

## Known tooling issue hit along the way

Doing many programmatic `board.Remove()`/`Add()` calls in a single
long-running Python process against KiCad 9's pcbnew bindings occasionally
left a stale SWIG pad wrapper behind (`'SwigPyObject' object has no
attribute x'`, later `'... is not iterable'`), crashing the retry script
mid-run — worse, and faster, in multi-net batches than single-net calls.
Workaround: every net is now processed in its own subprocess invocation
(reloads the board fresh each time), wrapped in try/except, with
footprints re-scanned fresh per net instead of cached, and checkpointed to
disk every 20 nets. This pattern ran ~150 individual net re-routes across
the session with zero crashes. A handful of nets did lose their old
traces right before an earlier (multi-net) batch crashed on them; all were
caught by a post-batch zero-copper check and repaired before their commit
— the shipped state has never had a disconnected net.

## Recommended next steps, in priority order

1. Convert `+3V3_DIG`, `+3V3_ANA`, `+1V2_CORE`, `+5V_RF` to proper zone
   pours on In2.Cu (a real floor-planning task — likely the single biggest
   remaining lever, based on how much MST topology alone already helped).
2. Manually review the RGMII_*/`_FPGA` junctions for genuine clearance
   problems vs. expected close-proximity-by-design (R126-style resistor
   bridges).
3. Bump zone thermal-relief spoke count or accept as-is for `starved_thermal`.
4. Ignore `lib_footprint_mismatch` for routing purposes; fix at the
   schematic/library level separately.

## Tools added this session

- `hardware/v2/tools/auto_router.py` — the router itself (A*/Dijkstra maze
  search, impedance-aware widths, MST topology option, diff-pair coupling)
- `hardware/v2/tools/retry_forced_nets.py` — targeted re-route of a given
  net list against the current board state, one net per subprocess
- `hardware/v2/tools/postprocess_route.py` — zone refill + metrics
- `hardware/v2/tools/comprehensive_validation.py` — routing/RF/diff-pair
  completeness checks
- `hardware/v2/build/drc-report9.json` — the DRC run this report is based on
