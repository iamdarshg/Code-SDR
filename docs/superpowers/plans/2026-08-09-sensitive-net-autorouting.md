# Sensitive-Net Autorouting Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Route and tune only the PCB's skew-sensitive differential and source-synchronous nets while preserving all existing out-of-scope board work.

**Architecture:** A candidate-only Python/pcbnew workflow snapshots the live board, deletes only failing selected-net copper, routes paired and grouped signals through obstacle-aware corridors, tunes geometric length, and promotes only after KiCad DRC and preservation gates pass. The live board is backed up before every run and remains unchanged on any failure.

**Tech Stack:** KiCad 9 `pcbnew`, `kicad-cli pcb drc`, Python 3.12 standard library, `unittest`, existing `freerouting_contract.py` board measurement helpers.

## Global Constraints

- The selected-net allowlist is exactly the scope in `docs/superpowers/specs/2026-08-09-sensitive-net-autorouting-design.md`.
- Do not move or rotate footprints.
- Do not modify non-selected tracks, vias, board drawings, zone outlines, net assignments, net classes, stack-up, or schematic.
- Retain four copper layers and board area below 100 cm2.
- Work on a timestamped candidate and create a byte-for-byte live-board backup first.
- Reject rather than promote a candidate that adds DRC errors or misses any connectivity, skew, via-parity, or preservation gate.

---

### Task 1: Encode routing groups and manufacturer-derived budgets

**Files:**
- Create: `hardware/v2/tools/sensitive_route_contract.py`
- Create: `hardware/v2/tools/tests/test_sensitive_route_contract.py`

**Interfaces:**
- Produces: `RoutePair`, `RouteBus`, `PAIR_GROUPS`, `BUS_GROUPS`, `SELECTED_NETS`, and `contract_errors(board_path: Path) -> list[str]`.
- Consumes: `measure_pairs(Path)` from `freerouting_contract.py` for legacy pair measurements.

- [ ] **Step 1: Write the failing contract test**

```python
def test_sensitive_contract_covers_all_approved_groups():
    assert PAIR_GROUPS["ADF second LO"].max_skew_mm == 0.25
    assert PAIR_GROUPS["ADC analog input"].max_skew_mm == 0.50
    assert PAIR_GROUPS["USB"].differential_impedance_ohm == 90
    assert BUS_GROUPS["RGMII TX"].max_spread_mm == 2.00
    assert BUS_GROUPS["RGMII RX"].max_spread_mm == 2.00
    assert BUS_GROUPS["ADC data"].max_spread_mm == 2.00
    assert "ADC_CLK" in SELECTED_NETS
```

- [ ] **Step 2: Run the test and confirm it fails because the module is absent**

Run: `C:\Program Files\KiCad\9.0\bin\python.exe -m unittest hardware.v2.tools.tests.test_sensitive_route_contract -v`

- [ ] **Step 3: Implement immutable contract records and the exact allowlist**

```python
@dataclass(frozen=True)
class RoutePair:
    positive: tuple[str, ...]
    negative: tuple[str, ...]
    max_skew_mm: float
    differential_impedance_ohm: int | None = None
    require_equal_vias: bool = True

@dataclass(frozen=True)
class RouteBus:
    members: tuple[tuple[str, ...], ...]
    max_spread_mm: float
```

Populate these records directly from the approved specification, including resistor-split RGMII members as `(base, base + "_FPGA")` tuples.

- [ ] **Step 4: Implement `contract_errors`**

Measure track centerline length, segment count, via count, and KiCad connectivity for every selected member. Report open members, pair skew above budget, bus spread above budget, and unequal pair via counts with exact numeric values.

- [ ] **Step 5: Run the contract tests**

Run: `C:\Program Files\KiCad\9.0\bin\python.exe -m unittest discover -s hardware/v2/tools/tests -p "test_*.py"`
Expected: all tests pass.

### Task 2: Implement candidate backup and preservation gates

**Files:**
- Create: `hardware/v2/tools/sensitive_route_preservation.py`
- Modify: `hardware/v2/tools/tests/test_sensitive_route_contract.py`

