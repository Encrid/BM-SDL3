# src/leaks.toml

This file names the resources of SDL3 that a module takes, for the count of `test --leaks` of each SDL3 module.
A release ships the file as `leaks.toml`, and each SDL3 module names it in `[leaks] with`.

## The pairs

| Takes | Gives back |
| --- | --- |
| `SDL_CreateWindow` | `SDL_DestroyWindow` |
| `SDL_GL_CreateContext` | `SDL_GL_DestroyContext` |
| `SDL_CreateSystemCursor` | `SDL_DestroyCursor` |
| `SDL_GetWindows` | `SDL_free` |

## The subsystems

A kind counts the subsystems of SDL3: `SDL_InitSubSystem` takes a subsystem by its flags, and `SDL_QuitSubSystem` gives it back.
Thus a module that starts its subsystem and does not stop it fails its count.
