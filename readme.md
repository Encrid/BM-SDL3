# BM-SDL3

Builds SDL3 3.4.18 from source, and gives its event queue, keyboard, mouse and one window through 4 native modules.

## Install

Python 3.11 or a later version.

## Usage

| Command | Result |
| --- | --- |
| `python project.py fetch` | Downloads what the build needs. |
| `python project.py build` | Builds the project. |
| `python project.py pack` | Packs the build of each part for a release. |
| `python project.py test` | Runs the tests. |
| `python project.py test --break` | Runs the tests with their claims made false. Each test must fail. |
| `python project.py test --branches` | Runs the tests on a build with probes. Each branch of the code must run. |
| `python project.py test --leaks` | Runs the tests on a build with counts. Each test must give back what the code takes. |
| `python project.py check` | Checks the project. |
| `python project.py clean` | Removes what the build made. Keeps what `fetch` downloaded. |
| `python project.py distclean` | Removes the folder `build`, also what `fetch` downloaded. |

With `--part <part>`, `build`, `pack` and `test` act on that part and on the parts that it uses: `.toolchain/docs/project.py.md`, *Parts*.

| Part | What it does | Release |
| --- | --- | --- |
| `sdl3` | Builds the SDL3 library, and releases it with the headers of SDL3 and `leaks.toml`. | `BL-SDL3.zip`, tag `sdl3-v<version>` |
| `events` | Pumps the event queue of SDL3, stops the run at a request to quit, and gives the dropped files. | `BM-SDL3-Events.zip`, tag `events-v<version>` |
| `keyboard` | Gives the keys, the modifiers and the text input. | `BM-SDL3-Keyboard.zip`, tag `keyboard-v<version>` |
| `mouse` | Gives the position, the buttons and the wheel, and sets the cursor. | `BM-SDL3-Mouse.zip`, tag `mouse-v<version>` |
| `video` | Opens one window, and gives its native handles for a WebGPU surface. | `BM-SDL3-Video.zip`, tag `video-v<version>` |
| `common` | Starts and stops a subsystem of SDL3 for each module. | No release: each module links it. |

A release holds its zip, with a folder for each target, and `sha256.txt`. A version is `<major>.<minor>.<patch>`: `docs/contributing/pull_request.md`, *The release*.
For each target, the zip of a module holds the module, its header in `include`, and `license`.

### SDL3

For each target, the zip of `sdl3` holds the library, its headers in `include/SDL3`, the notices, and `leaks.toml` for `[leaks] with` of a module.
The library is `SDL3.dll`, `SDL3.so` with that soname, or `SDL3.dylib` with the install name `@rpath/SDL3.dylib`. A module links it with no import library.

The library keeps GL, Vulkan and the GPU API. It loads the GL or Vulkan library of the system only for a window or a call that asks for it.
On macOS, a window that names no graphics API and no external context asks for GL.
`SDL_Metal_CreateView` gives a view with a CAMetalLayer on macOS, and no view on the other systems.

The modules share this one library, and so one queue of events and one state. Thus:

- Only `sdl3:events` calls `SDL_PumpEvents`, on the main thread, at the start of each round. No module calls `SDL_PollEvent` or `SDL_WaitEvent`.
- Each other module takes only its own types of event, with `SDL_PeepEvents`, or with `SDL_FlushEvents` when it reads no field of them.
- A module calls `SDL_Quit` only after the last subsystem stops, because `SDL_Quit` stops the subsystems of each module.

### The modules

Each module runs on Bridge-Host with BB-Native. `docs/extern/<part>` tells its manifest, its systems, its types and its messages.

- A key of a deployment replaces the default of the manifest: https://github.com/Encrid/Bridge-Host/blob/v0.2.0/readme.md#the-modules.
- `spans` holds the spans of the systems, and a cell for each type in each table: https://github.com/Encrid/Bridge/blob/v0.2.0/docs/extern/bridge.h.md#the-private-memory.
- The settings are a singleton that Bridge writes after `init`. A system with a grant to write it can change it at each round.
- The header makes each C type and its descriptor from one text: https://github.com/Encrid/Bridge/blob/v0.2.0/docs/extern/bridge_schema.h.md#the-macros.
- Each module that uses a type declares it in `init`, with the same descriptor, kind, storage and `on_destroy`: https://github.com/Encrid/Bridge/blob/v0.2.0/docs/extern/bridge_abi.h.md#declare.
- Each type states `BRIDGE_STRIP`, so an unload destroys no entity. `reload` gives 1, because the manifest states `reloadable=false`.
- A deployment holds one SDL3 library beside the modules, and names it in `content.ignore`. BB-Native needs `"none"` for each limit and timeout.
- `sdl3:keyboard` and `sdl3:mouse` need `sdl3:events`, and a module that opens a window, for example `sdl3:video`.
- When SDL3 cannot start, `init` writes `no events: <reason>` or `no video: <reason>` at the level 3, and gives 1. `<reason>` is the text of `SDL_GetError`.

## Contributing

| Document | Content |
| --- | --- |
| `docs/contributing/needs.md` | The needs that each decision meets. |
| `docs/contributing/decisions.md` | The steps of a decision. |
| `docs/contributing/coding_style.md` | How to write the code. |
| `docs/contributing/documentation_style.md` | How to write the documents. |
| `docs/contributing/tests.md` | How to write the tests. |
| `docs/contributing/commits.md` | How to write a commit. |
| `docs/contributing/pull_request.md` | How to write a pull request. |
| `docs/contributing/agreement.md` | The agreement `cla.md`, which each contributor signs. |

## License

The GNU Affero General Public License, version 3 only. The file `license` holds the text.
SDL3 is under the zlib licence: `third_party/sdl3/license`. Its Linux build holds code of SDL_linux_deps: `third_party/sdl3_linux/license`.