**Interfaces:**
- Produces: `BoardSnapshot.capture(board_path, selected_nets)`, `BoardSnapshot.compare(candidate_path) -> list[str]`, and `create_run_directory(live_board, build_root) -> RunPaths`.
- Consumes: `_track_signature`, `_zone_signature`, and `_footprint_signature` from `freerouting_contract.py`.

- [ ] **Step 1: Add a failing preservation test**

Create a temporary candidate, modify one non-selected track signature, and assert that comparison returns `non-selected copper changed`. Also assert that selected-net changes are accepted.

- [ ] **Step 2: Run the focused test and confirm failure**

Run: `C:\Program Files\KiCad\9.0\bin\python.exe -m unittest hardware.v2.tools.tests.test_sensitive_route_contract.SensitivePreservationTests -v`

- [ ] **Step 3: Implement timestamped run paths and byte-for-byte backup**

Each run directory contains:

```text
live-before-sensitive-autoroute.kicad_pcb
candidate.kicad_pcb
baseline-drc.json
candidate-drc.json
preservation.json
measurements.json
run.json
```

Use `shutil.copy2` and verify the backup SHA-256 equals the live file before candidate work.

- [ ] **Step 4: Implement preservation comparison**

Compare counters of all non-selected tracks/vias, footprint position/orientation/side, zone outline signatures, drawings, copper-layer count, net names/codes, and board-edge bounding box. Return deterministic errors rather than mutating the board.

- [ ] **Step 5: Run the focused and full tests**

Run the focused command from Step 2, then full test discovery. Expected: all pass.

### Task 3: Implement obstacle-aware paired routing

**Files:**
- Create: `hardware/v2/tools/sensitive_pair_router.py`
- Modify: `hardware/v2/tools/tests/test_sensitive_route_contract.py`

**Interfaces:**
- Produces: `route_pair(board, pair, geometry, pcbnew) -> RouteResult` and `tune_pair(board, pair, target_skew_mm, pcbnew) -> RouteResult`.
- Consumes: selected pair records and board obstacle geometry.

- [ ] **Step 1: Add failing geometry tests**

Test that paired paths use equal layer transitions, equal via counts, required center spacing, and reject paths intersecting inflated pad/track/keepout obstacles.

- [ ] **Step 2: Confirm geometry tests fail**

Run the focused `SensitivePairRouterTests` test case.

- [ ] **Step 3: Implement paired path search**

Use a 0.20 mm grid and A* state `(x, y, layer, heading)`. Inflate obstacles by track half-width plus KiCad clearance. Search a centerline and validate both conductor offsets at every step. Penalize corners, vias, layer changes, and departure from the direct route. Permit only F.Cu/B.Cu and paired F/B vias.

- [ ] **Step 4: Implement route emission**

Emit both conductors together with identical segment topology and via placement. Use the board's assigned differential width/gap; use 90-ohm USB geometry from the project rule when available. Never emit one conductor if the companion path fails validation.

- [ ] **Step 5: Implement pair tuning**

Add symmetric accordion folds in a verified clear tuning window. Tune the shorter member to the group budget while maintaining pair spacing and equal transition topology. Re-measure after emission.

- [ ] **Step 6: Run geometry and full tests**

Expected: obstacle, topology, and tuning tests pass.

### Task 4: Implement grouped ADC and RGMII routing

**Files:**
- Create: `hardware/v2/tools/sensitive_bus_router.py`
- Modify: `hardware/v2/tools/tests/test_sensitive_route_contract.py`

**Interfaces:**
- Produces: `route_bus(board, bus, pcbnew) -> list[RouteResult]` and `tune_bus(board, bus, pcbnew) -> list[RouteResult]`.
- Consumes: `RouteBus`, obstacle-map utilities, and split-net member definitions.

- [ ] **Step 1: Add failing grouped-routing tests**

Assert that total RGMII length includes both sides of each 33-ohm resistor, no intentional clock delay is inserted, ADC data spread is at most 2.00 mm, and `ADC_CLK` receives the shortest obstacle-clear route.

- [ ] **Step 2: Confirm the grouped tests fail**

Run the focused `SensitiveBusRouterTests` case.

- [ ] **Step 3: Implement shared-corridor bus routing**

