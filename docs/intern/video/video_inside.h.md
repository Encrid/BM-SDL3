# src/video_inside.h

This private header is shared by the two sources of the module.
It includes the headers of SDL3 and of BB-Native, and gives the slot of each type of the module.

## The slots

Bridge numbers the types of a module from 0, in the order of their declarations.
`PLACE` is 0 for `sdl3:Place`, `SETTINGS` is 1 for `sdl3:WindowSettings`, and `WINDOW` is 2 for `sdl3:Window`.
Each slot is a `constexpr`, and `bridge_init` of `src/sdl3_video.c` declares the types in that order.
The enum comes first, because the settings use it.

## Functions

The header gives no function.
