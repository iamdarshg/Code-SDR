# Code-SDR V2 controlled-impedance fabrication note

This note is mandatory and takes precedence over nominal trace dimensions when
the selected fabricator's stack differs from the KiCad model.

## Stackup model in the design file

| Order | Layer/material | Nominal thickness | Function |
|---:|---|---:|---|
| 1 | Top solder mask | 0.010 mm | include in field solve |
| 2 | F.Cu | 0.035 mm | signal/RF |
| 3 | FR-4 prepreg, model Dk 4.2 | 0.130 mm | outer dielectric |
| 4 | In1.Cu | 0.035 mm | uninterrupted GND reference |
| 5 | FR-4 core, model Dk 4.2 | 1.200 mm | center dielectric |
| 6 | In2.Cu | 0.035 mm | split `+3V3_ANA`/`+3V3_DIG` power |
| 7 | FR-4 prepreg, model Dk 4.2 | 0.130 mm | outer dielectric |
| 8 | B.Cu | 0.035 mm | signal/RF |
| 9 | Bottom solder mask | 0.010 mm | include in field solve |

Nominal finished board thickness is 1.60 mm and finish is ENIG. In2 is a split
power plane, not a ground plane. Do not approve a B.Cu controlled structure by
assuming a solid In2 ground reference.

## Required fabricator action

Before CAM approval, field-solve and propose production geometry using the
actual laminate/resin system, frequency-dependent Dk and loss tangent, pressed
dielectric thickness, finished/plated copper thickness, etch compensation,
trace trapezoid, and cured solder mask. Return the proposed stackup, trace
width/gap, tolerance, coupon construction, and expected impedance to design
engineering for written approval. Do not silently substitute a generic
four-layer pool stack.

- RF50: 50 ohm single-ended outer-layer target. The PCB's 0.23 mm width is a
  nominal design input, not proof of impedance. Review every top and bottom
  geometry and component/connector transition.
- USB 2.0: 90 ohm differential target, including the implemented width/gap and
  the connector/ESD/series-resistor transitions.
- RGMII: field-solve only where the final routed geometry is designated as
  controlled impedance; otherwise verify the documented single-ended geometry
  and timing-group consistency against the actual stack.

Provide controlled-impedance coupons on the production panel and report TDR
results. Lot acceptance is subject to the impedance tolerance agreed in the
written stackup approval; no tolerance is authorized merely by this note.

