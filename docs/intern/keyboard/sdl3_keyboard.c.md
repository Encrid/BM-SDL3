# src/sdl3_keyboard.c

This file holds the manifest of the module, and its entries `init`, `shutdown` and `reload`.

## The manifest

The kind `native_module` gives the words of the manifest as the macro `MANIFEST`.
The file puts that macro into the section of the target, with the line that the preset of the module tells.

## The start

`bridge_init` does these steps:

1. It starts the video subsystem of SDL3, because the keyboard events come through it.
2. If SDL3 refuses, it writes `no video` with the reason of SDL3, and gives 1.
3. It declares `sdl3:KeyboardSettings`, then `sdl3:Keyboard`. Both are singletons.
4. It attaches a `sdl3:Keyboard` with each field at zero to the entity 0.
5. It adds the system `keys` in the phase `input`.

Each type states `BRIDGE_STRIP`, so an unload of the module destroys no entity.
Each type also states `BRIDGE_ON_DESTROY_REMOVE`: `docs/extern/sdl3_keyboard.h.md`, *The declarations*.

`keys` has two optional terms: it reads `sdl3:KeyboardSettings`, and it writes `sdl3:Keyboard`.
Bridge writes the settings after `init`, so the first run of `keys` has them.

## The stop

`bridge_shutdown` stops the video subsystem that `bridge_init` started.
`SDL_Quit` does not count the modules that use SDL3. So `bridge_shutdown` calls it only after the last subsystem stops, when `SDL_WasInit(0)` gives 0.

`bridge_reload` gives 1. Bridge needs the entry, but never calls it, because the manifest states `reloadable=false`.

## Functions

| Function | Result |
| --- | --- |
| `bridge_init` | Starts the video of SDL3, then declares the types and the system. Gives 0, or 1 when SDL3 refuses. |
| `bridge_shutdown` | Stops the video subsystem of the module, and SDL3 after the last subsystem. Gives 0. |
| `bridge_reload` | Gives 1. |
