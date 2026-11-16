# Code-SDR V2 — fabrication package, revision 2.1

**Bare board: orderable. Assembly: NOT ready** — see "What is still open" before any assembly PO.

| | |
|---|---|
| Board of record | `hardware/v2/Code-SDR-V2.kicad_pcb`, sha256 `309414D2B371C6265629352FC0AA7252AA3E3B630488849187761D24B09FF486` (5,562,840 bytes) |
| Revision | **2.1** — `title_block (rev "2.1")` in the board, and `GeneralSpecs.ProjectId.Revision = "2.1"` in the `.gbrjob` |
| Exported | 2026-11-16, `kicad-cli` 9.0.7 (`C:\Program Files\KiCad\9.0\bin\`) |
| Size | 101.600 × 101.600 mm outline (101.700 × 101.700 including the 0.100 mm Edge.Cuts stroke) |
| Layers | 4 copper — F.Cu / In1.Cu / In2.Cu / B.Cu |
| Stackup | 1 oz (0.035 mm) all four copper layers; 0.13 / 1.20 / 0.13 mm FR4; 1.600 mm finished thickness |
| Finish | ENIG (`copper_finish "ENIG"` in the board, `"Finish": "ENIG"` in the job file) |
| Drill | 2 Excellon files (PTH / NPTH separate), 1,215 hits = **1,213 drills + 2 G85 slots** |

This package was exported **from the repository board itself**, not from an edited copy. Its
gerbers are content-identical (modulo the creation-date header line) to two earlier exports of the
same board, which is how the export was checked rather than trusted.

---

## 1. Contents

```
gerbers/    11 plotted layers + .gbrjob
            L1 F.Cu  L2 In1.Cu  L3 In2.Cu  L4 B.Cu
            F.Paste  B.Paste  F.Silkscreen  B.Silkscreen  F.Mask  B.Mask  Edge.Cuts
drill/      Code-SDR-V2-PTH.drl   Code-SDR-V2-NPTH.drl   (+ gerber drill maps)
assembly/   Code-SDR-V2-pos.csv       373 rows, mm, both sides, ref-for-ref
            Code-SDR-V2-BOM-flat.csv  one row per reference (373)
            Code-SDR-V2-BOM-grouped.csv  grouped by Value+Footprint+Description+DNP
MANIFEST.sha256
```

The sheet frame and title block are **not plotted**: `--include-border-title` was not passed and
`--board-plot-params` was not passed, so no stored plot setting could re-enable it. Verified by
coordinate extent (§3.6), not by the flag alone.

The project design-rule file `hardware/v2/Code-SDR-V2.kicad_dru` is **not** duplicated here — it is
a KiCad rule file, not a fab deliverable. It carries the five SMA edge-launch exceptions: the five
edge-launch connector pads cross Edge.Cuts on purpose (maximum overhang 4.26 mm, see §3.6). A fab
quoting this board should be told that those overhangs are intentional.

## 2. How to order

Etch only. Panel is 1-up, no rails, no breakaway. Closed corners, 101.6 mm square.

## 3. Validation — every number below is parsed from the files in this directory

Checks were run against the board the package came from; the harness is in
`evidence/fab-cs-ship/validate.py`. 6 / 6 PASS.

| # | check | result |
|---|---|---|
| 3.1 | copper layers in package vs `GetCopperLayerCount()` | **4 = 4** PASS |
| 3.2 | Edge_Cuts bbox vs `GetBoardPolygonOutlines()` | **101.600 × 101.600 vs 101.600 × 101.600**, delta **0.000000 mm**; the gerber is a closed 4-segment loop (4 draw / 4 move) PASS |
| 3.3 | hole census | **1,215 hits = 1,213 drills + 2 G85 slots**; board side 1,085 vias + 128 PTH pad holes + 2 NPTH = 1,215, slots 2 — reconciles exactly PASS |
| 3.4 | `.pos` rows vs footprints | **373 rows, 373 footprints**, 0 missing, 0 extra, 0 excluded PASS |
| 3.5 | `.gbrjob` fields | LayerNumber 4, Size 101.7, thickness 1.6, finish ENIG, Revision "2.1", 11 files listed — all equal to the board PASS |
| 3.6 | frame suppression | largest coordinate in any gerber **101.600 mm**; a plotted A4 sheet (297 × 210) would reach ~200–287 mm PASS |

### 3.3 detail — slots are not drills

| file | tool definitions | distinct diameters | drills | G85 slots | hits |
|---|---:|---:|---:|---:|---:|
| `Code-SDR-V2-PTH.drl` | 13 | 12 | 1,211 | **2** | 1,213 |
| `Code-SDR-V2-NPTH.drl` | 1 | 1 | 2 | 0 | 2 |
| **total** | **14** | **13** | **1,213** | **2** | **1,215** |

Two cautions when re-reading that table:

* **14 tool definitions, 13 distinct diameters.** PTH declares `T1C0.200` (ViaDrill) and
  `T2C0.200` (ComponentDrill) — same diameter, different aperture function, so a distinct-diameter
  count and a tool-definition count legitimately differ. Quote which one you mean.
* **A slot is not a circle.** The 2 slots are pad S1 of J202/J203, a 0.6 × 1.3 mm slot. Counting
  the `G85` lines as drills would report 1,215 drills and over-count by exactly 2.

PTH drill set: 0.200 (×2 tools) · 0.250 · 0.300 · 0.400 · 0.460 · 0.600 · 0.650 · 0.850 · 0.890 ·
1.025 · 1.300 · 1.570 mm. NPTH: 3.200 mm (2 holes). Smallest finished hole **0.200 mm**; smallest
PTH pad drill 0.200 mm.

* 0 blind, 0 buried, 0 microvia — **1,085 of 1,085 vias are through vias** on F.Cu→B.Cu. No HDI,
  no sequential lamination.
* 0 footprints on B.Cu — single-sided placement. All **128 of 128** PTH pads span F.Cu and B.Cu, as
  through-hole pads must. (A "43 THT pads" figure circulates in the programme's notes; measured on
  this board it is 128, so that figure does not belong to Code-SDR V2.)

### 3.7 DRC

`kicad-cli pcb drc --severity-all` on the board this package came from:
**222 violations, 0 errors, 0 unconnected**, all 222 at `warning` severity:

| type | count | what it is |
|---|---:|---|
| `lib_footprint_mismatch` | 199 | silk/fab artwork differences between the board and the library copy; 0 pad differences |
| `via_dangling` | 13 | stubs on a net that is otherwise complete |
| `track_dangling` | 10 | same |

No copper defect and no unconnected net. With `Code-SDR-V2.kicad_dru` deleted the count is **247**
(the 25 extra are `copper_edge_clearance` on the five edge-launch connectors), which is how the rule
file is proved to be loading.

## 4. Fiducials — **the shipped board has none, and that is a finding**

**Measured, and stated plainly: `hardware/v2/Code-SDR-V2.kicad_pcb` carries 0 fiducial footprints.**
String census over the whole 5.56 MB board file returns 0 occurrences of `FID`; the same scanner
returns 9 on each of the two evidence copies, so the zero is a real absence and not a silent miss.

The frequently quoted fiducial gap does **not** describe this board. Both figures belong to
evidence-only copies that added the fiducials:

| artefact | sha256 | fiducials | FID1/FID2 pour gap | fiducial pad local clearance |
|---|---|---:|---:|---:|
| **this package's board** (`hardware/v2/Code-SDR-V2.kicad_pcb`) | `309414D2…FF486` | **0** | **n/a — no fiducial exists** | n/a |
| evidence copy "fid9", pre-fix | `B1263807…D80C84` | 9 | **1.2505 mm** | 0.750 mm |
| evidence copy "tp55", post-fix + 18 test points | `8EC5741A…1DB7AD` | 9 | **1.7505 mm** | 1.250 mm |

Both 1.2505 and 1.7505 were re-measured here with exact polygon distance, and the difference is
exactly **+0.5000 mm** — equal to the 0.750 → 1.250 mm clearance change, which is the arithmetic
control that the measurement means what it claims. The 1.22 mm figure that circulated earlier was a
0.02 mm raster floor, not a measurement.

So on the question *"is the fiducial fix present in the shipped board"*: **the requirement is not
met, and not because of a thin margin — there is no fiducial on this board to measure.** Had the
evidence copy been shipped, it would have passed the 1.25 mm rule at 1.2505, i.e. by 0.0005 mm,
which is a pass carrying no manufacturing margin; the widened copy reaches 1.7505 mm.

Fiducials are not needed to etch this board, which is why the bare-board verdict is unaffected.
They **are** needed to place parts. Adding them is a 27-object copper/mask edit plus a zone refill
already shown to hold DRC at 222 on an evidence copy — but that is a **board edit**, and it must be
committed to the board in this repository before an assembly PO. It was not done here: this package
deliberately matches its board.

## 5. What is still open

**Assembly is not ready, and this package does not make it ready.** It closes the bare-board last
mile only.

| # | blocker | size |
|---|---|---|
| 1 | **No purchasable identifier on any BOM row.** No `MPN`, `Manufacturer`, `Supplier` or `SupplierPart` property exists anywhere in the nine schematic files — 3 rows mention a part number in free-text `Description` (`01_power.kicad_sch`: C11 Murata GRM31CR71A226KE15L, C12/C346 Murata GRM32ER71A476KE15L), and no BOM tool reads that column. | **373 / 373 rows** |
| 2 | **No fiducials on the board** (§4) | 9 objects |
| 3 | No assembly drawing | 1 document |
| 4 | Test points are absent; the 18 sites proven on an evidence copy are 0.45 mm probe lands, not ATE targets — 9 of 18 cannot take a 1.0 mm land without a copper change | 18 sites |

The board is 1.7 mm over the 100 mm price tier. Re-cutting to 100 mm is a cost decision, not a
process limit.

## 6. Provenance

* Board sha256 `309414D2…` is the file git holds at `hardware/v2/Code-SDR-V2.kicad_pcb`; the working
  tree was clean before and after this export.
* The 11 gerbers are **content-identical** (after removing the `%TF.CreationDate` and
  `%TF.ProjectId` header lines, which encode the export time and the file name) to two independent
  earlier exports of the same board — the 21-layer `fabpkg` export and the 11-layer `export_cs`
  export. Comparing against known-good output is what makes these files quotable.
* `%TF.ProjectId` embeds the **file name**, so a byte-hash comparison between a package exported as
  `cs.kicad_pcb` and one exported as `Code-SDR-V2.kicad_pcb` always differs by 9 bytes per gerber.
  That is a naming artefact, not a geometry difference.