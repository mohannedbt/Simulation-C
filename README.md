Repository overview

This repository contains small physics and visualization demos (raylib-based) and related utilities. I reorganized sources into `src/` and preserved headers in `include/`. No source code was modified; files were only moved to a clearer layout.

Top-level layout

- `src/`: primary C source files and subfolders. Notable folders:
	- `src/Gravity/`: gravity engines — `Engine.c` (v1) and `Particles_V2.c` (v2).
	- `src/Osci/`, `src/Rubix/`: small demos moved into subfolders.
- `include/`: public headers (unchanged). Example: `raygui.h`.
- `raylib/`: bundled raylib tree (included in this repo). We recommend installing raylib separately (see instructions below) — this folder is now ignored by `.gitignore` to avoid committing built libraries.
- `docs/`: preserved copy of concatenated notes (`docs/content.txt`).
- `examples/`: intended place for example projects (currently empty — you can move example folders here).

Note: If this repo originally contained committed `raylib/` sources, `.gitignore` now includes `/raylib/`. Git will still track files already committed; to untrack an existing `raylib/` you can run:

```bash
git rm -r --cached raylib
git commit -m "Stop tracking raylib/ (ignored)"
```

Dependencies

- raylib (https://www.raylib.com/) — examples use raylib and raygui. You can obtain raylib in one of the following ways:

- Install via package manager (recommended on Linux):

```bash
# Debian/Ubuntu
sudo apt install libraylib-dev

# Arch
sudo pacman -S raylib

# macOS (Homebrew)
brew install raylib
```

- Clone and build locally:

```bash
git clone https://github.com/raysan5/raylib.git
cd raylib/src
mkdir build && cd build
cmake ..
make -j
sudo make install   # optional: installs system-wide
```

- Or add raylib as a git submodule inside this repo (keeps raylib in-tree but manageable):

```bash
git submodule add https://github.com/raysan5/raylib.git raylib
git submodule update --init --recursive
```

After installing or building raylib, link with `-lraylib` and any platform link flags as documented by raylib.
- Standard build tools: `gcc`/`clang`, `make` and/or `cmake` as desired.

Quick build & run (Linux)

1) Build using system raylib (recommended if raylib is installed via your package manager):

```bash
# create output dir
mkdir -p bin

# Build the Engine demo
gcc -Iinclude -O2 -o bin/engine src/Engine.c -lraylib -lm

# Build the particles demo
gcc -Iinclude -O2 -o bin/particles src/particles.c -lraylib -lm

# Build the Particles_V2 demo
gcc -Iinclude -O2 -o bin/particles_v2 src/Particles_V2.c -lraylib -lm

# Run one of them
./bin/engine
```

Notes:
- The exact link flags for raylib on Linux might require additional libraries depending on your raylib build (e.g. `-lpthread -ldl -lrt -lGL -lm -lX11`). If the simple command above fails with undefined references, install raylib development package or follow raylib docs for required link flags.

2) Build using bundled `raylib/` sources (if you want to build raylib locally):

```bash
# build raylib (from the repo's raylib/ folder)
cd raylib
mkdir -p build && cd build
cmake ..
make -j

# then from repo root link against the built raylib (adjust path to library)
gcc -Iinclude -O2 -o bin/engine src/Engine.c -Lraylib/build -lraylib -lm
```

Windows/macOS

- Building on Windows or macOS follows the same principles but requires platform-specific raylib binaries or building raylib for that platform. Use a Visual Studio or Xcode toolchain or `cmake` to generate appropriate project files.

Project notes & next steps

- I only moved files and added this README; you can:
	- Move example folders into `examples/` and add per-example build scripts (Makefile or CMakeLists).
	- Add a top-level `CMakeLists.txt` or `Makefile` to automate building all demos in `src/`.
	- Clean up unused binaries (the `file` and `particles` files at the root) if they are not needed.

If you want, I can: add a top-level `Makefile`/`CMakeLists.txt`, move specific examples into `examples/`, or create per-example build scripts. Tell me which action you prefer.
