"""Comparable V1/V2 production BOM cost model.

The model uses public authorized-distributor/manufacturer pricing at 1,000
units, or the closest public price break when 1,000-unit pricing is not
published.  It reports both a BOM-only comparison and a comparable assembled
allowance; shipping, duties, taxes, NRE and test remain excluded.  The QPC6144
is quote-only, so its unit price is a budgetary variable and the generated
summary reports the maximum acceptable quote needed to keep V2 within USD 40
of V1.
"""

from __future__ import annotations

import csv
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
V1_BOM = ROOT / "hardware" / "redesign" / "V1_BOM.csv"
V2_TOOLS = ROOT / "hardware" / "v2" / "tools"
OUT_CSV = ROOT / "hardware" / "redesign" / "COST_COMPARISON.csv"
OUT_MD = ROOT / "hardware" / "redesign" / "COST_SUMMARY.md"

# Upstox mid-market close shown for 17 July 2026, checked 18 July 2026.
USD_INR = 96.312285
QPC6144_BUDGET_USD = 6.00
MAX_ALLOWED_DELTA_USD = 40.00

# Comparable 1,000-unit manufacturing allowances.  PCB fabrication must still
# be replaced by a written quote; assembly rates are current public JLCPCB
# standard-PCBA tiers for an order exceeding 100,000 total joints.
PCB_USD_PER_CM2_ALLOWANCE = 0.04
AUTO_ASSEMBLY_USD_PER_JOINT = 0.0011
MANUAL_ASSEMBLY_USD_PER_JOINT = 0.011
XRAY_USD_PER_BOARD = 0.24
BOARD_CONTEXT = {
    "V1": {"area_cm2": 48.1601, "smd_joints": 994, "manual_joints": 115},
    "V2": {"area_cm2": 160.2601, "smd_joints": 1301, "manual_joints": 119},
}


@dataclass(frozen=True)
class Price:
    usd: float
    basis: str
    source: str


AUTH = {
    "ADI": "https://www.analog.com/en/parametricsearch/11313",
    "DigiKey": "https://www.digikey.com/",
    "Johanson": "https://www.johansontechnology.com/products/integrated-passives/baluns/2450bl15b0200001e/",
    "Mouser": "https://www.mouser.com/",
    "Mini-Circuits": "https://www.minicircuits.com/",
    "pSemi": "https://www.psemi.com/pdf/datasheets/pe4312ds.pdf",
    "Qorvo": "https://www.qorvo.com/products/p/QPC6144",
    "TI": "https://www.ti.com/product/CDCLVC1104",
}


