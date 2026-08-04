# Ethernet and high-skew routing design

## Scope

Route only `MDI_A_P/N` through `MDI_D_P/N` between U30 and J30, and correct
only recognized differential pairs that currently exceed their assigned skew
tolerance or have unequal via counts.

## Routing contract

- Ethernet MDI routes use 0.20 mm tracks on F.Cu, the existing differential
  clearance, no vias, and no more than 1.0 mm pair skew.
- The analogue LO/IF/ADC routes retain their existing width families and use
  F.Cu/B.Cu only.  Each finished pair must meet its existing 0.75 mm or
  0.50 mm skew tolerance with equal via counts.
- Footprints, zones, compliant pairs and all unrelated user copper are
  immutable.  No signal route may be added to In1.Cu or In2.Cu.
- The live board is changed only after candidate DRC, preservation and pair
  measurements pass.

## Validation

KiCad DRC must not add violations, all eight MDI nets must be connected, and
each edited pair is measured after import.  Preserve a candidate/checkpoint
under `hardware/v2/build/` for review.
