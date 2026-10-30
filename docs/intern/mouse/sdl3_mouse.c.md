# src/sdl3_mouse.c

This file holds the manifest of the module, and its entries `init`, `shutdown` and `reload`.

## The manifest

The kind `native_module` gives the words of the manifest as the macro `MANIFEST`.
The file puts that macro into the section of the target, with the line that the preset of the module tells.

## The start

`bridge_init` does these steps:

1. It starts the video subsystem of SDL3, because the mouse events come through it.
2. If SDL3 refuses, it writes `no video` with the reason of SDL3, and gives 1.
3. It declares `sdl3:MouseSettings`, `sdl3:Cursor` and `sdl3:Mouse`. Each one is a singleton.
4. It attaches a `sdl3:Mouse` with each field at zero to the entity 0.
5. It adds the system `mouse` in the phase `input`.

Each type states `BRIDGE_STRIP`, so an unload of the module destroys no entity.
Each type also states `BRIDGE_ON_DESTROY_REMOVE`: `docs/extern/sdl3_mouse.h.md`, *The declarations*.

`mouse` has three optional terms: it reads the settings and the cursor, and it writes the mouse.
The app attaches `sdl3:Cursor` itself. Until it does, `mouse` changes no cursor.

## The stop

`bridge_shutdown` destroys the cursor that the module made, then stops the video subsystem that `bridge_init` started.
`SDL_Quit` does not count the modules that use SDL3. So `bridge_shutdown` calls it only after the last subsystem stops, when `SDL_WasInit(0)` gives 0.

`bridge_reload` gives 1. Bridge needs the entry, but never calls it, because the manifest states `reloadable=false`.

## Functions

| Function | Result |
| --- | --- |
| `bridge_init` | Starts the video of SDL3, then declares the types and the system. Gives 0, or 1 when SDL3 refuses. |
| `bridge_shutdown` | Frees the cursor, stops the video subsystem of the module, and SDL3 after the last subsystem. Gives 0. |
| `bridge_reload` | Gives 1. |
