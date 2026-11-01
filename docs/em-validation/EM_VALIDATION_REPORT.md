# Code-SDR EM/RF Validation Report (V2 hardware + dev-board)

**Evidence area:** `C:\Users\Darsh Gupta\engineering-portfolio-audit\evidence\code-sdr-em`
**Date:** 2026-10-30
**Engine:** 5n7/kimi-k3 (engineering validation subagent)
**Repo refs (read-only):** Code-SDR `main` @ `4cdf7e0f4efd7b8ced1cb830cf73fb65e48e368a`; dev-board `main` @ `f9b02bf159f584cd5fc28696f905ed223af99435`
**Method:** All analysis executed locally on byte-identical copies of committed inputs. No modification was made to either original repository. No full-wave solver and no measurement hardware exist in this environment; **all full-wave / physical-RF claims FAIL CLOSED** (see §8). Nothing below is a measured S-parameter, VNA result, or full-wave simulation.

---

## 1. Scope and input set

| Input | Path (repo) | Notes |
|---|---|---|
| V2 board | `hardware/v2/Code-SDR-V2.kicad_pcb` | 99×99 mm, 4-layer, 49 RF50 nets; SHA-256 `D31019FDC24664C0868AD13716A8120C6384F582991CF18BBB64D94BD93C624D` |
| V2 schematics | `hardware/v2/sheets/*.kicad_sch` (8 sheets) | hierarchical root + subsheets |
| Stackup / impedance | `hardware/v2/STACKUP_DRAWING.md`, `FABRICATOR_IMPEDANCE_NOTE.md` | Dk 4.2, 0.13 mm outer prepreg, 0.035 mm Cu, ENIG, mask 0.010 |
| RF analysis scripts | `hardware/redesign/rf_analysis.py`, `validate_plan.py`, CSVs | committed analytical budgets |
| RF50 audit scripts | `hardware/v2/tools/audit_rf50_routes.py`, `validate_route.py`, `export_rf50_dsn.py` | committed self-checks |
| Frequency plan | `hardware/redesign/FREQUENCY_PLAN.csv` | 7 tuning rows, 2 populations |
| Dev-board | `repos/dev-board` (release/final, production fab) | IWRL6432 + CC1352 + AD9609 radar/radio 4-layer board |

Inputs hashed at `outputs/INPUT_FILES_SHA256.txt` (104 files).

---

## 2. Results summary table

| # | Check | Kind | Result |
|---|---|---|---|
| 1 | Frequency-plan arithmetic (repo `validate_plan.py`, re-run) | analytical | **PASS** (identical output to committed) |
| 2 | Conditional RF budget/NF/filter script (repo `rf_analysis.py`, re-run) | analytical/conditional | **PASS** (byte-identical regeneration; status stays `conditional_unverified_mixer_driver_gain_and_noise`) |
| 3 | RF50 copper audit (repo `audit_rf50_routes.py`) | geometry/design-rule | **FAIL** — 6 nets nonconforming widths (0.10/0.15 mm), 2 nets on unsupported layers, **5 nets incomplete connectivity** (`RF_HIGH_SWITCHED`, `RF_LOW_FILT`, `RF_LOW_LNA_IN`, `RF_LOW_LNA_OUT`, `RF_LOW_LNA_OUT_RAW`) |
| 4 | Route layer-contract validation (repo `validate_route.py`) | geometry/design-rule | **FAIL** — signal tracks on In1/In2 planes (documented contract forbids; In2 is split power, In1 solid GND). 80+ nets listed |
| 5 | DRC re-run (kicad-cli, all-track-errors) | geometry/design-rule | **FAIL** — **391 violations** (`199 lib_footprint_mismatch`, `116 via_dangling`, `76 track_dangling`), 0 unconnected |
| 6 | Quasi-static microstrip Z0 (independent) | quasi-static analytical | **REFERENCE** — nominal 0.23 mm → **46.6 Ω** (thickness-corrected), ≠ 50 Ω ideal |
| 7 | Z0 sensitivity (width/Dk/h sweeps) | quasi-static analytical | 0.15 mm → 57.2 Ω; 0.10 mm → 67.3 Ω (observed board widths); Dk 3.8–4.6 → 48.8–44.7 Ω; h 0.10–0.16 → 39.3–52.9 Ω |
| 8 | Return-path / reference-plane audit (independent) | geometry analytical | **4 nets on B.Cu** over split In2 power (`LO_LOW_P/N`, `RF_HB_LNA_OUT`, `RF_HD_LNA_OUT`); **LO_LOW_P/N cross the In2 split on B.Cu**; 6 nets use vias; stitching vias reachable ≤3 mm for all |
| 9 | Frequency-plan independent verification | analytical | **PASS** (36 checks) with **1 WARN** — second-mixer image 2353.75–2393.75 MHz, 3.75 MHz inside QPQ1909 unproved transition (2390–2393.75) |
| 10 | IF2 anti-alias ladder independent ABCD recheck | analytical | **PASS** — max |Δ| 0.000485 dB vs committed CSV (0.001 dB quantization bound); alias edge 53.75 MHz @ −22.5 dB |
| 11 | Component values / resonance sanity | analytical | **PASS** — ladder L (200/360/390 nH) & bridge C (51/56 pF) present; 100 nF couplers, 2.2 µH chokes present |
| 12 | Dev-board RF launch geometry | geometry analytical | CC1352→TR1 (0900PC15A0036) launches 2.098 mm, 0.25 mm, Top, no vias; electrical length ≤0.036 λ (2.4 GHz) |
| 13 | Dev-board release manifest integrity | hash verification | **46/49 verify; 3 stale text files** (2 BOM CSVs = checkout CRLF artifact; RELEASE_NOTES.md = genuine drift) |
| 14 | Full-wave path (openEMS/CSXCAD/HFSS/FEKO…) | environment probe | **UNAVAILABLE — FAIL CLOSED** (see §8) |

