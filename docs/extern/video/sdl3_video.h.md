# include/sdl3_video.h

`include/sdl3_video.h` is the header of the module `sdl3:video@1.0.0`, the window of SDL3 for Bridge.
The module covers `SDL_video.h` of SDL3 for one window, and gives the native handles of the window for a WebGPU surface.
On macOS, it also uses `SDL_metal.h` for the CAMetalLayer of the window. It does not cover more windows, the displays, or the GL and Vulkan contexts.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:video@1.0.0` |
| `abi` | `1` |
| `config` | `sdl3:WindowSettings` |
| `settings` | `title width height hidden resizable fullscreen place x y high_dpi` |
| `pinned` | `true`, because SDL3 runs a window only on the main thread. |
| `reloadable` | `false`, because a reload would close the window. |

The manifest also gives a default for each of these keys of the module. A deployment that states one of them replaces its default.

| Key | Default | Reason |
| --- | --- | --- |
| `scratch` | `4096` | The declarations of the four types of `init`, and the value of `sdl3:Window` that it attaches. |
| `log_ring` | `256` | Room for 8 records of 32 bytes. The module writes at most one line in a call. |
| `spans` | `1024` | A cell for each of the four types in each table, and the spans of the system. |
| `spawns` | `"none"` | The module makes no entity. |
| `changes` | `96` | The attach of `sdl3:Window` in `init`: 24 bytes, and its value of 72 bytes. |
| `persistent` | `"none"` | The module keeps no data in the arena. |
| `hot_reload` | `false` | The module cannot reload. |

The header `bridge.h` of Bridge tells the room that `spans` needs in a world with many tables.

The file of the module is `sdl3_video`, with the suffix of a library. It runs on the backend of BB-Native, and loads the SDL3 library of BL-SDL3, which sits beside it.

## The system

| System | Phase | What it does |
| --- | --- | --- |
| `window` | `input` | At its first run, it opens the window with the settings, and on macOS its Metal view. At each run, it takes the window events, and writes `sdl3:Window`. |

The settings come after `init`, so the window opens at the first run of `window`.
If the window does not open, `window` tries again at its next run, until `max_system_failures` stops the module.
When the window is asked to close, `window` sets `closed`, and sets `surface` to `SDL3_SURFACE_NONE` and each handle to 0, in that round.
At its next run, it destroys the Metal view on macOS, and then the window.
The full name of the system for `[order]` is `sdl3:video:window`.

## sdl3:WindowSettings

`sdl3:WindowSettings` is the singleton of the settings. Bridge writes it from the deployment, or from the defaults of the manifest.

| Field | C type | Value |
| --- | --- | --- |
| `title` | `char[64]` | The title of the window, at most 63 bytes. The default is `Bridge`. |
| `width`, `height` | `uint32_t` | The size of the window, in the units of the screen. The defaults are 1280 and 720. |
| `hidden` | `bool` | `true`: the window does not show. The default is `false`. |
| `resizable` | `bool` | `true`: the user can change the size of the window. The default is `false`. |
| `fullscreen` | `bool` | `true`: the window covers the screen that it opens on. A hidden window covers it only when it shows. The default is `false`. |
| `place` | `sdl3_place_t` | Where the window opens. The default is `SDL3_PLACE_SYSTEM`. |
| `x`, `y` | `int32_t` | The place of the window for `SDL3_PLACE_AT`, in the units of the screen. The defaults are 0 and 0. |
| `high_dpi` | `bool` | `true`: on a screen of high density, the frame gets each pixel of the screen. The default is `false`. |

## sdl3:sdl3_place_t

`sdl3:sdl3_place_t` is an enum of `uint32_t`. A deployment states a constant by its name or by its value.

| Constant | Value | Where the window opens |
| --- | --- | --- |
| `SDL3_PLACE_SYSTEM` | 1 | Where the system puts it. With no place from the system, SDL3 puts it at the center. |
| `SDL3_PLACE_CENTER` | 2 | At the center of the screen. |
| `SDL3_PLACE_AT` | 3 | At `x` and `y`. |

## sdl3:Window

`sdl3:Window` is a singleton that only the module writes.

| Field | C type | Value |
| --- | --- | --- |
| `surface` | `sdl3_surface_t` | The kind of the window: *sdl3:sdl3_surface_t*. |
| `width`, `height` | `uint32_t` | The size of the frame, in pixels. |
| `closed` | `bool` | `true` from the round in which the window is asked to close. |
| `hinstance`, `hwnd` | `uint64_t` | On Windows: the instance of the program, and the window. |
| `layer` | `uint64_t` | On macOS: the `CAMetalLayer` of the Metal view of the window. |
| `x11_display`, `x11_window` | `uint64_t` | On X11: the display, and the number of the window. |
| `wl_display`, `wl_surface` | `uint64_t` | On Wayland: the display, and the surface of the window. |

`surface` tells which handles hold the window. The handles of the other kinds are 0.
A renderer releases its surface in the round when `surface` becomes `SDL3_SURFACE_NONE`, because the window goes after that round.
The app copies `sdl3:Window` into the window of a renderer, for example `webgpu:Window` of `webgpu:device`.

## sdl3:sdl3_surface_t

`sdl3:sdl3_surface_t` is an enum of `uint32_t`.

| Constant | Value | The window |
| --- | --- | --- |
| `SDL3_SURFACE_NONE` | 1 | No window: before the window opens, and from the round of the close. |
| `SDL3_SURFACE_WINDOWS` | 2 | A window of Windows: `hinstance` and `hwnd`. |
| `SDL3_SURFACE_METAL` | 3 | A window of macOS: `layer`. |
| `SDL3_SURFACE_X11` | 4 | A window of X11: `x11_display` and `x11_window`. |
| `SDL3_SURFACE_WAYLAND` | 5 | A window of Wayland: `wl_display` and `wl_surface`. |

## The C types

`include/sdl3_video.h` gives the C type of each type of the module, and its descriptor for Bridge.

| Type of Bridge | C type | Descriptor |
| --- | --- | --- |
| `sdl3:sdl3_place_t` | `sdl3_place_t` | `sdl3_place_t_desc` |
| `sdl3:sdl3_surface_t` | `sdl3_surface_t` | `sdl3_surface_t_desc` |
| `sdl3:WindowSettings` | `sdl3_window_settings_t` | `sdl3_window_settings_t_desc` |
| `sdl3:Window` | `sdl3_window_t` | `sdl3_window_t_desc` |

## The deployment

- The module needs `sdl3:events`, which pumps the events of the window.
- `phases` lists `events`, then `input`, then the phases of the app and of the renderers.
- `[module."sdl3:video@1.0.0".phases]` can map these phases to other names.
- `live = true`, because a round with no change and no record ends a run that is not live.
- A module that reads the window needs `read = ["sdl3:Window"]` in its table.
- A deployment gives the settings in `[module."sdl3:video@1.0.0".config]`.
- A deployment holds one SDL3 library, beside the SDL3 modules, so that each module loads that one library.
- The SDL3 library is no module, so `content.ignore` names it.
- The backend of BB-Native has no capability of a limit, so `work_limit`, `memory_limit` and the three timeouts of the module are `"none"`.

The readme of Bridge-Host tells each key of a deployment.

## Messages

- `no video: <reason>`: SDL3 cannot start its video. `<reason>` is the text of `SDL_GetError`. Then `init` gives 1, and the module does not load. The module writes it at the level 3.
- `no window: <reason>`: the window, or its Metal view on macOS, does not open. `<reason>` is the text of `SDL_GetError`. The system then gives 1. The module writes it at the level 3.
