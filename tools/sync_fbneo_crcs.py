#!/usr/bin/env python3
"""Update FBNeo's sfiii3 program-SIMM CRCs from a built ROM set.

FBNeo matches files inside a ROM set by CRC, so a modified program ROM needs
the driver table refreshed before the set will load.

    python3 tools/sync_fbneo_crcs.py <path-to-FBNeo-checkout>

Reads  build/sfiii3nr1.zip
Writes the SFIII3_990512_FLASH program-chip CRCs in
       <fbneo>/src/burn/drv/cps3/d_cps3.cpp
"""

import re
import sys
import zipfile
import zlib

BUILD_ZIP = "build/sfiii3nr1.zip"
DRIVER = "src/burn/drv/cps3/d_cps3.cpp"
CHIPS = [f"sfiii3-simm1.{i}" for i in range(4)] + [f"sfiii3-simm2.{i}" for i in range(4)]


def main(fbneo):
    with zipfile.ZipFile(BUILD_ZIP) as z:
        crcs = {name: z.read(name) for name in CHIPS}
        crcs = {name: zlib.crc32(data) & 0xFFFFFFFF for name, data in crcs.items()}

    with open(f"{fbneo}/{DRIVER}") as f:
        src = f.read()

    # Only rewrite the simm1/simm2 entries inside the SFIII3 flash macro
    # (region variants share it via #define, so one edit covers all).
    macro_start = src.index("SFIII3_990512_FLASH")
    macro_end = src.index("\n\n", macro_start)
    macro = src[macro_start:macro_end]

    for name, crc in crcs.items():
        pattern = re.compile(r'(\{ "%s",\s+0x[0-9a-f]+,\s+0x)[0-9a-f]{8}' % re.escape(name))
        macro, n = pattern.subn(r"\g<1>%08x" % crc, macro)
        if n != 1:
            sys.exit(f"error: expected exactly one entry for {name}, found {n}")

    src = src[:macro_start] + macro + src[macro_end:]
    with open(f"{fbneo}/{DRIVER}", "w") as f:
        f.write(src)
    print(f"updated {CHIPS[0]}..{CHIPS[-1]} CRCs in {fbneo}/{DRIVER}")
    for name, crc in crcs.items():
        print(f"  {name}: {crc:08x}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1].rstrip("/"))
