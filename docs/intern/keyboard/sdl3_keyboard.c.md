# src/keyboard/sdl3_keyboard.c

This file holds the manifest of `sdl3:keyboard`: `.toolchain/docs/presets.md`, *native_module*.
Its entries are `init`, `shutdown` and `reload`.

## The start

`bridge_init` does these steps:

1. For the keyboard events, `shared_start_video` starts the video of SDL3: `docs/intern/shared/shared_subsystem.c.md`. If SDL3 refuses, `bridge_init` gives 1.
2. It declares `sdl3:KeyboardSettings`, then `sdl3:Keyboard`. Both are singletons.
3. It attaches a `sdl3:Keyboard` of zeros to the entity 0.
4. It adds the system `keys` in the phase `input`.

Each type states `BRIDGE_STRIP` and `BRIDGE_ON_DESTROY_REMOVE`: `readme.md`, *The modules*, and `docs/extern/keyboard/sdl3_keyboard.h.md`, *The declarations*.

`keys` has two optional terms: it reads `sdl3:KeyboardSettings`, and it writes `sdl3:Keyboard`.
The first run of `keys` has the settings: `readme.md`, *The modules*.

## The stop

`bridge_shutdown` stops the video with `shared_stop_subsystem`: `docs/intern/shared/shared_subsystem.c.md`.
`bridge_reload` gives 1: `readme.md`, *The modules*.

## Functions

| Function | Result |
| --- | --- |
| `bridge_init` | Starts the video, declares the 2 types, attaches the keyboard, and adds `keys`. Gives 0, or 1 when SDL3 refuses. |
| `bridge_shutdown` | Stops the video. Gives 0. |
| `bridge_reload` | Gives 1. |
