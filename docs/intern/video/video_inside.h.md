# src/video/video_inside.h

The four sources of `sdl3:video` include this header.
It includes SDL3, BB-Native, `include/video/sdl3_video.h` and `src/shared/shared_subsystem.h`. It gives the slots and the private types, and declares the functions that the sources share.

## The slots

`PLACE` is 0 for `sdl3:sdl3_place_t`, `SURFACE` is 1 for `sdl3:sdl3_surface_t`, `SETTINGS` is 2 for `sdl3:WindowSettings`, and `WINDOW` is 3 for `sdl3:Window`.
Each slot is a `constexpr`, and `declare_types` of `src/video/sdl3_video.c` declares the types in that order.
Bridge numbers them from 0: https://github.com/Encrid/Bridge/blob/v0.2.0/docs/extern/bridge_abi.h.md#bridge_entries_t.
The two enums come first, because the settings and the window use them.

## Types

The header gives the two private types, because a source declares no type: `docs/contributing/coding_style.md`, *H1 - The contents of a header*.

| Type | Field | Value |
| --- | --- | --- |
| `video_kept_t` | `window` | The window that the module made, or null. |
| `video_kept_t` | `view` | The Metal view of that window, or null. |
| `video_kept_t` | `last` | The settings of the last run of `window`, with a zero in byte 63 of `title`. |
| `video_driver_t` | `name` | A name that `SDL_GetCurrentVideoDriver` gives. |
| `video_driver_t` | `surface` | The kind of each window of that driver. |

`src/video/video_window.c` keeps one `video_kept_t`: `docs/intern/video/video_window.c.md`, *The state*.
`src/video/video_state.c` keeps a table of `video_driver_t`: `docs/intern/video/video_state.c.md`, *The driver*.

## The messages

Each source writes its messages with `bridge_native_say` of BB-Native, which the header `bridge_native_log.h` gives.
Thus `bridge_native_say` is the writer of the messages of the code: `docs/contributing/tests.md`, *The messages of the code*.
The module has no writer of its own.

## Functions

The header gives no function of its own. It declares the functions that one source gives and another source calls.

| Function | Source | Result |
| --- | --- | --- |
| `video_close_window` | `src/video/video_window.c` | Destroys the window of the module. |
| `video_apply_settings` | `src/video/video_settings.c` | Asks SDL3 for the changed settings. |
| `video_get_place` | `src/video/video_settings.c` | Gives a point of SDL3 for `place`. |
| `video_find_driver` | `src/video/video_state.c` | Gives the row of the driver. |
| `video_write_handles` | `src/video/video_state.c` | Fills a value of `sdl3:Window` at the open. |
| `video_has_handle` | `src/video/video_state.c` | Tells if one handle is not 0. |
| `video_take_events` | `src/video/video_state.c` | Writes the window at each run. |