EXACT: dict[str, Price] = {
    # Common digital and conversion devices.
    "LMX2592RHAT": Price(34.90, "authorized 250-unit tier", AUTH["DigiKey"]),
    "AD9215BCPZ-105": Price(8.46, "manufacturer starting price", AUTH["ADI"]),
    "KSZ9031RNXCA": Price(3.42, "authorized 260-unit tier", AUTH["DigiKey"]),
    "RP2040": Price(0.70, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    # V1-only active devices.
    "BU7205HFV-TR": Price(1.4818, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "GRF3042": Price(2.19, "authorized 1,000-unit tier", AUTH["Mouser"]),
    "OPA2673IRGVR": Price(1.79, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "LIF-MD6000-6UMG64I": Price(10.43, "authorized 980-unit tier", AUTH["DigiKey"]),
    "BGA614H6327XTSA1": Price(0.928, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "LD1117V-3V3": Price(0.24, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "AP7381-50SA-7": Price(0.08, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "8M-Q": Price(0.30, "comparable serial-flash allowance", AUTH["DigiKey"]),
    "BGS12WN6E6327": Price(0.138, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "BGA614H6327XTSA1": Price(0.928, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    # V2 power/control/digital devices.
    "TLV62569DBVR": Price(0.37, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "TLV75525PDBVR": Price(0.19, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "TLV76733DRVR": Price(0.46, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "W25Q16JVSSIQ": Price(0.20, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "USBLC6-2SC6": Price(0.13, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "LIF-MD6000-6JMG80I": Price(13.07, "authorized 100-unit tier", AUTH["DigiKey"]),
    "CDCLVC1104PWR": Price(3.22, "authorized 100-unit tier", AUTH["Mouser"]),
    # V2 reference and RF/IF devices.
    "ADF4360-1BCPZ": Price(4.78, "manufacturer 1,000-unit price", AUTH["ADI"]),
    "GRF4001": Price(1.36, "authorized 2,500-unit reel tier", AUTH["Mouser"]),
    "GRF2101": Price(1.14, "authorized 1,000-unit tier", AUTH["Mouser"]),
    "ADL5801ACPZ-R7": Price(7.28, "manufacturer 1,000-unit price", AUTH["ADI"]),
    "LTC5548IUDB": Price(13.51, "manufacturer 1,000-unit price", AUTH["ADI"]),
    "QPC6144TR13": Price(
        QPC6144_BUDGET_USD,
        "budgetary quote variable; verify before release",
        AUTH["Qorvo"],
    ),
    "QPQ1909TR13": Price(0.79374, "authorized 1,000-unit tier", AUTH["Mouser"]),
    "GRF2013": Price(1.34, "authorized 1,000-unit tier", AUTH["Mouser"]),
    "PE4312C-Z": Price(2.61, "authorized 1,000-unit tier", AUTH["Mouser"]),
    "TC1-1-13M+": Price(
        1.19,
        "manufacturer 1,000-unit tier",
        "https://www.minicircuits.com/WebStore/dashboard.html?model=TC1-1-13M%2B",
    ),
    "LT5560EDD": Price(2.28, "manufacturer 1,000-unit price", AUTH["ADI"]),
    "AD8351ARMZ": Price(4.37, "manufacturer 1,000-unit price", AUTH["ADI"]),
}


VALUE_PRICE: dict[str, Price] = {
    "MABA-011108": Price(8.6478, "authorized 2,000-unit package", AUTH["DigiKey"]),
    "LFCN-2500+": Price(1.8666, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "2450BL15B0100001E": Price(0.24, "authorized-volume allowance", AUTH["DigiKey"]),
    "2450BL15B0200001E": Price(0.28, "authorized-volume allowance", AUTH["Johanson"]),
    "SiT8209_100.000MHz": Price(2.29, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "SiT8209AC-23-33E-100.000000X": Price(
        2.29,
        "authorized 1,000-unit tier",
        AUTH["DigiKey"],
    ),
    "ECS-250-20-33-TR_25MHz": Price(0.615, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "ABM8-272-T3_12MHz": Price(0.18, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "JK0654219NL": Price(2.965, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "CX2041NLT": Price(2.20, "authorized-volume allowance", AUTH["DigiKey"]),
    "SMBJ5.0A": Price(0.11, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "SMAJ5.0A": Price(
        0.17,
        "manufacturer public 500-unit tier",
        "https://estore.st.com/en/products/protections-and-emi-filters/"
        "transient-voltage-suppressors-tvs/smaj5-0a.html",
    ),
    "PESD5V0S1UL": Price(0.055, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "PESD1V0R1BCSF": Price(0.11, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "2.5A_PTC": Price(0.18, "authorized 1,000-unit tier", AUTH["DigiKey"]),
    "BOOTSEL": Price(0.12, "authorized 1,000-unit switch allowance", AUTH["DigiKey"]),
    "RESET": Price(0.12, "authorized 1,000-unit switch allowance", AUTH["DigiKey"]),
    "Cortex_SWD_10pin": Price(0.24, "volume connector allowance", AUTH["DigiKey"]),
    "USB_B_Micro": Price(0.32, "authorized 1,000-unit connector allowance", AUTH["DigiKey"]),
    "5V_INPUT_ONLY": Price(0.62, "volume terminal-block allowance", AUTH["DigiKey"]),
    "RF_LOW_0.1_2.5GHz": Price(2.28, "authorized 3,200-unit tier", AUTH["DigiKey"]),
    "RF_HIGH_2.3_4.2GHz": Price(2.28, "authorized 3,200-unit tier", AUTH["DigiKey"]),
    "RF_HIGH_4.2_6.8GHz": Price(2.28, "authorized 3,200-unit tier", AUTH["DigiKey"]),
    "RF_HIGH_6.8_9.2GHz": Price(2.28, "authorized 3,200-unit tier", AUTH["DigiKey"]),
    "RF_HIGH_8.0_10GHz": Price(2.28, "authorized 3,200-unit tier", AUTH["DigiKey"]),
}


def generic_price(ref: str, value: str, footprint: str, board: str) -> Price:
    """Return a conservative volume allowance for commodity BOM lines."""
    if ref.startswith("C"):
        if value.startswith("22uF"):
            usd = 0.14
        elif value.startswith("10uF"):
            usd = 0.075
        elif value.startswith(("4.7uF", "2.2uF")):
            usd = 0.035
        elif value.startswith("1uF"):
            usd = 0.025
        elif "C0G" in value or value.endswith("pF"):
            usd = 0.018 if "0201" in footprint else 0.012
        else:
            usd = 0.010
        return Price(usd, "commodity passive allowance", AUTH["DigiKey"])
    if ref.startswith("R"):
        return Price(
            0.009 if "0201" in footprint else 0.006,
            "commodity 1% resistor allowance",
            AUTH["DigiKey"],
        )
    if ref.startswith(("L", "FB")):
        usd = 0.18 if "0805" in footprint and "2.2uH" in value else 0.07
        return Price(usd, "commodity inductor/ferrite allowance", AUTH["DigiKey"])
    if ref.startswith("D"):
        return Price(0.07, "commodity diode allowance", AUTH["DigiKey"])
    if ref.startswith("Q"):
        if value == "MMBFJ113":
            return Price(0.10, "authorized 1,000-unit tier", AUTH["DigiKey"])
        if value == "BFP840ESD":
            return Price(0.28, "authorized 1,000-unit tier", AUTH["DigiKey"])
    if ref.startswith("Y"):
        return Price(0.20, "commodity crystal allowance", AUTH["DigiKey"])
    if ref.startswith("TP"):
        return Price(0.04, "test-point allowance", AUTH["DigiKey"])
    if ref.startswith(("J", "AE")):
        if board == "V1" and value == "Antenna_Chip":
            return Price(3.50, "authorized 1,000-unit SMA tier", AUTH["DigiKey"])
        if board == "V1" and value == "RJ45":
            return Price(0.85, "volume connector allowance", AUTH["DigiKey"])
        if value == "Conn_01x02_Pin":
            return Price(0.20, "volume connector allowance", AUTH["DigiKey"])
        if value == "Conn_01x04_Pin":
            return Price(0.45 if "Micro-Fit" in footprint else 0.12,
                         "volume connector allowance", AUTH["DigiKey"])
    if ref.startswith("T") and "MABA011108" in footprint:
        return VALUE_PRICE["MABA-011108"]
    raise KeyError(f"No price rule for {board} {ref} {value} {footprint}")


def price_for(ref: str, value: str, footprint: str, board: str) -> Price:
    if value in EXACT:
        return EXACT[value]
    if value in VALUE_PRICE:
        return VALUE_PRICE[value]
    return generic_price(ref, value, footprint, board)


def load_v1() -> list[dict[str, object]]:
    rows: list[dict[str, object]] = []
    with V1_BOM.open(newline="", encoding="utf-8-sig") as handle:
        for row in csv.DictReader(handle):
            refs = str(row["References"])
            value = str(row["Value"])
            footprint = str(row["Footprint"])
            qty = int(row["Quantity"])
            first_ref = refs.split(",")[0].split("-")[0]
            price = price_for(first_ref, value, footprint, "V1")
            rows.append(
                {
                    "board": "V1",
                    "sheet": "legacy flat schematic",
                    "references": refs,
                    "value": value,
                    "footprint": footprint,
                    "quantity": qty,
                    "unit_usd": price.usd,
                    "extended_usd": price.usd * qty,
                    "basis": price.basis,
                    "source": price.source,
                }
            )
    return rows


def load_v2() -> list[dict[str, object]]:
    sys.path.insert(0, str(V2_TOOLS))
    import design_model  # type: ignore

    grouped: dict[tuple[str, str, str], list[object]] = defaultdict(list)
    for component in design_model.components:
        grouped[(component.sheet, component.value, component.footprint)].append(component)

    rows: list[dict[str, object]] = []
    for (sheet, value, footprint), components in sorted(grouped.items()):
        refs = ",".join(component.ref for component in components)
        price = price_for(components[0].ref, value, footprint, "V2")
        rows.append(
            {
                "board": "V2",
                "sheet": sheet,
                "references": refs,
                "value": value,
                "footprint": footprint,
                "quantity": len(components),
                "unit_usd": price.usd,
                "extended_usd": price.usd * len(components),
                "basis": price.basis,
                "source": price.source,
            }
        )
    return rows


def money(value: float) -> str:
    return f"{value:.2f}"


def main() -> None:
    rows = load_v1() + load_v2()
    totals = {
        board: sum(float(row["extended_usd"]) for row in rows if row["board"] == board)
        for board in ("V1", "V2")
    }
    delta = totals["V2"] - totals["V1"]
    v2_without_qpc = totals["V2"] - QPC6144_BUDGET_USD
    qpc_ceiling = totals["V1"] + MAX_ALLOWED_DELTA_USD - v2_without_qpc
    manufacturing = {
        board: (
            float(BOARD_CONTEXT[board]["area_cm2"]) * PCB_USD_PER_CM2_ALLOWANCE
            + int(BOARD_CONTEXT[board]["smd_joints"]) * AUTO_ASSEMBLY_USD_PER_JOINT
            + int(BOARD_CONTEXT[board]["manual_joints"]) * MANUAL_ASSEMBLY_USD_PER_JOINT
            + XRAY_USD_PER_BOARD
        )
        for board in ("V1", "V2")
    }
    production_totals = {
        board: totals[board] + manufacturing[board] for board in ("V1", "V2")
    }
    production_delta = production_totals["V2"] - production_totals["V1"]
    production_qpc_ceiling = (
        production_totals["V1"]
        + MAX_ALLOWED_DELTA_USD
        - (production_totals["V2"] - QPC6144_BUDGET_USD)
    )

    with OUT_CSV.open("w", newline="", encoding="utf-8") as handle:
        fieldnames = [
            "board", "sheet", "references", "value", "footprint", "quantity",
            "unit_usd", "extended_usd", "basis", "source",
        ]
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            formatted = dict(row)
            formatted["unit_usd"] = f"{float(row['unit_usd']):.5f}"
            formatted["extended_usd"] = f"{float(row['extended_usd']):.5f}"
            writer.writerow(formatted)

    text = f"""# Comparable production BOM cost

Pricing basis: public manufacturer or authorized-distributor volume pricing at
1,000 units, or the closest public tier. Commodity lines use conservative
volume allowances. INR conversion uses the
[Upstox mid-market rate](https://upstox.com/currency-converter/usd-to-inr/)
shown for 17 July 2026 and checked 18 July 2026.

| Metric | USD | INR at {USD_INR:.5f} INR/USD |
|---|---:|---:|
| Existing V1 BOM | ${money(totals['V1'])} | Rs {totals['V1'] * USD_INR:,.0f} |
| Corrected V2 BOM, QPC6144 budgeted at ${QPC6144_BUDGET_USD:.2f} | ${money(totals['V2'])} | Rs {totals['V2'] * USD_INR:,.0f} |
| V2 minus V1 | ${money(delta)} | Rs {delta * USD_INR:,.0f} |

## Comparable 1,000-unit assembled allowance

This adds a conservative **${PCB_USD_PER_CM2_ALLOWANCE:.2f}/cm2** four-layer
PCB allowance, current public high-volume automated/manual joint rates, and
the same ${XRAY_USD_PER_BOARD:.2f}/board X-ray allowance to both designs.
It excludes setup/stencil amortization, electrical test, shipping, duties and
taxes. Replace the PCB allowance with quotes from the intended fabricator.
Assembly allowances use the published
[JLCPCB PCBA tiers](https://jlcpcb.com/help/article/pcb-assembly-price).

| Metric | USD | INR at {USD_INR:.5f} INR/USD |
|---|---:|---:|
| Existing V1 assembled allowance | ${money(production_totals['V1'])} | Rs {production_totals['V1'] * USD_INR:,.0f} |
| Corrected V2 assembled allowance | ${money(production_totals['V2'])} | Rs {production_totals['V2'] * USD_INR:,.0f} |
| V2 minus V1 | ${money(production_delta)} | Rs {production_delta * USD_INR:,.0f} |

On the BOM-only comparison, QPC6144 can cost up to ${qpc_ceiling:.2f}. After
the PCB/assembly allowances, its quote must be no more than
**${production_qpc_ceiling:.2f} per unit** to keep V2 within the hard
${MAX_ALLOWED_DELTA_USD:.0f} increment. At the
${QPC6144_BUDGET_USD:.2f} planning value, the full comparable redesign is
${MAX_ALLOWED_DELTA_USD - production_delta:.2f} below that limit.

The V2 total is most sensitive to the quote-only QPC6144. The LTC5548 IF balun
is the datasheet-recommended TC1-1-13M+ at its public manufacturer 1,000-unit
tier. Obtain a written QPC6144 production quote before purchase.
The detailed, reproducible row-level model is in `COST_COMPARISON.csv`.
"""
    OUT_MD.write_text(text, encoding="utf-8")

    print(f"V1 USD {totals['V1']:.2f}")
    print(f"V2 USD {totals['V2']:.2f}")
    print(f"Delta USD {delta:.2f}")
    print(f"QPC6144 ceiling USD {qpc_ceiling:.2f}")
    print(f"V2 INR {totals['V2'] * USD_INR:.0f}")
    print(f"Production allowance V2 USD {production_totals['V2']:.2f}")
    print(f"Production allowance delta USD {production_delta:.2f}")
    print(f"Production QPC6144 ceiling USD {production_qpc_ceiling:.2f}")


if __name__ == "__main__":
    main()
