# CS-G1: the 2 open dangling items, dispositioned (2026-11-18)

Board `hardware/v2/Code-SDR-V2.kicad_pcb`: sha256 `fdef9e97…` before, **`9ee6b2ec6cdb1be433d424bb73b350e6ba737d6b957406dad944e73a52d0c65b` after**.
KiCad 9.0.7. Command: `kicad-cli pcb drc --schematic-parity --severity-all --format json`.
Machine-readable record: `dangling_open2_20261118.json`. Analysis tool: `hardware/v2/tools/cs_g1_dead_branches.py`.

| | before | after |
|---|---:|---:|
| DRC warnings | 216 | **215** |
| track_dangling / via_dangling | 10 / 7 | 9 / 7 |
| lib_footprint_mismatch (waived) | 199 | 199 |
| errors / unconnected | 0 / 0 | 0 / 0 |
| schematic parity (U5–U8 CT, waived) | 4 | 4 (identical) |
| IPC-D-356 netlist (`kicad-cli pcb export ipcd356`) | — | **byte-identical** to before |

The only DRC identity removed is the FPGA_AUX_2V5 item. No identity was added. The 15
load-bearing waivers in `dangling_items_20261112.json` still match the board.

## 1. `FPGA_AUX_2V5` track `65a5838e…` (0.8427 mm): FIXED (pure trim)

Why the earlier ablation was inconclusive: it matched DRC items by position. DRC reports the
start point (50.138339, 65.458339), which three segments share (`65a5838e`, `6123ed84`,
`eeda488c`), so the ablation deleted a different segment. Matched by uuid, this segment is the
connection to **C323.1**. It runs at 45° through the pad, without ending on it, and its far end
(49.542476, 66.054203) overhangs **0.418 mm** past the pad into nothing. That overhang is what
DRC flags.

Fix: one coordinate. The end moves along the same line to (49.838339, 65.758339), which is inside
C323.1 (pad bbox x 49.40–49.96, y 65.29–65.91). The change only removes copper: the segment
keeps its start, direction, width, net and uuid, and still enters the pad. FPGA_AUX_2V5 is a 2.5 V
aux supply, not an RF/LO/ADC net. An in-memory zone refill gives the same result before and after
the trim, so no zone interacts with it. The board diff is one line.

## 2. `GRF2013_RFOUT_VDD` track `3c92c037…` (4.6519 mm): STAYS, owner decision

This net is U71 (GRF2013, IF gain stage) **pin 4 RFOUT/VDD**. It carries the amplifier's output
signal plus its DC feed: C235 (100 pF C0G DC block) goes to `IF_GAIN_OUT`, and L84 (33 nH choke)
goes to `+5V_RF`. It is a signal-chain net, so this cleanup pass does not reroute it, and removing
copper here does have an electrical effect.

Measured with uuid-matched dead-branch pruning (segments split at T-junctions, vias and pads they
cross):
- Only **2.544 mm** of `3c92c037` is dead: (45.6, 27.0) → (47.897, 28.094). The other 2.11 mm lies
  on top of the live trunk segment `b5f21179` (duplicate copper).
- The waived "load-bearing" item `5286bb29` (0.4123 mm) on the same net is **not load-bearing**.
  It is the tip of a **13.902 mm dead branch** (`11752218` part, `20db2a00`, `b63f570e`,
  `1cb36042`, `7e0a4757`, `5286bb29`) that leaves the trunk at (45.97, 32.72) and ends at
  (35.4, 31.2). The 2026-11-12 ablation counted it load-bearing because deleting the tip exposes
  the next segment as dangling. Its own record shows `unconnected_after_removal: 0`.
- The net has 38.5 mm of copper: U71.4→C235.1 is a 14.86 mm path between pads 2.6 mm apart,
  plus **16.45 mm of open stubs** on the amplifier output node.

Effect if removed, using the microstrip on the board stackup (0.23 mm on 0.13 mm FR-4, ε_eff ≈ 3.17,
≈0.12 pF/mm): about 2 pF total. At the 26.25 MHz IF this is negligible (≈3 kΩ against 50 Ω).
The open-stub quarter-wave frequencies are ≈3.0 GHz for the 13.9 mm branch and ≈16.5 GHz for the
2.54 mm stub. The GRF2013 is a broadband gain block, so whether the stubs matter for out-of-band
stability is an RF judgement for the owner. **Recommendation:** delete both dead branches (16.45 mm)
in a deliberate layout change on this node, then re-run DRC and the IF chain checks. They are
not removed here.

## Other findings (not changed)

- Five other waived "load-bearing" tracks are load-bearing **only up to a pad they cross**. The rest
  is dead overhang: `+3V3_ANA` 71bfce04 **4.861 mm**, `GND` 4323a962 1.644 mm, `RGMII_TXD0`
  f8a18475 0.806 mm, `RGMII_TXD2` 7348c5d1 0.517 mm, and `ADC_D6` 671d4cf3 0.495 mm (an ADC net,
  so it is excluded from cleanup). Each could be trimmed the same way as item 1, which would
  remove its DRC warning.
- **Stale In2 fills from 2a2ab19.** An in-memory refill adds 2.75 mm² on In2 (+3V3_DIG 2.29,
  +3V3_ANA 0.46). The 6 added patches sit exactly at the 6 GND vias removed in 2a2ab19
  ((34.04, 68.65), (23.6, 64.07), (24.18, 62.67), (24.42, 61.75), (23.6, 60.93), (25.1, 31.93)).
  They are orphan antipads. They are harmless electrically, but they are in the CS-G5 Gerbers.
  Refill before regenerating the fab package.
- `fab/board_fdef9e97/` is now **stale** by its own rule (board hash changed). The only copper
  difference is this 0.418 mm trim on F.Cu. Regenerate it, ideally after the In2 refill.
