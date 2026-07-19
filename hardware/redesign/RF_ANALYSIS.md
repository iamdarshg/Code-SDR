# V2 RF gain, noise and bandwidth analysis

This is the reproducible nominal design budget for the routed V2 hardware. It
uses a 40 MHz useful channel, 100 MSPS ADC, 2436.25 MHz first IF and 26.25 MHz
second IF. Thermal input noise is **-97.98 dBm** in 40 MHz.

| Selected input path | Gain at 0 dB DSA (dB) | Cascaded NF (dB) | 10 dB-SNR sensitivity (dBm) | ADC input noise (dBm) | Analog/ADC noise margin (dB) | ADC full-scale input over DSA range (dBm) |
|---|---:|---:|---:|---:|---:|---:|
| LOW_0.1_2.5 | 62.2 | 3.27 | -84.7 | -32.5 | 19.0 | -55.2 to -23.7 |
| HIGH_A_2.3_4.2 | 60.3 | 3.15 | -84.8 | -34.5 | 17.0 | -53.3 to -21.8 |
| HIGH_B_4.2_6.8 | 58.2 | 3.98 | -84.0 | -35.8 | 15.7 | -51.2 to -19.7 |
| HIGH_C_6.8_9.2 | 58.3 | 4.03 | -83.9 | -35.6 | 15.9 | -51.3 to -19.8 |
| HIGH_D_8.0_10 | 67.9 | 2.50 | -85.5 | -27.6 | 23.9 | -60.9 to -29.4 |

The second D-port LNA fixes the original top-band sensitivity shortfall. The
second GRF2013 after the PE4312 is also intentional: every path keeps more than
15 dB nominal analog-noise margin above the ADC's own integrated noise. The
PE4312 powers up at maximum attenuation and provides 31.5 dB of ADC-level
control. Front-end compression and blocker limits must still be verified on
first articles; the last column is an ADC-level calculation, not a guarantee
that every upstream stage remains linear at that input.

## Anti-alias filter

The differential filter is a symmetric 9-pole, approximately 46.5 MHz
low-pass ladder. Each half uses series inductors
200/360/390/360/200 nH and shunt capacitors 100/110/110/100 pF. The table is
the ideal response relative to DC with the implemented source/load model;
the RF budget separately reserves 0.5 dB for component Q and layout loss.

| Frequency (MHz) | Relative insertion (dB) |
|---:|---:|
| 6.25 | -0.13 |
| 26.25 | -0.04 |
| 40.00 | -0.03 |
| 46.25 | -0.10 |
| 48.00 | -2.11 |
| 50.00 | -8.41 |
| 53.75 | -20.61 |
| 60.00 | -35.73 |
| 75.00 | -60.26 |
| 83.50 | -70.67 |
| 100.00 | -87.10 |

The wanted 6.25-46.25 MHz band stays within 0.13 dB ideal ripple. The response
is about -20.6 dB at the first upper alias edge (53.75 MHz), -35.7 dB at
60 MHz and -87 dB at 100 MHz.

## Frequency-plan limitations and digital bandwidth

The five RF connectors are deliberately separate and the four high-band LNAs
are selected after amplification. That is the lowest-cost way to retain noise
figure and continuous 100 MHz-10 GHz tuning. The matched high-band LNAs
provide useful image discrimination, but they are not precision brick-wall
preselectors. For blocker-rich or calibrated image-free measurements, fit an
external filter ahead of the selected A/B/C/D connector. Adding four wideband
on-board filter banks would exceed the cost-first target and add insertion
loss ahead of the LNAs.

The ADC produces 1.0 Gbit/s of raw samples, which cannot be carried as payload
over GbE. FPGA firmware must digitally filter and decimate/resample before
Ethernet framing (for example 80 MSPS x 10 bit or 80 MSPS x 8 bit). This is a
firmware throughput requirement, not an analog-bandwidth reduction.

## Evidence

- [ADL5801 data sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ADL5801.pdf)
- [LTC5548 data sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/5548f.pdf)
- [LT5560 data sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/5560f.pdf)
- [AD8351 data sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/AD8351.pdf)
- [AD9215 data sheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ad9215.pdf)
- [GRF4001 data sheet](https://www.guerrilla-rf.com/includes/prodFiles/4001/GRF4001DS.pdf)
- [GRF2101 data sheet](https://www.guerrilla-rf.com/includes/prodFiles/2101/GRF2101DS.pdf)
- [GRF2013 data sheet](https://www.guerrilla-rf.com/includes/prodFiles/2013/GRF2013DS.pdf)
- [QPQ1909 data sheet](https://www.mouser.com/datasheet/2/412/QPQ1909_Data_Sheet-1815271.pdf)
- [PE4312 data sheet](https://www.psemi.com/pdf/datasheets/pe4312ds.pdf)
