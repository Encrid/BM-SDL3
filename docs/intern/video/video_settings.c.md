# src/video/video_settings.c

This file asks SDL3 for each setting that changed since the last run of `window`: `docs/extern/video/sdl3_video.h.md`, *The system*.

## A run

`video_apply_settings` compares `now`, the settings of this run, with `last`, the settings of the last run:

```pseudo
PROCEDURE video_apply_settings(api : bridge_api_t, window : SDL_Window, last : sdl3_window_settings_t, now : sdl3_window_settings_t)
   IF now.title <> last.title THEN
      CALL report(api, SDL_SetWindowTitle(window, now.title), "title")
   ENDIF
   IF now.width <> last.width OR now.height <> last.height THEN
      CALL report(api, SDL_SetWindowSize(window, now.width, now.height), "size")
   ENDIF
   CALL apply_flags(api, window, now, last)
   at <- video_get_place(now)
   was <- video_get_place(last)
   IF at.x <> was.x OR at.y <> was.y THEN
      CALL report(api, SDL_SetWindowPosition(window, at.x, at.y), "place")
   ENDIF
   IF now.high_dpi <> last.high_dpi THEN
      CALL bridge_native_say(api, BRIDGE_LOG_WARN, "no change of high_dpi: it acts only when the window opens")
   ENDIF
   last <- now
ENDPROCEDURE

PROCEDURE report(api : bridge_api_t, done : BOOLEAN, setting : STRING)
   IF NOT done THEN
      CALL bridge_native_say(api, BRIDGE_LOG_WARN, "no change of %s: %s", setting, SDL_GetError())
   ENDIF
ENDPROCEDURE
```

`last` takes `now` also after a change that SDL3 refused. Thus the module asks for each change one time, and writes its line one time.
The size comes before the place, because SDL3 centers the window with the size that the run asks for.
The module never calls `SDL_SyncWindow`, because a round does not wait for the windowing system.
The limits of the sides are in `docs/extern/video/sdl3_video.h.md`, *sdl3:WindowSettings*.

## The requests

| Setting | Request of SDL3 |
| --- | --- |
| `title` | `SDL_SetWindowTitle` |
| `width`, `height` | `SDL_SetWindowSize` |
| `hidden` | `SDL_HideWindow` or `SDL_ShowWindow` |
| `resizable` | `SDL_SetWindowResizable` |
| `fullscreen` | `SDL_SetWindowFullscreen` |
| `place`, `x`, `y` | `SDL_SetWindowPosition` |
| `high_dpi` | None |

`report` writes one line for a refused request, with the name of the request: `docs/extern/video/sdl3_video.h.md`, *Messages*.
`apply_flags` asks for `hidden`, `resizable` and `fullscreen`, in that order. `set_hidden` selects the request of `hidden`.

## The place

`video_get_place` gives the point that SDL3 gets for the place of the settings:

| `place` | The point |
| --- | --- |
| `SDL3_PLACE_CENTER` | `SDL_WINDOWPOS_CENTERED` for x and for y. |
| `SDL3_PLACE_AT` | `x` and `y`. |
| Each other value | `SDL_WINDOWPOS_UNDEFINED` for x and for y. |

`create_window` of `src/video/video_window.c` gives the same point to SDL3 when the window opens.
Only the point of `SDL3_PLACE_AT` holds `x` and `y`, so a change of them with another place gives the same point: `docs/extern/video/sdl3_video.h.md`, *sdl3:WindowSettings*.
`SDL3_PLACE_SYSTEM` and each value that is no constant take the last row: `docs/extern/video/sdl3_video.h.md`, *sdl3:sdl3_place_t*.
After the open, `SDL_SetWindowPosition` replaces `SDL_WINDOWPOS_UNDEFINED` with the place that the window has.
The source of SDL3 shows it: https://github.com/libsdl-org/SDL/blob/release-3.4.18/src/video/SDL_video.c.

## Functions

| Function | Result |
| --- | --- |
| `video_apply_settings` | Asks SDL3 for each setting that changed, then copies `now` into `last`. |
| `apply_flags` | Asks SDL3 for `hidden`, `resizable` and `fullscreen`, if they changed. |
| `set_hidden` | Hides or shows the window, and gives the result of SDL3. |
| `report` | Writes the line of a request that SDL3 refused. |
| `video_get_place` | Gives the point of SDL3 for the place of the settings. |
