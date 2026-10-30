# src/events_inside.h

This private header is shared by the two sources of the module.
It includes the headers of SDL3 and of BB-Native, and gives the slot of each type of the module.

## The slots

Bridge numbers the types of a module from 0, in the order of their declarations.
So the header gives each slot as a `constexpr`: `DROP` is 0 for `sdl3:Drop`, and `SETTINGS` is 1 for `sdl3:EventsSettings`.
`bridge_init` of `src/sdl3_events.c` declares the two types in that order.

## Functions

The header gives no function.
