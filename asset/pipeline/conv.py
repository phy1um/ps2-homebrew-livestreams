import struct
import sys
import logging

import obj

log = logging.Logger("obj2gif")
log.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
log.addHandler(ch)

def LE(s):
    return f"<{s}"

def BE(s):
    return f">{s}"

obj_file = open(sys.argv[1], "r")
o = obj.parse(obj_file)
obj_file.close()
count = 0
rows = []
for f in o.face_coords():
    [a, b, c] = f
    rows.append(struct.pack(LE("ffff"), 0, 0, 0, 0))
    rows.append(struct.pack(LE("ffff"), a.x, a.y, a.z, 1.0))
    rows.append(struct.pack(LE("ffff"), 0, 0, 0, 0))
    rows.append(struct.pack(LE("ffff"), b.x, b.y, b.z, 1.0))
    rows.append(struct.pack(LE("ffff"), 0, 0, 0, 0))
    rows.append(struct.pack(LE("ffff"), c.x, c.y, c.z, 1.0))
    count += 1
log.info(f"found {count} faces")
obj_file.close()

out_file = None
if len(sys.argv) > 2:
    out_file = open(sys.argv[2], "wb")
else:
    out_file = sys.stdout.buffer

for r in rows:
    out_file.write(r)

