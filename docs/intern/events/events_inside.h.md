# src/events/events_inside.h

The two sources of `sdl3:events` include this header.
It includes SDL3, BB-Native, `include/events/sdl3_events.h` and `src/common/common_subsystem.h`, and gives the slots.

## The slots

`DROP` is 0 for `sdl3:Drop`, and `SETTINGS` is 1 for `sdl3:EventsSettings`. Each slot is a `constexpr`.
`bridge_init` of `src/events/sdl3_events.c` declares the two types in that order.
Bridge numbers them from 0: https://github.com/Encrid/Bridge/blob/v0.2.0/docs/extern/bridge_abi.h.md#bridge_entries_t.

## Functions

The header gives no function.
