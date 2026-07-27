# One-pass preservation-first Freerouting design

## Objective

Run exactly one Freerouting autoroute pass against the current uncommitted
Code-SDR V2 PCB. Improve or complete recognized differential nets first, then
connect as many other currently open non-ground nets as possible. Preserve all
existing user work outside the differential-net allowlist.

## Authoritative input

The authoritative input is the working-tree version of
`hardware/v2/Code-SDR-V2.kicad_pcb`, not `HEAD` and not a regenerated board.
The working project currently includes user placement, rotation, routing, via,
zone and project-rule edits that must survive.

Before export, copy the working PCB and project file into a timestamped
gitignored checkpoint directory and record SHA-256 hashes. All routing and SES
import work happens against a separate candidate copy. The live PCB is replaced
only after the acceptance gates pass.

## Editable differential-net allowlist

Existing copper may be adjusted only on these recognized pairs:

- USB: `USB_DP_CONN`/`USB_DM_CONN` and `USB_DP`/`USB_DM`.
- Ethernet MDI: `MDI_A_P`/`MDI_A_N` through `MDI_D_P`/`MDI_D_N`.
- LO and mixer: LMX low-band raw, ADF second-LO raw, `LO2`, `LO_LOW`,
  `IF_LOW`, `IF_HIGH`, and `LT_IN` positive/negative pairs.
- IF and ADC analogue: `IF2`, all four `IF2_F*` sections, `IF2_LPF`,
  `AD8351_IN`, `AD8351_OUT`, and `ADC_VIN` positive/negative pairs.

Completed pairs that already meet their tolerance and have balanced via counts
remain protected; this includes the current exactly matched connector-side USB
pair. Only incomplete or out-of-tolerance pair copper is unlocked for gentle
optimizer movement or, when necessary, rerouting.

## Protection and routing contract

1. Export a Specctra DSN from the checkpointed working board.
2. Mark every pre-existing track and via outside the differential allowlist as
   protected wiring. Within the allowlist, unlock only incomplete or
   out-of-tolerance pairs; preserve already-compliant pairs.
3. Mark both `In1.Cu` and `In2.Cu` as power/reference layers. Freerouting may
   create signal tracks only on `F.Cu` and `B.Cu`.
4. Preserve the two internal GND zones and exclude GND from track routing.
5. Put differential classes before ordinary digital, power and default classes
   so the single pass attempts differential connectivity first.
6. Use the current board widths/clearances and the existing 0.45/0.20 mm
   through-via definition. Do not enable blind, buried or microvias.
7. Run official Freerouting v2.2.4 headlessly exactly once with maximum passes
   set to one. Normal shove/pull and trace optimization are allowed inside this
   invocation. No second router execution is permitted.
8. Import the resulting SES into the candidate copy only.

## Preservation verification

Capture geometric signatures before routing and compare them with the imported
candidate. The candidate is rejected unless:

- Every footprint retains the same reference, position, rotation and side.
- Board outline, stackup and every zone retain the same geometry, layer and net.
- Every pre-existing non-differential segment and via remains geometrically
  present with the same net, layer, width and drill/diameter.
- Existing RF50 copper outside the unlocked differential subset is unchanged
  and remains connected. Any unlocked RF/IF pair must retain its assigned
  width, outer-layer/reference-plane contract and balanced via count.
- No signal item is placed on `In1.Cu` or `In2.Cu`.
- Schematic parity remains unchanged and KiCad reports no new DRC violations
  relative to the pre-route checkpoint.

## Routing-quality verification

The result must reduce the number of unconnected items. Completed pair members
must have equal via counts and meet the existing route validator tolerances:

- USB and Ethernet MDI pairs: no more than 1.0 mm length skew.
- LO/mixer pairs: no more than 0.75 mm length skew.
- IF/ADC analogue pairs: no more than 0.50 mm length skew.

For a pair that Freerouting cannot complete in the single pass, preserve the
best candidate only if its skew does not regress and all preservation gates
still pass. Report before/after lengths, skew, via counts, total opens, track
count, via count and DRC deltas.

## Failure handling

Any export, router, SES import or acceptance-gate failure leaves the working
PCB and project files byte-for-byte untouched. Keep the checkpoint, DSN, SES,
candidate PCB and logs in the gitignored build directory for inspection and
report the failed gate. Do not silently fall back to a second pass, regenerate
the board, or reset user changes.
