Gravity engines
===============

This folder contains two gravity engine demos preserved from the top-level sources:

- `Engine.c` — v1: a solar-system style N-body demo using a Verlet-style (Velocity-Verlet) integrator and fixed bodies (sun + planets). Focused on visualization with simple per-body trails.
- `Particles_V2.c` — v2: a more general, vectorized particle/universe implementation with dynamic vectors, explicit force accumulation per-particle, and helper utilities to manage arbitrary-dimension vectors. Designed as a structural upgrade over earlier single-file code.

Comparison (high level)
- Purpose: `Engine.c` is a demo tuned for a solar-system visualization; `Particles_V2.c` is a reusable framework for N-body experiments.
- Data structures: `Engine.c` uses simple structs with per-particle arrays for trails; `Particles_V2.c` introduces a `Vector` type and per-particle `Vector*` fields for position/velocity/force.
- Integration: `Engine.c` provides `IntegrateVV()` (Velocity-Verlet) and a simple Euler-like `Integrate()`; `Particles_V2.c` separates force calculation and leaves integration open to the caller (intent is extensible).
- UI/Rendering: `Engine.c` is immediately runnable with `raylib` drawing and trails; `Particles_V2.c` includes `raygui` and is structured for adding UI controls but focuses on data structure correctness.
- Extensibility: `Particles_V2.c` is easier to extend to higher dimensions and alternate force laws. `Engine.c` is simpler and more approachable for visual demos.

Usage
- Both files require `raylib` to compile and `Particles_V2.c` expects `raygui.h` in `include/`.
- Build quick (Linux, system raylib installed):

```bash
mkdir -p bin
gcc -Iinclude -O2 -o bin/engine src/Gravity/Engine.c -lraylib -lm
gcc -Iinclude -O2 -o bin/particles_v2 src/Gravity/Particles_V2.c -lraylib -lm
```

Notes
- If you want help adding a small `Makefile` or `CMakeLists.txt` to build both automatically, tell me and I'll add one.
