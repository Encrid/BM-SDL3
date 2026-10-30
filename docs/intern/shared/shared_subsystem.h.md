# src/shared/shared_subsystem.h

This header declares the start and the stop of a subsystem of SDL3, for the private header of each module.
It includes `SDL_init.h` of SDL3 and `bridge_native.h` of BB-Native, because the functions name their types.

## Functions

| Function | Result |
| --- | --- |
| `shared_start_video` | Declared here, given by `src/shared/shared_subsystem.c`. |
| `shared_start_events` | Declared here, given by `src/shared/shared_subsystem.c`. |
| `shared_stop_subsystem` | Declared here, given by `src/shared/shared_subsystem.c`. |
