# Code-SDR V2 stackup drawing

Nominal total board thickness: **1.60 mm**. Copper finish: **ENIG**.

```text
TOP
  solder mask                  0.010 mm nominal
  F.Cu signal/RF               0.035 mm nominal copper
  FR-4 prepreg (model Dk 4.2)  0.130 mm nominal pressed dielectric
  In1.Cu solid GND             0.035 mm nominal copper
  FR-4 core (model Dk 4.2)     1.200 mm nominal dielectric
  In2.Cu split power           0.035 mm nominal copper
  FR-4 prepreg (model Dk 4.2)  0.130 mm nominal pressed dielectric
  B.Cu signal/RF               0.035 mm nominal copper
  solder mask                  0.010 mm nominal
BOTTOM
```

The numerical sum of nominal materials and copper differs from the rounded
1.60 mm finished-board target; the fabricator must return its proposed pressed
stack and finished tolerance for approval. Controlled geometries must be
field-solved as required by `FABRICATOR_IMPEDANCE_NOTE.md`.
