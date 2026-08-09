from __future__ import annotations

import sys
from pathlib import Path
import unittest


TOOLS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS))

from sensitive_route_contract import BUS_GROUPS, PAIR_GROUPS, SELECTED_NETS  # noqa: E402
from auto_router import (  # noqa: E402
    CLEARANCE_MM,
    DIFF_PAIR_90OHM_GAP_MM,
    VIA_DIAMETER_MM,
    VIA_DRILL_MM,
    parse_selected_nets,
    parse_selected_order,
    parse_positive_mm,
    diff_pair_partner_suffix,
    orient_partner_endpoints,
)


class SensitiveRouteContractTests(unittest.TestCase):
    def test_contract_covers_approved_pair_budgets(self) -> None:
        self.assertEqual(PAIR_GROUPS["ADF second LO"].max_skew_mm, 0.25)
        self.assertEqual(PAIR_GROUPS["ADC analog input"].max_skew_mm, 0.50)
        self.assertEqual(PAIR_GROUPS["USB"].differential_impedance_ohm, 90)

    def test_contract_covers_approved_bus_budgets(self) -> None:
        self.assertEqual(BUS_GROUPS["RGMII TX"].max_spread_mm, 2.00)
        self.assertEqual(BUS_GROUPS["RGMII RX"].max_spread_mm, 2.00)
        self.assertEqual(BUS_GROUPS["ADC data"].max_spread_mm, 2.00)
        self.assertIn("ADC_CLK", SELECTED_NETS)

    def test_autorouter_selected_net_parser_is_explicit(self) -> None:
        self.assertEqual(parse_selected_nets(" USB_DP,USB_DM, USB_DP "), {"USB_DP", "USB_DM"})
        self.assertEqual(parse_selected_nets(""), set())
        self.assertEqual(parse_selected_order("ADC_D4, ADC_D3,ADC_D4"), ("ADC_D4", "ADC_D3"))

    def test_usb_connector_nets_are_detected_as_a_pair(self) -> None:
        self.assertEqual(diff_pair_partner_suffix("USB_DP_CONN"), ("USB", "_DP_CONN", "_DM_CONN"))
        self.assertEqual(diff_pair_partner_suffix("USB_DM_CONN"), ("USB", "_DM_CONN", "_DP_CONN"))

    def test_pair_partner_endpoints_are_oriented_to_matching_ends(self) -> None:
        self.assertEqual(
            orient_partner_endpoints((0.0, 0.0), (10.0, 0.0), (10.0, 1.0), (0.0, 1.0)),
            ((0.0, 1.0), (10.0, 1.0)),
        )

    def test_autorouter_geometry_meets_board_minimums(self) -> None:
        self.assertGreaterEqual(CLEARANCE_MM, 0.25)
        self.assertGreaterEqual(DIFF_PAIR_90OHM_GAP_MM, 0.20)
        self.assertGreaterEqual(VIA_DIAMETER_MM, 0.50)
        self.assertGreaterEqual(VIA_DRILL_MM, 0.25)

    def test_autorouter_grid_can_be_refined_for_dense_escape(self) -> None:
        self.assertEqual(parse_positive_mm("0.10", 0.25), 0.10)
        self.assertEqual(parse_positive_mm("", 0.25), 0.25)
        with self.assertRaises(ValueError):
            parse_positive_mm("0", 0.25)


if __name__ == "__main__":
    unittest.main()
