# One-pass preservation-first Freerouting Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Run exactly one protected Freerouting pass that improves differential-pair routing and connects additional open nets without overwriting the user's current PCB work.

**Architecture:** A dedicated contract snapshots the working board, creates a candidate-only DSN, and protects all non-editable copper. A driver invokes Freerouting v2.2.4 once, imports only into a candidate copy, validates it, and replaces the live board only after all gates pass.

**Tech Stack:** KiCad 9 `pcbnew`, Specctra DSN/SES, Freerouting v2.2.4, Python `unittest`, KiCad CLI DRC.

## Global Constraints

- Use the uncommitted working-tree PCB/project as the source of truth.
- Hash a checkpoint before routing; never regenerate, reset or overwrite the live board before acceptance.
- Protect every non-differential segment/via. Unlock only incomplete or out-of-tolerance recognized differential pairs.
- Keep In1/In2 unavailable to new signal routes; preserve pre-existing In2 user tracks unchanged and signal-route new copper only on F.Cu/B.Cu.
- Exclude GND; retain 0.45/0.20 mm through vias; do not enable blind, buried or microvias.
- Invoke Freerouting exactly once with `-mp 1`; never retry it.
- Do not commit or push the user’s PCB/project files without a separate request.

---

### Task 1: Implement and test the preservation contract

**Files:**

- Create: `hardware/v2/tools/freerouting_contract.py`
- Create: `hardware/v2/tools/tests/test_freerouting_contract.py`

**Interfaces:**

- `snapshot_board(board_path: Path) -> BoardSnapshot`
- `transform_dsn(source: str, editable_nets: set[str]) -> str`
- `compare_preservation(before: BoardSnapshot, candidate_path: Path) -> list[str]`
- `measure_pairs(board_path: Path) -> dict[str, PairMeasurement]`

- [ ] **Step 1: Write failing DSN-transform tests**

```python
def test_transform_dsn_protects_noneditable_wiring():
    source = "(wiring (wire (path F.Cu 150 0 0 10 0) (net LOCKED)) (wire (path F.Cu 150 0 1 10 1) (net MDI_A_P)))"
    transformed = transform_dsn(source, {"MDI_A_P", "MDI_A_N"})
    assert "(net LOCKED) (type protect)" in transformed
    assert "(net MDI_A_P) (type protect)" not in transformed

def test_transform_dsn_makes_internal_layers_power():
    transformed = transform_dsn(FOUR_LAYER_DSN, set())
    assert "(layer In1.Cu\n      (type power)" in transformed
    assert "(layer In2.Cu\n      (type power)" in transformed
    assert "use_layer F.Cu B.Cu" in transformed
```

- [ ] **Step 2: Run the test to confirm it fails**

Run: `python -m unittest hardware.v2.tools.tests.test_freerouting_contract -v`

Expected: FAIL because the contract module does not exist.

- [ ] **Step 3: Implement immutable signatures and DSN rewriting**

```python
def is_editable_pair(pair: PairMeasurement) -> bool:
    return not pair.connected or pair.skew_mm > pair.tolerance_mm or pair.positive_vias != pair.negative_vias

def transform_dsn(source: str, editable_nets: set[str]) -> str:
    source = mark_existing_wiring_protected(source, editable_nets)
    source = mark_internal_layers_as_power(source, ("In1.Cu", "In2.Cu"))
    return rewrite_classes(source, allowed_layers=("F.Cu", "B.Cu"), omit_nets={"GND"})
```

Capture footprint position/rotation/side, zone geometry, protected tracks/vias, RF50 outside the editable subset, DRC baseline and open count. Return one concrete preservation error per changed protected item.

- [ ] **Step 4: Run tests and syntax check**

Run: `python -m unittest hardware.v2.tools.tests.test_freerouting_contract -v; python -m py_compile hardware/v2/tools/freerouting_contract.py`

Expected: PASS with no compiler output.

- [ ] **Step 5: Commit only tools/tests**

```powershell
git add hardware/v2/tools/freerouting_contract.py hardware/v2/tools/tests/test_freerouting_contract.py
git commit -m "feat(hardware): add protected freerouting contract"
```

### Task 2: Implement and test the single-invocation driver

**Files:**

- Create: `hardware/v2/tools/run_one_pass_freerouting.py`
- Modify: `hardware/v2/tools/tests/test_freerouting_contract.py`

