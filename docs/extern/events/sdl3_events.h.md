# include/sdl3_events.h

`include/sdl3_events.h` is the header of the module `sdl3:events@1.0.0`, the event queue of SDL3 for Bridge.
The module covers `SDL_events.h` of SDL3. It pumps the queue for the other SDL3 modules, ends the run on a quit, and gives the dropped files.
Each other SDL3 module needs it, because only this module pumps the queue.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:events@1.0.0` |
| `abi` | `1` |
| `config` | `sdl3:EventsSettings` |
| `settings` | `drops quit` |
| `pinned` | `true`, because SDL3 pumps its queue only on the main thread. |
| `reloadable` | `false` |

The manifest also gives a default for each of these keys of the module. A deployment that states one of them replaces its default.

| Key | Default | Reason |
| --- | --- | --- |
| `scratch` | `4096` | The declarations of `init`, and the value of one drop at a time. |
| `log_ring` | `32` | A line for each drop that is left out, and one line for the drops past `drops`. |
| `spans` | `1024` | A cell for each of the two types in each table, and the spans of `pump`. |
| `spawns` | `16` | One entity for each drop of a round. |
| `changes` | `17152` | 1072 bytes for each drop: the attach of its 1024 bytes and 24, and its destroy in the next round, 24. |
| `persistent` | `"none"` | The module keeps no data in the arena. |
| `hot_reload` | `false` | The module cannot reload. |

A deployment that raises `drops` also raises `spawns` to `drops`, and `changes` to 1072 bytes for each drop.
The header `bridge.h` of Bridge tells the room that `spans` needs in a world with many tables.

The file of the module is `sdl3_events`, with the suffix of a library. It runs on the backend of BB-Native, and loads the SDL3 library of BL-SDL3, which sits beside it.

## The system pump

`pump` runs in the phase `events`, and does these steps:

1. It clears the events that no module took in the round before.
2. It destroys each `sdl3:Drop` of the round before.
3. It pumps the queue of SDL3.
4. It spawns one `sdl3:Drop` for each file that was dropped, at most `drops` in a round.
5. If `quit` is `true` and the queue holds `SDL_EVENT_QUIT`, it gives `BRIDGE_END`.

SDL3 sends `SDL_EVENT_QUIT` when the last window asks to close, and when the system asks the program to quit.
The module takes only the drops of files. It leaves out a dropped text, and a drop with no path.
The full name of the system for `[order]` is `sdl3:events:pump`.

## sdl3:EventsSettings

`sdl3:EventsSettings` is the singleton of the settings. Bridge writes it from the deployment, or from the defaults of the manifest.

| Field | C type | Value |
| --- | --- | --- |
| `drops` | `uint32_t` | The most files that `pump` takes in one round. The default is 16. |
| `quit` | `bool` | `true`: `SDL_EVENT_QUIT` ends the run. The default is `true`. |

## sdl3:Drop

`sdl3:Drop` is a table, with one entity for each file that was dropped on a window in the round.

| Field | C type | Value |
| --- | --- | --- |
| `path` | `char[1024]` | The path of the file, in UTF-8. |

## The deployment

- `phases` lists `events` first, before the phases of the other SDL3 modules.
- A module that reads the drops needs `read = ["sdl3:Drop"]` in its table.
- A deployment gives the settings in `[module."sdl3:events@1.0.0".config]`.
- A deployment holds one SDL3 library, beside the SDL3 modules. A second copy is a second SDL3, which gets no event of the first.
- The SDL3 library is no module, so `content.ignore` names it.
- The backend of BB-Native has no capability of a limit, so `work_limit`, `memory_limit` and the three timeouts of the module are `"none"`.

The readme of Bridge-Host tells each key of a deployment.

## Messages

- `a dropped path of <bytes> bytes is left out, and 1023 is the most`: a path does not fit in `path`. The module writes it at the level 4.
- `<count> dropped files are left out, and <drops> is the most in a round`: more files came than `drops`. The module writes it at the level 4.
- `no events: <reason>`: SDL3 cannot start its events. `<reason>` is the text of `SDL_GetError`. Then `init` gives 1, and the module does not load.
