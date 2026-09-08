"""Identify disconnected copper islands with no component pad or zone connection.

Deleting these islands never completes a required component connection. It
removes abandoned copper left by package corrections before required rerouting.
"""
import argparse
import json
from pathlib import Path
from recovery_route import Copper

ap=argparse.ArgumentParser(description=__doc__)
ap.add_argument('geometry',type=Path)
ap.add_argument('output',type=Path)
a=ap.parse_args()
d=json.loads(a.geometry.read_text())
c=Copper(d)
islands=[]
for net in sorted({n['net'] for n in c.nodes.values()}):
    for group in c.groups(net):
        if not any(c.nodes[u]['kind'] in ('pad','zone') for u in group):
            islands.append({'net':net,'uuids':group})
result={'source_sha256':d['sha256'],'islands':islands,'removed_items':sum(len(i['uuids']) for i in islands)}
a.output.write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps({'islands':len(islands),'items':result['removed_items']}),flush=True)
