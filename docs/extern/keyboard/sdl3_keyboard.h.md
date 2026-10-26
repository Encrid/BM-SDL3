# include/sdl3_keyboard.h

`include/sdl3_keyboard.h` is the header of the module `sdl3:keyboard@1.0.0`, the keyboard of SDL3 for Bridge.
The module covers `SDL_keyboard.h` of SDL3: the keys, the modifiers and the typed text.
It does not cover the screen keyboard, or the text that an input method composes before it is typed.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:keyboard@1.0.0` |
| `abi` | `1` |
| `config` | `sdl3:KeyboardSettings` |
| `settings` | `text repeat` |
| `pinned` | `true`, because SDL3 gives the keyboard events on the main thread. |
| `reloadable` | `false` |

The manifest also gives a default for each of these keys of the module. A deployment that states one of them replaces its default.

| Key | Default | Reason |
| --- | --- | --- |
| `scratch` | `4096` | The declarations of `init`, and the value of `sdl3:Keyboard` that it attaches. |
| `log_ring` | `8` | The module writes at most one line in a call. |
| `spans` | `1024` | A cell for each of the two types in each table, and the spans of `keys`. |
| `spawns` | `"none"` | The module makes no entity. |
| `changes` | `1120` | The attach of `sdl3:Keyboard` in `init`: 24 bytes, and its value of 1096 bytes. |
| `persistent` | `"none"` | The module keeps no data in the arena. |
| `hot_reload` | `false` | The module cannot reload. |

The header `bridge.h` of Bridge tells the room that `spans` needs in a world with many tables.

The file of the module is `sdl3_keyboard`, with the suffix of a library. It runs on the backend of BB-Native, and loads the SDL3 library of BL-SDL3, which sits beside it.

## The system keys

`keys` runs in the phase `input`, and does these steps:

1. If `text` is `true`, it starts the text input on each window. If it is `false`, it stops it.
2. It clears the hits and the text of the round before.
3. It takes the key events and the text events of the round, and writes `sdl3:Keyboard`.

It starts the text input on each window, not only on the window with the focus, because a window can open with no focus.
The full name of the system for `[order]` is `sdl3:keyboard:keys`.

## sdl3:KeyboardSettings

`sdl3:KeyboardSettings` is the singleton of the settings. Bridge writes it from the deployment, or from the defaults of the manifest.
After the load, a system with a grant to write it can change it at each round. `keys` uses the new values at its next run.

| Field | C type | Value |
| --- | --- | --- |
| `text` | `bool` | `true`: `keys` starts the text input, and `false` stops it. The default is `true`. |
| `repeat` | `bool` | `true`: a repeat of a held key is a hit too. The default is `false`. |

## sdl3:Keyboard

`sdl3:Keyboard` is a singleton that the module writes again at each round.

| Field | C type | Value |
| --- | --- | --- |
| `key` | `uint8_t[512]` | 1 for each key that is down, at the index of its scancode. |
| `hit` | `uint8_t[512]` | 1 for each key that went down in the round. A repeat of a held key is a hit only if `repeat` is `true`. |
| `mods` | `uint32_t` | The modifiers that are down, as the bits of `SDL_Keymod`. |
| `text` | `char[64]` | The text that was typed in the round, in UTF-8, with a zero after it. |
| `text_len` | `uint32_t` | The length of `text`, in bytes. |

The module leaves out a key event with a scancode of 512 or more.
A text of more than 63 bytes in a round is cut after its last whole character that fits. The rest of the text of that round is left out.

## The C types

`include/sdl3_keyboard.h` gives the C type of each type of the module, and its descriptor for Bridge.

| Type of Bridge | C type | Descriptor |
| --- | --- | --- |
| `sdl3:KeyboardSettings` | `sdl3_keyboard_settings_t` | `sdl3_keyboard_settings_t_desc` |
| `sdl3:Keyboard` | `sdl3_keyboard_t` | `sdl3_keyboard_t_desc` |

## The deployment

- The module needs `sdl3:events`, and `sdl3:video` for a window that takes the keyboard focus.
- A module that reads the keyboard needs `read = ["sdl3:Keyboard"]` in its table.
- A module that changes the settings, for example the app, needs `write = ["sdl3:KeyboardSettings"]` in its table.
- A deployment gives the settings in `[module."sdl3:keyboard@1.0.0".config]`.
- A deployment holds one SDL3 library, beside the SDL3 modules, so that each module loads that one library.
- The SDL3 library is no module, so `content.ignore` names it.
- The backend of BB-Native has no capability of a limit, so `work_limit`, `memory_limit` and the three timeouts of the module are `"none"`.

The readme of Bridge-Host tells each key of a deployment.

## Messages

- `no video: <reason>`: SDL3 cannot start its video. `<reason>` is the text of `SDL_GetError`. Then `init` gives 1, and the module does not load. The module writes it at the level 3.
