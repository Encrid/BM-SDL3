# include/sdl3_events.h

`include/sdl3_events.h` is the header of the module `sdl3:events@1.0.0`, the event queue of SDL3 for Bridge.
The module covers `SDL_events.h` of SDL3. It pumps the queue for the other SDL3 modules, ends the run on a quit, and gives the dropped files.
Each other SDL3 module needs it, because only this module pumps the queue.

## The module

| Word of the manifest | Value |
| --- | --- |
| The identity | `sdl3:events@1.0.0` |
| `abi` | `1` |
| `config` | `none` |
| `pinned` | `true`, because SDL3 pumps its queue only on the main thread. |
| `reloadable` | `false` |
| `spawns` | `16`, the default: one entity for each dropped file of a round. |
| `changes` | `32768`, the default: the room of the drops of a round. |

The file of the module is `sdl3_events`, with the suffix of a library. It runs on the backend of BB-Native, and loads the SDL3 library of BM-SDL3, which sits beside it.
A deployment that states `spawns` or `changes` for the module replaces these defaults.

## The system pump

`pump` runs in the phase `events`, and does these steps:

1. It clears the events that no module took in the round before.
2. It destroys each `sdl3:Drop` of the round before.
3. It pumps the queue of SDL3.
4. It spawns one `sdl3:Drop` for each file that was dropped, at most 16 in a round.
5. If the queue holds `SDL_EVENT_QUIT`, it gives `BRIDGE_END`.

SDL3 sends `SDL_EVENT_QUIT` when the last window asks to close, and when the system asks the program to quit.
The full name of the system for `[order]` is `sdl3:events:pump`.

## sdl3:Drop

`sdl3:Drop` is a table, with one entity for each file that was dropped on a window in the round.

| Field | C type | Value |
| --- | --- | --- |
| `path` | `char[1024]` | The path of the file, in UTF-8. |

## The deployment

- `phases` lists `events` first, before the phases of the other SDL3 modules.
- A module that reads the drops needs `read = ["sdl3:Drop"]` in its table.
- The SDL3 library is no module, so `content.ignore` names it.
- The backend of BB-Native has no capability of a limit, so `work_limit`, `memory_limit` and the three timeouts of the module are `"none"`.

The readme of Bridge-Host tells each key of a deployment.
## Messages

- `a dropped path of <bytes> bytes is left out, and 1023 is the most`: a path does not fit in `path`. The module writes it at the level 4.