---

## 3. Frequency-plan coverage (checks #1, #9)

Independently re-derived from `FREQUENCY_PLAN.csv` (7 rows, both populations):

- **Default assembly:** continuous 100–10000 MHz (merged interval `[(100.0,10000.0)]`).
- **HF-bypass assembly:** continuous 10–10000 MHz (merged `[(10.0,10000.0)]`).
- LO ranges all inside LMX2592 20–9800 MHz (peak used 9776.25 MHz, 23.75 MHz margin below the 9800 rated limit; the repo cites 13.75 MHz margin to a 9786.25 MHz requirement — both margins satisfy the datasheet).
- Mixer limits (ADL5801 RF/LO ≤6 GHz; LTC5548 RF 2–14 GHz, LO 1–12 GHz) satisfied for every row.
- LO2 = 2400 MHz via ADF4360-1 N=480 @ 5 MHz PFD (R=20, P=16, B=30, A=0) — arithmetic consistent.
- QPQ1909 BAW passband 2402.5–2493.5 MHz covers wanted IF1 2406.25–2446.25 MHz with **zero overlap** with image 2353.75–2393.75 MHz.
- **WARN:** image upper edge 2393.75 > proved stopband ceiling 2390 MHz → 3.75 MHz of image sits in an **unproved transition region** of the QPQ1909 (Qorvo's low-side attenuation is 5 MHz-integrated, not pointwise). This is a documented limitation, exactly as the repo's own `RF_ANALYSIS.md` states; functional image rejection is not established.

`outputs/frequency_plan_verification.json` and `em_frequency_plan_run.txt`; plot `plots/frequency_plan_coverage.png`.

---

## 4. Impedance basis (checks #6, #7) — quasi-static, clearly labeled

Nominal RF50 geometry (F.Cu microstrip, w=0.23 mm, h=0.13 mm, t=0.035 mm, Dk=4.2, no mask model):

- **Z0 ≈ 46.6 Ω** (Hammerstad-Jensen with thickness correction; εeff 3.234). The 0.23 mm × 0.13 mm geometry *does not compute to 50 Ω* under this closed-form model; it is a 46–48 Ω design input needing a fabricator field solve (`FABRICATOR_IMPEDANCE_NOTE.md` correctly requires this and does not claim 50 Ω).
- Widths actually observed on the committed board for protected RF50 nets: `{0.23: 322, 0.15: 240, 0.10: 25}` segments. Quasi-static Z0 for 0.15 mm ≈ 57 Ω and 0.10 mm ≈ 67 Ω (vs 46.6 Ω at 0.23 mm) — i.e., the committed board routes a meaningful share of RF50 copper at impedance-derating widths (0.15/0.10 mm), which conflicts with the nominal 0.23 mm 50-Ω contract. These are model grades (no field solve, no roughness, no mask), given as sensitivity evidence, not part of an acceptance claim.
- Sweeps: width 0.18–0.30 mm → 52.7–40.3 Ω; Dk 3.8–4.6 → 48.8–44.7 Ω; prepreg h 0.10–0.16 → 39.3–52.9 Ω. Fab tolerances alone move Z0 ±6 Ω/±4 Ω (h, Dk).
- Model attenuation per 10 mm at 2.436 GHz ≈ 0.083 dB (conductor) + 0.080 dB (dielectric, tanD 0.02) = 0.16 dB; skin depth 1.34 µm. Longest RF50 net 95.44 mm is 1.395 λ at 2.436 GHz (quasi-static basis) — electrically long; launches/vias dominate residual uncertainty.

`outputs/microstrip_quasistatic.json`, `em_microstrip_quasistatic_run.txt`; plot `plots/microstrip_quasistatic.png`.

---

## 5. Return-path / reference-plane audit (check #8) — analytical geometry

Stackup zones (from board): `F.Cu GND` zone, `In1.Cu GND` solid, `In2.Cu` split `+3V3_ANA` (y<45.3) / `+3V3_DIG` (y>45.3), no In2 ground. B.Cu has a 344-via GND-stitching budget.

RF50 nets **on B.Cu** (microstrip referenced to split *power*, not ground):

| Net | Length (mm) | B.Cu segs | Crosses In2 split? | Vias | GND-stitch ≤3 mm |
|---|---:|---:|---|---:|---:|
| `LO_LOW_N` | 34.95 | 2 | **yes** | 2 | 2 |
| `LO_LOW_P` | 34.75 | 2 | **yes** | 2 | 2 |
| `RF_HB_LNA_OUT` | 42.81 | 4 | no | 2 | 2 |
| `RF_HD_LNA_OUT` | 15.32 | 4 | no | 2 | 2 |

Plus `IF_HIGH_SE` and `IF_SELECTED` use vias (F.Cu↔B.Cu transitions) with reachable stitch vias.

**Analytical conclusions (no full-wave claim):**
1. `LO_LOW_P/N` (the low-band mixer LOs, ~1.3–4.9 GHz) route 34.8–35.0 mm on B.Cu and **cross the In2 power split**. Across a split, the return-current path must detour; without a documented (and field-solved) treatment this is a return-path risk consistent with the repo's own README warning ("B.Cu controlled signals need an explicitly reviewed return path because their adjacent In2 reference is a split power plane").
2. `RF_HB_LNA_OUT` (4.2–6.8 GHz LNA output, 42.8 mm) and `RF_HD_LNA_OUT` sit over split power on B.Cu — same class of risk.
3. All flagged nets have a GND stitch via within 3 mm, which mitigates but does not eliminate the discontinuity; impedance continuity cannot be claimed without EM field solve or TDR.

`outputs/return_path_analysis.json`, `em_return_path_run.txt`.

---

## 6. Committed self-check gates on the V2 board (checks #3–#5)

These are the repo's OWN analytical/design-rule gates, run in a copy:

- **`audit_rf50_routes.py` — FAIL.** `widths={0.23:322, 0.1:25, 0.15:240}`; 10 unique width/layer problem classes (0.100 mm on `GRF2013_RFOUT_VDD`, `LTC_LO_IN`; 0.150 mm on 6 nets; B.Cu/inner tracks on `IF_HIGH_SE`, `IF_SELECTED`); **5 nets have incomplete pad–track connectivity** (`RF_HIGH_SWITCHED`, `RF_LOW_FILT`, `RF_LOW_LNA_IN`, `RF_LOW_LNA_OUT`, `RF_LOW_LNA_OUT_RAW`).
- **`validate_route.py` — FAIL.** Signal tracks found on In1/In2 (documented as solid-GND / split-power only): 80+ net names incl. RGMII, ADC, clocks, `LO2_P/N`, `IF_HIGH_SE`. Route-statistics CSV still emits (261 rows) but the gate raises before writing assertions.
- **kicad-cli DRC (all-track-errors) — FAIL, 391 violations / 0 unconnected.** Breakdown: 199 `lib_footprint_mismatch`, 116 `via_dangling`, 76 `track_dangling`. 116 dangling vias + 76 dangling tracks indicate the committed routed board is not in the clean state its documentation claims, and mask/parity review is incomplete.

**Interpretation (analytical, no hardware):** the committed `Code-SDR-V2.kicad_pcb` is **not first-article release-ready on its own documented gates**, and must not be cited as pass. Optionally, ROUTE_STATISTICS can support re-work: full per-net geometry is in `outputs/rf50_net_geometry.json`.

---

## 7. IF2 anti-alias filter (checks #10, #11) — analytical recheck

Independent ABCD re-derivation from documented values (L 200/360/390/360/200 nH, bridge C 51/56/56/51 pF → half-circuit 102/112/112/102 pF, finite ports LT5560 1200∥0.7 pF + R79/R80 100 Ω + 2×2.2 µH chokes + AD8351 5 k∥0.8 pF + 2×100 nF couplers):

- Point-by-point vs committed `FILTER_RESPONSE.csv`: **max |Δ| = 0.000485 dB** (0.001 dB CSV quantization). Model is transcription-consistent.
- Wanted band 6.25–46.25 MHz, first alias edge 53.75 MHz at **−22.5 dB** (finite ports, ideal coils). Q50/Q30 variants: −23.2/−23.7 dB at edge; 46.25 MHz in-band finite-port/Q50 = −2.63 dB. These remain **model grades** — no real coil Q/SRF, no layout/tolerance effects.
- Ladder internal section resonances land inside the band by design (Chebyshev-type ladder); flatness is bounded by the FILTER_RESPONSE wanted-band attenuation, verified independently.

`outputs/filter_independent_recheck.json`, `em_filter_recheck_run.txt`; plot `plots/if2_filter_response.png`.

---

## 8. Full-wave / physical-RF path — FAIL CLOSED

**Available:** none. Probes run and logged in `outputs/ENVIRONMENT.md`:
- `openEMS`, `csxcad`, `AppCSXCAD`, `openems_cli`, `feko`, `hfss` not on PATH; no EM solver under `C:\Program Files`, `C:\Program Files (x86)`, `D:\`, Octave package dir, or Python (`import openEMS` → ModuleNotFoundError).
- scikit-rf 1.9.0 is present but unused (no measured Touchstone data exists to load; synthesizing S-parameters from nothing would be fabrication).
- No VNA/instrumentation connected; no physical hardware to measure.

**Consequence:** this evidence area contains **analytical / quasi-static / design-rule results only**. No statement in this report (or in any derived materials) claims physical RF performance, impedance certification, image rejection, sensitivity, or EMI behavior from measurements or full-wave simulation. Any such claim requires: (a) an actual full-wave solver (openEMS/CSXCAD field-solve of the exported board with the documented stackup and fabricator Dk/loss data, or equivalent), and (b) first-article VNA/TDR/NF/PLL per `FIRST_ARTICLE_VALIDATION.md`. Those steps are **not** satisfied here.

---

## 9. Dev-board (independent board) — feasible analytical checks only

No existing dedicated EM/full-wave validation path exists in the repo (docs contain DRC/ERC, differential pair length-matching, SHA-256 release manifest; the RF content is module-level CC1352/IWRL6432 with vendor-matched balun IPC and explicit "Tune with VNA" deferrals). Feasible analytical checks run:

1. **RF launch geometry** (`dev-real-production.kicad_pcb`): nets `/XCC1352R1F3RGZR/RF_P_2_4GHZ`, `RF_N_2_4GHZ`, `RF_P_SUB_1GHZ`, `RF_N_SUB_1GHZ` → `TR1` (Johanson 0900PC15A0036 IPC): **2.098 mm, 0.25 mm, Top Layer, zero vias, P/N skew 0.000 mm** (matches committed length audit). Electrical length ≤0.036 λ at 2.4 GHz → electrically short launches, but **no impedance claim** — the design itself defers tuning to VNA (`XCC1352R1F3RGZR_breakout.kicad_sch` text: "Tune with VNA on final PCB/antenna layout"). No full-wave/measurement executed.
2. **Release manifest integrity:** `release/final/SHA256SUMS.csv` → **46/49 verify**; 3 text files differ (all binaries — Gerbers, drill, PDFs — verify).
   - `dev-real-bom.csv` / `dev-real-production-bom.csv`: raw git blobs equal the manifest hashes (`1cac2110…`); on-disk bytes differ (`593e3494…`) only because this checkout has `core.autocrlf=true` (LF→CRLF). **Checkout artifact, not tampering** — but worth knowing: hashing checked-out files on a CRLF Windows checkout cannot reproduce the manifest.
   - `RELEASE_NOTES.md`: manifest `b4d69beb…` matches **no** committed blob (HEAD `d3b04b4f…`, worktree CRLF `a460290a…`). **Genuine manifest staleness** — the recorded hash corresponds to no stored or on-disk version; the manifest should be regenerated from the intended release bytes.
3. No other EM path exists to run; the IWRL6432 mmWave radar is a TI module whose RF behavior is defined by the vendor module/datasheet and final antenna, not by this PCB's traces.

Evidence: `outputs/devboard_rf_launch_geometry.json`, `devboard_stackup_scan.txt`, `devboard_rules_scan.txt`, manifest verification logs §10.

---

## 10. Identified issues (analytical; no fix applied — read-only mission)

| ID | Severity | Finding | Evidence |
|---|---|---|---|
| EM-1 | **High** | Committed V2 board fails its own RF50 copper audit: 5 nets incomplete, 0.10/0.15 mm widths on protected RF nets | `audit_rf50_routes_run.txt` |
| EM-2 | **High** | Committed V2 board fails its own routing contract: signal tracks on In1/In2 planes | `validate_route_run.txt` |
| EM-3 | **High** | DRC 391 violations (116 dangling vias + 76 dangling tracks + 199 lib mismatches) on the committed board | `drc_v2_recheck.json` |
| EM-4 | **High** | 4 RF50 nets on B.Cu over split power; `LO_LOW_P/N` crosses the In2 split — return-path discontinuity risk | `return_path_analysis.json` |
| EM-5 | **Medium** | Nominal 0.23 mm × 0.13 mm computes to 46.6 Ω, not 50 Ω (quasi-static); board also uses 0.15/0.10 mm widths (57/67 Ω) | `microstrip_quasistatic.json` |
| EM-6 | **Medium** | Second-mixer image extends 3.75 MHz into QPQ1909 unproved transition region | `frequency_plan_verification.json` |
| EM-7 | **Low** | Dev-board RELEASE_NOTES.md SHA-256 manifest stale (no matching blob); BOM CSVs not reproducible on CRLF checkouts | §9 |

None of these were "fixed" — the mission is validation-only. All findings are from committed data.

---

## 11. Integrity & provenance artifacts

- `outputs/INPUT_FILES_SHA256.txt` — 104 input files (committed repo copies).
- `outputs/GENERATED_FILES_SHA256.txt` — 29 generated outputs/plots.
- `outputs/COMMANDS.md` — exact commands used.
- `outputs/ENVIRONMENT.md` — tool versions + fail-closed full-wave probe detail.
- Original repos untouched: Code-SDR working tree dirty baseline (66 pre-existing porcelain lines) recorded before and after; dev-board clean before and after; no git mutations performed.

## 12. Bottom line

- **Analytical EM state of Code-SDR V2:** frequency plan, IF2 filter model, and component values are consistent (with the documented 3.75 MHz BAW-transition caveat). **But the committed board fails its own design-rule/return-path gates (EM-1…EM-4), and Z0 does not close at 50 Ω quasi-statically (EM-5).** The design is a well-documented candidate, **not** fabricate-ready on these gates.
- **Full-wave and measured claims: none.** Every path requiring a solver or instrument fails closed and is documented as such. Any future "EM validated" claim must cite an executed openEMS/full-wave run or VNA/TDR data with raw outputs, per the rules of this audit program. — End of report.