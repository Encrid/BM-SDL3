# src/mouse/sdl3_mouse.c

This file holds the manifest of `sdl3:mouse`: `.toolchain/docs/presets.md`, *native_module*.
Its entries are `init`, `shutdown` and `reload`.

## The start

`bridge_init` does these steps:

1. For the mouse events, `common_start_video` starts the video of SDL3: `docs/intern/common/common_subsystem.c.md`. If SDL3 refuses, `bridge_init` gives 1.
2. It declares `sdl3:MouseSettings`, `sdl3:Cursor` and `sdl3:Mouse`. Each one is a singleton.
3. It attaches a `sdl3:Mouse` of zeros to the entity 0.
4. It adds the system `mouse` in the phase `input`.

Each type states `BRIDGE_STRIP` and `BRIDGE_ON_DESTROY_REMOVE`: `readme.md`, *The modules*, and `docs/extern/mouse/sdl3_mouse.h.md`, *The declarations*.

`mouse` has three optional terms: it reads the settings and the cursor, and it writes the mouse.
The app attaches `sdl3:Cursor` itself. Until it does, `mouse` changes no cursor.

## The stop

`bridge_shutdown` destroys the cursor that the module made, then stops the video with `common_stop_subsystem`: `docs/intern/common/common_subsystem.c.md`.
`bridge_reload` gives 1: `readme.md`, *The modules*.

## Functions

| Function | Result |
| --- | --- |
| `bridge_init` | Starts the video, declares the 3 types, attaches the mouse, and adds `mouse`. Gives 0, or 1 when SDL3 refuses. |
| `bridge_shutdown` | Frees the cursor, and stops the video. Gives 0. |
| `bridge_reload` | Gives 1. |
