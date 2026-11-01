"""Export a placement-identical DSN containing only 50-ohm RF nets.

The resulting session cannot route power, control, ground, digital, or
differential-baseband nets because those nets are absent from its network
section.  All included nets are restricted to the outer microstrip layers at
the nominal 50-ohm width for the project's symmetric 0.13 mm dielectrics.
"""

from __future__ import annotations

from pathlib import Path

from design_model import all_nets
from generate_pcb import _format_dsn_class, _remove_dsn_net


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "build" / "Code-SDR-V2.dsn"
OUTPUT = ROOT / "build" / "Code-SDR-V2-rf50.dsn"

# Single-ended transmission-line sections intended for a nominal 50-ohm
# environment.  Digital enables/locks and native balanced IF/baseband nodes
# are deliberately excluded even when their names share an RF prefix.
RF50_NETS = {
    "RF_HA_IN", "RF_HA_LNA_IN", "RF_HA_LNA_OUT_RAW", "RF_HA_LNA_OUT",
    "RF_HB_IN", "RF_HB_LNA_IN", "RF_HB_LNA_OUT_RAW", "RF_HB_LNA_OUT",
    "RF_HC_IN", "RF_HC_LNA_IN", "RF_HC_LNA_OUT_RAW", "RF_HC_LNA_OUT",
    "RF_HD_IN", "RF_HD_LNA_IN", "RF_HD_LNA_OUT_RAW", "RF_HD_LNA_OUT",
    "RF_HD_LNA1_OUT", "RF_HD_LNA2_IN", "RF_HD_LNA2_OUT_RAW",
    "RF_HIGH_SWITCHED", "LTC_RF_IN", "LTC_LO_IN",
    "LMX_LO_HIGH_P_RAW", "LMX_LO_HIGH_N_RAW", "LO_HIGH_P", "LO_HIGH_N_TERM",
    "RF_LOW_IN", "RF_LOW_FILT", "RF_LOW_LNA_IN", "RF_LOW_LNA_OUT_RAW",
    "RF_LOW_LNA_OUT", "LMX_LO_LOW_P_RAW", "LMX_LO_LOW_N_RAW",
    "LO_LOW_P", "LO_LOW_N", "ADF_LO2_P_RAW", "ADF_LO2_N_RAW",
    "IF_HIGH_SE", "IF_LOW_SE", "IF_SELECTED", "IF_BAW_OUT",
    "GRF2013_RFIN", "GRF2013_RFOUT_VDD", "IF_GAIN_OUT", "PE4312_RF1",
    "PE4312_RF2", "IF_DSA_OUT", "IF_DSA_GAINED", "LT_INPUT_SE",
}


def main() -> None:
    source = SOURCE.read_text(encoding="utf-8")
    missing = sorted(net for net in RF50_NETS if f"    (net {net}\n" not in source)
    if missing:
        raise RuntimeError("50-ohm nets missing from source DSN: " + ", ".join(missing))

    for net_name in sorted(set(all_nets()) - RF50_NETS - {"GND"}):
        if f"    (net {net_name}\n" in source:
            source = _remove_dsn_net(source, net_name)

    # KiCad names the catch-all class either from the project netclass or
    # ``kicad_default`` when this exporter is fed a locked routing checkpoint.
    class_start = source.find("    (class ")
    network_close = source.find("  )\n  (wiring", class_start)
    if class_start < 0 or network_close < 0:
        raise RuntimeError("Cannot locate DSN class block")
    rf_class = _format_dsn_class(
        "RF_50_ONLY", sorted(RF50_NETS), 230, 180, ("F.Cu", "B.Cu")
    )
    source = source[:class_start] + rf_class + source[network_close:]
    OUTPUT.write_text(source, encoding="utf-8")
    print(f"Exported {OUTPUT.name}: {len(RF50_NETS)} nets, outer layers only, 0.23 mm width")


if __name__ == "__main__":
    main()
