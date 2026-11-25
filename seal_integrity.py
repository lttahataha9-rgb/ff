#!/usr/bin/env python3
"""
Computes FNV-1a 64-bit hash of the __TEXT,__text section in a 64-bit Mach-O
binary (or fat binary containing arm64) and patches the IntegritySeal struct
in __DATA,__data before ldid signing.
"""
import struct
import sys

MARKER = 0x4349434144415F53  # "CICADA_S"
SALT   = 0xA55A1234DEADBEEF
MH_MAGIC_64 = 0xFEEDFACF
FAT_MAGIC   = 0xCAFEBABE
FAT_CIGAM   = 0xBEBAFECA
LC_SEGMENT_64 = 0x19


def fnv1a_64(data: bytes) -> int:
    h = 0xCBF29CE484222325
    for b in data:
        h ^= b
        h = (h * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
    return h if h != 0 else 0x9E3779B97F4A7C15


def find_text_section_in_macho(buf: bytes, base_off: int):
    magic, = struct.unpack_from("<I", buf, base_off)
    if magic != MH_MAGIC_64:
        raise RuntimeError(f"Unexpected Mach-O magic: 0x{magic:08x}")
    # mach_header_64: magic(4), cputype(4), cpusubtype(4), filetype(4), ncmds(4), sizeofcmds(4), flags(4), reserved(4) = 32 bytes
    _, _, _, _, ncmds, _, _, _ = struct.unpack_from("<IIIIIIII", buf, base_off)
    off = base_off + 32
    for _ in range(ncmds):
        cmd, cmdsize = struct.unpack_from("<II", buf, off)
        if cmd == LC_SEGMENT_64:
            # segment_command_64: cmd(4), cmdsize(4), segname(16), vmaddr(8), vmsize(8), fileoff(8), filesize(8), maxprot(4), initprot(4), nsects(4), flags(4) = 72 bytes
            segname = buf[off + 8 : off + 24].split(b"\x00", 1)[0]
            nsects, = struct.unpack_from("<I", buf, off + 64)
            if segname == b"__TEXT":
                sect_off = off + 72
                for _ in range(nsects):
                    # section_64: sectname(16), segname(16), addr(8), size(8), offset(4), align(4), reloff(4), nreloc(4), flags(4), reserved1(4), reserved2(4), reserved3(4) = 80 bytes
                    sectname = buf[sect_off : sect_off + 16].split(b"\x00", 1)[0]
                    if sectname == b"__text":
                        _, size, file_off = struct.unpack_from("<QQI", buf, sect_off + 32)
                        return base_off + file_off, size
                    sect_off += 80
        off += cmdsize
    raise RuntimeError("__TEXT,__text section not found")


def seal_binary(path: str):
    with open(path, "rb") as f:
        buf = bytearray(f.read())

    magic_be, = struct.unpack_from(">I", buf, 0)
    slices = []
    if magic_be == FAT_MAGIC:
        nfat, = struct.unpack_from(">I", buf, 4)
        for i in range(nfat):
            _, _, offset, _ = struct.unpack_from(">IIII", buf, 8 + i * 20)
            slices.append(offset)
    else:
        slices.append(0)

    marker_bytes = struct.pack("<Q", MARKER)
    patched = 0
    for base_off in slices:
        text_off, text_size = find_text_section_in_macho(buf, base_off)
        text_hash = fnv1a_64(bytes(buf[text_off : text_off + text_size]))
        sealed_val = text_hash ^ SALT

        # Locate IntegritySeal { marker, hashXorMask } in this slice
        idx = buf.find(marker_bytes, base_off)
        if idx == -1:
            raise RuntimeError("IntegritySeal marker not found in binary")
        # Clear marker so it's not trivially searchable in __DATA, and write sealed_val
        struct.pack_into("<QQ", buf, idx, 0x1F2E3D4C5B6A7988, sealed_val)
        patched += 1
        print(f"[seal_integrity] Sealed __TEXT,__text (size={text_size}, hash=0x{text_hash:016x})")

    if patched == 0:
        raise RuntimeError("No slices patched")

    with open(path, "wb") as f:
        f.write(buf)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <macho_path>", file=sys.stderr)
        sys.exit(1)
    seal_binary(sys.argv[1])
