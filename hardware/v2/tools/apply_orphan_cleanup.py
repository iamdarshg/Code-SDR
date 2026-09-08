"""Apply a hash-bound no-pad/no-zone copper-island removal plan."""
import argparse
import hashlib
import json
from pathlib import Path
import pcbnew as p

ap=argparse.ArgumentParser(description=__doc__)
for name in ('board','plan','output'):
    ap.add_argument(name,type=Path)
a=ap.parse_args()
plan=json.loads(a.plan.read_text())
sha=hashlib.sha256(a.board.read_bytes()).hexdigest()
if sha!=plan['source_sha256'] or a.board.resolve()==a.output.resolve():
    raise ValueError('Require matching source and a separate candidate')
b=p.LoadBoard(str(a.board))
remove={u for island in plan['islands'] for u in island['uuids']}
tracks={t.m_Uuid.AsString():t for t in b.GetTracks()}
if not remove.issubset(tracks):
    raise ValueError('Plan contains something other than existing track/via copper')
for uid in remove:
    b.Delete(tracks[uid])
b.BuildConnectivity()
p.ZONE_FILLER(b).Fill(b.Zones())
p.SaveBoard(str(a.output),b)
plan['output_sha256']=hashlib.sha256(a.output.read_bytes()).hexdigest()
a.output.with_suffix('.orphan-cleanup.json').write_text(json.dumps(plan,indent=2)+'\n')
print('Removed',len(remove),'items in',len(plan['islands']),'abandoned copper islands',flush=True)
