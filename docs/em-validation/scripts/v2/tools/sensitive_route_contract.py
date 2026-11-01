"""Electrical routing contract for the board's timing-sensitive nets."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class RoutePair:
    positive: tuple[str, ...]
    negative: tuple[str, ...]
    max_skew_mm: float
    differential_impedance_ohm: int | None = None
    require_equal_vias: bool = True


@dataclass(frozen=True)
class RouteBus:
    members: tuple[tuple[str, ...], ...]
    max_spread_mm: float


def _pair(positive: str, negative: str, skew: float) -> RoutePair:
    return RoutePair((positive,), (negative,), skew)


PAIR_GROUPS = {
    "USB": RoutePair(("USB_DP_CONN", "USB_DP"), ("USB_DM_CONN", "USB_DM"), 0.50, 90),
    "Ethernet MDI A": _pair("MDI_A_P", "MDI_A_N", 1.00),
    "Ethernet MDI B": _pair("MDI_B_P", "MDI_B_N", 1.00),
    "Ethernet MDI C": _pair("MDI_C_P", "MDI_C_N", 1.00),
    "Ethernet MDI D": _pair("MDI_D_P", "MDI_D_N", 1.00),
    "LMX low-band LO": _pair("LMX_LO_LOW_P_RAW", "LMX_LO_LOW_N_RAW", 0.25),
    "ADF second LO": _pair("ADF_LO2_P_RAW", "ADF_LO2_N_RAW", 0.25),
    "Second LO": _pair("LO2_P", "LO2_N", 0.25),
    "Low-band LO": _pair("LO_LOW_P", "LO_LOW_N", 0.25),
    "Low-band IF": _pair("IF_LOW_P", "IF_LOW_N", 0.25),
    "High-band IF": _pair("IF_HIGH_P", "IF_HIGH_N", 0.25),
    "LT5560 RF": _pair("LT_IN_P", "LT_IN_N", 0.25),
    "Second IF": _pair("IF2_P", "IF2_N", 0.50),
    "IF2 filter 1": _pair("IF2_F1_P", "IF2_F1_N", 0.50),
    "IF2 filter 2": _pair("IF2_F2_P", "IF2_F2_N", 0.50),
    "IF2 filter 3": _pair("IF2_F3_P", "IF2_F3_N", 0.50),
    "IF2 filter 4": _pair("IF2_F4_P", "IF2_F4_N", 0.50),
    "IF2 filter output": _pair("IF2_LPF_P", "IF2_LPF_N", 0.50),
    "ADC driver input": _pair("AD8351_IN_P", "AD8351_IN_N", 0.50),
    "ADC driver output": _pair("AD8351_OUT_P", "AD8351_OUT_N", 0.50),
    "ADC analog input": _pair("ADC_VIN_P", "ADC_VIN_N", 0.50),
}


def _split_members(names: tuple[str, ...]) -> tuple[tuple[str, ...], ...]:
    return tuple((name, f"{name}_FPGA") for name in names)


BUS_GROUPS = {
    "RGMII TX": RouteBus(_split_members((
        "RGMII_TXD0", "RGMII_TXD1", "RGMII_TXD2", "RGMII_TXD3", "RGMII_TX_EN",
    )), 2.00),
    "RGMII RX": RouteBus(_split_members((
        "RGMII_RXD0", "RGMII_RXD1", "RGMII_RXD2", "RGMII_RXD3", "RGMII_RX_DV",
    )), 2.00),
    "ADC data": RouteBus(tuple((f"ADC_D{index}",) for index in range(10)) + (("ADC_OR",),), 2.00),
}


SELECTED_NETS = frozenset(
    net
    for pair in PAIR_GROUPS.values()
    for net in pair.positive + pair.negative
) | frozenset(
    net
    for bus in BUS_GROUPS.values()
    for member in bus.members
    for net in member
) | frozenset({"ADC_CLK", "RGMII_TXC", "RGMII_TXC_FPGA", "RGMII_RXC", "RGMII_RXC_FPGA"})
