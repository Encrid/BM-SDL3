# include/sdl3_mouse.h

`include/sdl3_mouse.h` is the header of the module `sdl3:mouse@1.0.0`, the mouse of SDL3 for Bridge.
The module covers `SDL_mouse.h` of SDL3: where the mouse is, its buttons and its wheel, and the shape of the cursor.
It does not cover more than one mouse, or a cursor that an image makes.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:mouse@1.0.0` |
| `abi` | `1` |
| `config` | `sdl3:MouseSettings` |
| `settings` | `natural` |
| `pinned` | `true`, because SDL3 gives the mouse events on the main thread. |
| `reloadable` | `false` |

The manifest also gives a default for each of these keys of the module. A deployment that states one of them replaces its default.

| Key | Default | Reason |
| --- | --- | --- |
| `scratch` | `4096` | The declarations of `init`, and the value of `sdl3:Mouse` that it attaches. |
| `log_ring` | `8` | The module writes at most one line in a call. |
| `spans` | `1024` | A cell for each of the three types in each table, and the spans of `mouse`. |
| `spawns` | `"none"` | The module makes no entity. |
| `changes` | `64` | The attach of `sdl3:Mouse` in `init`: 24 bytes, and its value of 36 bytes, rounded up to 40. |
| `persistent` | `"none"` | The module keeps no data in the arena. |
| `hot_reload` | `false` | The module cannot reload. |

The header `bridge.h` of Bridge tells the room that `spans` needs in a world with many tables.

The file of the module is `sdl3_mouse`, with the suffix of a library. It runs on the backend of BB-Native, and loads the SDL3 library of BL-SDL3, which sits beside it.

## The system mouse

`mouse` runs in the phase `input`. It applies the `sdl3:Cursor` that the app wrote: the shape when it changes, and `hidden` and `relative` at each round.
Then it takes the mouse events of the round, and writes `sdl3:Mouse`.
The full name of the system for `[order]` is `sdl3:mouse:mouse`.

## sdl3:MouseSettings

`sdl3:MouseSettings` is the singleton of the settings. Bridge writes it from the deployment, or from the defaults of the manifest.
After the load, a system with a grant to write it can change it at each round. `mouse` uses the new value at its next run.

| Field | C type | Value |
| --- | --- | --- |
| `natural` | `bool` | `true`: `wheel_y` keeps the direction that the system gives. The default is `false`. |

## sdl3:Mouse

`sdl3:Mouse` is a singleton that the module writes again at each round.

| Field | C type | Value |
| --- | --- | --- |
| `x`, `y` | `float` | The position of the last motion, in pixels of the frame of `sdl3:Window`, from the top left. |
| `dx`, `dy` | `float` | The motion in the round, in the same pixels. |
| `wheel_x`, `wheel_y` | `float` | The turn of the wheel in the round. A positive `wheel_x` turns to the right. A positive `wheel_y` turns away from the user, unless `natural` is `true`. |
| `held` | `uint32_t` | The buttons that are down, as the bits of `SDL_BUTTON_MASK`. |
| `pressed`, `released` | `uint32_t` | The buttons that went down and up in the round, as the same bits. |

The module leaves out a button of 0, or of more than 32.

## sdl3:Cursor

`sdl3:Cursor` is a singleton that the app writes, and that the module reads.

| Field | C type | Value |
| --- | --- | --- |
| `shape` | `uint32_t` | A value of `SDL_SystemCursor`. A value of `SDL_SYSTEM_CURSOR_COUNT` or more changes nothing. |
| `hidden` | `bool` | `true`: the cursor does not show. |
| `relative` | `bool` | `true`: each window holds the mouse, and only the motion counts. |

## The C types

`include/sdl3_mouse.h` gives the C type of each type of the module, and its descriptor for Bridge.

| Type of Bridge | C type | Descriptor |
| --- | --- | --- |
| `sdl3:MouseSettings` | `sdl3_mouse_settings_t` | `sdl3_mouse_settings_t_desc` |
| `sdl3:Cursor` | `sdl3_cursor_t` | `sdl3_cursor_t_desc` |
| `sdl3:Mouse` | `sdl3_mouse_t` | `sdl3_mouse_t_desc` |

## The deployment

- The module needs `sdl3:events`, and `sdl3:video` for a window that takes the mouse.
- A module that reads the mouse needs `read = ["sdl3:Mouse"]`, and the module that sets the cursor needs `write = ["sdl3:Cursor"]`.
- A module that changes the setting, for example the app, needs `write = ["sdl3:MouseSettings"]` in its table.
- A deployment gives the settings in `[module."sdl3:mouse@1.0.0".config]`.
- A deployment holds one SDL3 library, beside the SDL3 modules, so that each module loads that one library.
- The SDL3 library is no module, so `content.ignore` names it.
- The backend of BB-Native has no capability of a limit, so `work_limit`, `memory_limit` and the three timeouts of the module are `"none"`.

The readme of Bridge-Host tells each key of a deployment.

## Messages

- `no video: <reason>`: SDL3 cannot start its video. `<reason>` is the text of `SDL_GetError`. Then `init` gives 1, and the module does not load. The module writes it at the level 3.
