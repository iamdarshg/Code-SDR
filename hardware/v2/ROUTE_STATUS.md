# Code-SDR V2 route status

- Result: **PARTIAL / REQUIRES CLEANUP**
- DRC violations: 100
- Unconnected items: 283
- Schematic/PCB parity findings: 0
- Route-invariant exit: 1

## Route-invariant output

```text
Traceback (most recent call last):
  File "/home/runner/work/Code-SDR/Code-SDR/hardware/v2/tools/validate_route.py", line 180, in <module>
    main()
  File "/home/runner/work/Code-SDR/Code-SDR/hardware/v2/tools/validate_route.py", line 127, in main
    positive_length, positive_vias = aggregate(positive)
                                     ^^^^^^^^^^^^^^^^^^^
  File "/home/runner/work/Code-SDR/Code-SDR/hardware/v2/tools/validate_route.py", line 120, in aggregate
    raise AssertionError(f"Routed net is missing from statistics: {net_name}")
AssertionError: Routed net is missing from statistics: USB_DP
```

The legacy `Wideband-Test-V1` board remains intentionally unrouted and marked
**DO NOT FABRICATE**. It is retained only as audit evidence; V2 is the corrected
receiver/control-board deliverable.
