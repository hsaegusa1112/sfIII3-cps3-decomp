# Personal-use build guide (3rd Strike arcade, modified)

Personal fork of gibletto/sfIII3-cps3-decomp: generous input buffer + human-like
sparring bot. The build is byte-exact against the arcade program ROM; mods live
on `personal/mods` and are opt-in.

## What you need

- Your own `sfiii3nr1.zip` ROM set → place at `rom/sfiii3nr1.zip` (never committed).
- Python 3.
- **Windows:** the Win32 SHC tools in `bin/` run natively — `build.bat` or `make`.
- **macOS (Apple Silicon):**
  1. `brew install meson` (FBNeo only) — nothing else; the SHC tools run via wibo.
  2. Get wibo (runs the Win32 exes natively under Rosetta):
     ```
     mkdir -p .tools && curl -sL -o .tools/wibo \
       https://github.com/decompals/wibo/releases/download/1.2.0/wibo-macos \
       && chmod +x .tools/wibo
     ```
  3. Docker Desktop (or any linux/amd64 container runtime) — needed only for
     rof2bin, which scribbles scratch files at the drive root (macOS forbids that;
     the container's `/` is writable):
     ```
     docker build --platform linux/amd64 -f Dockerfile.macos -t sf3-build .
     ```

## Build

```
make PYTHON=python3            # macOS (wibo + docker rof2bin)
make                           # Windows
```

Output:
- `build/prog.bin` — the program image; the build prints whether its SHA-1
  matches the arcade original (`tools/prog.sha1`). A pristine tree **must** say
  "matches". Modified builds won't (that's the point) — the hash check is
  informational and doesn't fail the build.
- `build/sfiii3nr1.zip` — your ROM set with the two program SIMMs replaced.

## Run (FBNeo)

Stock FBNeo matches ROM files by CRC, so a modified set needs a local FBNeo
build whose driver table knows the new hashes:

```
git clone https://github.com/finalburnneo/FBNeo.git
cd FBNeo
find src/burn/drv -name 'd_*.cpp' | sort > /tmp/drv_manifest.txt
perl src/dep/scripts/gamelist.pl -o src/dep/generated/driverlist.h \
     -l /tmp/gamelist.txt -f /tmp/drv_manifest.txt
make -f makefile sdl2 -j8        # needs brew: sdl2 sdl2_image sdl2_ttf sdl2_gfx

# point the driver at your build's hashes, rebuild, play:
<sf3 repo>/tools/sync_fbneo_crcs.py <path-to-FBNeo>
make -f makefile sdl2 -j8
mkdir -p romdir && cp <sf3 repo>/build/sfiii3nr1.zip romdir/
./fbneo -d romdir/ sfiii3nr1     # or no -d: set roms dir in the UI
```

MAME note (upstream README): the CPS3 program SIMMs are flash — MAME keeps them
in nvram and loads that over the ROM files, so use an empty
`-nvram_directory` when first booting a new build.

## Where the mods live

- Input buffer: `include/personal_config.h` + the parse-time scaling in
  `cmd_data_set` (`src/cmd_main_2.c`). Universal slots (< 16: jump, dash,
  blocking, red-parry, ukemi) keep arcade values by design.
- Sparring bot: `src/bot_*.c` — active when P2 is CPU in Versus mode.

## Training mode

Coin in, pick your character, then **hold 3P (all three punches) while
confirming** — you get a mirror match against a training dummy: timer
frozen, no KO (health refills when either side nears death), SA gauge
always full, stun disabled.

In fight (P1 controls):
- **hold 3P ~3/4 sec** — cycle dummy: STAND, CROUCH, BLOCK HI, BLOCK LO,
  RAND GRD, JUMP, CPU (current mode shown top-center)
- **hold 3K ~3/4 sec** — reset positions, health and gauge

Code: `src/training.c` (+ `include/training.h`), wired at PLMAIN (dummy
input), Game_Main (upkeep + timer), PLPCA (KO clamp), manage_2 (match-end
gate), sel_pl / next_cpu (entry + mirror).
