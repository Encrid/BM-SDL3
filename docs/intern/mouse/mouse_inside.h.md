# src/mouse/mouse_inside.h

The three sources of `sdl3:mouse` include this header.
It includes SDL3, BB-Native, `include/mouse/sdl3_mouse.h` and `src/shared/shared_subsystem.h`. It gives the slots and the private type of the cursor, and declares the functions of the cursor.

## The slots

`SETTINGS` is 0 for `sdl3:MouseSettings`, `CURSOR` is 1 for `sdl3:Cursor`, and `MOUSE` is 2 for `sdl3:Mouse`.
Each slot is a `constexpr`, and `bridge_init` of `src/mouse/sdl3_mouse.c` declares the types in that order.
Bridge numbers them from 0: https://github.com/Encrid/Bridge/blob/v0.2.0/docs/extern/bridge_abi.h.md#bridge_entries_t.

## Types

The header gives the private type of the cursor, because a source declares no type: `docs/contributing/coding_style.md`, *H1 - The contents of a header*.

| Type | Field | Value |
| --- | --- | --- |
| `mouse_kept_t` | `made` | The cursor that the module made, or null. |
| `mouse_kept_t` | `shape` | The shape that `made` shows. |

`src/mouse/mouse_cursor.c` keeps one `mouse_kept_t`: `docs/intern/mouse/mouse_cursor.c.md`, *The state*.

## Functions

| Function | Result |
| --- | --- |
| `mouse_apply_cursor` | Declared here, given by `src/mouse/mouse_cursor.c`. |
| `mouse_free_cursor` | Declared here, given by `src/mouse/mouse_cursor.c`. |
