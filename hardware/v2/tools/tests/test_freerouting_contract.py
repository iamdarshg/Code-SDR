"""Unit tests for the preservation-first Specctra DSN contract."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path
import importlib.util
import shutil
import tempfile


TOOLS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS))

from freerouting_contract import snapshot_board, transform_dsn  # noqa: E402
from run_one_pass_freerouting import run_one_pass  # noqa: E402
from freerouting_monitor import summarize_unconnected  # noqa: E402
from route_ethernet_and_skew import ethernet_route_contract  # noqa: E402


FOUR_LAYER_DSN = """(pcb sample
  (structure
    (layer F.Cu
      (type signal)
    )
    (layer In1.Cu
      (type signal)
    )
    (layer In2.Cu
      (type signal)
    )
    (layer B.Cu
      (type signal)
    )
  )
  (network
    (class kicad_default LOCKED MDI_A_P MDI_A_N
      (circuit
        (use_layer F.Cu In1.Cu In2.Cu B.Cu)
      )
    )
  )
  (wiring
    (wire (net LOCKED) (path F.Cu 150 0 0 10 0))
    (wire (net MDI_A_P) (path F.Cu 150 0 1 10 1))
  )
)"""


class DsnContractTests(unittest.TestCase):
    def test_transform_protects_noneditable_existing_wiring(self) -> None:
        transformed = transform_dsn(FOUR_LAYER_DSN, {"MDI_A_P", "MDI_A_N"})
        self.assertIn("(wire (net LOCKED) (type protect) (path", transformed)
        self.assertIn("(wire (net MDI_A_P) (path", transformed)
        self.assertNotIn("(wire (net MDI_A_P) (type protect)", transformed)

    def test_transform_reserves_both_internal_layers_and_limits_classes(self) -> None:
        transformed = transform_dsn(FOUR_LAYER_DSN, set())
        self.assertIn("(layer In1.Cu\n      (type power)", transformed)
        self.assertIn("(layer In2.Cu\n      (type power)", transformed)
        self.assertIn("(use_layer F.Cu B.Cu)", transformed)

    def test_transform_places_editable_differential_nets_before_default(self) -> None:
        transformed = transform_dsn(
            FOUR_LAYER_DSN,
            {"MDI_A_P", "MDI_A_N", "USB_DP", "USB_DM"},
        )
        self.assertIn("(class DIFF_DIGITAL USB_DM USB_DP MDI_A_N MDI_A_P", transformed)
        self.assertLess(
            transformed.index("(class DIFF_DIGITAL"),
            transformed.index("(class kicad_default"),
        )
        default_header = transformed.split("\n", 1)[0] if False else transformed[
            transformed.index("(class kicad_default"): transformed.index("(circuit", transformed.index("(class kicad_default"))
        ]
        self.assertNotIn("MDI_A_P", default_header)
        self.assertNotIn("MDI_A_N", default_header)

    def test_route_only_transform_excludes_noneditable_open_nets(self) -> None:
        transformed = transform_dsn(FOUR_LAYER_DSN, {"MDI_A_P", "MDI_A_N"}, route_only=True)
        default_header = transformed[
            transformed.index("(class kicad_default"): transformed.index("(circuit", transformed.index("(class kicad_default"))
        ]
        self.assertNotIn("LOCKED", default_header)
        self.assertIn("(rule (width 200)(clearance 120))", transformed)

    @unittest.skipUnless(importlib.util.find_spec("pcbnew"), "requires KiCad Python")
    def test_snapshot_supports_kicad_9_vias_and_zones(self) -> None:
        board = TOOLS.parent / "Code-SDR-V2.kicad_pcb"
        snapshot = snapshot_board(board)
        self.assertGreater(len(snapshot.footprints), 0)
        self.assertGreater(len(snapshot.zones), 0)
        self.assertGreater(sum(snapshot.protected_items.values()), 0)

    def test_status_groups_remaining_connections_by_net(self) -> None:
        report = {
            "unconnected_items": [
                {"items": [{"description": "Pad 1 [MDI_A_P] of J1"}, {"description": "Pad 2 [MDI_A_P] of U1"}]},
                {"items": [{"description": "Pad 1 [MDI_A_P] of J1"}, {"description": "Pad 3 [MDI_A_P] of U1"}]},
                {"items": [{"description": "Pad 1 [+3V3_DIG] of C1"}]},
            ]
        }
        summary = summarize_unconnected(report)
        self.assertEqual(summary.total, 3)
        self.assertEqual(summary.by_net, {"+3V3_DIG": 1, "MDI_A_P": 2})

    def test_ethernet_route_contract_uses_200_um_and_balanced_crossover_vias(self) -> None:
        contract = ethernet_route_contract()
        self.assertEqual(contract["width_mm"], 0.20)
        self.assertTrue(contract["allow_vias"])
        self.assertEqual(contract["max_signal_vias_per_net"], 2)
        self.assertEqual(contract["layers"], ("F.Cu", "B.Cu"))
        self.assertEqual(contract["via_diameter_mm"], 0.50)
        self.assertEqual(contract["pad_neck_width_mm"], 0.15)
        self.assertEqual(contract["pad_neck_length_mm"], 2.0)


@unittest.skipUnless(importlib.util.find_spec("pcbnew"), "requires KiCad Python")
class OnePassDriverTests(unittest.TestCase):
    def test_dry_run_does_not_modify_live_board(self) -> None:
        source_board = TOOLS.parent / "Code-SDR-V2.kicad_pcb"
        source_project = TOOLS.parent / "Code-SDR-V2.kicad_pro"
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            live = root / "live.kicad_pcb"
            project = root / "live.kicad_pro"
            shutil.copy2(source_board, live)
            shutil.copy2(source_project, project)
            original = live.read_bytes()

            result = run_one_pass(live, project, root / "runs", None, dry_run=True)

            self.assertEqual(live.read_bytes(), original)
            self.assertTrue(result.candidate_path.exists())
            self.assertTrue(result.dsn_path.exists())
            self.assertEqual(result.router_invocations, 0)

    def test_dry_run_accepts_an_unlimited_router_timeout(self) -> None:
        source_board = TOOLS.parent / "Code-SDR-V2.kicad_pcb"
        source_project = TOOLS.parent / "Code-SDR-V2.kicad_pro"
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            live = root / "live.kicad_pcb"
            project = root / "live.kicad_pro"
            shutil.copy2(source_board, live)
            shutil.copy2(source_project, project)
            result = run_one_pass(
                live, project, root / "runs", None, dry_run=True, router_timeout=None,
            )
            self.assertEqual(result.router_invocations, 0)


if __name__ == "__main__":
    unittest.main()