**Interfaces:**

- `run_one_pass(board_path: Path, project_path: Path, work_root: Path, router_jar: Path | None, dry_run: bool = False, download_official_router: bool = False) -> RouteResult`
- `RouteResult` contains checkpoint, candidate, DSN, SES, pair measurements, before/after opens, DRC delta and preservation errors.

- [ ] **Step 1: Write a failing isolation test**

```python
def test_dry_run_does_not_modify_live_board(tmp_path: Path):
    live = copy_fixture_board(tmp_path / "live.kicad_pcb")
    original = live.read_bytes()
    result = run_one_pass(live, fixture_project(tmp_path), tmp_path / "run", fake_jar(), dry_run=True)
    assert live.read_bytes() == original
    assert result.candidate_path.exists()
    assert result.dsn_path.exists()
```

- [ ] **Step 2: Run the test to confirm it fails**

Run: `python -m unittest hardware.v2.tools.tests.test_freerouting_contract.OnePassDriverTests.test_dry_run_does_not_modify_live_board -v`

Expected: FAIL because `run_one_pass` does not exist.

- [ ] **Step 3: Implement isolated routing**

```python
def run_one_pass(board_path, project_path, work_root, router_jar, *, dry_run=False):
    run_dir = create_timestamped_run_dir(work_root)
    checkpoint = checkpoint_inputs(board_path, project_path, run_dir)
    before = snapshot_board(checkpoint.board_path)
    candidate = copy_candidate(checkpoint)
    dsn_path = export_and_transform_dsn(candidate.board_path, run_dir, editable_nets(before))
    if dry_run:
        return RouteResult.from_dry_run(run_dir, before)
    invoke_router_once(router_jar, dsn_path, run_dir / "output.ses")
    import_ses(candidate.board_path, run_dir / "output.ses")
    return validate_candidate(before, candidate.board_path, run_dir)
```

`invoke_router_once` records command, stdout, stderr and return code. It must contain `-mp 1` and reject a second call.

- [ ] **Step 4: Run all unit tests**

Run: `python -m unittest hardware.v2.tools.tests.test_freerouting_contract -v`

Expected: PASS; the live fixture stays byte-identical after dry run.

- [ ] **Step 5: Commit only tools/tests**

```powershell
git add hardware/v2/tools/run_one_pass_freerouting.py hardware/v2/tools/tests/test_freerouting_contract.py
git commit -m "feat(hardware): add one-pass freerouting driver"
```

### Task 3: Execute and validate the one pass

**Files:**

- Create: a uniquely timestamped run directory below `hardware/v2/build/one-pass-freerouting/`
- Modify only after acceptance: `hardware/v2/Code-SDR-V2.kicad_pcb`

- [ ] **Step 1: Baseline in an isolated run directory**

Acquire the official Freerouting v2.2.4 JAR only if absent, store it inside the timestamped run directory, and record SHA-256. Run the driver with `--dry-run`; require the live board hash to be unchanged and baseline pair/open reports to exist.

- [ ] **Step 2: Invoke Freerouting exactly once**

Run: `python hardware/v2/tools/run_one_pass_freerouting.py --board hardware/v2/Code-SDR-V2.kicad_pcb --project hardware/v2/Code-SDR-V2.kicad_pro --work-root hardware/v2/build/one-pass-freerouting --download-official-router`

Expected: one router process and one SES file with `-mp 1`.

- [ ] **Step 3: Gate candidate import**

Reject a candidate that moves footprints/zones, changes protected copper, adds new In1/In2 signal copper, changes RF50 outside unlocked pairs, adds DRC/parity problems, fails to reduce opens, or regresses completed pair skew/via tolerance. On rejection, preserve only the candidate/run data and leave the live board byte-identical.

- [ ] **Step 4: Import accepted candidate and verify**

Run: `kicad-cli pcb drc --format json --severity-all --all-track-errors --schematic-parity --units mm -o hardware/v2/build/one-pass-freerouting/final-drc.json hardware/v2/Code-SDR-V2.kicad_pcb`

Expected: report records one invocation, preservation success, no new DRC/parity issue, reduced opens and before/after pair data.

- [ ] **Step 5: Report without committing**

Report checkpoint path, remaining opens and every pair’s before/after skew. Leave the live PCB/project changes uncommitted for user review.
