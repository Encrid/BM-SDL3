# src/events/sdl3_events.c

This file holds the manifest of `sdl3:events`: `.toolchain/docs/presets.md`, *native_module*.
Its entries are `init`, `shutdown` and `reload`.

## The start

`bridge_init` does these steps:

1. `common_start_events` starts the events of SDL3: `docs/intern/common/common_subsystem.c.md`. If SDL3 refuses, `bridge_init` gives 1.
2. It declares `sdl3:Drop` as a table, then `sdl3:EventsSettings` as a singleton.
3. It adds the system `pump` in the phase `events`.

Each type states `BRIDGE_STRIP` and `BRIDGE_ON_DESTROY_REMOVE`: `readme.md`, *The modules*, and `docs/extern/events/sdl3_events.h.md`, *The declarations*.

`pump` has two terms. It reads each `sdl3:Drop`, so it finds the drops of the round before. It reads `sdl3:EventsSettings` as an optional term.
`pump` also states that it changes `sdl3:Drop`, because it attaches and destroys the drops.
The first run of `pump` has the settings: `readme.md`, *The modules*.

## The stop

`bridge_shutdown` stops the events with `common_stop_subsystem`: `docs/intern/common/common_subsystem.c.md`.
`bridge_reload` gives 1: `readme.md`, *The modules*.

## Functions

| Function | Result |
| --- | --- |
| `bridge_init` | Starts the events, declares `sdl3:Drop` and `sdl3:EventsSettings`, and adds `pump`. Gives 0, or 1 when SDL3 refuses. |
| `bridge_shutdown` | Stops the events. Gives 0. |
| `bridge_reload` | Gives 1. |
