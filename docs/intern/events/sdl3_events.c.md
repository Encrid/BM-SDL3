# src/sdl3_events.c

This file holds the manifest of the module, and its entries `init`, `shutdown` and `reload`.

## The manifest

The kind `native_module` gives the words of the manifest as the macro `MANIFEST`.
The file puts that macro into the section of the target, with the line that the preset of the module tells.

## The start

`bridge_init` does these steps:

1. It starts the events subsystem of SDL3.
2. If SDL3 refuses, it writes `no events` with the reason of SDL3, and gives 1.
3. It declares `sdl3:Drop` as a table, then `sdl3:EventsSettings` as a singleton.
4. It adds the system `pump` in the phase `events`.

Each type states `BRIDGE_STRIP`, so an unload of the module destroys no entity.
Each type also states `BRIDGE_ON_DESTROY_REMOVE`: `docs/extern/sdl3_events.h.md`, *The declarations*.

`pump` has two terms. It reads each `sdl3:Drop`, so it finds the drops of the round before. It reads `sdl3:EventsSettings` as an optional term.
`pump` also states that it changes `sdl3:Drop`, because it attaches and destroys the drops.
Bridge writes the settings after `init`, so the first run of `pump` has them.

## The stop

`bridge_shutdown` stops the events subsystem that `bridge_init` started.
`SDL_Quit` does not count the modules that use SDL3. So `bridge_shutdown` calls it only after the last subsystem stops, when `SDL_WasInit(0)` gives 0.

`bridge_reload` gives 1. Bridge needs the entry, but never calls it, because the manifest states `reloadable=false`.

## Functions

| Function | Result |
| --- | --- |
| `bridge_init` | Starts the events of SDL3, then declares the types and the system. Gives 0, or 1 when SDL3 refuses. |
| `bridge_shutdown` | Stops the events subsystem of the module, and SDL3 after the last subsystem. Gives 0. |
| `bridge_reload` | Gives 1. |
