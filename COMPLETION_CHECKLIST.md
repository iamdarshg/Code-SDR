# Code-SDR V2 PCB Routing - Completion Checklist

**Date:** 2026-07-20
**Branch:** `claude/rev-b-pcb-routing-fab-yp4v6y`
**Status:** ⏳ AWAITING ROUTING EXECUTION

## Infrastructure Setup - ✅ COMPLETE

### Workflows Created
- ✅ `.github/workflows/v2-route.yml` - Complete routing pipeline
  - Exports DSN from current PCB
  - Downloads and runs FreeRouting 2.2.4
  - Imports SES result back into PCB
  - Runs post-route processing (RF via fences)
  - Validates final design
  - Auto-commits if successful
  
- ✅ `.github/workflows/v2-validate-fab-ready.yml` - Fabrication validation
  - Runs schematic ERC check
  - Analyzes PCB routing status
  - Generates readiness report
  - Uploads validation artifacts

### Tools & Scripts Created
- ✅ `hardware/v2/tools/route_v2_complete.py` - Orchestration script
  - Validates PCB structure
  - Creates baseline metrics
  - Coordinates routing pipeline
  - Generates reports
  
- ✅ `hardware/v2/tools/check_fab_readiness.py` - Local validation
  - Check PCB structure locally
  - Generate readiness assessments
  - No KiCad CLI required
  - JSON report output

### Documentation Created
- ✅ `ROUTING_STRATEGY.md` - Detailed routing strategy
  - Design constraints documented
  - Phased routing approach
  - FreeRouting configuration
  - Validation requirements
  
- ✅ `ROUTING_STATUS.md` - Progress tracking
  - Current PCB metrics
  - Acceptance criteria
  - Debugging notes
  - Developer reference
  
- ✅ `COMPLETION_CHECKLIST.md` - This document

## Current PCB State - ✅ VERIFIED

```
Location:   hardware/v2/Code-SDR-V2.kicad_pcb
Footprints: 344 ✅
Nets:       1,518 ✅
Tracks:     0 ❌ (needs routing)
Vias:       0 ❌ (needs routing)
Zones:      3 ✅ (GND, power planes)
Layers:     20 ✅ (4-layer + dielectrics)
```

### Verified Components
- ✅ All 344 footprints properly placed
- ✅ All 1,518 nets properly defined
- ✅ All critical nets present:
  - ✅ RF paths (8 patterns)
  - ✅ USB differential (2 signals)
  - ✅ Ethernet MDI (2 pairs each × 4 ports)
  - ✅ ADC signals (10 data lines)
  - ✅ FPGA signals (multiple)
- ✅ Proper layer stack (F.Cu, In1 GND, In2 Power, B.Cu)
- ✅ Power planes verified
- ✅ Ground plane solid on In1

## Routing Execution - ⏳ PENDING

### What Happens When Routing Workflow Runs:

1. **Baseline Capture**
   - [ ] Create snapshot of unrouted PCB
   - [ ] Record metrics before routing

2. **DSN Export**
   - [ ] Export PCB to SPECCTRA DSN format
   - [ ] Exclude GND from routing (solid plane)
   - [ ] Verify DSN is complete

3. **FreeRouting Execution**
   - [ ] Download FreeRouting 2.2.4
   - [ ] Run with 10 passes, single-threaded mode
   - [ ] 45-minute timeout
   - [ ] Generate SES file with routes

4. **SES Import**
   - [ ] Validate SES file exists and has content
   - [ ] Import routes into original PCB
   - [ ] Update schematic hierarchy paths
   - [ ] Expect ~1,000 tracks, ~350 vias

5. **Post-Processing**
   - [ ] Add RF via fences with `add_rf_via_fences.py`
   - [ ] Preserve critical routing constraints
   - [ ] Refill zones

6. **Validation**
   - [ ] Run design validation
   - [ ] Check RF constraints (F.Cu only, no vias)
   - [ ] Check differential pair skew
   - [ ] Verify no signal on GND plane
   - [ ] Generate validation report

7. **Auto-Commit (if successful)**
   - [ ] Commit routed PCB to branch
   - [ ] Push to origin/claude/rev-b-pcb-routing-fab-yp4v6y
   - [ ] Upload artifacts (logs, reports, PCB)

## Acceptance Criteria - TARGETS FOR ROUTING

### Routing Completion
- [ ] **0 unconnected nets** (DRC: unconnected_items = 0)
- [ ] **0 unrouted nets** (KiCad connectivity: 0)
- [ ] **800-1,200 track segments**
- [ ] **300-400 vias**
- [ ] **All 1,518 nets connected**

### Design Rule Compliance
- [ ] **DRC: zero error-severity violations**
- [ ] **DRC: zero electrical rule violations**
- [ ] **No signal routing on In1 GND plane**
- [ ] **All RF paths on F.Cu only, zero vias**
- [ ] **All matched pairs within skew tolerance**
- [ ] **All bus signals within skew tolerance**

