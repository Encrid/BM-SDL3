# include/sdl3_video.h

`include/sdl3_video.h` is the header of the module `sdl3:video@1.0.0`, the window of SDL3 for Bridge.
The module covers `SDL_video.h` of SDL3 for one window and its GL context, of GL 4.1 core by default.
It does not cover more windows, the displays, Vulkan or Metal.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:video@1.0.0` |
| `abi` | `1` |
| `config` | `sdl3:WindowSettings` |
| `settings` | `title width height hidden vsync resizable fullscreen place x y gl_major gl_minor core depth stencil samples high_dpi` |
| `pinned` | `true`, because SDL3 runs a window only on the main thread. |
| `reloadable` | `false`, because a reload would close the window. |

The manifest also gives a default for each of these keys of the module. A deployment that states one of them replaces its default.

| Key | Default | Reason |
| --- | --- | --- |
| `scratch` | `4096` | The declarations of `init`, and the value of `sdl3:Window` that it attaches. |
| `log_ring` | `8` | The module writes at most one line in a call. |
| `spans` | `1024` | A cell for each of the three types in each table, and the spans of the two systems. |
| `spawns` | `"none"` | The module makes no entity. |
| `changes` | `48` | The attach of `sdl3:Window` in `init`: 24 bytes, and its value of 24 bytes. |
| `persistent` | `"none"` | The module keeps no data in the arena. |
| `hot_reload` | `false` | The module cannot reload. |

The header `bridge.h` of Bridge tells the room that `spans` needs in a world with many tables.

The file of the module is `sdl3_video`, with the suffix of a library. It runs on the backend of BB-Native, and loads the SDL3 library of BL-SDL3, which sits beside it.

## The systems

| System | Phase | What it does |
| --- | --- | --- |
| `window` | `input` | At its first run, it opens the window and its context with the settings. At each run, it takes the window events, and writes `sdl3:Window`. |
| `present` | `present` | While `gl_proc` is not 0, it swaps the buffers of the window, so the frame of the round shows. |

The settings come after `init`, so the window opens at the first run of `window`.
If the window does not open, `window` tries again at its next run.
When the window is asked to close, `window` sets `closed` and sets `gl_proc` to 0 in that round.
The full names of the systems for `[order]` are `sdl3:video:window` and `sdl3:video:present`.

## sdl3:WindowSettings

`sdl3:WindowSettings` is the singleton of the settings. Bridge writes it from the deployment, or from the defaults of the manifest.

| Field | C type | Value |
| --- | --- | --- |
| `title` | `char[64]` | The title of the window. The default is `Bridge`. |
| `width`, `height` | `uint32_t` | The size of the window, in the units of the screen. The defaults are 1280 and 720. |
| `hidden` | `bool` | `true`: the window does not show. The default is `false`. |
| `vsync` | `int32_t` | The swap interval: 0 does not wait, 1 waits for the screen, and -1 waits unless the frame is late. The default is 1. |
| `resizable` | `bool` | `true`: the user can change the size of the window. The default is `false`. |
| `fullscreen` | `bool` | `true`: the window covers the screen that it opens on. The default is `false`. |
| `place` | `sdl3:Place` | Where the window opens. The default is `system`. |
| `x`, `y` | `int32_t` | The place of the window for `at`, in the units of the screen. The defaults are 0 and 0. |
| `gl_major`, `gl_minor` | `uint32_t` | The version of GL of the context. The defaults are 4 and 1. |
| `core` | `bool` | `true`: the core profile. macOS gives GL 4.1 only in the core profile. The default is `true`. |
| `depth`, `stencil` | `uint32_t` | The bits of depth and of stencil of each pixel. The defaults are 24 and 8. |
| `samples` | `uint32_t` | The samples of each pixel. 0 is no multisampling. The default is 0. |
| `high_dpi` | `bool` | `true`: on a screen of high density, the frame gets each pixel of the screen. The default is `false`. |

## sdl3:Place

`sdl3:Place` is an enum of `uint32_t`.

| Name | Value | Where the window opens |
| --- | --- | --- |
| `system` | 1 | Where the system puts it. |
| `center` | 2 | At the center of the screen. |
| `at` | 3 | At `x` and `y`. |

## sdl3:Window

`sdl3:Window` is a singleton that only the module writes.

| Field | C type | Value |
| --- | --- | --- |
| `gl_proc` | `uint64_t` | The address of `SDL_GL_GetProcAddress`, which gives each GL function of the context. It is 0 before the window opens, and from the round of the close. |
| `width`, `height` | `uint32_t` | The size of the frame, in pixels. |
| `samples` | `uint32_t` | The samples of each pixel that the context got. |
| `closed` | `bool` | `true` from the round in which the window is asked to close. |

A renderer stops its GL calls in the round when `gl_proc` becomes 0, because the context goes after that round.

## The deployment

- The module needs `sdl3:events`, which pumps the events of the window.
- `phases` lists `events`, then `input`, then the phases of the app and of the renderer, then `present`.
- `[module."sdl3:video@1.0.0".phases]` can map these phases to other names.
- `live = true`, because a round with no change and no record ends a run that is not live.
- A module that reads the window needs `read = ["sdl3:Window"]` in its table.
- A deployment gives the settings in `[module."sdl3:video@1.0.0".config]`.
- A deployment holds one SDL3 library, beside the SDL3 modules. A second copy is a second SDL3, which gets no event of the first.
- The SDL3 library is no module, so `content.ignore` names it.
- The backend of BB-Native has no capability of a limit, so `work_limit`, `memory_limit` and the three timeouts of the module are `"none"`.

The readme of Bridge-Host tells each key of a deployment.

## Messages

- `no video: <reason>`: SDL3 cannot start its video. `<reason>` is the text of `SDL_GetError`. Then `init` gives 1, and the module does not load.
- `no window: <reason>`: the window or its context does not open. `<reason>` is the text of `SDL_GetError`. The system then gives 1.
- `no swap interval <vsync>: <reason>`: the driver refuses the swap interval. The swaps keep the interval of the driver. The module writes it at the level 4.
