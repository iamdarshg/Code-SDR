# Ethernet and High-Skew Routing Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Connect all U30-to-J30 Ethernet MDI pairs at 0.20 mm and bring only failing recognised differential pairs within their skew limits.

**Architecture:** Create a candidate from the working PCB, route explicitly scoped outer-layer tracks, then apply it only when preservation, KiCad DRC and pair measurements pass.

**Tech Stack:** KiCad 9 `pcbnew`, `kicad-cli pcb drc`, Python `unittest`.

## Global Constraints

- Preserve user copper, footprints and zones outside the named editable nets.
- Use F.Cu/B.Cu only; MDI is F.Cu only, 0.20 mm, zero vias, 1.0 mm maximum skew.
- Keep existing LO/IF/ADC width families and use their current 0.75/0.50 mm skew limits.

---

### Task 1: Capture and test the scoped route contract

**Files:**
- Modify: `hardware/v2/tools/tests/test_freerouting_contract.py`
- Create: `hardware/v2/tools/route_ethernet_and_skew.py`

- [ ] **Step 1: Write a failing test**

```python
def test_ethernet_route_contract_uses_200_um_and_no_vias():
    assert ethernet_route_contract()["width_mm"] == 0.20
    assert ethernet_route_contract()["allow_vias"] is False
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `python -m unittest hardware.v2.tools.tests.test_freerouting_contract -v`

Expected: import failure before the route helper exists.

- [ ] **Step 3: Implement the scoped candidate route helper**

```python
def ethernet_route_contract() -> dict[str, object]:
    return {"width_mm": 0.20, "allow_vias": False, "layers": ("F.Cu",)}
```

- [ ] **Step 4: Run the tests**

Run: `python -m unittest hardware.v2.tools.tests.test_freerouting_contract -v`

Expected: PASS.

### Task 2: Route and accept the candidate

**Files:**
- Modify only after all gates pass: `hardware/v2/Code-SDR-V2.kicad_pcb`
- Create: `hardware/v2/build/ethernet-and-skew-routing/`

- [ ] **Step 1: Snapshot the live board and route the eligible MDI and skew-failing pairs in a candidate.**
- [ ] **Step 2: Run `kicad-cli pcb drc --format json --severity-all --all-track-errors` on the baseline and candidate.**
- [ ] **Step 3: Reject any candidate that moves protected items, adds inner signal copper, leaves an MDI net unconnected, or misses an edited pair tolerance.**
- [ ] **Step 4: Copy an accepted candidate over the live board and re-run DRC/pair measurement.**
