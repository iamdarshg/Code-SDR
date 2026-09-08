"""Join nearby disconnected conductors using obstacle-checked straight segments."""
import argparse
import ast
import json
from pathlib import Path
from shapely.geometry import LineString
from shapely.ops import nearest_points
from recovery_route import Copper
from sensitive_route_contract import PAIR_GROUPS

ap=argparse.ArgumentParser(description=__doc__)
ap.add_argument('geometry',type=Path)
ap.add_argument('output',type=Path)
ap.add_argument('--distance',type=float,default=15)
ap.add_argument('--clearance',type=float,default=.115)
ap.add_argument('--signal-width',type=float,default=.15)
ap.add_argument('--exclude-sensitive',action='store_true')
a=ap.parse_args()
d=json.loads(a.geometry.read_text())
c=Copper(d)
tree=ast.parse(Path(__file__).with_name('export_rf50_dsn.py').read_text())
rf=set(ast.literal_eval(next(n.value for n in tree.body if isinstance(n,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='RF50_NETS' for t in n.targets))))
sensitive=set(rf)
for pair in PAIR_GROUPS.values():sensitive.update(pair.positive+pair.negative)
summary={}
for net in sorted({n['net'] for n in c.nodes.values()}):
    if not net or net.startswith('unconnected-('):continue
    if a.exclude_sensitive and net in sensitive:continue
    width=.23 if net in rf else .30 if net.startswith(('+','VIN')) or net=='GND' else a.signal_width
    count=0
    for attempt in range(120):
        groups=c.groups(net)
        if len(groups)<2:break
        shapes=[c.group_shapes(g) for g in groups]
        choices=[]
        for i in range(len(groups)):
            for j in range(i):
                for layer in c.outer:
                    if layer not in shapes[i] or layer not in shapes[j]:continue
                    if shapes[i][layer].is_empty or shapes[j][layer].is_empty:continue
                    distance=shapes[i][layer].distance(shapes[j][layer])
                    if distance>a.distance:continue
                    p,q=nearest_points(shapes[i][layer],shapes[j][layer])
                    choices.append((distance,layer,list(p.coords)[0],list(q.coords)[0]))
        route=None
        for distance,layer,start,end in sorted(choices):
            if distance<.001:continue
            shape=LineString([start,end]).buffer(width/2,quad_segs=8)
            if c.copper_clear(shape,layer,net,clearance=a.clearance):
                route={'net':net,'segments':[{'start':start,'end':end,'layer':layer,'width':width}],'vias':[]}
                break
        if route is None:break
        c.accept_planned(route)
        if len(c.groups(net))>=len(groups):raise RuntimeError('Exact groups did not merge')
        count+=1
    if count:print(net,count,flush=True)
    summary[net]=count
a.output.write_text(json.dumps({'source_sha256':d['sha256'],'routes':c.routes,'summary':summary},indent=2)+'\n')
print('Planned',len(c.routes),'direct connections',flush=True)