### Critical Net Constraints
- [ ] **RF paths:** 8 nets, F.Cu only, 0 vias
- [ ] **USB D+/D-:** skew ≤ 1.0 mm
- [ ] **Ethernet MDI A-D:** skew ≤ 1.0 mm each
- [ ] **LO pairs:** skew ≤ 0.75 mm
- [ ] **IF pairs:** skew ≤ 0.50-0.75 mm
- [ ] **ADC data bus:** skew ≤ 5.0 mm
- [ ] **RGMII TX/RX:** skew ≤ 5.0 mm
- [ ] **Kelvin sense pairs:** valid routing

### File Integrity
- [ ] **PCB opens in KiCad without errors**
- [ ] **All 344 footprints accounted for**
- [ ] **All 1,518 nets accounted for**
- [ ] **No corrupted or truncated data**
- [ ] **Zones fill successfully**
- [ ] **All constraint flags honored**

### Fabrication Readiness
- [ ] **ERC: zero electrical errors**
- [ ] **DRC: comprehensive checks pass**
- [ ] **Gerber generation succeeds**
- [ ] **Drill file generation succeeds**
- [ ] **BOM complete and accurate**
- [ ] **Assembly data valid**

## How to Trigger Routing

### Option 1: GitHub Actions (Automated)
The workflow will run automatically when:
- Code is pushed to `claude/rev-b-pcb-routing-fab-yp4v6y`
- The `.route-trigger` file is modified

**Current State:** `.route-trigger` is committed ✅

### Option 2: Manual Trigger
From GitHub Actions UI:
1. Go to: https://github.com/iamdarshg/Code-SDR/actions
2. Select: "Code-SDR V2 Routing Pipeline"
3. Click: "Run workflow"
4. Select: `claude/rev-b-pcb-routing-fab-yp4v6y`
5. Click: "Run workflow"

### Option 3: Local Execution (if KiCad available)
```bash
cd hardware/v2
python3 tools/route_v2_complete.py --stage all
```

## Expected Workflow Duration

- **Download FreeRouting:** ~2 minutes
- **Export DSN:** ~1 minute
- **FreeRouting routing:** ~30-45 minutes (configured)
- **SES import:** ~2 minutes
- **Validation:** ~3 minutes
- **Commit & push:** ~2 minutes
- **Total:** ~45-55 minutes

## Success Indicators

When the workflow completes successfully:
1. ✅ Workflow shows GREEN status
2. ✅ Artifacts contain routed PCB with 800+ tracks
3. ✅ Validation report shows "READY"
4. ✅ Commit appears on branch with routed PCB
5. ✅ All skew metrics within tolerance

## If Routing Fails

### Diagnostic Steps:
1. Check workflow logs in GitHub Actions
2. Download `Code-SDR-V2-routing-results` artifact
3. Review:
   - `build/erc.json` - Schematic errors
   - `build/routing-status.json` - Routing metrics
   - `build/route.log` - FreeRouting log
   - `build/drc-final.json` - DRC errors
4. Common issues:
   - **No routes generated:** Check DSN export validity
   - **Partial routing:** FreeRouting timeout or memory issues
   - **Validation failures:** Review skew or layer violations
   - **Import error:** Check SES file format

### Recovery Options:
1. **Adjust FreeRouting parameters:**
   - Increase timeout in workflow
   - Change pass count (-mp flag)
   - Enable multi-threading (-mt flag)

2. **Constrained routing:**
   - Route critical nets separately
   - Use staged approach for net groups
   - Add keepouts around problem areas

3. **Manual intervention:**
   - Route remaining nets manually in KiCad
   - Use interactive router for complex connections

## Related Documentation

- Strategy: `ROUTING_STRATEGY.md` - Design constraints and approach
- Status: `ROUTING_STATUS.md` - Progress and acceptance criteria
- Tools: `hardware/v2/tools/*.py` - All routing and validation scripts
- Workflows: `.github/workflows/v2-*.yml` - CI/CD configuration

## Next Actions

1. **Immediate:**
   - ✅ Infrastructure complete
   - ✅ Documentation complete
   - ✅ Ready to execute routing

2. **Execute:**
   - ⏳ Trigger routing workflow (manual or automatic)
   - ⏳ Monitor progress in GitHub Actions
   - ⏳ Review generated artifacts

3. **Validate:**
   - ⏳ Check routing metrics
   - ⏳ Verify all constraints passed
   - ⏳ Inspect routed PCB visually if needed

4. **Finalize:**
   - ⏳ Confirm PCB is ready for fabrication
   - ⏳ Generate manufacturing outputs
   - ⏳ Archive final design

---

**Status:** This checklist is complete. The infrastructure and documentation are ready. Awaiting routing workflow execution.

**Last Updated:** 2026-07-20 by Claude Code
