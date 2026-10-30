# src/video/sdl3_video.c

This file holds the manifest of `sdl3:video`: `.toolchain/docs/presets.md`, *native_module*.
Its entries are `init`, `shutdown` and `reload`.

## The start

`bridge_init` does these steps:

1. `shared_start_video` starts the video of SDL3: `docs/intern/shared/shared_subsystem.c.md`. If SDL3 refuses, `bridge_init` gives 1.
2. `declare_types` declares the four types of the module, in the order of their slots.
3. It attaches a `sdl3:Window` to the entity 0, with `surface` at `SDL3_SURFACE_NONE` and each other field at zero.
4. It adds the system `window` in the phase `input`.

Each type states `BRIDGE_STRIP` and `BRIDGE_ON_DESTROY_REMOVE`: `readme.md`, *The modules*.
Each enum states `BRIDGE_TABLE`, and the two singletons state `BRIDGE_SINGLETON`: `docs/extern/video/sdl3_video.h.md`, *The declarations*.

`window` has two optional terms: it reads the settings, and it writes the window.
It states no change, because it attaches and detaches nothing.
Step 3 does not leave `surface` at zero, because zero is no constant of `sdl3:sdl3_surface_t`: `docs/extern/video/sdl3_video.h.md`, *sdl3:sdl3_surface_t*.

## The stop

`bridge_shutdown` calls `video_close_window` of `src/video/video_window.c`, so the Metal view and the window go before the video of SDL3.
Then it stops the video with `shared_stop_subsystem`: `docs/intern/shared/shared_subsystem.c.md`.
`bridge_reload` gives 1: `readme.md`, *The modules*.

## Functions

| Function | Result |
| --- | --- |
| `bridge_init` | Starts the video, declares the 4 types, attaches the window, and adds `window`. Gives 0, or 1 when SDL3 refuses. |
| `declare_types` | Declares the enums `sdl3:sdl3_place_t` and `sdl3:sdl3_surface_t`, then the singletons `sdl3:WindowSettings` and `sdl3:Window`. |
| `bridge_shutdown` | Destroys the window, and stops the video. Gives 0. |
| `bridge_reload` | Gives 1. |
