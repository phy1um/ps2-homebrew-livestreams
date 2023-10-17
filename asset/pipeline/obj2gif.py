
import struct
import sys
import logging
import argparse

import obj

from random import random
import math

log = logging.Logger("obj2gif")
log.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
log.addHandler(ch)

def get_colour(s):
    if s == None:
        return get_colour("0x70707080")
    n = int(s, 16)
    return ((n>>24)&0xff, (n>>16)&0xff, (n>>8)&0xff, n&0xff)

parse = argparse.ArgumentParser(prog="obj2gif", description="converts a .obj to a PS2 GIF-ready binary file")
parse.add_argument("file")
parse.add_argument("-o", "--output")
parse.add_argument("-c", "--colour")

args = parse.parse_args()

with open (args.file, "r") as f:
    o = obj.parse(f)
count = 0
rows = []
(r, g, blu, alpha) = get_colour(args.colour)
for f in o.face_coords():
    [a, b, c] = f
    rows.append(struct.pack("<iiii", r, g, blu, alpha))
    rows.append(struct.pack("<ffff", a.x, a.y, a.z, 1.0))
    rows.append(struct.pack("<iiii", r, g, blu, alpha))
    rows.append(struct.pack("<ffff", b.x, b.y, b.z, 1.0))
    rows.append(struct.pack("<iiii", r, g, blu, alpha))
    rows.append(struct.pack("<ffff", c.x, c.y, c.z, 1.0))
    count += 1
log.info(f"found {count} faces")

out_file = None
if args.output:
    out_file = open(args.output, "wb")
else:
    out_file = sys.stdout.buffer

for r in rows:
    out_file.write(r)

