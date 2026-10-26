# include/sdl3_keyboard.h

`include/sdl3_keyboard.h` is the header of the module `sdl3:keyboard@1.0.0`, the keyboard of SDL3 for Bridge.
The module covers `SDL_keyboard.h` of SDL3: the keys, the modifiers and the typed text.
It does not cover the screen keyboard, or the text that an input method composes before it is typed.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:keyboard@1.0.0` |
| `abi` | `1` |
| `config` | `none` |
| `pinned` | `true`, because SDL3 gives the keyboard events on the main thread. |
| `reloadable` | `false` |

The file of the module is `sdl3_keyboard`, with the suffix of a library. It runs on the backend of BB-Native, and loads the SDL3 library of BM-SDL3, which sits beside it.

## The system keys

`keys` runs in the phase `input`. It starts the text input on the window that has the keyboard focus.
Then it takes the key events and the text events of the round, and writes `sdl3:Keyboard`.
The full name of the system for `[order]` is `sdl3:keyboard:keys`.

## sdl3:Keyboard

`sdl3:Keyboard` is a singleton that the module writes again at each round.

| Field | C type | Value |
| --- | --- | --- |
| `key` | `uint8_t[512]` | 1 for each key that is down, at the index of its scancode. |
| `hit` | `uint8_t[512]` | 1 for each key that went down in the round. A repeat of a held key is not a hit. |
| `mods` | `uint32_t` | The modifiers that are down, as the bits of `SDL_Keymod`. |
| `text` | `char[64]` | The text that was typed in the round, in UTF-8. |
| `text_len` | `uint32_t` | The length of `text`, in bytes. |

## The deployment

- The module needs `sdl3:events`, and `sdl3:video` for a window that takes the keyboard focus.
- A module that reads the keyboard needs `read = ["sdl3:Keyboard"]` in its table.
- The SDL3 library is no module, so `content.ignore` names it.
- The backend of BB-Native has no capability of a limit, so `work_limit`, `memory_limit` and the three timeouts of the module are `"none"`.

The readme of Bridge-Host tells each key of a deployment.
## Messages

The module writes no message.
