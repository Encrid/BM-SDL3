# include/sdl3_video.h

`include/sdl3_video.h` is the header of the module `sdl3:video@1.0.0`, the window of SDL3 for Bridge.
The module covers `SDL_video.h` of SDL3 for one window and its GL 4.1 core context.
It does not cover more windows, the displays, Vulkan or Metal.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:video@1.0.0` |
| `abi` | `1` |
| `config` | `sdl3:WindowSettings` |
| `settings` | `title width height hidden vsync` |
| `pinned` | `true`, because SDL3 runs a window only on the main thread. |
| `reloadable` | `false`, because a reload would close the window. |

The file of the module is `sdl3_video`, with the suffix of a library. It runs on the backend of BB-Native, and loads the SDL3 library of BM-SDL3, which sits beside it.

## The systems

| System | Phase | What it does |
| --- | --- | --- |
| `window` | `input` | At its first run, it opens the window and its context with the settings. At each run, it takes the window events, and writes `sdl3:Window`. |
| `present` | `present` | It swaps the buffers of the window, so the frame of the round shows. |

The settings come after `init`, so the window opens at the first run of `window`.
When the window is asked to close, `window` sets `closed` and sets `gl_proc` to 0 in that round.
The full names of the systems for `[order]` are `sdl3:video:window` and `sdl3:video:present`.

## sdl3:WindowSettings

`sdl3:WindowSettings` is the singleton of the settings. Bridge writes it from the deployment, or from the defaults of the manifest.

| Field | C type | Value |
| --- | --- | --- |
| `title` | `char[64]` | The title of the window. The default is `Bridge`. |
| `width`, `height` | `uint32_t` | The size of the window, in the units of the screen. The defaults are 1280 and 720. |
| `hidden` | `bool` | `true`: the window does not show. The default is `false`. |
| `vsync` | `bool` | `true`: each swap waits for the screen. The default is `true`. |

## sdl3:Window

`sdl3:Window` is a singleton that only the module writes.

| Field | C type | Value |
| --- | --- | --- |
| `gl_proc` | `uint64_t` | The address of `SDL_GL_GetProcAddress`, which gives each GL function of the context. It is 0 before the window opens, and from the round of the close. |
| `width`, `height` | `uint32_t` | The size of the frame, in pixels. |
| `closed` | `bool` | `true` from the round in which the window is asked to close. |

A renderer stops its GL calls in the round when `gl_proc` becomes 0, because the context goes after that round.

## The deployment

- The module needs `sdl3:events`, which pumps the events of the window.
- `phases` lists `events`, then `input`, then the phases of the app and of the renderer, then `present`.
- `[module."sdl3:video@1.0.0".phases]` can map these phases to other names.
- `live = true`, because a round with no change and no record ends a run that is not live.
- A module that reads the window needs `read = ["sdl3:Window"]` in its table.
- A deployment gives the settings in `[module."sdl3:video@1.0.0".config]`.
- The SDL3 library is no module, so `content.ignore` names it.
- The backend of BB-Native keeps no limit, so the six limits of the module are `"none"`.

The readme of Bridge-Host tells each key of a deployment.
## Messages

- `no window: <reason>`: the window or its context does not open. `<reason>` is the text of `SDL_GetError`. The system then gives 1.
