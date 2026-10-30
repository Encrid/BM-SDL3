# include/keyboard/sdl3_keyboard.h

`include/keyboard/sdl3_keyboard.h` is the header of the module `sdl3:keyboard@1.0.0`, the keyboard of SDL3 for Bridge.
The module covers `SDL_keyboard.h` of SDL3: the keys, the modifiers and the typed text.
It does not cover the screen keyboard, or the text that an input method composes before it is typed.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:keyboard@1.0.0` |
| `abi` | `2` |
| `config` | `sdl3:KeyboardSettings` |
| `settings` | `text repeat` |
| `pinned` | `true`, because SDL3 gives the keyboard events on the main thread. |
| `reloadable` | `false` |

`readme.md`, *The modules*, tells how a deployment replaces a default.

| Key | Default | Reason |
| --- | --- | --- |
| `scratch` | `4096` | `init` declares 2 types, and attaches the value of `sdl3:Keyboard`. |
| `log_ring` | `256` | Room for 8 records of 32 bytes. Only `init` writes a line. |
| `spans` | `1024` | Two types, and the spans of `keys`. |
| `spawns` | `"none"` | The module makes no entity. |
| `changes` | `1120` | `init` attaches `sdl3:Keyboard`: 24 bytes, and 1096 for the value. |
| `persistent` | `"none"` | No data to keep. |
| `hot_reload` | `false` | The module cannot reload. |

The file of the module is `sdl3_keyboard`, with the suffix of a library.

## The system keys

`keys` runs in the phase `input`, and does these steps:

1. If `text` is `true`, it starts the text input on each window. If it is `false`, it stops it.
2. It clears the hits and the text of the round before.
3. It takes the key events and the text events of the round, and writes `sdl3:Keyboard`.

It starts the text input on each window, not only on the window with the focus, because a window can open with no focus.
While `sdl3:KeyboardSettings` or `sdl3:Keyboard` is detached, `keys` gives 0 and does nothing.
Its full name for `[order]` is `sdl3:keyboard:keys`.

## sdl3:KeyboardSettings

`sdl3:KeyboardSettings` is the singleton of the settings: `readme.md`, *The modules*. `keys` reads `text` and `repeat` again at each run.

| Field | C type | Value |
| --- | --- | --- |
| `text` | `bool` | `true`: `keys` starts the text input, and `false` stops it. The default is `true`. |
| `repeat` | `bool` | `true`: a repeat of a held key is a hit too. The default is `false`. |

## sdl3:Keyboard

`keys` writes the singleton `sdl3:Keyboard` again at each round.

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

The header gives these types: `readme.md`, *The modules*.

| Type of Bridge | C type | Descriptor | Macro of the text |
| --- | --- | --- | --- |
| `sdl3:KeyboardSettings` | `sdl3_keyboard_settings_t` | `sdl3_keyboard_settings_t_desc` | `SDL3_KEYBOARD_SETTINGS_FIELDS` |
| `sdl3:Keyboard` | `sdl3_keyboard_t` | `sdl3_keyboard_t_desc` | `SDL3_KEYBOARD_FIELDS` |

## The declarations

`readme.md`, *The modules*, tells the rules.

| Type of Bridge | Kind | Storage layout | `on_destroy` |
| --- | --- | --- | --- |
| `sdl3:KeyboardSettings` | `BRIDGE_STRUCT` | `BRIDGE_SINGLETON` | `BRIDGE_ON_DESTROY_REMOVE` |
| `sdl3:Keyboard` | `BRIDGE_STRUCT` | `BRIDGE_SINGLETON` | `BRIDGE_ON_DESTROY_REMOVE` |

## The deployment

- The module needs `sdl3:events` and a window: `readme.md`, *The modules*.
- A module that reads the keyboard needs `read = ["sdl3:Keyboard"]` in its table.
- A module that changes `text` or `repeat`, for example the app, needs `write = ["sdl3:KeyboardSettings"]` in its table.
- A deployment gives the settings in `[module."sdl3:keyboard@1.0.0".config]`.

`readme.md`, *The modules*, tells the rules of each deployment.

## Messages

`readme.md`, *The modules*, tells its message.
