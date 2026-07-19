# V2 design calculations

This directory contains the reproducible design and cost contract for the
modular 100 MHz-10 GHz V2 receiver.

- `FREQUENCY_PLAN.csv` defines path boundaries, LO modes/ranges and both IFs.
- `validate_plan.py` checks continuous coverage, mixer/LO limits, BAW margin,
  ADC Nyquist margin and the exact ADF4360-1 integer-N plan.
- `rf_analysis.py` calculates Friis cascades, sensitivity, ADC-noise margin and
  the differential anti-alias response.
- `NOISE_BUDGET.csv`, `FILTER_RESPONSE.csv` and `RF_ANALYSIS.md` are generated
  calculation results.
- `cost_model.py` compares the complete V1 and V2 fitted BOMs.
- `COST_COMPARISON.csv` and `COST_SUMMARY.md` are generated cost results.
- `POWER_TREE.csv` records rail capacity, normal operating load policy and
  partitioning.

Run:

```powershell
python hardware/redesign/validate_plan.py
python hardware/redesign/rf_analysis.py
python hardware/redesign/cost_model.py
```

Public volume prices are planning data, not purchase quotes. Requote all
non-generic parts and especially the quote-only QPC6144 before ordering.
