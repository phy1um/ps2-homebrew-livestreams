import struct

HEADER_FMT = "<BBBHHBHHHHBB"
FOOTER= b"\x00"*8 + b"TRUEVISION-XFILE" + b"\x2e\x00"

class TGA:
    def __init__(self, header, ident, col, img):
        self.header = header
        self.ident = ident
        self.col = col
        self.img = img

class TgaHeader:
    def __init__(self, id_len, col_map_type,
            img_type, col_spec, img_spec):
        self.id_len = id_len
        self.col_map_type = col_map_type
        self.img_type = img_type
        self.col_spec = col_spec
        self.img_spec = img_spec

    def __str__(self):
        return f"< |id|={self.id_len} CM type={self.col_map_type} IMG type={self.img_type} >\nCM SPEC={self.col_spec}\nIMG SPEC={self.img_spec}"

class TgaImgSpec:
    def __init__(self, xo, yo, w, h, bits_per_pix, desc):
        self.xorigin = xo
        self.yorigin = yo
        self.width = w
        self.height = h
        self.bits_per_pixel = bits_per_pix
        self.desc = desc

    def __str__(self):
        return f"[ {self.xorigin} {self.yorigin} {self.width} {self.height} {self.bits_per_pixel} {self.desc} ]"

class TgaColSpec:
    def __init__(self, map_origin, map_len, entry_bits):
        self.map_origin = map_origin
        self.map_len = map_len
        self.entry_bits = entry_bits

    def __str__(self):
        return f"[ {self.map_origin} {self.map_len} {self.entry_bits} ]"

def parse(b):
    header_size = struct.calcsize(HEADER_FMT)
    header_bytes = b[:header_size]
    v = struct.unpack(HEADER_FMT, header_bytes)
    col_spec = TgaColSpec(v[3], v[4], v[5])
    img_spec = TgaImgSpec(v[6], v[7], v[8], v[9], v[10], v[11])
    header = TgaHeader(v[0], v[1], v[2], col_spec, img_spec)
    head = header_size
    ident = b[head:head+header.id_len] 
    head = head+header.id_len
    col_map_size = header.col_spec.map_len * (header.col_spec.entry_bits // 8)
    col_data = b[head:head+col_map_size]
    head = head+col_map_size
    img_size = header.img_spec.width * header.img_spec.height * (header.img_spec.bits_per_pixel // 8)
    img_data = b[head:head+img_size]
    return TGA(header, 
            ident.decode("ascii"), 
            handle_pixels(header.col_spec.entry_bits, col_data),
            handle_pixels(header.img_spec.bits_per_pixel, img_data))
     

def handle_pixels(bits_per_pix, img_data):
    bpp = bits_per_pix // 8
    if bpp == 1:
        return list(map(lambda x: x[0], struct.iter_unpack("B", img_data)))
    elif bpp == 2:
        return list(map(lambda x: x[0], struct.iter_unpack("<H", img_data)))
    elif bpp == 3:
        return threeify(img_data)
    elif bpp == 4:
        return list(map(lambda x: x[0], struct.iter_unpack("<I", img_data)))
    else:
        raise Exception(f"unsupported BPP in image data: {bpp}")

def encode_pixels(bits_per_pix, data):
    bpp = bits_per_pix // 8
    if bpp == 1:
        return struct.pack("B"*len(data), *data)
    elif bpp == 2:
        return struct.pack("<H"*len(data), *data)
    elif bpp == 3:
        return unthreeify(data)
    elif bpp == 4:
        return struct.pack("<I"*len(data), *data)
    else:
        raise Exception(f"unsupported BPP in image data: {bpp}")

def threeify(d):
    fsts = d[::3]
    snds = d[1::3]
    thrds = d[2::3]
    if len(fsts) != len(snds) or len(fsts) != len(thrds):
        raise Exception("unbalanced pixel data, not a multiple of 3")

    out = []
    for i in range(len(fsts)):
        b = fsts[i]
        g = snds[i]
        r = thrds[i]
        out.append(b | (g<<8) | (r<<16))
    return out

def unthreeify(pixels):
    out = bytearray()
    for p in pixels:
        b = p&0xff
        g = (p>>8)&0xff
        r = (p>>16)&0xff
        out.append(b)
        out.append(g)
        out.append(r)
    return out

def write(tga):
    header_bytes = struct.pack(HEADER_FMT, 
            tga.header.id_len,
            tga.header.col_map_type,
            tga.header.img_type,
            tga.header.col_spec.map_origin,
            tga.header.col_spec.map_len,
            tga.header.col_spec.entry_bits,
            tga.header.img_spec.xorigin,
            tga.header.img_spec.yorigin,
            tga.header.img_spec.width,
            tga.header.img_spec.height,
            tga.header.img_spec.bits_per_pixel,
            tga.header.img_spec.desc)

    ident_bytes = tga.ident.encode("ascii")
    col_bytes = encode_pixels(tga.header.col_spec.entry_bits, tga.col)
    img_bytes = encode_pixels(tga.header.img_spec.bits_per_pixel, tga.img)
    return header_bytes + ident_bytes + col_bytes + img_bytes + FOOTER

if __name__ == "__main__":
    """ test script - read tga in, output "out.tga" which should be identical
    """
    import sys
    with open(sys.argv[1], "rb") as f:
        bb = f.read()
        tga = parse(bb)
        print("READ TGA WITH HEADER:")
        print(str(tga.header))
        nh = write(tga)
        with open("out.tga", "wb") as r:
            r.write(nh)

