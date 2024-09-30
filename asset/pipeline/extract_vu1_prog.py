import struct

ELF_SECTION_HEADER_COUNT_OFFSET = 0x30
ELF_SECTION_HEADER_OFFSET = 0x20
ELF_SECTION_STRING_TABLE_INDEX_OFFSET = 0x32
SECTION_HEADER_SIZE = 10*4

def get_cstring(b, offset):
    build = []
    while b[offset] != 0:
        build.append(b[offset])
        offset += 1
    return bytes(build).decode()

if __name__ == '__main__':
    import sys
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} <dvp.vuobj> <output.vuprog>")
        sys.exit(1)
    with open(sys.argv[1], "rb") as f:
        elf_bin = f.read() 
    section_header_offset = struct.unpack_from("<I", elf_bin, ELF_SECTION_HEADER_OFFSET)[0]
    section_header_count = struct.unpack_from("<H", elf_bin, ELF_SECTION_HEADER_COUNT_OFFSET)[0]
    shstrtab_index = struct.unpack_from("<H", elf_bin, ELF_SECTION_STRING_TABLE_INDEX_OFFSET)[0]
    shstrtab_offset = struct.unpack_from("<I", elf_bin, section_header_offset + (SECTION_HEADER_SIZE*shstrtab_index) + (4*4))[0]
    for i in range(section_header_count):
        # conveniently the offset we want is the first field of the section header
        name_offset = struct.unpack_from("<I", elf_bin, section_header_offset + (SECTION_HEADER_SIZE*i))[0]
        name = get_cstring(elf_bin, shstrtab_offset+name_offset)
        if name == ".vutext":
            # get fields 4, 5 of the section header
            bytes_offset = struct.unpack_from("<I", elf_bin, section_header_offset + (SECTION_HEADER_SIZE*i) + (4*4))[0]
            bytes_len = struct.unpack_from("<I", elf_bin, section_header_offset + (SECTION_HEADER_SIZE*i) + (5*4))[0]
            prog_bytes = elf_bin[bytes_offset:bytes_offset+bytes_len] 
            with open(sys.argv[2], "wb") as f:
                f.write(prog_bytes)
            sys.exit(0)
    print("failed to find .vutext section")
    sys.exit(1)

