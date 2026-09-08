"""Checkpoint local control/power routing, with explicit filled/capped pin escapes."""
import argparse
import ast
import json
import time
from pathlib import Path
from shapely import union_all
from recovery_route import Copper
from plan_pin_escapes import add_escapes
from sensitive_route_contract import PAIR_GROUPS

ap=argparse.ArgumentParser(description=__doc__)
ap.add_argument('geometry',type=Path)
ap.add_argument('output',type=Path)
ap.add_argument('--seconds',type=float,default=600)
ap.add_argument('--start-after',default='')
ap.add_argument('--coarse',action='store_true')
ap.add_argument('--nets-file',type=Path)
ap.add_argument('--skip-escapes',action='store_true')
ap.add_argument('--fine-escape',action='store_true',help='Use 0.10 mm signal neckdowns and a permissive search mask; whole-board DRC must validate the result')
ap.add_argument('--search-clearance',type=float,default=.06)
ap.add_argument('--include-sensitive',action='store_true',help='Allow explicitly requested timing-sensitive nets; requires post-route length/skew review')
a=ap.parse_args()
d=json.loads(a.geometry.read_text())
c=Copper(d)
tree=ast.parse(Path(__file__).with_name('export_rf50_dsn.py').read_text())
rf=set(ast.literal_eval(next(n.value for n in tree.body if isinstance(n,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='RF50_NETS' for t in n.targets))))
for pair in PAIR_GROUPS.values():rf.update(pair.positive+pair.negative)
nets_requested=json.loads(a.nets_file.read_text()) if a.nets_file else None
if a.include_sensitive and nets_requested:
    rf.difference_update(nets_requested)
nets=sorted({n['net'] for n in c.nodes.values()}-rf)
nets=[n for n in nets if n and not n.startswith('unconnected-(')]
if nets_requested is not None:
    requested=nets_requested
    available=set(nets)
    nets=[n for n in requested if n in available]
nets=[n for n in nets if len(c.groups(n))>1]
nets=[n for n in nets if n>a.start_after]
initial={n:len(c.groups(n)) for n in nets}
escapes=[] if a.skip_escapes else add_escapes(c,nets)
start=time.monotonic()
summary={}
processed=[]
def checkpoint():
    improved={n for n in nets if len(c.groups(n))<initial[n]}
    routes=[r for r in c.routes if r['net'] in improved]
    a.output.write_text(json.dumps({'source_sha256':d['sha256'],'routes':routes,'filled_capped_escapes':[e for e in escapes if e['net'] in improved],'summary':summary,'processed_nets':processed},indent=2)+'\n')
for net in nets:
    width=.3 if net.startswith(('+','VIN','FPGA_VCC','PHY_AVDD')) or net in ('GND','RP_CORE','CHASSIS_GND') else (.1 if a.fine_escape else .15)
    for attempt in range(30):
        if time.monotonic()-start>a.seconds:break
        groups=c.groups(net)
        if len(groups)<2:break
        shapes=[union_all(list(c.group_shapes(g).values())) for g in groups]
        pairs=sorted((shapes[i].distance(shapes[j]),i,j) for i in range(len(groups)) for j in range(i))
        found=None
        for _,i,j in pairs[:3]:
            if shapes[i].area>shapes[j].area:i,j=j,i
            choices=((3.,.12),(6.,.15)) if a.coarse else ((4.,.08),(8.,.10))
            for margin,pitch in choices:
                found=c.maze_route(net,groups[i],groups[j],width,margin,pitch,
                                   max_pops=15000 if a.coarse else 400000,
                                   clearance=a.search_clearance if a.fine_escape else None)
                if found:break
            if found:break
        if not found:break
        c.accept_planned(found)
        if len(c.groups(net))>=len(groups):raise RuntimeError('Exact groups did not merge')
        summary[net]={'before':initial[net],'after':len(c.groups(net))}
        checkpoint()
        print(net,summary[net],flush=True)
    processed.append(net)
    checkpoint()
    print(net,'finished',summary.get(net,{'before':initial[net],'after':len(c.groups(net))}),flush=True)
    if time.monotonic()-start>a.seconds:break
checkpoint()
print('Saved',a.output,'after',round(time.monotonic()-start),'seconds',flush=True)
