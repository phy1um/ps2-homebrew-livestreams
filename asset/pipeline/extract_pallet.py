from tga import parse, write
import sys
import argparse

parser = argparse.ArgumentParser(description="extract palette from TGA file")
parser.add_argument("file", type=str, help="input colour mapped TGA file")
parser.add_argument("output", type=str, help="name of output file for pallet")
parser.add_argument("-f", "--format", type=str, default="RAW", help="format for output, either RAW or TGA")

if __name__ == "__main__":
    args = parser.parse_args()
    with open(args.file, "rb") as f:
        bb = f.read()
        tga = parse(bb)
        if args.format == "RAW":
            with open(args.output, "wb") as out:
                for b in tga.col:
                    out.write(b.to_bytes(1, "little"))
        elif args.format == "TGA":
            tga.img = tga.col
            tga.col = []
            tga.header.img_spec.width = tga.header.col_spec.map_len
            tga.header.img_spec.height = 1
            tga.header.img_spec.bits_per_pixel = tga.header.col_spec.entry_bits
            tga.header.img_spec.xorigin = 0
            tga.header.img_spec.yorigin = 0
            tga.header.img_type = 2
            tga.header.col_map_type = 0
            tga.header.col_spec.map_len = 0
            tga.header.col_spec.entry_bits = 1
            updated = write(tga)
            with open(args.output, "wb") as out:
                out.write(updated)
        else:
            print(f"cannot output unknown format {args.format}")
            sys.exit(1)

