import pcbnew
from pcbnew import wxPoint as P
from math import cos, sin, pi

# exec(open("/home/sven/filament-clock/place_segs.py").read())

# Parameters
SCALE = 1000000
l = 42 * SCALE # segment length
edge_margin = 5 * SCALE
angle_deg = 10
hole_margin = 8 * SCALE

def line(start, end, layer, width=0.05):
    l = pcbnew.DRAWSEGMENT()
    l.SetShape(pcbnew.S_SEGMENT)
    l.SetStart(start)
    l.SetEnd(end)
    l.SetLayer(layer)
    l.SetWidth(int(width * SCALE))
    return l

def arc(center, start, angle, layer, width=0.05):
    a = pcbnew.DRAWSEGMENT()
    a.SetShape(pcbnew.S_ARC)
    a.SetCenter(center)
    a.SetArcStart(start)
    a.SetAngle(angle * 10)
    a.SetLayer(layer)
    a.SetWidth(int(width * SCALE))
    return a

# Derived Parameters
angle_rad = angle_deg / 360 * 2*pi
angle_sin = sin(angle_rad)
hole_y_shift = hole_margin * sin(pi/2 - angle_rad) / (1 - cos(pi/2 - angle_rad)) - hole_margin

b = pcbnew.GetBoard()
d = [b.FindModuleByReference(f'D{i}') for i in [*range(1,8)]]

#                   D1 = d[0]
#                 +-----------+
#                /           /
#     D6 = d[5] /           / D2 = d[1]
#              /           /
#             +-----------+
#            / D7 = d[6] /
# D5 = d[4] /           / D3 = d[2]
#          /           /
#         +-----------+
#           D4 = d[3]

d[0].SetOrientationDegrees(0)
d[0].SetPosition(P(l*angle_sin, -l))

d[1].SetOrientationDegrees(90 - angle_deg)
d[1].SetPosition(P(l/2*(1+angle_sin), -l/2))

d[2].SetOrientationDegrees(90 - angle_deg)
d[2].SetPosition(P(l/2*(1-angle_sin), l/2))

d[3].SetOrientationDegrees(0)
d[3].SetPosition(P(-l*angle_sin, l))

d[4].SetOrientationDegrees(90 - angle_deg)
d[4].SetPosition(P(-l/2*(1+angle_sin), l/2))

d[5].SetOrientationDegrees(90 - angle_deg)
d[5].SetPosition(P(-l/2*(1-angle_sin), -l/2))

d[6].SetOrientationDegrees(0)
d[6].SetPosition(P(0, 0))

# Board Outline
edge = b.GetLayerID('Edge.Cuts')

# Remove existing outline
for drawing in b.GetDrawings():
    if drawing.GetLayer() == edge:
        b.Remove(drawing)

arc = [
    arc(d[0].GetPosition() - P(l/2,0), d[0].GetPosition() + P(-l/2, -edge_margin), -90 + angle_deg, edge),
    arc(d[0].GetPosition() + P(l/2,0), d[0].GetPosition() + P(l/2, -edge_margin), 90 + angle_deg, edge),
    arc(d[3].GetPosition() + P(l/2,0), d[3].GetPosition() + P(l/2, edge_margin), -90 + angle_deg, edge),
    arc(d[3].GetPosition() - P(l/2,0), d[3].GetPosition() + P(-l/2, edge_margin), 90 + angle_deg, edge),
]
for a in arc:
    b.Add(a)

b.Add(line(arc[0].GetArcStart(), arc[1].GetArcStart(), edge))
b.Add(line(arc[1].GetArcEnd(), arc[2].GetArcEnd(), edge))
b.Add(line(arc[2].GetArcStart(), arc[3].GetArcStart(), edge))
b.Add(line(arc[3].GetArcEnd(), arc[0].GetArcEnd(), edge))

# Mounting holes
h = [b.FindModuleByReference(f'H{i}') for i in [*range(1,5)]]
h[0].SetPosition(d[0].GetPosition() - P(l/2, 0) + P(hole_margin - hole_y_shift, hole_margin))
h[3].SetPosition(d[0].GetPosition() + P(l/2, 0) + P(-hole_margin - hole_y_shift, hole_margin))
h[1].SetPosition(d[3].GetPosition() - P(l/2, 0) + P(hole_margin + hole_y_shift, -hole_margin))
h[2].SetPosition(d[3].GetPosition() + P(l/2, 0) + P(-hole_margin + hole_y_shift, -hole_margin))

# ALS
als = b.FindModuleByReference('U5')
als.SetPosition(d[6].GetPosition() + P(-l/2 * sin(angle_rad), l/2 * cos(angle_rad)))

# Switches
sw = [b.FindModuleByReference(f'SW{i}') for i in [1, 2]]
sw[0].SetPosition(P(-17 * SCALE, 17.75 * SCALE))
sw[1].SetPosition(sw[0].GetPosition() + P(-8 * SCALE * angle_sin, 8 * SCALE))

# Connectors
j = [b.FindModuleByReference(f'J{i}') for i in [1, 2]]
j[0].SetPosition(P((h[1].GetPosition().x + h[2].GetPosition().x) / 2,
    (h[1].GetPosition().y + h[2].GetPosition().y) / 2))

pcbnew.Refresh()
