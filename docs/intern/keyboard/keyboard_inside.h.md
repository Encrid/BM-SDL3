# src/keyboard_inside.h

This private header is shared by the three sources of the module.
It includes the headers of SDL3 and of BB-Native, gives the slot of each type of the module, and declares the function of the text.

## The slots

Bridge numbers the types of a module from 0, in the order of their declarations.
So the header gives each slot as a `constexpr`: `SETTINGS` is 0 for `sdl3:KeyboardSettings`, and `KEYBOARD` is 1 for `sdl3:Keyboard`.
`bridge_init` of `src/sdl3_keyboard.c` declares the two types in that order.

## Functions

| Function | Result |
| --- | --- |
| `keyboard_append_text` | Declared here, given by `src/keyboard_text.c`. |
