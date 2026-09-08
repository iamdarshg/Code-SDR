"""Remove only explicitly reported track/via-to-pad shorts in a candidate.

Creates opens requiring rerouting; a zero-short result is not a release gate.
"""
import argparse
import hashlib
import json
from pathlib import Path
import pcbnew as p

ap = argparse.ArgumentParser(description=__doc__)
for name in ('board', 'geometry', 'drc', 'output'):
    ap.add_argument(name, type=Path)
a = ap.parse_args()
sha = hashlib.sha256(a.board.read_bytes()).hexdigest()
g = json.loads(a.geometry.read_text())
if g['sha256'] != sha or a.board.resolve() == a.output.resolve():
    raise ValueError('Require matching snapshot and separate output')
items = {i['uuid']: i for i in g['items']}
remove = {}
for v in json.loads(a.drc.read_text())['violations']:
    if v['type'] != 'shorting_items':
        continue
    objects = [items[i['uuid']] for i in v['items']]
    pads = [i for i in objects if i['kind'] == 'pad']
    copper = [i for i in objects if i['kind'] in ('track', 'via')]
    if len(pads) != 1 or len(copper) != 1 or pads[0]['net'] == copper[0]['net']:
        raise ValueError('Short requires separate manual review: '+v['description'])
    remove[copper[0]['uuid']] = dict(item=copper[0], reason=v['description'], pad=pads[0]['ref']+'.'+pads[0]['number'])
b = p.LoadBoard(str(a.board))
for t in list(b.GetTracks()):
    if t.m_Uuid.AsString() in remove:
        b.Delete(t)
b.BuildConnectivity()
p.ZONE_FILLER(b).Fill(b.Zones())
if hashlib.sha256(a.board.read_bytes()).hexdigest() != sha:
    raise RuntimeError('Source changed')
p.SaveBoard(str(a.output), b)
receipt = dict(source_sha256=sha, output_sha256=hashlib.sha256(a.output.read_bytes()).hexdigest(), removed=list(remove.values()))
a.output.with_suffix('.short-removal.json').write_text(json.dumps(receipt, indent=2)+'\n')
print('Removed', len(remove), 'confirmed wrong-net track/via items; rerouting required.', flush=True)
