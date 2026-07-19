"""Single source of truth for the Code-SDR V2 schematic and PCB.

The model is intentionally explicit: every fitted pad has a named net or is
marked NC.  Both CAD generators consume this file so that hierarchy, BOM,
footprints, and PCB connectivity cannot silently diverge.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, Iterable, List, Tuple


ROOT = Path(__file__).resolve().parents[1]


@dataclass
class Component:
    sheet: str
    ref: str
    value: str
    footprint: str
    pins: Dict[str, str | None]
    xy: Tuple[float, float]
    rotation: float = 0.0
    description: str = ""
    datasheet: str = ""
    symbol: str | None = None
    pin_names: Dict[str, str] = field(default_factory=dict)

    @property
    def symbol_name(self) -> str:
        return self.symbol or self.value.replace(" ", "_").replace("/", "_")


SHEETS = [
    ("01_power", "Power entry and low-noise rails"),
    ("02_controller", "RP2040 control, flash, USB and debug"),
    ("03_fpga_adc", "CrossLink FPGA, AD9215 and shared 100 MHz sampling clock"),
    ("04_ethernet", "KSZ9031 RGMII and integrated-magnetics RJ45"),
    ("05_reference_lo", "100 MHz reference, LMX2592 and fixed 2.410 GHz LO"),
    ("06_rf_low", "100 MHz to 2.5 GHz low-band front end"),
    ("07_rf_high", "2.3 GHz to 10 GHz four-port high-band front end"),
    ("08_if_chain", "2.43625 GHz IF, DSA, second mixer and ADC driver"),
]


components: List[Component] = []


def add(
    sheet: str,
    ref: str,
    value: str,
    footprint: str,
    pins: Dict[str, str | None],
    xy: Tuple[float, float],
    *,
    rotation: float = 0.0,
    description: str = "",
    datasheet: str = "",
    symbol: str | None = None,
    pin_names: Dict[str, str] | None = None,
) -> Component:
    item = Component(
        sheet=sheet,
        ref=ref,
        value=value,
        footprint=footprint,
        pins=pins,
        xy=xy,
        rotation=rotation,
        description=description,
        datasheet=datasheet,
        symbol=symbol,
        pin_names=pin_names or {},
    )
    components.append(item)
    return item


def two_pin(
    sheet: str,
    ref: str,
    value: str,
    a: str,
    b: str,
    xy: Tuple[float, float],
    *,
    footprint: str,
    symbol: str,
    description: str = "",
    datasheet: str = "",
    rotation: float = 0.0,
) -> Component:
    return add(
        sheet,
        ref,
        value,
        footprint,
        {"1": a, "2": b},
        xy,
        rotation=rotation,
        description=description,
        datasheet=datasheet,
        symbol=symbol,
        pin_names={"1": "1", "2": "2"},
    )


def cap(
    sheet: str,
    ref: str,
    value: str,
    rail: str,
    xy: Tuple[float, float],
    *,
    size: str = "0402",
) -> Component:
    fp = {
        "0201": "Capacitor_SMD:C_0201_0603Metric",
        "0402": "Capacitor_SMD:C_0402_1005Metric",
        "0603": "Capacitor_SMD:C_0603_1608Metric",
        "0805": "Capacitor_SMD:C_0805_2012Metric",
        "1206": "Capacitor_SMD:C_1206_3216Metric",
    }[size]
    return two_pin(
        sheet,
        ref,
        value,
        rail,
        "GND",
        xy,
        footprint=fp,
        symbol="Device:C_Small",
        description=f"{rail} local bypass",
    )


def resistor(
    sheet: str,
    ref: str,
    value: str,
    a: str,
    b: str,
    xy: Tuple[float, float],
    *,
    size: str = "0402",
    rotation: float = 0.0,
) -> Component:
    fp = {
        "0201": "Resistor_SMD:R_0201_0603Metric",
        "0402": "Resistor_SMD:R_0402_1005Metric",
        "0603": "Resistor_SMD:R_0603_1608Metric",
        "0805": "Resistor_SMD:R_0805_2012Metric",
    }[size]
    return two_pin(
        sheet,
        ref,
        value,
        a,
        b,
        xy,
        footprint=fp,
        symbol="Device:R_Small",
        rotation=rotation,
    )


def inductor(
    sheet: str,
    ref: str,
    value: str,
    a: str,
    b: str,
    xy: Tuple[float, float],
    *,
    size: str = "0805",
) -> Component:
    fp = {
        "0402": "Inductor_SMD:L_0402_1005Metric_Pad0.77x0.64mm_HandSolder",
        "0603": "Inductor_SMD:L_0603_1608Metric_Pad1.05x0.95mm_HandSolder",
        "0805": "CodeSDR:Inductor_0805_HandSolder",
    }[size]
    return two_pin(
        sheet,
        ref,
        value,
        a,
        b,
        xy,
        footprint=fp,
        symbol="Device:L_Small",
    )


# ---------------------------------------------------------------------------
# 01 - Power
# ---------------------------------------------------------------------------
add(
    "01_power",
    "J1",
    "5V_INPUT_ONLY",
    "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS-1,5-2_1x02_P5.00mm_Horizontal",
    {"1": "VIN_5V", "2": "GND"},
    (22, 91),
    symbol="Connector_Generic:Conn_01x02",
    description="Regulated 5 V input, 3 A minimum; never connect ESC battery here",
)
two_pin(
    "01_power",
    "F1",
    "2.5A_PTC",
    "VIN_5V",
    "VIN_FUSED",
    (30, 91),
    footprint="Fuse:Fuse_1812_4532Metric",
    symbol="Device:Polyfuse",
)
two_pin(
    "01_power",
    "D1",
    "SMAJ5.0A",
    "VIN_FUSED",
    "GND",
    (35, 95),
    footprint="Diode_SMD:D_SMA",
    symbol="Device:D_TVS",
    description="400 W, 5 V unidirectional input TVS in the matching SMA package",
    datasheet="https://www.st.com/resource/en/datasheet/smaj5-0a.pdf",
)
two_pin(
    "01_power",
    "FB1",
    "600R@100MHz_2A",
    "VIN_FUSED",
    "+5V_RF",
    (40, 91),
    footprint="CodeSDR:Inductor_0805_HandSolder",
    symbol="Device:FerriteBead_Small",
)
cap("01_power", "C1", "22uF", "VIN_FUSED", (34, 87), size="0805")
cap("01_power", "C2", "22uF", "+5V_RF", (43, 87), size="0805")

add(
    "01_power",
    "U1",
    "TLV62569DBVR",
    "Package_TO_SOT_SMD:SOT-23-5",
    {"1": "VIN_FUSED", "2": "GND", "3": "SW_3V3", "4": "VIN_FUSED", "5": "FB_3V3"},
    (50, 91),
    description="2 A synchronous 3.3 V digital buck",
    datasheet="https://www.ti.com/lit/ds/symlink/tlv62569.pdf",
    pin_names={"1": "EN", "2": "GND", "3": "SW", "4": "VIN", "5": "FB"},
)
inductor("01_power", "L1", "2.2uH_2.5A", "SW_3V3", "+3V3_DIG", (57, 91))
resistor("01_power", "R1", "453k_1%", "+3V3_DIG", "FB_3V3", (55, 87))
resistor("01_power", "R2", "100k_1%", "FB_3V3", "GND", (59, 87))
cap("01_power", "C3", "10uF", "VIN_FUSED", (48, 95), size="0805")
cap("01_power", "C4", "22uF", "+3V3_DIG", (62, 91), size="0805")
cap("01_power", "C5", "100nF", "+3V3_DIG", (65, 91))

add(
    "01_power",
    "U2",
    "TLV62569DBVR",
    "Package_TO_SOT_SMD:SOT-23-5",
    {"1": "VIN_FUSED", "2": "GND", "3": "SW_1V2", "4": "VIN_FUSED", "5": "FB_1V2"},
    (50, 81),
    description="2 A synchronous 1.2 V core buck",
    datasheet="https://www.ti.com/lit/ds/symlink/tlv62569.pdf",
    pin_names={"1": "EN", "2": "GND", "3": "SW", "4": "VIN", "5": "FB"},
)
inductor("01_power", "L2", "2.2uH_2.5A", "SW_1V2", "+1V2_CORE", (57, 81))
resistor("01_power", "R3", "100k_1%", "+1V2_CORE", "FB_1V2", (55, 77))
resistor("01_power", "R4", "100k_1%", "FB_1V2", "GND", (59, 77))
cap("01_power", "C6", "10uF", "VIN_FUSED", (48, 85), size="0805")
cap("01_power", "C7", "22uF", "+1V2_CORE", (62, 81), size="0805")
cap("01_power", "C8", "100nF", "+1V2_CORE", (65, 81))

add(
    "01_power",
    "U3",
    "TLV75525PDBVR",
    "Package_TO_SOT_SMD:SOT-23-5",
    {"1": "+3V3_DIG", "2": "GND", "3": "+3V3_DIG", "4": None, "5": "+2V5_AUX"},
    (72, 87),
    description="500 mA high-PSRR 2.5 V auxiliary LDO",
    datasheet="https://www.ti.com/lit/ds/symlink/tlv755p.pdf",
    pin_names={"1": "IN", "2": "GND", "3": "EN", "4": "NC", "5": "OUT"},
)
cap("01_power", "C9", "1uF", "+3V3_DIG", (69, 82), size="0603")
cap("01_power", "C10", "2.2uF", "+2V5_AUX", (76, 82), size="0603")

add(
    "01_power",
    "U4",
    "TLV76733DRVR",
    "Package_SON:WSON-6-1EP_2x2mm_P0.65mm_EP1x1.6mm_ThermalVias",
    {
        "1": "+3V3_ANA",
        "2": "+3V3_ANA",
        "3": "GND",
        "4": "VIN_FUSED",
        "5": "GND",
        "6": "VIN_FUSED",
        "7": "GND",
    },
    (85, 87),
    description="1 A low-noise/high-PSRR 3.3 V analog LDO",
    datasheet="https://www.ti.com/lit/ds/symlink/tlv767.pdf",
    pin_names={"1": "OUT", "2": "SNS", "3": "GND", "4": "EN", "5": "GND", "6": "IN", "7": "EP"},
)
cap("01_power", "C11", "4.7uF", "VIN_FUSED", (81, 82), size="0805")
cap("01_power", "C12", "4.7uF", "+3V3_ANA", (89, 82), size="0805")
resistor("01_power", "R5", "0", "GND", "CHASSIS_GND", (28, 84), size="0603")


# ---------------------------------------------------------------------------
# 02 - RP2040 controller
# ---------------------------------------------------------------------------
rp_names: Dict[str, str] = {
    "1": "IOVDD", "10": "IOVDD", "22": "IOVDD", "33": "IOVDD", "42": "IOVDD", "49": "IOVDD",
    "2": "GPIO0", "3": "GPIO1", "4": "GPIO2", "5": "GPIO3", "6": "GPIO4", "7": "GPIO5",
    "8": "GPIO6", "9": "GPIO7", "11": "GPIO8", "12": "GPIO9", "13": "GPIO10", "14": "GPIO11",
    "15": "GPIO12", "16": "GPIO13", "17": "GPIO14", "18": "GPIO15", "19": "TESTEN",
    "20": "XIN", "21": "XOUT", "23": "DVDD", "24": "SWCLK", "25": "SWDIO", "26": "RUN",
    "27": "GPIO16", "28": "GPIO17", "29": "GPIO18", "30": "GPIO19", "31": "GPIO20", "32": "GPIO21",
    "34": "GPIO22", "35": "GPIO23", "36": "GPIO24", "37": "GPIO25", "38": "GPIO26_ADC0",
    "39": "GPIO27_ADC1", "40": "GPIO28_ADC2", "41": "GPIO29_ADC3", "43": "ADC_AVDD",
    "44": "VREG_VIN", "45": "VREG_VOUT", "46": "USB_DM", "47": "USB_DP", "48": "USB_VDD",
    "50": "DVDD", "51": "QSPI_SD3", "52": "QSPI_SCLK", "53": "QSPI_SD0", "54": "QSPI_SD2",
    "55": "QSPI_SD1", "56": "QSPI_SS_N", "57": "EP_GND",
}
rp_pins = {str(i): None for i in range(1, 58)}
for p in ["1", "10", "22", "33", "42", "49", "43", "44", "48"]:
    rp_pins[p] = "+3V3_DIG"
for p in ["23", "50"]:
    rp_pins[p] = "RP_CORE"
rp_pins.update({
    "2": "FPGA_SPI_SCK_MCU", "3": "FPGA_SPI_MOSI", "4": "FPGA_SPI_MISO", "5": "FPGA_SPI_CS_N",
    "6": "SYNTH_SCK", "7": "SYNTH_SDI", "8": "LMX_CSB", "9": "LMX_CE",
    "11": "ADF_LE", "12": "ADF_CLK", "13": "ADF_DATA", "14": "LNA_EN_LOW",
    "15": "LNA_EN_H1", "16": "LNA_EN_H2", "17": "LNA_EN_H3", "18": "DSA_LE",
    "27": "DSA_DATA", "28": "DSA_CLK", "29": "FPGA_RESET_N", "30": "IF_PATH_SEL",
    "31": "HSW_V1", "32": "HSW_V2", "34": "ADC_PDWN",
    "35": "LNA_EN_H4", "36": "MIXER_EN_HIGH",
    "37": "PHY_RESET_N", "38": "LMX_LOCK", "39": "ADF_LOCK", "40": "ADC_OR", "41": "FPGA_CDONE",
    "19": "GND", "20": "RP_XIN", "21": "RP_XOUT", "24": "SWD_CLK", "25": "SWD_IO",
    "26": "RP_RUN", "45": "RP_CORE", "46": "USB_DM", "47": "USB_DP",
    "51": "QSPI_SD3", "52": "QSPI_SCLK", "53": "QSPI_SD0", "54": "QSPI_SD2",
    "55": "QSPI_SD1", "56": "QSPI_SS_N", "57": "GND",
})
add(
    "02_controller",
    "U10",
    "RP2040",
    "Package_DFN_QFN:QFN-56-1EP_7x7mm_P0.4mm_EP3.2x3.2mm_ThermalVias",
    rp_pins,
    (35, 35),
    description="Control processor only; no ESC or motor-power functions",
    datasheet="https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf",
    pin_names=rp_names,
)
add(
    "02_controller",
    "U11",
    "W25Q16JVSSIQ",
    "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm",
    {"1": "QSPI_SS_N", "2": "QSPI_SD1", "3": "QSPI_SD2", "4": "GND", "5": "QSPI_SD0",
     "6": "QSPI_SCLK", "7": "QSPI_SD3", "8": "+3V3_DIG"},
    (52, 35),
    description="16 Mbit QSPI boot flash",
    datasheet=(
        "https://www.winbond.com/hq/support/documentation/downloadV2022.jsp?"
        "__locale=en&xmlPath=/support/resources/.content/item/DA00-W25Q16JV_1.html&level=1"
    ),
    pin_names={"1": "CS_N", "2": "DO_IO1", "3": "WP_N_IO2", "4": "GND", "5": "DI_IO0",
               "6": "CLK", "7": "HOLD_N_IO3", "8": "VCC"},
)
add(
    "02_controller",
    "Y10",
    "ABM8-272-T3_12MHz",
    "Crystal:Crystal_SMD_3225-4Pin_3.2x2.5mm",
    {"1": "RP_XIN", "2": "GND", "3": "RP_XOUT", "4": "GND"},
    (24, 35),
    description="12 MHz, 10 pF load crystal",
    datasheet="https://abracon.com/datasheets/ABM8-272-T3.pdf",
    pin_names={"1": "X1", "2": "CASE", "3": "X2", "4": "CASE"},
)
cap("02_controller", "C20", "15pF_C0G", "RP_XIN", (21, 31))
cap("02_controller", "C21", "15pF_C0G", "RP_XOUT", (27, 31))
cap("02_controller", "C22", "1uF", "RP_CORE", (34, 27), size="0603")
for index, x in enumerate([29, 32, 35, 38, 41, 44], start=23):
    cap("02_controller", f"C{index}", "100nF", "+3V3_DIG", (x, 25))
# RP2040's IOVDD bypasses above do not replace the local VREG_VIN,
# USB_VDD, ADC_AVDD and external-flash supply capacitors.
for ref, x, y in [
    ("C306", 32, 29), ("C307", 38, 29), ("C308", 44, 29), ("C309", 52, 31),
]:
    cap("02_controller", ref, "100nF", "+3V3_DIG", (x, y))

add(
    "02_controller",
    "J10",
    "USB_B_Micro",
    "Connector_USB:USB_Micro-B_Molex-105017-0001",
    {"1": "USB_VBUS_SENSE", "2": "USB_DM_CONN", "3": "USB_DP_CONN", "4": None, "5": "GND", "6": "CHASSIS_GND"},
    (18, 48),
    symbol="Connector:USB_B_Micro",
    description="USB data/control only; main board power is J1",
    datasheet="https://www.molex.com/pdm_docs/sd/1050170001_sd.pdf",
)
add(
    "02_controller",
    "U12",
    "USBLC6-2SC6",
    "Package_TO_SOT_SMD:SOT-23-6",
    {"1": "USB_DM_CONN", "2": "GND", "3": "USB_DP_CONN", "4": "USB_DP_CONN",
     "5": "USB_VBUS_SENSE", "6": "USB_DM_CONN"},
    (27, 48),
    description="Low-capacitance USB ESD protection; paired flow-through pins stay on the connector side of the 27 ohm series resistors",
    datasheet="https://www.st.com/resource/en/datasheet/usblc6-2.pdf",
    pin_names={"1": "I/O1", "2": "GND", "3": "I/O2", "4": "I/O2", "5": "VBUS", "6": "I/O1"},
)
resistor("02_controller", "R20", "27R", "USB_DM_CONN", "USB_DM", (32, 47))
resistor("02_controller", "R21", "27R", "USB_DP_CONN", "USB_DP", (32, 50))
resistor("02_controller", "R22", "100k", "USB_VBUS_SENSE", "GND", (21, 53))
cap("02_controller", "C174", "100nF", "USB_VBUS_SENSE", (24, 53))
add(
    "02_controller",
    "J11",
    "Cortex_SWD_10pin",
    "Connector_PinHeader_1.27mm:PinHeader_2x05_P1.27mm_Vertical",
    {"1": "+3V3_DIG", "2": "SWD_IO", "3": "GND", "4": "SWD_CLK", "5": "GND",
     "6": None, "7": None, "8": None, "9": "GND", "10": "RP_RUN"},
    (57, 48),
    symbol="Connector_Generic:Conn_02x05_Odd_Even",
)
two_pin(
    "02_controller", "SW1", "BOOTSEL", "QSPI_SS_N", "GND", (48, 48),
    footprint="Button_Switch_SMD:SW_SPST_TL3342", symbol="Switch:SW_Push",
)
two_pin(
    "02_controller", "SW2", "RESET", "RP_RUN", "GND", (48, 53),
    footprint="Button_Switch_SMD:SW_SPST_TL3342", symbol="Switch:SW_Push",
)
resistor("02_controller", "R23", "100k", "+3V3_DIG", "RP_RUN", (43, 53))
resistor(
    "02_controller", "R149", "33R", "FPGA_SPI_SCK_MCU", "FPGA_SPI_SCK", (52, 29),
)


# ---------------------------------------------------------------------------
# 03 - FPGA and ADC
# ---------------------------------------------------------------------------
fpga_ball_functions = {
    **{f"A{i}": f"DPHY_N_{i}" for i in range(1, 11)},
    **{f"B{i}": f"DPHY_P_{i}" for i in range(1, 11)},
    "C1": "GND", "C2": "GNDA_DPHY1", "C9": "GNDA_DPHY0", "C10": "GND",
    "D1": "PB48", "D2": "VCCPLL_DPHY1", "D4": "VCCA_DPHY1", "D5": "VCCAUX",
    "D6": "GNDPLL_DPHY", "D7": "VCCPLL_DPHY0", "D9": "PB16A", "D10": "PB16B",
    "E1": "PB34A", "E2": "PB34B", "E4": "VCC", "E5": "GND", "E6": "VCC",
    "E7": "VCCA_DPHY0", "E9": "PB12A", "E10": "PB12B",
    "F1": "PB38A", "F2": "PB38B", "F4": "VCCIO0", "F5": "VCCIO1", "F6": "VCCIO2",
    "F7": "VCCIO2", "F9": "PB6A", "F10": "PB6B",
    "G1": "PB50", "G2": "GND", "G4": "VCCIO1", "G5": "GND", "G6": "VCCGPLL",
    "G7": "GNDGPLL", "G9": "PB2A", "G10": "PB2B",
    "H1": "PB52", "H2": "CRESET_B", "H9": "PB2D", "H10": "PB2C",
    "J1": "PB53", "J2": "PB49", "J3": "PB43D", "J4": "PB38D", "J5": "PB34D",
    "J6": "PB29D", "J7": "PB29A", "J8": "PB16D", "J9": "PB6D", "J10": "PB6C",
    "K1": "PB51", "K2": "PB47", "K3": "PB43C", "K4": "PB38C", "K5": "PB34C",
    "K6": "PB29C", "K7": "PB29B", "K8": "PB16C", "K9": "PB12D", "K10": "PB12C",
}
fpga_pins = {ball: None for ball in fpga_ball_functions}
for ball in ["C1", "C2", "C9", "C10", "D6", "E5", "G2", "G5", "G7"]:
    fpga_pins[ball] = "GND"
# The unused D-PHY analog and PLL supplies (D2/D4/D7/E7) are intentionally
# unpowered, as permitted by FPGA-TN-02013.  Keep every used supply group on
# its own locally filtered island.
for ball in ["E4", "E6"]:
    fpga_pins[ball] = "FPGA_VCC_1V2"
fpga_pins["G6"] = "FPGA_PLL_1V2"
fpga_pins["D5"] = "FPGA_AUX_2V5"
fpga_pins["F4"] = "FPGA_VCCIO0_3V3"
for ball in ["F5", "G4"]:
    fpga_pins[ball] = "FPGA_VCCIO1_3V3"
for ball in ["F6", "F7"]:
    fpga_pins[ball] = "FPGA_VCCIO2_3V3"
fpga_pins.update({
    # CrossLink ctfBGA80 sysCONFIG pins. These must not be moved to generic I/O.
    "G1": "FPGA_SPI_MOSI",   # PB50 / MOSI
    "H1": "FPGA_SPI_CS_N",   # PB52 / SPI_SS
    "J1": "FPGA_SPI_SCK",    # PB53 / SPI_SCK
    "J2": "FPGA_CDONE",      # PB49 / open-drain CDONE
    "K1": "FPGA_SPI_MISO",   # PB51 / MISO
    # ADC clock uses a true primary-clock input; data stays in the same bank.
    "D9": "ADC_CLK",
    "D10": "ADC_D0", "E9": "ADC_D1", "E10": "ADC_D2", "F9": "ADC_D3",
    "F10": "ADC_D4", "G9": "ADC_D5", "G10": "ADC_D6", "H9": "ADC_D7",
    "H10": "ADC_D8", "J8": "ADC_D9", "J9": "ADC_OR",
    # RGMII clocks use bank-1 primary-clock pins PCLKT1_0 and PCLKT1_1.
    "E1": "RGMII_TXD0_FPGA", "E2": "RGMII_TXD1_FPGA", "F1": "RGMII_TXD2_FPGA",
    "F2": "RGMII_TXD3_FPGA", "J3": "RGMII_TX_EN_FPGA", "J7": "RGMII_TXC_FPGA",
    "J4": "RGMII_RXD0_FPGA", "J5": "RGMII_RXD1_FPGA", "K3": "RGMII_RXD2_FPGA",
    "K4": "RGMII_RXD3_FPGA", "K5": "RGMII_RX_DV_FPGA", "K6": "RGMII_RXC_FPGA",
    "J6": "MDC", "K7": "MDIO", "K2": "PHY_INT_N",
})
fpga_pins["H2"] = "FPGA_RESET_N"
add(
    "03_fpga_adc",
    "U20",
    "LIF-MD6000-6JMG80I",
    "CodeSDR:LIF-MD6000-6JMG80I",
    fpga_pins,
    (88, 35),
    description="0.65 mm pitch CrossLink package; enough I/O for ADC plus RGMII without HDI-only 0.4 mm UMG64",
    datasheet="https://www.latticesemi.com/view_document?document_id=51662",
    pin_names=fpga_ball_functions,
)
resistor("03_fpga_adc", "R30", "4.7k", "FPGA_VCCIO0_3V3", "FPGA_RESET_N", (78, 46))
resistor("03_fpga_adc", "R33", "4.7k", "FPGA_VCCIO0_3V3", "FPGA_CDONE", (82, 46))
resistor("03_fpga_adc", "R148", "4.7k", "FPGA_VCCIO0_3V3", "FPGA_SPI_CS_N", (86, 46))
for ref, value, source, island, x in [
    ("FB33", "120R@100MHz", "+1V2_CORE", "FPGA_VCC_1V2", 72),
    ("FB34", "220R@100MHz", "+1V2_CORE", "FPGA_PLL_1V2", 76),
    ("FB35", "120R@100MHz", "+3V3_DIG", "FPGA_VCCIO0_3V3", 80),
    ("FB36", "120R@100MHz", "+3V3_DIG", "FPGA_VCCIO1_3V3", 84),
    ("FB37", "120R@100MHz", "+3V3_DIG", "FPGA_VCCIO2_3V3", 88),
    ("FB38", "120R@100MHz", "+2V5_AUX", "FPGA_AUX_2V5", 92),
]:
    two_pin(
        "03_fpga_adc", ref, value, source, island, (x, 20),
        footprint="Inductor_SMD:L_0402_1005Metric",
        symbol="Device:FerriteBead",
    )
for ref, value, rail, x, y, size in [
    ("C310", "10uF", "FPGA_VCC_1V2", 72, 24, "0805"),
    ("C311", "100nF", "FPGA_VCC_1V2", 74, 26, "0402"),
    ("C312", "100nF", "FPGA_VCC_1V2", 76, 26, "0402"),
    ("C313", "1uF", "FPGA_PLL_1V2", 78, 24, "0402"),
    ("C314", "100nF", "FPGA_PLL_1V2", 80, 26, "0402"),
    ("C315", "2.2uF", "FPGA_VCCIO0_3V3", 82, 24, "0402"),
    ("C316", "100nF", "FPGA_VCCIO0_3V3", 84, 26, "0402"),
    ("C317", "2.2uF", "FPGA_VCCIO1_3V3", 86, 24, "0402"),
    ("C318", "100nF", "FPGA_VCCIO1_3V3", 88, 26, "0402"),
    ("C319", "100nF", "FPGA_VCCIO1_3V3", 90, 26, "0402"),
    ("C320", "2.2uF", "FPGA_VCCIO2_3V3", 92, 24, "0402"),
    ("C321", "100nF", "FPGA_VCCIO2_3V3", 94, 26, "0402"),
    ("C322", "100nF", "FPGA_VCCIO2_3V3", 96, 26, "0402"),
    ("C323", "1uF", "FPGA_AUX_2V5", 98, 24, "0402"),
]:
    cap("03_fpga_adc", ref, value, rail, (x, y), size=size)
for index, net in enumerate([
    "RGMII_TXD0", "RGMII_TXD1", "RGMII_TXD2", "RGMII_TXD3",
    "RGMII_TX_EN", "RGMII_TXC",
]):
    resistor(
        "03_fpga_adc", f"R{120 + index}", "33R",
        f"{net}_FPGA", net, (86 + index * 4, 52),
    )
adc_names = {
    "1": "DNC", "2": "CLK", "3": "DNC", "4": "PDWN", "5": "DNC", "6": "DNC", "7": "DNC", "8": "DNC",
    "9": "D0", "10": "D1", "11": "D2", "12": "D3", "13": "D4", "14": "D5", "15": "DRGND",
    "16": "DRVDD", "17": "D6", "18": "D7", "19": "D8", "20": "D9", "21": "OR", "22": "MODE",
    "23": "SENSE", "24": "VREF", "25": "REFB", "26": "REFT", "27": "AVDD", "28": "AGND",
    "29": "VIN+", "30": "VIN-", "31": "AGND", "32": "AVDD", "33": "EP",
}
adc_pins = {str(i): None for i in range(1, 34)}
adc_pins.update({
    "2": "ADC_CLK", "4": "ADC_PDWN", "9": "ADC_D0", "10": "ADC_D1", "11": "ADC_D2",
    "12": "ADC_D3", "13": "ADC_D4", "14": "ADC_D5", "15": "GND", "16": "+3V3_DIG",
    "17": "ADC_D6", "18": "ADC_D7", "19": "ADC_D8", "20": "ADC_D9", "21": "ADC_OR",
    "22": "ADC_MODE", "23": "ADC_SENSE", "24": "ADC_VREF", "25": "ADC_REFB", "26": "ADC_REFT",
    "27": "+3V3_ANA", "28": "GND", "29": "ADC_VIN_P", "30": "ADC_VIN_N",
    "31": "GND", "32": "+3V3_ANA", "33": "GND",
})
add(
    "03_fpga_adc",
    "U21",
    "AD9215BCPZ-105",
    "Package_CSP:LFCSP-32-1EP_5x5mm_P0.5mm_EP3.6x3.6mm_ThermalVias",
    adc_pins,
    (108, 36),
    description="10-bit ADC operated at 100 MSPS (105 MSPS maximum); DNC pins intentionally floating",
    datasheet="https://www.analog.com/media/en/technical-documentation/data-sheets/ad9215.pdf",
    pin_names=adc_names,
)
resistor("03_fpga_adc", "R31", "33R", "REF_100M_ADC", "ADC_CLK", (116, 25))
cap("03_fpga_adc", "C40", "100nF", "+3V3_ANA", (112, 21))
cap("03_fpga_adc", "C41", "10uF", "+3V3_ANA", (116, 21), size="0805")
cap("03_fpga_adc", "C42", "100nF", "+3V3_DIG", (103, 25))
cap("03_fpga_adc", "C43", "10uF", "+3V3_DIG", (106, 25), size="0805")
cap("03_fpga_adc", "C44", "100nF", "+3V3_ANA", (108, 49))
cap("03_fpga_adc", "C45", "10uF", "+3V3_ANA", (112, 49), size="0805")
cap("03_fpga_adc", "C46", "100nF", "ADC_VREF", (116, 49))
cap("03_fpga_adc", "C47", "100nF", "ADC_REFB", (120, 49))
cap("03_fpga_adc", "C48", "100nF", "ADC_REFT", (124, 49))
resistor("03_fpga_adc", "R32", "0R", "ADC_SENSE", "GND", (121, 42))
resistor("03_fpga_adc", "R147", "10k", "ADC_PDWN", "GND", (125, 42))
cap("03_fpga_adc", "C159", "10uF", "ADC_VREF", (116, 53), size="0805")
cap("03_fpga_adc", "C154", "10uF", "ADC_REFB", (120, 53), size="0805")
cap("03_fpga_adc", "C155", "10uF", "ADC_REFT", (124, 53), size="0805")
resistor("03_fpga_adc", "R90", "2.00k_1%", "+3V3_ANA", "ADC_MODE", (115, 42))
resistor("03_fpga_adc", "R91", "1.00k_1%", "ADC_MODE", "GND", (118, 42))
cap("03_fpga_adc", "C156", "10nF", "ADC_MODE", (118, 46))


# ---------------------------------------------------------------------------
# 04 - Ethernet
# ---------------------------------------------------------------------------
ksz_names = {
    "1": "AVDDH", "2": "TXRXP_A", "3": "TXRXM_A", "4": "AVDDL", "5": "TXRXP_B", "6": "TXRXM_B",
    "7": "TXRXP_C", "8": "TXRXM_C", "9": "AVDDL", "10": "TXRXP_D", "11": "TXRXM_D", "12": "AVDDH",
    "13": "NC", "14": "DVDDL", "15": "LED2_PHYAD1", "16": "DVDDH", "17": "LED1_PHYAD0",
    "18": "DVDDL", "19": "TXD0", "20": "TXD1", "21": "TXD2", "22": "TXD3", "23": "DVDDL",
    "24": "GTX_CLK", "25": "TX_EN", "26": "DVDDL", "27": "RXD3_MODE3",
    "28": "RXD2_MODE2", "29": "VSS", "30": "DVDDL", "31": "RXD1_MODE1",
    "32": "RXD0_MODE0", "33": "RX_DV_CLK125_EN", "34": "DVDDH",
    "35": "RX_CLK_PHYAD2", "36": "MDC", "37": "MDIO", "38": "INT_N_PME_N2",
    "39": "DVDDL", "40": "DVDDH", "41": "CLK125_NDO_LED_MODE",
    "42": "RESET_N", "43": "LDO_O", "44": "AVDDL_PLL", "45": "XO", "46": "XI", "47": "NC",
    "48": "ISET", "49": "EP_GND",
}
ksz_pins = {str(i): None for i in range(1, 50)}
ksz_pins.update({
    "1": "PHY_AVDDH_3V3", "2": "MDI_A_P", "3": "MDI_A_N", "4": "PHY_AVDDL_1V2", "5": "MDI_B_P",
    "6": "MDI_B_N", "7": "MDI_C_P", "8": "MDI_C_N", "9": "PHY_AVDDL_1V2", "10": "MDI_D_P",
    "11": "MDI_D_N", "12": "PHY_AVDDH_3V3", "14": "+1V2_CORE", "15": "PHY_LED2", "16": "+3V3_DIG",
    "17": "PHY_LED1", "18": "+1V2_CORE", "19": "RGMII_TXD0", "20": "RGMII_TXD1",
    "21": "RGMII_TXD2", "22": "RGMII_TXD3", "23": "+1V2_CORE", "24": "RGMII_TXC",
    "25": "RGMII_TX_EN", "26": "+1V2_CORE", "27": "RGMII_RXD3", "28": "RGMII_RXD2",
    "29": "GND", "30": "+1V2_CORE", "31": "RGMII_RXD1", "32": "RGMII_RXD0",
    "33": "RGMII_RX_DV", "34": "+3V3_DIG", "35": "RGMII_RXC", "36": "MDC",
    "37": "MDIO", "38": "PHY_INT_N", "39": "+1V2_CORE", "40": "+3V3_DIG",
    "41": "PHY_CLK125_NDO", "42": "PHY_RESET_N",
    "44": "PHY_PLL_1V2", "45": "PHY_XO", "46": "PHY_XI", "48": "PHY_ISET", "49": "GND",
})
add(
    "04_ethernet",
    "U30",
    "KSZ9031RNXCA",
    "Package_DFN_QFN:QFN-48-1EP_7x7mm_P0.5mm_EP3.5x3.5mm_ThermalVias",
    ksz_pins,
    (145, 35),
    description="Gigabit RGMII PHY using external 1.2 V rail; LDO_O left floating",
    datasheet="https://ww1.microchip.com/downloads/aemDocuments/documents/UNG/ProductDocuments/DataSheets/KSZ9031RNX-Data-Sheet-DS00002117J.pdf",
    pin_names=ksz_names,
)
resistor("04_ethernet", "R40", "12.1k_1%", "PHY_ISET", "GND", (138, 47))
resistor("04_ethernet", "R41", "10k", "+3V3_DIG", "PHY_RESET_N", (142, 47))
for index, net in enumerate([
    "RGMII_RXD0", "RGMII_RXD1", "RGMII_RXD2", "RGMII_RXD3",
    "RGMII_RX_DV", "RGMII_RXC",
]):
    resistor(
        "04_ethernet", f"R{126 + index}", "33R",
        net, f"{net}_FPGA", (138 + index * 4, 53),
    )
resistor("04_ethernet", "R132", "2.2k", "+3V3_DIG", "MDIO", (138, 57))
resistor("04_ethernet", "R133", "2.2k", "+3V3_DIG", "PHY_INT_N", (142, 57))
for index, net in enumerate(["RGMII_RXD3", "RGMII_RXD2", "RGMII_RXD1", "RGMII_RXD0"]):
    resistor("04_ethernet", f"R{134 + index}", "4.7k", "+3V3_DIG", net, (146 + index * 4, 57))
resistor("04_ethernet", "R138", "4.7k", "RGMII_RXC", "GND", (162, 57))
resistor("04_ethernet", "R139", "4.7k", "RGMII_RX_DV", "GND", (166, 57))
resistor("04_ethernet", "R140", "4.7k", "PHY_CLK125_NDO", "GND", (170, 57))
resistor("04_ethernet", "R141", "10k", "+3V3_DIG", "PHY_LED1", (174, 57))
resistor("04_ethernet", "R142", "10k", "+3V3_DIG", "PHY_LED2", (178, 57))
two_pin(
    "04_ethernet", "FB30", "600R@100MHz", "+1V2_CORE", "PHY_PLL_1V2", (148, 47),
    footprint="Inductor_SMD:L_0603_1608Metric_Pad1.05x0.95mm_HandSolder",
    symbol="Device:FerriteBead_Small",
)
two_pin(
    "04_ethernet", "FB31", "120R@100MHz", "+3V3_DIG", "PHY_AVDDH_3V3", (136, 27),
    footprint="Inductor_SMD:L_0603_1608Metric_Pad1.05x0.95mm_HandSolder",
    symbol="Device:FerriteBead_Small",
    description="KSZ9031 AVDDH isolation required by the hardware design checklist",
)
two_pin(
    "04_ethernet", "FB32", "120R@100MHz", "+1V2_CORE", "PHY_AVDDL_1V2", (146, 27),
    footprint="Inductor_SMD:L_0603_1608Metric_Pad1.05x0.95mm_HandSolder",
    symbol="Device:FerriteBead_Small",
    description="KSZ9031 AVDDL isolation required by the hardware design checklist",
)
# Microchip requires one local 0.1 uF at each analog-supply pin, bulk
# capacitance after each ferrite, one bypass per digital-supply pin, and an
# additional 0.01 uF on AVDDL_PLL.
for ref, value, rail, x, y, size in [
    ("C295", "100nF", "PHY_AVDDH_3V3", 138, 29, "0402"),
    ("C296", "100nF", "PHY_AVDDH_3V3", 141, 29, "0402"),
    ("C297", "10uF", "PHY_AVDDH_3V3", 144, 29, "0805"),
    ("C298", "100nF", "PHY_AVDDL_1V2", 148, 29, "0402"),
    ("C299", "100nF", "PHY_AVDDL_1V2", 151, 29, "0402"),
    ("C300", "10uF", "PHY_AVDDL_1V2", 154, 29, "0805"),
    ("C301", "10nF", "PHY_PLL_1V2", 151, 27, "0402"),
    ("C302", "100nF", "+3V3_DIG", 157, 29, "0402"),
    ("C303", "100nF", "+1V2_CORE", 160, 29, "0402"),
    ("C304", "100nF", "+1V2_CORE", 163, 29, "0402"),
    ("C305", "100nF", "+1V2_CORE", 166, 29, "0402"),
]:
    cap("04_ethernet", ref, value, rail, (x, y), size=size)
add(
    "04_ethernet",
    "Y30",
    "ECS-250-20-33-TR_25MHz",
    "Crystal:Crystal_SMD_3225-4Pin_3.2x2.5mm",
    {"1": "PHY_XI", "2": "GND", "3": "PHY_XO", "4": "GND"},
    (157, 47),
    description="25 MHz, 20 pF, 40 ohm maximum-ESR PHY crystal",
    datasheet="https://ecsxtal.com/store/pdf/ECX-32.pdf",
    pin_names={"1": "X1", "2": "CASE", "3": "X2", "4": "CASE"},
)
cap("04_ethernet", "C157", "33pF_C0G", "PHY_XI", (155, 51))
cap("04_ethernet", "C158", "33pF_C0G", "PHY_XO", (159, 51))
add(
    "04_ethernet",
    "J30",
    "JK0654219NL",
    "Connector_RJ:RJ45_Pulse_JK0654219NL_Horizontal",
    {
        "1": "MAG_CT3", "2": "MDI_C_N", "3": "MDI_C_P", "4": "MDI_B_P",
        "5": "MDI_B_N", "6": "MAG_CT2", "7": "MAG_CT4", "8": "MDI_D_P",
        "9": "MDI_D_N", "10": "MDI_A_N", "11": "MDI_A_P", "12": "MAG_CT1",
        "13": "PHY_LED2", "14": "MAG_LED2_A", "15": "PHY_LED1", "16": "MAG_LED1_A",
        "SH": "CHASSIS_GND",
    },
    (173, 35),
    description="Integrated 1:1 magnetics and shielded RJ45; footprint is release-specific",
    datasheet="https://productfinder.pulseelectronics.com/api/open/part-attachments/datasheet/JK0654219NL",
    pin_names={
        "1": "MXCT3", "2": "MX3-", "3": "MX3+", "4": "MX2+", "5": "MX2-",
        "6": "MXCT2", "7": "MXCT4", "8": "MX4+", "9": "MX4-", "10": "MX1-",
        "11": "MX1+", "12": "MXCT1", "13": "LED2_K", "14": "LED2_A",
        "15": "LED1_K", "16": "LED1_A", "SH": "SHIELD",
    },
)
for ref, net, x in [
    ("C150", "MAG_CT1", 164), ("C151", "MAG_CT2", 167),
    ("C152", "MAG_CT3", 170), ("C153", "MAG_CT4", 173),
]:
    cap("04_ethernet", ref, "100nF", net, (x, 47))
resistor("04_ethernet", "R42", "330R", "+3V3_DIG", "MAG_LED1_A", (166, 51))
resistor("04_ethernet", "R43", "330R", "+3V3_DIG", "MAG_LED2_A", (171, 51))
for index, (rail, x, y, size) in enumerate([
    ("+3V3_DIG", 136, 24, "0402"), ("+3V3_DIG", 139, 24, "0402"),
    ("+1V2_CORE", 142, 24, "0402"), ("+1V2_CORE", 145, 24, "0402"),
    ("+1V2_CORE", 148, 24, "0402"), ("PHY_PLL_1V2", 151, 24, "0402"),
    ("+3V3_DIG", 154, 24, "0805"), ("+1V2_CORE", 157, 24, "0805"),
    ("PHY_PLL_1V2", 160, 24, "0805"),
], start=50):
    cap("04_ethernet", f"C{index}", "10uF" if size == "0805" else "100nF", rail, (x, y), size=size)


# ---------------------------------------------------------------------------
# 05 - References and local oscillators
# ---------------------------------------------------------------------------
add(
    "05_reference_lo",
    "Y40",
    "SiT8209AC-23-33E-100.000000X",
    "Oscillator:Oscillator_SMD_SiT_PQFN-4Pin_3.2x2.5mm",
    {"1": "+3V3_ANA", "2": "GND", "3": "REF_100M_OSC", "4": "+3V3_ANA"},
    (119, 88),
    description="100 MHz low-jitter 3.3 V reference oscillator; OE is asserted high",
    datasheet="https://www.sitime.com/support/resource-library/datasheets/sit8209-datasheet",
    pin_names={"1": "OE", "2": "GND", "3": "OUT", "4": "VDD"},
)
add(
    "05_reference_lo",
    "U42",
    "CDCLVC1104PWR",
    "Package_SO:TSSOP-8_3x3mm_P0.65mm",
    {
        "1": "REF_100M_OSC", "2": "+3V3_ANA", "3": "REF_100M_ADC", "4": "GND",
        "5": "REF_100M_ADF", "6": "+3V3_ANA", "7": None, "8": "REF_100M_LMX",
    },
    (126, 88),
    description="Low-additive-jitter 1:4 LVCMOS fanout; one output intentionally unused",
    datasheet="https://www.ti.com/lit/ds/symlink/cdclvc1104.pdf",
    pin_names={"1": "CLKIN", "2": "1G", "3": "Y0", "4": "GND", "5": "Y2",
               "6": "VDD", "7": "Y3", "8": "Y1"},
)
resistor("05_reference_lo", "R50", "33R", "REF_100M_LMX", "LMX_REF_PRE", (132, 87))
resistor("05_reference_lo", "R51", "33R", "REF_100M_ADF", "ADF_REF", (132, 90))
cap("05_reference_lo", "C60", "100nF", "+3V3_ANA", (119, 93))
cap("05_reference_lo", "C61", "10uF", "+3V3_ANA", (123, 93), size="0805")
cap("05_reference_lo", "C270", "100nF", "+3V3_ANA", (127, 93))
cap("05_reference_lo", "C271", "1uF", "+3V3_ANA", (130, 93), size="0603")
two_pin(
    "05_reference_lo", "C200", "100nF", "LMX_REF_PRE", "LMX_REF", (135, 87),
    footprint="Capacitor_SMD:C_0402_1005Metric", symbol="Device:C_Small",
    description="LMX2592 reference-input AC coupling",
)
cap("05_reference_lo", "C272", "100nF", "LMX_REF_N", (137, 93))

lmx_names = {
    "1": "CE", "2": "GND", "3": "VbiasVCO", "4": "GND", "5": "NC", "6": "GND",
    "7": "VccDIG", "8": "OSCinP", "9": "OSCinM", "10": "VregIN", "11": "VccCP",
    "12": "CPout", "13": "GND", "14": "GND", "15": "VccMASH", "16": "SCK",
    "17": "SDI", "18": "RFoutBM", "19": "RFoutBP", "20": "MUXout", "21": "VccBUF",
    "22": "RFoutAM", "23": "RFoutAP", "24": "CSB", "25": "GND", "26": "VccVCO2",
    "27": "VbiasVCO2", "28": "NC", "29": "VrefVCO2", "30": "NC", "31": "GND",
    "32": "NC", "33": "VbiasVARAC", "34": "GND", "35": "Vtune", "36": "VrefVCO",
    "37": "VccVCO", "38": "VregVCO", "39": "GND", "40": "GND", "41": "DAP",
}
lmx_pins = {str(i): None for i in range(1, 42)}
for p in ["2", "4", "6", "13", "14", "25", "31", "34", "39", "40", "41"]:
    lmx_pins[p] = "GND"
for p in ["7", "11", "15", "21", "26", "37"]:
    lmx_pins[p] = "+3V3_ANA"
lmx_pins.update({
    "1": "LMX_CE", "3": "LMX_VBIAS1", "8": "LMX_REF", "9": "LMX_REF_N", "10": "LMX_VREG_IN",
    "12": "LMX_CPOUT", "16": "SYNTH_SCK", "17": "SYNTH_SDI", "18": "LMX_LO_HIGH_N_RAW",
    "19": "LMX_LO_HIGH_P_RAW", "20": "LMX_LOCK", "22": "LMX_LO_LOW_N_RAW",
    "23": "LMX_LO_LOW_P_RAW", "24": "LMX_CSB",
    "27": "LMX_VBIAS2", "29": "LMX_VREF2", "33": "LMX_VARAC", "35": "LMX_VTUNE",
    "36": "LMX_VREF", "38": "LMX_VREG",
})
add(
    "05_reference_lo",
    "U40",
    "LMX2592RHAT",
    "Package_DFN_QFN:Texas_RHA0040B_VQFN-40-1EP_6x6mm_P0.5mm_EP4.15x4.15mm_ThermalVias",
    lmx_pins,
    (140, 88),
    description="20 MHz to 9.8 GHz first LO; both outputs biased per data sheet",
    datasheet="https://www.ti.com/lit/ds/symlink/lmx2592.pdf",
    pin_names=lmx_names,
)
for ref, value, net, x, y, size in [
    ("C62", "10uF", "LMX_VBIAS1", 132, 82, "0805"), ("C63", "1uF", "LMX_VREG_IN", 135, 82, "0603"),
    ("C64", "1uF", "LMX_VBIAS2", 138, 82, "0603"), ("C65", "10uF", "LMX_VREF2", 141, 82, "0805"),
    ("C66", "10uF", "LMX_VARAC", 144, 82, "0805"), ("C67", "10uF", "LMX_VREF", 147, 82, "0805"),
    ("C68", "1uF", "LMX_VREG", 150, 82, "0603"), ("C69", "100nF", "+3V3_ANA", 153, 82, "0402"),
]:
    cap("05_reference_lo", ref, value, net, (x, y), size=size)
for ref, x in [
    ("C273", 132), ("C274", 136), ("C275", 140),
    ("C276", 144), ("C277", 148), ("C278", 152),
]:
    cap("05_reference_lo", ref, "100nF", "+3V3_ANA", (x, 78))
for ref, net, x in [
    ("C279", "+3V3_ANA", 156),
    ("C280", "+3V3_ANA", 160),
    ("C281", "+3V3_ANA", 164),
]:
    cap("05_reference_lo", ref, "10uF", net, (x, 78), size="0805")
resistor("05_reference_lo", "R143", "10k", "LMX_CE", "GND", (132, 96))
resistor("05_reference_lo", "R52", "50R", "+3V3_ANA", "LMX_LO_HIGH_P_RAW", (149, 91))
resistor("05_reference_lo", "R53", "50R", "+3V3_ANA", "LMX_LO_HIGH_N_RAW", (149, 94))
resistor("05_reference_lo", "R54", "50R", "+3V3_ANA", "LMX_LO_LOW_P_RAW", (154, 91))
resistor("05_reference_lo", "R55", "50R", "+3V3_ANA", "LMX_LO_LOW_N_RAW", (154, 94))
for ref, raw, net, x, y in [
    ("C201", "LMX_LO_HIGH_P_RAW", "LO_HIGH_P", 149, 98),
    ("C202", "LMX_LO_HIGH_N_RAW", "LO_HIGH_N_TERM", 152, 98),
    ("C203", "LMX_LO_LOW_P_RAW", "LO_LOW_P", 155, 98),
    ("C204", "LMX_LO_LOW_N_RAW", "LO_LOW_N", 158, 98),
]:
    two_pin(
        "05_reference_lo", ref, "100pF_C0G", raw, net, (x, y),
        footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
        description="LMX2592 RF-output DC block",
    )
resistor("05_reference_lo", "R92", "50R", "LO_HIGH_N_TERM", "GND", (152, 102), size="0201")
cap("05_reference_lo", "C70", "4.7nF_C0G", "LMX_CPOUT", (136, 96))
resistor("05_reference_lo", "R56", "68R_1%", "LMX_CPOUT", "LMX_VTUNE", (141, 96))
cap("05_reference_lo", "C71", "100nF", "LMX_VTUNE", (146, 96))

adf_names = {
    "1": "CPGND", "2": "AVDD", "3": "AGND", "4": "RFOUTA", "5": "RFOUTB", "6": "VVCO",
    "7": "VTUNE", "8": "AGND", "9": "AGND", "10": "AGND", "11": "AGND", "12": "CC",
    "13": "RSET", "14": "CN", "15": "DGND", "16": "REFIN", "17": "CLK", "18": "DATA",
    "19": "LE", "20": "MUXOUT", "21": "DVDD", "22": "AGND", "23": "CE", "24": "CP", "25": "EP",
}
adf_pins = {
    "1": "GND", "2": "+3V3_ANA", "3": "GND", "4": "ADF_LO2_P_RAW", "5": "ADF_LO2_N_RAW",
    "6": "+3V3_ANA", "7": "ADF_VTUNE", "8": "GND", "9": "GND", "10": "GND", "11": "GND",
    "12": "ADF_CC", "13": "ADF_RSET", "14": "ADF_CN", "15": "GND", "16": "ADF_REF",
    "17": "ADF_CLK", "18": "ADF_DATA", "19": "ADF_LE", "20": "ADF_LOCK", "21": "+3V3_ANA",
    "22": "GND", "23": "+3V3_ANA", "24": "ADF_CPOUT", "25": "GND",
}
add(
    "05_reference_lo",
    "U41",
    "ADF4360-1BCPZ",
    "Package_CSP:LFCSP-24-1EP_4x4mm_P0.5mm_EP2.5x2.5mm_ThermalVias",
    adf_pins,
    (165, 88),
    description="Fixed 2.410 GHz integer-N second LO: 100 MHz/R20, P16, B30, A2",
    datasheet="https://www.analog.com/media/en/technical-documentation/data-sheets/adf4360-1.pdf",
    pin_names=adf_names,
)
cap("05_reference_lo", "C72", "10nF", "ADF_CC", (168, 96))
two_pin(
    "05_reference_lo", "C73", "10uF", "ADF_CN", "+3V3_ANA", (171, 96),
    footprint="Capacitor_SMD:C_0805_2012Metric", symbol="Device:C_Small",
    description="ADF4360-1 CN compensation capacitor to VVCO",
)
resistor("05_reference_lo", "R58", "4.7k_1%", "ADF_RSET", "GND", (158, 96))
cap("05_reference_lo", "C74", "1nF_C0G", "ADF_CPOUT", (155, 96))
resistor("05_reference_lo", "R59", "620R_1%", "ADF_CPOUT", "ADF_LF1", (161, 96))
cap("05_reference_lo", "C75", "3.3nF_C0G", "ADF_LF1", (164, 100))
resistor("05_reference_lo", "R93", "51R", "ADF_LF1", "ADF_VTUNE", (168, 100))
cap("05_reference_lo", "C205", "15nF_C0G", "ADF_VTUNE", (172, 100))
resistor("05_reference_lo", "R94", "51R", "+3V3_ANA", "ADF_LO2_P_RAW", (158, 104))
resistor("05_reference_lo", "R95", "51R", "+3V3_ANA", "ADF_LO2_N_RAW", (163, 104))
for ref, raw, net, x in [
    ("C206", "ADF_LO2_P_RAW", "LO2_P", 168),
    ("C207", "ADF_LO2_N_RAW", "LO2_N", 173),
]:
    two_pin(
        "05_reference_lo", ref, "100pF_C0G", raw, net, (x, 104),
        footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
        description="ADF4360-1 RF-output DC block",
    )
for ref, net, x in [("C76", "+3V3_ANA", 158), ("C77", "+3V3_ANA", 161), ("C78", "+3V3_ANA", 164)]:
    cap("05_reference_lo", ref, "100nF", net, (x, 80))


# ---------------------------------------------------------------------------
# 06 - Low RF input and first mixer
# ---------------------------------------------------------------------------
add(
    "06_rf_low",
    "J50",
    "RF_LOW_0.1_2.5GHz",
    "Connector_Coaxial:SMA_Molex_73251-1153_EdgeMount_Horizontal",
    {"1": "RF_LOW_IN", "2": "GND"},
    (5, 63),
    symbol="Connector:Conn_Coaxial",
    description="Dedicated 100 MHz to 2.5 GHz edge-launch RF input",
    datasheet="https://www.molex.com/pdm_docs/sd/732511153_sd.pdf",
)
two_pin(
    "06_rf_low", "D50", "PESD1V0R1BCSF", "RF_LOW_IN", "GND", (12, 63),
    footprint="CodeSDR:SOD962-2_DSN0603", symbol="Device:D_TVS",
    description="0.1 pF RF-port ESD protection; avoids the multi-pF low-band TVS loss",
    datasheet="https://assets.nexperia.com/documents/short-data-sheet/PESD1V0R1BCSF_SDS.pdf",
)
add(
    "06_rf_low",
    "FL50",
    "LFCN-2500+",
    "CodeSDR:MiniCircuits_FV1206",
    {"1": "RF_LOW_IN", "2": "GND", "3": "RF_LOW_FILT", "4": "GND"},
    (20, 63),
    description="LTCC low-pass preselector; 0.1-2.5 GHz receive path",
    datasheet="https://www.minicircuits.com/pdfs/LFCN-2500+.pdf",
    pin_names={"1": "IN", "2": "GND", "3": "OUT", "4": "GND"},
)
add(
    "06_rf_low",
    "U50",
    "GRF4001",
    "CodeSDR:GRF_DFN6_1.5x1.5mm",
    {"1": "LNA_EN_LOW_IC", "2": "GND", "3": "RF_LOW_LNA_IN", "4": "RF_LOW_LNA_OUT_RAW",
     "5": "GND", "6": "GND", "7": "GND"},
    (30, 63),
    description="Internally matched 0.1-6 GHz LNA; 3.3 V/45 mA cost-first low-noise stage",
    datasheet="https://www.guerrilla-rf.com/includes/prodFiles/4001/GRF4001DS.pdf",
    pin_names={"1": "VENABLE", "2": "GND", "3": "RF_IN", "4": "RF_OUT_VDD",
               "5": "GND_NC", "6": "GND_NC", "7": "EP"},
)
two_pin(
    "06_rf_low", "C208", "100pF_C0G", "RF_LOW_FILT", "RF_LOW_LNA_IN", (25, 63),
    footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="GRF4001 input DC block",
)
cap("06_rf_low", "C209", "0.2pF_C0G", "RF_LOW_LNA_IN", (28, 68), size="0201")
inductor("06_rf_low", "L80", "220nH", "+3V3_ANA", "RF_LOW_LNA_OUT_RAW", (33, 68), size="0603")
two_pin(
    "06_rf_low", "C210", "100pF_C0G", "RF_LOW_LNA_OUT_RAW", "RF_LOW_LNA_OUT", (36, 63),
    footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="GRF4001 output DC block",
)
resistor("06_rf_low", "R60", "0R", "LNA_EN_LOW", "LNA_EN_LOW_IC", (30, 72))
cap("06_rf_low", "C80", "100pF_C0G", "+3V3_ANA", (34, 72))
cap("06_rf_low", "C81", "100nF", "+3V3_ANA", (37, 72))
add(
    "06_rf_low",
    "U51",
    "ADL5801ACPZ-R7",
    "Package_CSP:LFCSP-24-1EP_4x4mm_P0.5mm_EP2.5x2.5mm_ThermalVias",
    {
        "1": "GND", "2": "GND", "3": "LO_LOW_P", "4": "LO_LOW_N", "5": "GND", "6": "GND",
        "7": "+5V_RF", "8": "GND", "9": "GND", "10": "ADL_VSET", "11": "ADL_DETO",
        "12": "GND", "13": "+5V_RF", "14": "GND", "15": "ADL_RF_UNUSED", "16": "ADL_RF_DRIVE",
        "17": "GND", "18": "+5V_RF", "19": "GND", "20": "IF_LOW_P", "21": "IF_LOW_N",
        "22": None, "23": "GND", "24": "+5V_RF", "25": "GND",
    },
    (48, 63),
    description="Low-band first mixer; differential 2.43625 GHz IF",
    datasheet="https://www.analog.com/en/products/adl5801.html",
    pin_names={str(i): name for i, name in enumerate([
        "GND", "GND", "LOIP", "LOIN", "GND", "GND", "VPLO", "GND", "ENBL_N", "VSET", "DETO",
        "GND", "VPDT", "GND", "RFIN", "RFIP", "GND", "VPRF", "GND", "IFOP", "IFON", "NC", "GND", "VPLO", "EP"
    ], 1)},
)
two_pin(
    "06_rf_low", "C211", "1nF_C0G", "RF_LOW_LNA_OUT", "ADL_RF_DRIVE", (42, 63),
    footprint="Capacitor_SMD:C_0402_1005Metric", symbol="Device:C_Small",
    description="ADL5801 driven RF input AC coupling",
)
cap("06_rf_low", "C212", "1nF_C0G", "ADL_RF_UNUSED", (43, 67))
resistor("06_rf_low", "R61", "1.5k_1%", "+5V_RF", "ADL_VSET", (45, 70))
cap("06_rf_low", "C213", "100pF_C0G", "ADL_VSET", (46, 74))
cap("06_rf_low", "C214", "100nF", "ADL_DETO", (49, 74))
cap("06_rf_low", "C82", "100nF", "+5V_RF", (50, 70))
cap("06_rf_low", "C83", "10uF", "+5V_RF", (54, 70), size="0805")
add(
    "06_rf_low",
    "T50",
    "2450BL15B0200001E",
    "CodeSDR:Johanson_0805_6P_Balun",
    {"1": "IF_LOW_SE", "2": "+5V_RF", "3": "IF_LOW_P", "4": "IF_LOW_N", "5": "GND", "6": None},
    (61, 63),
    description="Active 2.4-2.5 GHz 50-to-200 ohm balun; center-tap DC feed biases the ADL5801 IF outputs",
    datasheet="https://www.johansontechnology.com/docs/4500/Balun-2450BL15B0200001E.pdf",
    pin_names={"1": "UNBALANCED", "2": "DC_FEED_RF_GND", "3": "BALANCED_1",
               "4": "BALANCED_2", "5": "GND", "6": "NC"},
)
cap("06_rf_low", "C282", "100pF_C0G", "+5V_RF", (58, 58))
cap("06_rf_low", "C283", "100nF", "+5V_RF", (61, 58))


# ---------------------------------------------------------------------------
# 07 - High RF front end, manual input ports and post-LNA SP4T
# ---------------------------------------------------------------------------
high_paths = [
    # suffix, y, characterized band, LNA, U reference, input series C, input shunt C,
    # VDD match capacitor, enable series R
    ("A", 76.0, "2.3_4.2GHz", "GRF4001", "U60", "100pF", "0.2pF", None, "0R"),
    ("B", 84.0, "4.2_6.8GHz", "GRF2101", "U61", "0.6pF", "22pF", None, "1.5k"),
    ("C", 92.0, "6.8_9.2GHz", "GRF2101", "U62", "0.8pF", "0.3pF", "2.2pF", "200R"),
    ("D", 100.0, "8.0_10GHz", "GRF2101", "U66", "0.5pF", "0.2pF", "1.0pF", "2.9k"),
]
for index, (suffix, y, band, lna, uref, cin, csh_in, vdd_match, ren) in enumerate(high_paths):
    add(
        "07_rf_high",
        f"J6{index}",
        f"RF_HIGH_{band}",
        "Connector_Coaxial:SMA_Molex_73251-1153_EdgeMount_Horizontal",
        {"1": f"RF_H{suffix}_IN", "2": "GND"},
        (5, y),
        symbol="Connector:Conn_Coaxial",
        description="Dedicated sub-band port removes a lossy/expensive 10 GHz input switch",
        datasheet="https://www.molex.com/pdm_docs/sd/732511153_sd.pdf",
    )
    two_pin(
        "07_rf_high", f"D6{index}", "PESD1V0R1BCSF", f"RF_H{suffix}_IN", "GND", (12, y),
        footprint="CodeSDR:SOD962-2_DSN0603", symbol="Device:D_TVS",
        description="0.1 pF RF-port ESD protection; placed before the input DC block",
        datasheet="https://assets.nexperia.com/documents/short-data-sheet/PESD1V0R1BCSF_SDS.pdf",
    )
    two_pin(
        "07_rf_high", f"C21{index+5}", f"{cin}_C0G", f"RF_H{suffix}_IN", f"RF_H{suffix}_LNA_IN",
        (17, y), footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
        description=f"{lna} {band} input series match/DC block",
    )
    cap("07_rf_high", f"C21{index+9}", f"{csh_in}_C0G", f"RF_H{suffix}_LNA_IN", (20, y + 3), size="0201")
    if lna == "GRF4001":
        lna_pins = {
            "1": f"RF_H{suffix}_ENABLE", "2": "GND", "3": f"RF_H{suffix}_LNA_IN",
            "4": f"RF_H{suffix}_LNA_OUT_RAW", "5": "GND", "6": "GND", "7": "GND",
        }
        description = f"{band} internally matched broadband low-noise branch, 3.3 V/45 mA"
        datasheet = "https://www.guerrilla-rf.com/includes/prodFiles/4001/GRF4001DS.pdf"
    else:
        lna_pins = {
            "1": f"RF_H{suffix}_ENABLE", "2": "GND", "3": f"RF_H{suffix}_LNA_IN",
            "4": f"RF_H{suffix}_LNA_OUT_RAW", "5": "GND", "6": "+3V3_ANA", "7": "GND",
        }
        description = f"{band} manufacturer-characterized GRF2101 match; inactive branch is disabled"
        datasheet = "https://www.guerrilla-rf.com/includes/prodFiles/2101/GRF2101DS.pdf"
    add(
        "07_rf_high",
        uref,
        lna,
        "CodeSDR:GRF_DFN6_1.5x1.5mm",
        lna_pins,
        (23, y),
        description=description,
        datasheet=datasheet,
        pin_names={"1": "VENABLE", "2": "GND_NC", "3": "RF_IN", "4": "RF_OUT_VDD"
                   if lna == "GRF4001" else "RF_OUT", "5": "GND_NC",
                   "6": "GND_NC" if lna == "GRF4001" else "VDD", "7": "EP"},
    )
    resistor(
        "07_rf_high", f"R{100+index}", ren, f"LNA_EN_H{index+1}", f"RF_H{suffix}_ENABLE",
        (27, y + 3), size="0201",
    )
    if lna == "GRF4001":
        inductor(
            "07_rf_high", "L83", "220nH", "+3V3_ANA", f"RF_H{suffix}_LNA_OUT_RAW",
            (30, y + 3), size="0603",
        )
        two_pin(
            "07_rf_high", "C223", "100pF_C0G", f"RF_H{suffix}_LNA_OUT_RAW",
            f"RF_H{suffix}_LNA_OUT", (33, y),
            footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
            description="GRF4001 output DC block",
        )
    else:
        lna_output_net = (
            "RF_HD_LNA1_OUT" if suffix == "D" else f"RF_H{suffix}_LNA_OUT"
        )
        resistor(
            "07_rf_high", f"R{104+index}", "0R", f"RF_H{suffix}_LNA_OUT_RAW",
            lna_output_net, (33, y), size="0201",
        )
    cap("07_rf_high", f"C22{4+index}", "100nF", "+3V3_ANA", (30, y + 6), size="0402")
    if vdd_match:
        cap(
            "07_rf_high", f"C22{8+index}", f"{vdd_match}_C0G",
            "+3V3_ANA", (35, y + 3), size="0201",
        )

# The manufacturer-characterized 8-10 GHz GRF2101 tune has only about
# 12.3 dB gain at the top of band.  A second $1.14 stage on the D branch,
# still ahead of the selector, is the cheapest way to prevent the mixer/BAW
# losses from dominating the 9.2-10 GHz noise figure.
two_pin(
    "07_rf_high", "C291", "0.5pF_C0G", "RF_HD_LNA1_OUT", "RF_HD_LNA2_IN",
    (39, 100), footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="Second D-port GRF2101 8-10 GHz input series match",
)
cap("07_rf_high", "C292", "0.2pF_C0G", "RF_HD_LNA2_IN", (42, 103), size="0201")
add(
    "07_rf_high",
    "U67",
    "GRF2101",
    "CodeSDR:GRF_DFN6_1.5x1.5mm",
    {
        "1": "RF_HD2_ENABLE", "2": "GND", "3": "RF_HD_LNA2_IN",
        "4": "RF_HD_LNA2_OUT_RAW", "5": "GND", "6": "+3V3_ANA", "7": "GND",
    },
    (46, 100),
    description="Second 8-10 GHz D-port LNA; improves top-band NF at minimal BOM cost",
    datasheet="https://www.guerrilla-rf.com/includes/prodFiles/2101/GRF2101%20%208000-10000%20MHz.pdf",
    pin_names={"1": "VENABLE", "2": "GND_NC", "3": "RF_IN", "4": "RF_OUT",
               "5": "GND_NC", "6": "VDD", "7": "EP"},
)
resistor(
    "07_rf_high", "R112", "2.9k", "LNA_EN_H4", "RF_HD2_ENABLE",
    (46, 104), size="0201",
)
resistor(
    "07_rf_high", "R111", "0R", "RF_HD_LNA2_OUT_RAW", "RF_HD_LNA_OUT",
    (50, 100), size="0201",
)
cap("07_rf_high", "C293", "100nF", "+3V3_ANA", (46, 106), size="0402")
cap("07_rf_high", "C294", "1.0pF_C0G", "+3V3_ANA", (50, 103), size="0201")
add(
    "07_rf_high",
    "U64",
    "QPC6144TR13",
    "CodeSDR:QPC6144_LGA24_4x4mm",
    {
        "1": "GND", "2": "GND", "3": "GND", "4": "GND",
        # Port assignment is physical, not alphabetical: the 6.8-10 GHz C/D
        # branches use the nearest left/right RF pins to minimize line loss.
        "5": "RF_HC_LNA_OUT", "6": "GND", "7": "GND", "8": "RF_HA_LNA_OUT",
        "9": "GND", "10": "GND", "11": "RF_HB_LNA_OUT", "12": "GND",
        "13": "GND", "14": "RF_HD_LNA_OUT", "15": "GND", "16": "+3V3_ANA",
        "17": "HSW_V1", "18": "HSW_V2", "19": "GND", "20": "GND",
        "21": "GND", "22": "RF_HIGH_SWITCHED", "23": "GND", "24": "GND",
        "25": "GND",
    },
    (40, 86),
    description=(
        "50 MHz to 12 GHz high-isolation SP4T after the branch LNAs; "
        "LC0/LC1 low select 1.8 V logic and V3/VSS are grounded"
    ),
    datasheet="https://www.qorvo.com/products/d/da010159",
    pin_names={
        "1": "LC0", "2": "GND", "3": "GND", "4": "GND", "5": "RF4",
        "6": "GND", "7": "GND", "8": "RF3", "9": "GND", "10": "GND",
        "11": "RF2", "12": "GND", "13": "GND", "14": "RF1", "15": "GND",
        "16": "VDD", "17": "V1", "18": "V2", "19": "V3", "20": "VSS_GND",
        "21": "GND", "22": "RFC", "23": "GND", "24": "LC1", "25": "EP_GND",
    },
)
cap("07_rf_high", "C260", "100nF", "+3V3_ANA", (41, 91), size="0402")
cap("07_rf_high", "C261", "10uF", "+3V3_ANA", (45, 91), size="0805")
resistor("07_rf_high", "R144", "100k", "HSW_V1", "GND", (41, 95))
resistor("07_rf_high", "R145", "100k", "HSW_V2", "GND", (45, 95))

add(
    "07_rf_high",
    "U63",
    "LTC5548IUDB",
    "CodeSDR:DFN-12-1EP_3x2mm_P0.5mm",
    {
        "1": "GND", "2": "IF_HIGH_P", "3": "IF_HIGH_N", "4": "GND", "5": "LTC_RF_IN",
        "6": "GND", "7": "MIXER_EN_HIGH", "8": "GND", "9": "+3V3_ANA", "10": "GND",
        "11": "LTC_LO_IN", "12": "GND", "13": "GND",
    },
    (52, 86),
    description="2-14 GHz high-linearity first mixer, X2 disabled",
    datasheet="https://www.analog.com/media/en/technical-documentation/data-sheets/5548f.pdf",
    pin_names={"1": "GND", "2": "IF+", "3": "IF-", "4": "GND", "5": "RF", "6": "GND",
               "7": "EN", "8": "X2", "9": "VCC", "10": "GND", "11": "LO",
               "12": "GND", "13": "EP"},
)
two_pin(
    "07_rf_high", "C232", "100pF_C0G", "RF_HIGH_SWITCHED", "LTC_RF_IN", (46, 86),
    footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="LTC5548 RF-input DC block",
)
two_pin(
    "07_rf_high", "C233", "100pF_C0G", "LO_HIGH_P", "LTC_LO_IN", (49, 82),
    footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="LTC5548 single-ended LO-input DC block",
)
cap("07_rf_high", "C93", "100nF", "+3V3_ANA", (55, 92))
cap("07_rf_high", "C94", "10uF", "+3V3_ANA", (59, 92), size="0805")
add(
    "07_rf_high",
    "T60",
    "TC1-1-13M+",
    "Transformer_SMD:Transformer_MiniCircuits_AT224-1A",
    {"1": "IF_HIGH_SE", "2": None, "3": "GND", "4": "IF_HIGH_N", "6": "IF_HIGH_P"},
    (65, 86),
    description="LTC5548 datasheet-recommended 1:1, 50 ohm differential-to-single-ended IF transformer",
    datasheet="https://www.minicircuits.com/pdfs/TC1-1-13M+.pdf",
    pin_names={"1": "SECONDARY_DOT", "2": "NC", "3": "SECONDARY_GND",
               "4": "PRIMARY", "6": "PRIMARY_DOT"},
)
resistor("08_if_chain", "R146", "100k", "IF_PATH_SEL", "GND", (73, 73))


# ---------------------------------------------------------------------------
# 08 - Common IF and ADC driver
# ---------------------------------------------------------------------------
add(
    "08_if_chain",
    "U70",
    "BGS12WN6E6327",
    "CodeSDR:Infineon_TSNP6_1.1x0.7mm",
    {"1": "IF_HIGH_SE", "2": "GND", "3": "IF_LOW_SE", "4": "+3V3_DIG", "5": "IF_SELECTED", "6": "IF_PATH_SEL"},
    (73, 67),
    description="First-IF low/high path selector at 2.43625 GHz",
    datasheet="https://www.infineon.com/assets/row/public/documents/24/49/infineon-bgs12wn6-datasheet-en.pdf",
    pin_names={"1": "RF2", "2": "GND", "3": "RF1", "4": "VDD", "5": "RFC", "6": "CTRL"},
)
add(
    "08_if_chain",
    "FL70",
    "QPQ1909TR13",
    "CodeSDR:QPQ1909_1.4x1.2mm",
    {"1": "IF_SELECTED", "2": "GND", "3": "GND", "4": "IF_BAW_OUT", "5": "GND"},
    (80, 67),
    description="2402.5-2493.5 MHz BAW filter; public five-pin pinout and complete 40 MHz channel",
    datasheet="https://www.mouser.com/datasheet/2/412/QPQ1909_Data_Sheet-1815271.pdf",
    pin_names={"1": "IN", "2": "GND", "3": "GND", "4": "OUT", "5": "GND"},
)
add(
    "08_if_chain",
    "U71",
    "GRF2013",
    "CodeSDR:GRF_DFN6_1.5x1.5mm",
    {"1": "GRF2013_ENABLE", "2": "GND", "3": "GRF2013_RFIN", "4": "GRF2013_RFOUT_VDD",
     "5": "GND", "6": "GND", "7": "GND"},
    (87, 67),
    description="2.4 GHz IF gain block using the released data-sheet 5 V/90 mA 0.7-3.8 GHz tune",
    datasheet="https://www.guerrilla-rf.com/includes/prodFiles/2013/GRF2013DS.pdf",
    pin_names={"1": "VENABLE", "2": "GND_NC", "3": "RF_IN", "4": "RF_OUT_VDD",
               "5": "GND_NC", "6": "GND_NC", "7": "EP"},
)
two_pin(
    "08_if_chain", "C234", "100pF_C0G", "IF_BAW_OUT", "GRF2013_RFIN", (84, 67),
    footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="GRF2013 input DC block",
)
resistor("08_if_chain", "R70", "499R_1%", "+5V_RF", "GRF2013_ENABLE", (87, 73))
inductor(
    "08_if_chain", "L84", "33nH", "+5V_RF", "GRF2013_RFOUT_VDD",
    (91, 73), size="0402",
)
cap("08_if_chain", "C100", "100nF", "+5V_RF", (95, 73))
two_pin(
    "08_if_chain", "C235", "100pF_C0G", "GRF2013_RFOUT_VDD", "IF_GAIN_OUT", (91, 67),
    footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="GRF2013 output DC block",
)
add(
    "08_if_chain",
    "U72",
    "PE4312C-Z",
    "Package_DFN_QFN:QFN-20-1EP_4x4mm_P0.5mm_EP2.6x2.6mm_ThermalVias",
    {
        "1": "DSA_C16", "2": "PE4312_RF1", "3": "DSA_DATA_PIN", "4": "DSA_CLK",
        "5": "DSA_LE", "6": "+3V3_DIG", "7": "GND", "8": "GND", "9": "+3V3_DIG",
        "10": "GND", "11": "GND", "12": "GND", "13": "+3V3_DIG",
        "14": "PE4312_RF2", "15": "+3V3_DIG", "16": "+3V3_DIG", "17": "+3V3_DIG",
        "18": "GND", "19": "+3V3_DIG", "20": "+3V3_DIG", "21": "GND",
    },
    (96, 67),
    description="Active 1 MHz-4 GHz, 0.5 dB-step, 31.5 dB serial DSA; powers up at maximum attenuation",
    datasheet="https://www.psemi.com/pdf/datasheets/pe4312ds.pdf",
    pin_names={
        "1": "C16", "2": "RF1", "3": "DATA", "4": "CLOCK", "5": "LE", "6": "VDD",
        "7": "PUP1", "8": "PUP2", "9": "VDD", "10": "GND", "11": "GND",
        "12": "VSS_EXT_GND", "13": "P/S", "14": "RF2", "15": "C8", "16": "C4",
        "17": "C2", "18": "GND", "19": "C1", "20": "C0.5", "21": "EP",
    },
)
two_pin(
    "08_if_chain", "C236", "100pF_C0G", "IF_GAIN_OUT", "PE4312_RF1", (94, 67),
    footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="PE4312 RF1 DC block; guarantees the required 0 VDC RF-port condition",
)
two_pin(
    "08_if_chain", "C237", "100pF_C0G", "PE4312_RF2", "IF_DSA_OUT", (100, 67),
    footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="PE4312 RF2 DC block; guarantees the required 0 VDC RF-port condition",
)
two_pin(
    "08_if_chain", "R77", "10k_1%", "DSA_DATA", "DSA_DATA_PIN", (94, 76),
    footprint="Resistor_SMD:R_0402_1005Metric", symbol="Device:R_Small",
    description="PE4312 pin 3 package-resonance damping; place at the device",
)
two_pin(
    "08_if_chain", "R78", "10k_1%", "+3V3_DIG", "DSA_C16", (98, 76),
    footprint="Resistor_SMD:R_0402_1005Metric", symbol="Device:R_Small",
    description="PE4312 pin 1 damping and maximum-attenuation power-up strap",
)
cap("08_if_chain", "C101", "100nF", "+3V3_DIG", (102, 76))
add(
    "08_if_chain",
    "U75",
    "GRF2013",
    "CodeSDR:GRF_DFN6_1.5x1.5mm",
    {
        "1": "GRF2013B_ENABLE", "2": "GND", "3": "IF_DSA_OUT",
        "4": "GRF2013B_RFOUT_VDD", "5": "GND", "6": "GND", "7": "GND",
    },
    (102, 72),
    description=(
        "Second 2.4 GHz IF gain block after the fail-safe DSA; restores "
        "10-bit ADC quantization margin on the 8-10 GHz worst-case path"
    ),
    datasheet="https://www.guerrilla-rf.com/includes/prodFiles/2013/GRF2013DS.pdf",
    pin_names={"1": "VENABLE", "2": "GND_NC", "3": "RF_IN", "4": "RF_OUT_VDD",
               "5": "GND_NC", "6": "GND_NC", "7": "EP"},
)
resistor("08_if_chain", "R81", "499R_1%", "+5V_RF", "GRF2013B_ENABLE", (103, 77))
inductor(
    "08_if_chain", "L97", "33nH", "+5V_RF", "GRF2013B_RFOUT_VDD",
    (106, 77), size="0402",
)
cap("08_if_chain", "C284", "100nF", "+5V_RF", (109, 77))
two_pin(
    "08_if_chain", "C238", "100pF_C0G", "GRF2013B_RFOUT_VDD", "IF_DSA_GAINED",
    (107, 72),
    footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="Second GRF2013 output DC block",
)
add(
    "08_if_chain",
    "T70",
    "2450BL15B0100001E",
    "CodeSDR:Johanson_0805_6P_Balun",
    {"1": "LT_INPUT_SE", "2": "LT_INPUT_CT", "3": "LT_IN_P", "4": "LT_IN_N", "5": "GND", "6": None},
    (103, 67),
    description="Active, low-cost 2.4-2.5 GHz 50-to-100 ohm input balun",
    datasheet="https://www.johansontechnology.com/docs/4499/Balun-2450BL15B0100001E.pdf",
    pin_names={"1": "UNBALANCED", "2": "DC_FEED_RF_GND", "3": "BALANCED_1", "4": "BALANCED_2",
               "5": "GND", "6": "NC"},
)
two_pin(
    "08_if_chain", "C245", "100pF_C0G", "IF_DSA_GAINED", "LT_INPUT_SE", (101, 63),
    footprint="Capacitor_SMD:C_0201_0603Metric", symbol="Device:C_Small",
    description="LT5560 input AC coupling",
)
cap("08_if_chain", "C246", "1.0pF_C0G", "LT_INPUT_SE", (103, 63), size="0201")
add(
    "08_if_chain",
    "U73",
    "LT5560EDD",
    "Package_DFN_QFN:DFN-8-1EP_3x3mm_P0.5mm_EP1.65x2.38mm_ThermalVias",
    {"1": "LO2_N", "2": "+3V3_DIG", "3": "LT_IN_P", "4": "LT_IN_N", "5": "IF2_N",
     "6": "IF2_P", "7": "+3V3_ANA", "8": "LO2_P", "9": "GND"},
    (110, 67),
    description="2.410 GHz second mixer producing 26.25 MHz center IF",
    datasheet="https://www.analog.com/en/products/lt5560.html",
    pin_names={"1": "LO-", "2": "EN", "3": "IN+", "4": "IN-", "5": "OUT-", "6": "OUT+",
               "7": "VCC", "8": "LO+", "9": "EP"},
)
resistor("08_if_chain", "R71", "3.0R_1%", "LT_INPUT_CT", "GND", (106, 74))
cap("08_if_chain", "C102", "1nF_C0G", "+3V3_ANA", (113, 74))
cap("08_if_chain", "C247", "1uF", "+3V3_ANA", (116, 74), size="0603")
inductor("08_if_chain", "L85", "2.2uH", "+3V3_ANA", "IF2_P", (114, 62), size="0603")
inductor("08_if_chain", "L86", "2.2uH", "+3V3_ANA", "IF2_N", (114, 72), size="0603")

# Differential 9th-order, 0.1 dB Chebyshev anti-alias low-pass, 46.5 MHz,
# normalized for 100 ohm differential source/load. The two terminations make
# the real LT5560 and AD8351 impedances predictable and suppress passband peaking.
resistor("08_if_chain", "R79", "100R_1%", "IF2_P", "IF2_N", (116, 76))
filter_sections = [
    ("200nH", "IF2_P", "IF2_F1_P", "IF2_N", "IF2_F1_N", 118),
    ("360nH", "IF2_F1_P", "IF2_F2_P", "IF2_F1_N", "IF2_F2_N", 122),
    ("390nH", "IF2_F2_P", "IF2_F3_P", "IF2_F2_N", "IF2_F3_N", 126),
    ("360nH", "IF2_F3_P", "IF2_F4_P", "IF2_F3_N", "IF2_F4_N", 130),
    ("200nH", "IF2_F4_P", "IF2_LPF_P", "IF2_F4_N", "IF2_LPF_N", 134),
]
for index, (value, net_a_p, net_b_p, net_a_n, net_b_n, x) in enumerate(filter_sections):
    inductor("08_if_chain", f"L{87 + index * 2}", value, net_a_p, net_b_p, (x, 65), size="0603")
    inductor("08_if_chain", f"L{88 + index * 2}", value, net_a_n, net_b_n, (x, 69), size="0603")
for index, (value, net_p, net_n, x) in enumerate([
    ("100pF_C0G", "IF2_F1_P", "IF2_F1_N", 120),
    ("110pF_C0G", "IF2_F2_P", "IF2_F2_N", 124),
    ("110pF_C0G", "IF2_F3_P", "IF2_F3_N", 128),
    ("100pF_C0G", "IF2_F4_P", "IF2_F4_N", 132),
]):
    two_pin(
        "08_if_chain", f"C{262 + index * 2}", value, net_p, net_n, (x, 67),
        footprint="Capacitor_SMD:C_0402_1005Metric", symbol="Device:C_Small",
        description="Differential anti-alias ladder shunt capacitor",
    )
resistor("08_if_chain", "R80", "100R_1%", "IF2_LPF_P", "IF2_LPF_N", (136, 76))
two_pin(
    "08_if_chain", "C254", "100nF", "IF2_LPF_P", "AD8351_IN_P", (137, 65),
    footprint="Capacitor_SMD:C_0402_1005Metric", symbol="Device:C_Small",
    description="AD8351 INHI AC coupling",
)
two_pin(
    "08_if_chain", "C255", "100nF", "IF2_LPF_N", "AD8351_IN_N", (137, 69),
    footprint="Capacitor_SMD:C_0402_1005Metric", symbol="Device:C_Small",
    description="AD8351 INLO AC coupling",
)
add(
    "08_if_chain",
    "U74",
    "AD8351ARMZ",
    "Package_SO:MSOP-10_3x3mm_P0.5mm",
    {"1": "+5V_RF", "2": "AD8351_RGP1", "3": "AD8351_IN_P", "4": "AD8351_IN_N",
     "5": "AD8351_RGP2", "6": "GND", "7": "AD8351_OUT_N", "8": "AD8351_OUT_P",
     "9": "+5V_RF", "10": "ADC_VCM"},
    (143, 67),
    description="Low-distortion differential ADC driver; gain set for AD9215 full scale",
    datasheet="https://www.analog.com/en/products/ad8351.html",
    pin_names={"1": "PWUP", "2": "RGP1", "3": "INHI", "4": "INLO", "5": "RGP2",
               "6": "COMM", "7": "OPLO", "8": "OPHI", "9": "VPOS", "10": "VOCM"},
)
resistor("08_if_chain", "R72", "33R_1%", "AD8351_RGP1", "AD8351_RGP2", (143, 74))
resistor("08_if_chain", "R73", "25R_1%", "AD8351_OUT_P", "ADC_VIN_P", (149, 65))
resistor("08_if_chain", "R74", "25R_1%", "AD8351_OUT_N", "ADC_VIN_N", (149, 69))
resistor("08_if_chain", "R75", "10k_1%", "+3V3_ANA", "ADC_VCM", (145, 78))
resistor("08_if_chain", "R76", "10k_1%", "ADC_VCM", "GND", (149, 78))
cap("08_if_chain", "C256", "100nF", "ADC_VCM", (153, 78))
cap("08_if_chain", "C105", "100nF", "+5V_RF", (143, 82))
cap("08_if_chain", "C106", "10uF", "+5V_RF", (147, 82), size="0805")


def by_sheet(sheet: str) -> List[Component]:
    return [item for item in components if item.sheet == sheet]


def all_nets() -> List[str]:
    return sorted({net for item in components for net in item.pins.values() if net})


def fitted_pad_count() -> int:
    return sum(sum(net is not None for net in item.pins.values()) for item in components)


def assert_model() -> None:
    refs = [item.ref for item in components]
    by_ref = {item.ref: item for item in components}
    assert len(refs) == len(set(refs)), "Duplicate references in V2 model"
    assert all(item.sheet in {sheet for sheet, _ in SHEETS} for item in components)
    assert all(item.footprint for item in components)
    assert all(item.pins for item in components)
    assert {"GND", "+3V3_DIG", "+3V3_ANA", "+1V2_CORE", "+5V_RF"}.issubset(all_nets())
    fpga_signal_balls = {
        ball for ball, function in fpga_ball_functions.items()
        if function.startswith("PB") or function == "CRESET_B"
    }
    fpga_signal_nets = {
        net for ball, net in fpga_pins.items()
        if ball in fpga_signal_balls and net
    }
    assert len(fpga_signal_nets) <= len(fpga_signal_balls)
    assert fpga_pins["D5"] == "FPGA_AUX_2V5"
    assert fpga_pins["E4"] == fpga_pins["E6"] == "FPGA_VCC_1V2"
    assert fpga_pins["G6"] == "FPGA_PLL_1V2"
    assert all(fpga_pins[ball] is None for ball in ["D2", "D4", "D7", "E7"])
    assert by_ref["R30"].value == by_ref["R33"].value == by_ref["R148"].value == "4.7k"
    assert by_ref["R149"].pins["2"] == "FPGA_SPI_SCK"
    assert lmx_pins["5"] is None and lmx_pins["28"] is None and lmx_pins["30"] is None and lmx_pins["32"] is None
    assert all(adc_pins[p] is None for p in ["1", "3", "5", "6", "7", "8"])
    assert ksz_pins["1"] == ksz_pins["12"] == "PHY_AVDDH_3V3"
    assert ksz_pins["4"] == ksz_pins["9"] == "PHY_AVDDL_1V2"
    assert by_ref["U66"].pins["1"] != by_ref["U67"].pins["1"]
    assert by_ref["C262"].pins == {"1": "IF2_F1_P", "2": "IF2_F1_N"}
    assert all(ref not in by_ref for ref in ["C263", "C265", "C267", "C269"])


assert_model()
