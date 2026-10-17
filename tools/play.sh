#!/bin/bash
# Rebuild the modded ROM, sync its CRCs into the local FBNeo, and launch the game.
set -e
REPO="$(cd "$(dirname "$0")/.." && pwd)"
FBNEO="${FBNEO_DIR:-$HOME/Documents/GitHub/FBNeo}"

make -C "$REPO" PYTHON=python3 MOD=1 -j8
python3 "$REPO/tools/sync_fbneo_crcs.py" "$FBNEO"
make -C "$FBNEO" -f makefile sdl2 -j8 2>&1 | tail -1

cd "$FBNEO"
exec ./fbneo sfiii3nr1
