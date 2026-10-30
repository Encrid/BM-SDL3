# include/mouse/sdl3_mouse.h

`include/mouse/sdl3_mouse.h` is the header of the module `sdl3:mouse@1.0.0`, the mouse of SDL3 for Bridge.
The module covers `SDL_mouse.h` of SDL3: where the mouse is, its buttons and its wheel, and the shape of the cursor.
It does not cover more than one mouse, or a cursor that an image makes.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:mouse@1.0.0` |
| `abi` | `2` |
| `config` | `sdl3:MouseSettings` |
| `settings` | `natural` |
| `pinned` | `true`, because SDL3 gives the mouse events on the main thread. |
| `reloadable` | `false` |

`readme.md`, *The modules*, tells how a deployment replaces a default.

| Key | Default | Reason |
| --- | --- | --- |
| `scratch` | `4096` | `init` declares 3 types, and attaches the value of `sdl3:Mouse`. |
| `log_ring` | `256` | Room for 8 records of 32 bytes. Only `init` writes a line. |
| `spans` | `1024` | Three types, and the spans of `mouse`. |
| `spawns` | `"none"` | The module makes no entity. |
| `changes` | `64` | `init` attaches `sdl3:Mouse`: 24 bytes, and 36 for the value, rounded up to 40. |
| `persistent` | `"none"` | No data to keep. |
| `hot_reload` | `false` | The module cannot reload. |

The file of the module is `sdl3_mouse`, with the suffix of a library.

## The system mouse

`mouse` runs in the phase `input`. It applies the `sdl3:Cursor` that the app wrote: the shape when it changes, and `hidden` and `relative` at each round.
Then it takes the mouse events of the round, and writes `sdl3:Mouse`.
While `sdl3:MouseSettings` or `sdl3:Mouse` is detached, `mouse` gives 0 and does nothing.
While `sdl3:Cursor` is gone, before the app attaches it or after a module detaches it, `mouse` changes no cursor, but it writes `sdl3:Mouse`.
Its full name for `[order]` is `sdl3:mouse:mouse`.

## sdl3:MouseSettings

`sdl3:MouseSettings` is the singleton of the settings: `readme.md`, *The modules*. `mouse` reads `natural` again at each run.

| Field | C type | Value |
| --- | --- | --- |
| `natural` | `bool` | `true`: the wheel keeps the sign that the system gives: *sdl3:Mouse*. The default is `false`. |

## sdl3:Mouse

`mouse` writes the singleton `sdl3:Mouse` again at each round.

| Field | C type | Value |
| --- | --- | --- |
| `x`, `y` | `float` | The position of the last motion, in pixels of the frame of `sdl3:Window`, from the top left. |
| `dx`, `dy` | `float` | The motion in the round, in the same pixels. |
| `wheel_x`, `wheel_y` | `float` | The turn of the wheel in the round. A positive `wheel_x` turns to the right. A positive `wheel_y` turns away from the user. If `natural` is `true`, the module keeps the sign that the system gives, also for a wheel that the system flips. |
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

When `relative` changes on the window with the keyboard focus, SDL3 drops the motions that wait in the queue. So the motions of that round are lost.

## The C types

The header gives these types: `readme.md`, *The modules*.

| Type of Bridge | C type | Descriptor | Macro of the text |
| --- | --- | --- | --- |
| `sdl3:MouseSettings` | `sdl3_mouse_settings_t` | `sdl3_mouse_settings_t_desc` | `SDL3_MOUSE_SETTINGS_FIELDS` |
| `sdl3:Cursor` | `sdl3_cursor_t` | `sdl3_cursor_t_desc` | `SDL3_CURSOR_FIELDS` |
| `sdl3:Mouse` | `sdl3_mouse_t` | `sdl3_mouse_t_desc` | `SDL3_MOUSE_FIELDS` |

## The declarations

`readme.md`, *The modules*, tells the rules.

| Type of Bridge | Kind | Storage layout | `on_destroy` |
| --- | --- | --- | --- |
| `sdl3:MouseSettings` | `BRIDGE_STRUCT` | `BRIDGE_SINGLETON` | `BRIDGE_ON_DESTROY_REMOVE` |
| `sdl3:Cursor` | `BRIDGE_STRUCT` | `BRIDGE_SINGLETON` | `BRIDGE_ON_DESTROY_REMOVE` |
| `sdl3:Mouse` | `BRIDGE_STRUCT` | `BRIDGE_SINGLETON` | `BRIDGE_ON_DESTROY_REMOVE` |

## The deployment

- The module needs `sdl3:events` and a window: `readme.md`, *The modules*.
- A module that reads the mouse needs `read = ["sdl3:Mouse"]`, and the module that sets the cursor needs `write = ["sdl3:Cursor"]`.
- A module that changes `natural`, for example the app, needs `write = ["sdl3:MouseSettings"]` in its table.
- A deployment gives the settings in `[module."sdl3:mouse@1.0.0".config]`.

`readme.md`, *The modules*, tells the rules of each deployment.

## Messages

`readme.md`, *The modules*, tells its message.
