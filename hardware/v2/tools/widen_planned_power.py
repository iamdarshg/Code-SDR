"""Widen new supply segments wherever actual copper clearance permits."""
import argparse
import json
import math
from pathlib import Path
from recovery_route import Copper

ap=argparse.ArgumentParser(description=__doc__)
for name in ('geometry','plan','output'):ap.add_argument(name,type=Path)
a=ap.parse_args()
d=json.loads(a.geometry.read_text());plan=json.loads(a.plan.read_text())
if plan['source_sha256']!=d['sha256']:raise ValueError('Stale plan')
c=Copper(d)
for r in plan['routes']:c.accept_planned(r)
changes=[]
for route in plan['routes']:
    net=route['net']
    if not (net.startswith(('+','VIN','FPGA_VCC','PHY_AVDD')) or net=='RP_CORE'):continue
    for seg in route['segments']:
        old=seg['width']
        for width in (1.2,1.,.8,.6,.5,.4,old):
            if width<old:continue
            if c.line_clear(seg['start'],seg['end'],seg['layer'],net,width):
                seg['width']=width
                if width!=old:changes.append({'net':net,'old':old,'new':width,'length_mm':math.dist(seg['start'],seg['end'])})
                break
plan['power_width_changes']=changes
a.output.write_text(json.dumps(plan,indent=2)+'\n')
print('Widened',len(changes),'new power segments',flush=True)
