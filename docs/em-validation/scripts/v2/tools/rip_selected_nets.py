"""Remove tracks and vias only from explicitly named nets in an isolated board."""
from pathlib import Path
import argparse, json
import pcbnew as p

ap = argparse.ArgumentParser(description=__doc__)
ap.add_argument('board', type=Path)
ap.add_argument('output', type=Path)
ap.add_argument('--net', action='append', default=[])
ap.add_argument('--uuid', action='append', default=[])
a = ap.parse_args()
if not a.net and not a.uuid:
    ap.error('provide at least one --net or --uuid')
b = p.LoadBoard(str(a.board))
wanted = set(a.net)
wanted_uuid = set(a.uuid)
removed = []
for item in list(b.GetTracks()):
    if item.GetNetname() in wanted or item.m_Uuid.AsString() in wanted_uuid:
        removed.append({'net': item.GetNetname(), 'uuid': item.m_Uuid.AsString(),
                        'kind': 'via' if isinstance(item, p.PCB_VIA) else 'track'})
        b.Remove(item)
b.BuildConnectivity()
p.ZONE_FILLER(b).Fill(b.Zones())
p.SaveBoard(str(a.output), b)
print(json.dumps({'removed': len(removed), 'by_net': {n: sum(x['net']==n for x in removed) for n in sorted(wanted)},
                  'output': str(a.output)}))
