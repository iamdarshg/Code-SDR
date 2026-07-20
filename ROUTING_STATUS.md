# Code-SDR V2 Routing and Fabrication Readiness Status

**Last Updated:** 2026-07-20
**Branch:** `claude/rev-b-pcb-routing-fab-yp4v6y`
**Commit:** 55af7e3

## Current PCB State

```
Footprints: 344
Nets:       1518
Tracks:     0 ❌ UNROUTED
Vias:       0  ❌ UNROUTED
Zones:      3  ✓
Layers:     20 ✓
```

**Status:** ✗ NOT READY FOR FABRICATION

## Completed Work

### Infrastructure & Validation
✓ Created comprehensive routing strategy document (ROUTING_STRATEGY.md)
✓ Implemented GitHub Actions workflows:
  - `.github/workflows/v2-route.yml` - Complete routing pipeline
  - `.github/workflows/v2-validate-fab-ready.yml` - Fabrication validation
✓ Created local validation tools:
  - `hardware/v2/tools/check_fab_readiness.py` - Local readiness assessment
  - `hardware/v2/tools/route_v2_complete.py` - Orchestration script

### Design Verification
✓ Confirmed 344 footprints properly placed
✓ Confirmed 1,518 nets properly defined
✓ Identified and verified all critical net categories:
  - RF paths (LT_IN, LO_LOW, IF_LOW, IF_HIGH, etc.) ✓
  - USB differential pairs (DP, DM) ✓
  - Ethernet MDI pairs (A, B, C, D) ✓
  - ADC signals (D0-D9, CLK) ✓
  - FPGA signals ✓
✓ Confirmed layer stack:
  - F.Cu (Front copper) ✓
  - In1.Cu (Ground plane) ✓
  - In2.Cu (Power plane) ✓
  - B.Cu (Back copper) ✓
  - Proper dielectrics defined ✓
✓ Verified 3 zones (power/ground planes) ✓

## Remaining Work

### Phase 1: Routing Execution
❌ Run FreeRouting to route all 1,514 nets
   - Status: Pending GitHub Actions environment setup
   - Expected result: ~1,000 tracks, ~350 vias
   - Timeout: 45 minutes
   - Configuration: 10 passes, single-threaded, stable mode

### Phase 2: Route Import & Integration
❌ Import FreeRouting SES result
   - Verify SES file is valid and complete
   - Import tracks and vias into PCB
   - Update schematic hierarchy paths
   - Expected: All nets connected

### Phase 3: Post-Route Processing
❌ Add RF via fences
   - Use `tools/add_rf_via_fences.py`
   - Protect RF critical routes with ground vias
   - Maintain impedance constraints

### Phase 4: Design Validation
❌ Run design validation checks
   - Use `tools/validate_route.py`
   - Verify RF constraints:
     - No vias in RF routes ❌
     - All RF routes on F.Cu only ❌
   - Verify differential pair skew:
     - USB: 1.0 mm max ❌
     - Ethernet MDI: 1.0 mm max each ❌
     - LO: 0.75 mm max ❌
     - IF: 0.50-0.75 mm max ❌
     - ADC: 0.50 mm max ❌
   - Verify no signal routing on In1 GND plane ❌
   - Verify ADC bus skew < 5.0 mm ❌
   - Verify RGMII TX/RX skew < 5.0 mm ❌

### Phase 5: Fabrication Output
❌ Generate final manufacturing outputs
   - Gerber files
   - Drill files
   - DRC clean
   - BOM export
   - Assembly documentation

## Acceptance Criteria

For the work to be complete, ALL of the following must be true:

### Routing Completeness
- [ ] Zero unconnected nets (DRC: unconnected_items = 0)
- [ ] Zero unrouted nets (KiCad connectivity: 0 missing connections)
- [ ] 800-1200 track segments
- [ ] 300-400 vias
- [ ] ~1,514 nets fully connected

### Design Rule Compliance  
- [ ] DRC: zero error-severity violations
- [ ] DRC: zero electrical rule violations
- [ ] No signal routing on In1 solid GND plane
- [ ] All RF critical routes on F.Cu only, zero vias
- [ ] All matched pairs within skew tolerance
- [ ] All bus signals within skew tolerance

