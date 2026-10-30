# src/keyboard/keyboard_inside.h

The three sources of `sdl3:keyboard` include this header.
It includes SDL3, BB-Native, `include/keyboard/sdl3_keyboard.h` and `src/shared/shared_subsystem.h`, gives the slots, and declares the function of the text.

## The slots

`SETTINGS` is 0 for `sdl3:KeyboardSettings`, and `KEYBOARD` is 1 for `sdl3:Keyboard`. Each slot is a `constexpr`.
`bridge_init` of `src/keyboard/sdl3_keyboard.c` declares the two types in that order.
Bridge numbers them from 0: https://github.com/Encrid/Bridge/blob/v0.2.0/docs/extern/bridge_abi.h.md#bridge_entries_t.

## Functions

| Function | Result |
| --- | --- |
| `keyboard_append_text` | Declared here, given by `src/keyboard/keyboard_text.c`. |
