from __future__ import annotations

import sys
from pathlib import Path
import unittest


TOOLS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS))

from sensitive_route_contract import BUS_GROUPS, PAIR_GROUPS, SELECTED_NETS  # noqa: E402


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


if __name__ == "__main__":
    unittest.main()