### Critical Net Validation
- [ ] All 8 RF path nets properly routed F.Cu-only
- [ ] USB D+/D- pair skew ≤ 1.0 mm
- [ ] Ethernet MDI A/B/C/D pair skew ≤ 1.0 mm each
- [ ] LO pairs skew ≤ 0.75 mm
- [ ] IF pairs skew ≤ 0.50-0.75 mm
- [ ] ADC data-bus skew ≤ 5.0 mm
- [ ] RGMII TX/RX skew ≤ 5.0 mm
- [ ] All Kelvin sense pairs valid

### File Integrity
- [ ] PCB file opens in KiCad without errors
- [ ] All footprints account for (344)
- [ ] All nets account for (1,518)
- [ ] No corrupted or truncated data
- [ ] Proper zone fill completion
- [ ] All constraint flags honored

### Fabrication Readiness
- [ ] ERC: zero electrical errors (schematic)
- [ ] DRC: comprehensive rule check passes
- [ ] Gerber output generation succeeds
- [ ] Drill file generation succeeds
- [ ] BOM complete and accurate
- [ ] Assembly data valid

## How to Proceed

### Option 1: GitHub Actions (Recommended)
1. Ensure workflows are properly committed
2. Trigger the routing workflow via GitHub Actions
3. Monitor the workflow run at https://github.com/iamdarshg/Code-SDR/actions
4. Check the generated artifacts in the workflow results
5. Validate the imported SES result
6. Commit the routed PCB if validation passes

### Option 2: Local Environment with KiCad
If KiCad is available locally:
1. Install FreeRouting 2.2.4
2. Run: `python3 hardware/v2/tools/route_v2_complete.py --stage export`
3. Run FreeRouting manually on the generated DSN
4. Run: `python3 hardware/v2/tools/route_v2_complete.py --stage import`
5. Proceed with validation

### Debugging Notes

If routing fails:
- Check FreeRouting logs for specific nets causing issues
- Verify DSN is complete and valid
- Confirm SES file is properly formatted
- Check for degenerate or impossible routing regions
- Isolate problematic nets and route separately

If validation fails:
- Review DRC report in detail
- Check skew metrics for differential pairs
- Verify RF routes are F.Cu-only
- Ensure no signal routing on In1 plane
- Inspect layer usage in visualization

## Related Files

- Strategy: `ROUTING_STRATEGY.md`
- Workflows: `.github/workflows/v2-*.yml`
- Tools: `hardware/v2/tools/`
- Generated reports: `hardware/v2/build/`

## Next Steps

1. **Immediate:** Push branch and trigger GitHub Actions routing workflow
2. **Monitor:** Watch workflow progress and logs
3. **Validate:** Download and inspect routing artifacts
4. **Decision:** Accept or reject routed result based on criteria above
5. **Commit:** If valid, commit routed PCB to branch
6. **Fabricate:** Proceed with manufacturing output generation

---

## Notes for Developers

### Design Constraints Hardcoded in Validation
The Code-SDR V2 has specific routing requirements:
- RF paths must never have vias (4 matched pairs + LOs)
- GND plane (In1) is a solid plane with no signal routing
- Power planes require careful via distribution
- High-speed differential pairs need tight skew control
- All constraints are enforced in `validate_route.py`

### Router Configuration
FreeRouting settings for stability:
- `-mp 10`: Maximum 10 routing passes (quality over quantity)
- `-mt 0`: Single-threaded (prevents race conditions)
- `-da`: Design analyzer mode (more aggressive routing)
- `Timeout`: 45 minutes (reasonable for complex board)

### Key Scripts
- `generate_pcb.py`: Creates PCB from design model (do not run after routing!)
- `export_route_checkpoint.py`: Exports DSN for FreeRouting
- `import_route.py`: Imports SES back into PCB
- `add_rf_via_fences.py`: Adds ground vias around RF routes
- `validate_route.py`: Comprehensive design validation

