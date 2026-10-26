# include/sdl3_mouse.h

`include/sdl3_mouse.h` is the header of the module `sdl3:mouse@1.0.0`, the mouse of SDL3 for Bridge.
The module covers `SDL_mouse.h` of SDL3: where the mouse is, its buttons and its wheel, and the shape of the cursor.
It does not cover more than one mouse, or a cursor that an image makes.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:mouse@1.0.0` |
| `abi` | `1` |
| `config` | `none` |
| `pinned` | `true`, because SDL3 gives the mouse events on the main thread. |
| `reloadable` | `false` |

The file of the module is `sdl3_mouse`, with the suffix of a library. It runs on the backend of BB-Native, and loads the SDL3 library of BM-SDL3, which sits beside it.

## The system mouse

`mouse` runs in the phase `input`. It applies the `sdl3:Cursor` that the app wrote.
Then it takes the mouse events of the round, and writes `sdl3:Mouse`.
The full name of the system for `[order]` is `sdl3:mouse:mouse`.

## sdl3:Mouse

`sdl3:Mouse` is a singleton that the module writes again at each round.

| Field | C type | Value |
| --- | --- | --- |
| `x`, `y` | `float` | The position in the window, in pixels from the top left. |
| `dx`, `dy` | `float` | The motion in the round, in pixels. |
| `wheel_x`, `wheel_y` | `float` | The turn of the wheel in the round. A positive `wheel_y` turns away from the user. |
| `held` | `uint32_t` | The buttons that are down, as the bits of `SDL_BUTTON_MASK`. |
| `pressed`, `released` | `uint32_t` | The buttons that went down and up in the round, as the same bits. |

## sdl3:Cursor

`sdl3:Cursor` is a singleton that the app writes, and that the module reads.

| Field | C type | Value |
| --- | --- | --- |
| `shape` | `uint32_t` | A value of `SDL_SystemCursor`. |
| `hidden` | `bool` | `true`: the cursor does not show. |
| `relative` | `bool` | `true`: the window holds the mouse, and only the motion counts. |

## The deployment

- The module needs `sdl3:events`, and `sdl3:video` for a window that takes the mouse.
- A module that reads the mouse needs `read = ["sdl3:Mouse"]`, and the module that sets the cursor needs `write = ["sdl3:Cursor"]`.
- The SDL3 library is no module, so `content.ignore` names it.
- The backend of BB-Native has no capability of a limit, so `work_limit`, `memory_limit` and the three timeouts of the module are `"none"`.

The readme of Bridge-Host tells each key of a deployment.
## Messages

The module writes no message.
