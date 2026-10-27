# src/leaks.toml

This file names the resources of SDL3 that a module takes, for the count of `test --leaks` of each SDL3 module.
A release ships the file as `leaks.toml`, and each SDL3 module names it in `[leaks] with`.

## The pairs

| Takes | Gives back |
| --- | --- |
| `SDL_Metal_CreateView` | `SDL_Metal_DestroyView` |
| `SDL_GL_CreateContext` | `SDL_GL_DestroyContext` |
| `SDL_CreateSystemCursor` | `SDL_DestroyCursor` |
| `SDL_GetWindows` | `SDL_free` |

## The windows

The kind `sdl3_window` counts the windows. `SDL_CreateWindow` and `SDL_CreateWindowWithProperties` take one at their result.
`SDL_DestroyWindow` gives one back at its first argument.
A window is a kind, because one call gives back the window of either call that makes one.

## The subsystems

The kind `sdl3_subsystem` counts the subsystems of SDL3. `SDL_InitSubSystem` takes one at its result, which is true when the subsystem starts.
`SDL_QuitSubSystem` gives one back at its first argument, the flags of the subsystem, which are not 0.
Thus a module that starts its subsystem and does not stop it fails its count.