Order pads along each endpoint bank, allocate parallel lanes through a shared corridor, and route members sequentially while treating accepted lanes as obstacles. Preserve pin order where possible to minimize crossings and vias.

- [ ] **Step 4: Implement resistor-aware total-length measurement**

For RGMII, sum base and `_FPGA` segment lengths around the series resistor. Tune only straight segments away from the PHY, FPGA, and resistor escape regions.

- [ ] **Step 5: Implement grouped tuning**

Select the longest member as the group reference and add obstacle-checked accordion length to shorter members until spread is at most 2.00 mm. Keep `ADC_CLK`, `RGMII_TXC`, and `RGMII_RXC` free of deliberate phase-delay meanders.

- [ ] **Step 6: Run grouped and full tests**

Expected: all tests pass.

### Task 5: Build the candidate-only orchestration command

**Files:**
- Create: `hardware/v2/tools/run_sensitive_autoroute.py`
- Modify: `hardware/v2/tools/tests/test_sensitive_route_contract.py`

**Interfaces:**
- Produces CLI: `run_sensitive_autoroute.py --board PATH --build-root PATH [--promote] [--time-limit-seconds N]`.
- Consumes all contract, preservation, pair-router, and bus-router modules.

- [ ] **Step 1: Add failing dry-run and rollback tests**

Assert that default mode never changes the live board, a failed gate leaves the live SHA-256 unchanged, and `--promote` creates a verified backup before replacing the live board.

- [ ] **Step 2: Confirm orchestration tests fail**

Run the focused `SensitiveAutorouteWorkflowTests` case.

- [ ] **Step 3: Implement orchestration**

Create run paths, calculate baseline DRC, snapshot preservation state, copy the candidate, remove only failing selected-net copper, route pairs and buses in priority order, refill zones in a separate KiCad Python process, and run candidate DRC.

- [ ] **Step 4: Implement strict promotion gates**

Reject on router timeout, no-path result, selected open endpoint, tolerance failure, via mismatch, new DRC finding, preservation difference, non-four-layer result, or board area at least 100 cm2. Write every error to `run.json`.

- [ ] **Step 5: Implement atomic promotion and rollback**

Copy the verified candidate to a temporary sibling, replace the live board, rerun live DRC and contract checks, then restore the backup immediately if live verification differs.

- [ ] **Step 6: Run orchestration and full tests**

Expected: all tests pass and dry-run leaves the live PCB hash unchanged.

### Task 6: Execute, verify, and package the routed PCB

**Files:**
- Modify only on successful promotion: `hardware/v2/Code-SDR-V2.kicad_pcb`
- Create: timestamped reports under `hardware/v2/build/sensitive-autoroute/`
- Update: `hardware/v2/releases/Code-SDR-V2-sensitive-routed.zip`

**Interfaces:**
- Consumes: `run_sensitive_autoroute.py --promote`.
- Produces: verified live PCB, backup, candidate, DRC/measurement/preservation reports, and release archive.

- [ ] **Step 1: Run candidate routing without promotion**

Run:

```powershell
& 'C:\Program Files\KiCad\9.0\bin\python.exe' hardware/v2/tools/run_sensitive_autoroute.py `
  --board hardware/v2/Code-SDR-V2.kicad_pcb `
  --build-root hardware/v2/build/sensitive-autoroute `
  --time-limit-seconds 3600
```

- [ ] **Step 2: Inspect the generated measurements and failure report**

If a path fails, adjust only routing order, cost weights, or allowed tuning windows; do not relax electrical or preservation gates.

- [ ] **Step 3: Run the verified promotion**

Repeat Step 1 with `--promote`. Expected: promotion succeeds and post-promotion live verification matches the candidate.

- [ ] **Step 4: Run fresh independent verification**

Run full unit tests, `kicad-cli pcb drc --format json`, contract measurement, preservation comparison, copper-layer count, and board-area checks against the live file.

- [ ] **Step 5: Rebuild the release archive**

Include the KiCad project, live PCB, schematic, rules, symbol tables, final DRC report, measurements, preservation report, and pre-route backup.

- [ ] **Step 6: Review git scope**

Use `git status --short` and `git diff --stat`; exclude KiCad lock files and unrelated user changes. Do not commit or push without an explicit user request.
