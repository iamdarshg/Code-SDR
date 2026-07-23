# Code-SDR V2 Routing - Quick Start Guide

## Current Status
- **PCB:** Completely unrouted (0 tracks, 0 vias)
- **Nets:** 1,518 to route
- **Expected Result:** 800-1,200 tracks, 300-400 vias
- **Time:** ~45-55 minutes on GitHub Actions

## How to Start Routing RIGHT NOW

### Method 1: Push to Branch (Automatic Trigger)
```bash
git push origin claude/rev-b-pcb-routing-fab-yp4v6y
```
The workflow will automatically trigger.

### Method 2: Manual GitHub Actions Trigger
1. Go to: https://github.com/iamdarshg/Code-SDR/actions
2. Click: "Route Code-SDR V2" workflow
3. Click: "Run workflow" button
4. Select: `claude/rev-b-pcb-routing-fab-yp4v6y` branch
5. Click: "Run workflow"

## What Happens During Routing

1. **PCB Validation** (1 min)
   - Verify 344 footprints ✓
   - Verify 1,518 nets ✓
   - Check baseline metrics

2. **DSN Export** (1 min)
   - Convert PCB to SPECCTRA format
   - Exclude GND plane from routing

3. **FreeRouting Execution** (30-45 min)
   - Download FreeRouting 2.2.4
   - Run with 10 passes, single-threaded
   - Generate routes for all nets

4. **SES Import** (2 min)
   - Import routed tracks and vias
   - Update PCB file

5. **Validation** (3 min)
   - Check RF constraints (F.Cu only, no vias)
   - Check differential pair skew
   - Verify no signal on GND plane
   - Generate statistics

6. **Auto-Commit** (2 min)
   - If successful, commit routed PCB
   - Upload artifacts with detailed reports

## Monitor Progress

1. Go to: https://github.com/iamdarshg/Code-SDR/actions
2. Click on the running workflow
3. Watch real-time output
4. Download artifacts when complete

## Expected Success Output

```
✓ Tracks: 800-1200+
✓ Vias: 300-400+
✓ Unconnected: 0
✓ DRC Errors: 0
✓ RF Constraints: Pass
✓ Skew Checks: Pass
✓ Status: READY FOR FAB
```

## If Routing Fails

1. Check workflow logs for specific error
2. Common issues:
   - FreeRouting timeout (increase timeout parameter)
   - Memory issues (increase -Xmx flag)
   - Partial routing (re-run with adjusted parameters)
3. Review DRC report in artifacts
4. Adjust constraints if needed
5. Re-run workflow

## Files Generated During Routing

**Artifacts uploaded (30-day retention):**
- `pcb-status.json` - Pre-routing metrics
- `final-status.json` - Post-routing metrics
- `route.log` - FreeRouting logs
- `ROUTE_STATISTICS.csv` - Per-net routing stats
- `drc.json` - Design rule check results
- `Code-SDR-V2.kicad_pcb` - Routed PCB (if successful)

## Key Constraints (Automatically Validated)

- RF routes: F.Cu only, zero vias
- USB D+/D-: ≤1.0 mm skew
- Ethernet MDI: ≤1.0 mm each
- LO pairs: ≤0.75 mm skew
- IF pairs: ≤0.50-0.75 mm skew
- ADC bus: ≤5.0 mm skew
- GND plane: Solid, no signals

---

**Ready to route?** Push the branch or trigger the workflow now!
