# Hardware recommendation

Do not fabricate `Wideband-Test-V1`. Use the modular V2 KiCad project in
[`hardware/v2`](v2).

V2 is a cost-first dual-conversion receiver/control board with:

- continuous 100 MHz-10 GHz tuning and 40 MHz real instantaneous bandwidth;
- separate LOW, A, B, C and D RF inputs to avoid a lossy 10 GHz input switch;
- 2436.25 MHz first IF, fixed 2410 MHz second LO and 26.25 MHz ADC IF;
- shared low-jitter 100 MHz reference and 100 MSPS AD9215 sampling;
- two GRF2013 IF gain stages around a 31.5 dB PE4312 attenuator;
- corrected CrossLink, RP2040, KSZ9031/RGMII, clocks, rails and startup states;
- standard single-assembly-side, four-layer FR-4 construction;
- no negative rail and no fitted high-band preselector bank.

The high-band LNAs provide useful band discrimination but are not precision
preselectors. Use an external filter for blocker-rich or calibrated image-free
measurements.

The SDR receiver/control PCB and ESC are deliberately distinct. V2 contains no
ESC or motor-power circuitry.

See [HARDWARE_VALIDATION.md](HARDWARE_VALIDATION.md) for the full audit,
implemented corrections, limitations and first-article validation plan.
Reproducible frequency, gain/noise/filter and BOM-cost calculations are in
[`redesign`](redesign).
