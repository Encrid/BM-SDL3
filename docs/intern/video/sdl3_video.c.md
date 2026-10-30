# src/sdl3_video.c

This file holds the manifest of the module, and its entries `init`, `shutdown` and `reload`.

## The manifest

The kind `native_module` gives the words of the manifest as the macro `MANIFEST`.
The file puts that macro into the section of the target, with the line of `.toolchain/docs/presets.md`, *native_module*.

## The start

`bridge_init` does these steps:

1. It starts the video subsystem of SDL3.
2. If SDL3 refuses, it writes `no video` with the reason of SDL3, and gives 1.
3. `declare_types` declares the four types of the module, in the order of their slots.
4. It attaches a `sdl3:Window` to the entity 0, with `surface` at `SDL3_SURFACE_NONE` and each other field at zero.
5. It adds the system `window` in the phase `input`.

Each type states `BRIDGE_STRIP`, so an unload of the module destroys no entity. Each type also states `BRIDGE_ON_DESTROY_REMOVE`.
Each enum states `BRIDGE_TABLE`, and the two singletons state `BRIDGE_SINGLETON`: `docs/extern/sdl3_video.h.md`, *The declarations*.

`window` has two optional terms: it reads the settings, and it writes the window.
It states no change, because it attaches and detaches nothing.
Step 4 does not leave `surface` at zero, because zero is no constant of `sdl3:sdl3_surface_t`: `docs/extern/sdl3_video.h.md`, *sdl3:sdl3_surface_t*.

## The stop

`bridge_shutdown` calls `video_close_window` of `src/video_window.c`, so the Metal view and the window go before the video of SDL3.
Then it stops the video subsystem that `bridge_init` started.
`SDL_Quit` does not count the modules that use SDL3. So `bridge_shutdown` calls it only after the last subsystem stops, when `SDL_WasInit(0)` gives 0.

`bridge_reload` gives 1. Bridge needs the entry, but never calls it, because the manifest states `reloadable=false`.

## Functions

| Function | Result |
| --- | --- |
| `bridge_init` | Starts the video of SDL3, then declares the types and the system. Gives 0, or 1 when SDL3 refuses. |
| `declare_types` | Declares the enums `sdl3:sdl3_place_t` and `sdl3:sdl3_surface_t`, then the singletons `sdl3:WindowSettings` and `sdl3:Window`. |
| `bridge_shutdown` | Destroys the window, stops the video subsystem of the module, and SDL3 after the last subsystem. Gives 0. |
| `bridge_reload` | Gives 1. |
