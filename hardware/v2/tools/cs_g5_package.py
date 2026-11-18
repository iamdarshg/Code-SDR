#!/usr/bin/env python3
"""Generate a Code-SDR v2 fab package tied to the board's sha256: Gerbers+job, drill, positions,
DRC (--severity-all --schematic-parity), MANIFEST_SHA256.json (every file, the board, the waiver files).
usage: cs_g5_package.py <repo_root> <kicad_cli>   -> hardware/v2/fab/board_<sha8>/"""
import collections, hashlib, json, subprocess, sys
from pathlib import Path
repo, cli = Path(sys.argv[1]), sys.argv[2]
board = repo / "hardware/v2/Code-SDR-V2.kicad_pcb"
sha = lambda p: hashlib.sha256(Path(p).read_bytes()).hexdigest()
bs = sha(board); out = repo / f"hardware/v2/fab/board_{bs[:8]}"; (out / "gerbers").mkdir(parents=True, exist_ok=True)
cmds = [[cli, "pcb", "export", "gerbers", "--output", str(out / "gerbers") + "/", str(board)],
        [cli, "pcb", "export", "drill", "--output", str(out / "gerbers") + "/", str(board)],
        [cli, "pcb", "export", "pos", "--format", "ascii", "--units", "mm", "--output", str(out / "positions.pos"), str(board)],
        [cli, "pcb", "drc", "--format", "json", "--severity-all", "--schematic-parity", "--output", str(out / "drc_receipt.json"), str(board)]]
for c in cmds:
    r = subprocess.run(c, capture_output=True, text=True)
    if r.returncode not in (0, 5):  # drc exits 5 when it reports violations/warnings
        sys.exit(f"failed: {' '.join(c)}\n{r.stderr}")
d = json.load(open(out / "drc_receipt.json")); v = d["violations"]
waivers = sorted((repo / "hardware/v2/drc_waivers").glob("*.json"))
man = {"board": "hardware/v2/Code-SDR-V2.kicad_pcb", "board_sha256": bs, "kicad_cli": "9.0.7",
       "git_commit_parent": subprocess.run(["git", "-C", str(repo), "rev-parse", "HEAD"], capture_output=True, text=True).stdout.strip(),
       "commands": ["kicad-cli pcb export gerbers --output gerbers/ <board>", "kicad-cli pcb export drill --output gerbers/ <board>",
                    "kicad-cli pcb export pos --format ascii --units mm --output positions.pos <board>",
                    "kicad-cli pcb drc --format json --severity-all --schematic-parity --output drc_receipt.json <board>"],
       "drc_summary": {"errors": sum(1 for x in v if x["severity"] == "error"), "warnings": sum(1 for x in v if x["severity"] == "warning"),
                       "warnings_by_type": dict(collections.Counter(x["type"] for x in v)), "unconnected": len(d["unconnected_items"]),
                       "schematic_parity": len(d["schematic_parity"])},
       "waivers": {str(p.relative_to(repo)): sha(p) for p in waivers},
       "files": {str(p.relative_to(out)): sha(p) for p in sorted(out.rglob("*")) if p.is_file() and p.name != "MANIFEST_SHA256.json"}}
(out / "MANIFEST_SHA256.json").write_text(json.dumps(man, indent=1) + "\n")
print(json.dumps({"out": str(out.relative_to(repo)), "board_sha256": bs, "files": len(man["files"]), **man["drc_summary"]}, indent=1))
