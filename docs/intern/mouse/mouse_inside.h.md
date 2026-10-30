# src/mouse_inside.h

This private header is shared by the three sources of the module.
It includes the headers of SDL3 and of BB-Native, and gives the slot of each type and the private type of the cursor.
It also declares the functions of the cursor.

## The slots

Bridge numbers the types of a module from 0, in the order of their declarations.
`SETTINGS` is 0 for `sdl3:MouseSettings`, `CURSOR` is 1 for `sdl3:Cursor`, and `MOUSE` is 2 for `sdl3:Mouse`.
Each slot is a `constexpr`, and `bridge_init` of `src/sdl3_mouse.c` declares the types in that order.

## Types

A source declares no type, so the header gives the private type of the cursor: `docs/contributing/coding_style.md`, *H1 - The contents of a header*.

| Type | Field | Value |
| --- | --- | --- |
| `mouse_kept_t` | `made` | The cursor of SDL3 that the module made, or null. |
| `mouse_kept_t` | `shape` | The shape that `made` shows. |

`src/mouse_cursor.c` keeps one `mouse_kept_t`: `docs/intern/mouse_cursor.c.md`, *The state*.

## Functions

| Function | Result |
| --- | --- |
| `mouse_apply_cursor` | Declared here, given by `src/mouse_cursor.c`. |
| `mouse_free_cursor` | Declared here, given by `src/mouse_cursor.c`. |
