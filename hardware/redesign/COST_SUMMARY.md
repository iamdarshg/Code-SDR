# Comparable production BOM cost

Pricing basis: public manufacturer or authorized-distributor volume pricing at
1,000 units, or the closest public tier. Commodity lines use conservative
volume allowances. INR conversion uses the
[Upstox mid-market rate](https://upstox.com/currency-converter/usd-to-inr/)
shown for 17 July 2026 and checked 18 July 2026.

| Metric | USD | INR at 96.31229 INR/USD |
|---|---:|---:|
| Existing V1 BOM | $119.43 | Rs 11,503 |
| Corrected V2 BOM, QPC6144 budgeted at $6.00 | $147.90 | Rs 14,245 |
| V2 minus V1 | $28.47 | Rs 2,742 |

## Comparable 1,000-unit assembled allowance

This adds a conservative **$0.04/cm2** four-layer
PCB allowance, current public high-volume automated/manual joint rates, and
the same $0.24/board X-ray allowance to both designs.
It excludes setup/stencil amortization, electrical test, shipping, duties and
taxes. Replace the PCB allowance with quotes from the intended fabricator.
Assembly allowances use the published
[JLCPCB PCBA tiers](https://jlcpcb.com/help/article/pcb-assembly-price).

| Metric | USD | INR at 96.31229 INR/USD |
|---|---:|---:|
| Existing V1 assembled allowance | $123.96 | Rs 11,939 |
| Corrected V2 assembled allowance | $157.29 | Rs 15,149 |
| V2 minus V1 | $33.33 | Rs 3,210 |

On the BOM-only comparison, QPC6144 can cost up to $17.53. After
the PCB/assembly allowances, its quote must be no more than
**$12.67 per unit** to keep V2 within the hard
$40 increment. At the
$6.00 planning value, the full comparable redesign is
$6.67 below that limit.

The V2 total is most sensitive to the quote-only QPC6144. The LTC5548 IF balun
is the datasheet-recommended TC1-1-13M+ at its public manufacturer 1,000-unit
tier. Obtain a written QPC6144 production quote before purchase.
The detailed, reproducible row-level model is in `COST_COMPARISON.csv`.
