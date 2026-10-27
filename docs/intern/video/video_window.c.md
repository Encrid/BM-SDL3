# src/video_window.c

This file holds the system `window`. It opens the window, uses `src/video_settings.c` and `src/video_state.c` at each run, and destroys the window after its close.

## The state

A native module gets only its API at each call, so the window has no other place between two rounds.
The file keeps it in one static `video_kept_t`, `kept`: the window, its Metal view, and the settings of the last run.
A `[[style.exception]]` of the project excuses this struct from the rule G4, with that reason.

## A round

`bridge_window` runs one time in each round, in the phase `input`:

```pseudo
FUNCTION bridge_window() RETURNS I32
   s <- bridge_native_get_single(api, SETTINGS)
   w <- bridge_native_get_single(api, WINDOW)
   IF s = nullptr OR w = nullptr THEN
      RETURN 0
   ENDIF
   IF w.closed THEN
      CALL video_close_window()
      RETURN 0
   ENDIF
   now <- s
   now.title[63] <- 0
   IF kept.window = nullptr THEN
      IF open_window(api, now, w) <> 0 THEN
         RETURN 1
      ENDIF
   ENDIF
   CALL video_apply_settings(api, kept.window, kept.last, now)
   CALL video_take_events(kept.window, w)
   RETURN 0
ENDFUNCTION
```

`bridge_native_get_single` gives null for a singleton that a module detached, so the first `IF` gives 0: `docs/extern/sdl3_video.h.md`, *The system*.
A window that closed never opens again, because `closed` comes first.
`now` is a copy of the settings with a zero in the last byte of `title`. Thus SDL3 never reads past the title.
In the run of the open, `video_apply_settings` finds no change, because `open_window` copies `now` into `kept.last`.

## The open

`open_window` does these steps:

1. `video_find_driver` gives the row of the video driver: `docs/intern/video_state.c.md`, *The driver*.
2. It copies `now` into `kept.last`.
3. If the driver has a row, `create_window` makes the window.
4. If the window is there, it makes the Metal view with `SDL_Metal_CreateView`.
5. If the window is there, `video_write_handles` writes the surface and the handles into a new value.
6. If the new value holds a handle, it writes the value into `sdl3:Window`, and gives 0.
7. Else, it calls `video_close_window`, writes `no window` and the error of SDL3, and gives 1.

`SDL_Metal_CreateView` makes a view only on the driver `cocoa`. On each other driver, it gives null and does not change the window.
The source of SDL3 shows it: https://github.com/libsdl-org/SDL/blob/release-3.4.18/src/video/SDL_video.c.
Thus no step asks for the platform, and no condition of the file depends on the platform.

A driver with no row, a window that fails, and a view that fails each leave the new value with no handle.
So step 6 finds each of the three failures. A window that does not open leaves no handle in `sdl3:Window`.
For a driver with no row, the reason is the error that `video_find_driver` sets.
For a view that fails, the reason is the error of `SDL_Metal_GetLayer`, which gets no view.

## The window of SDL3

`create_window` gives the settings to `SDL_CreateWindowWithProperties` as properties, so the window opens at its place with one request.
The properties are those of https://wiki.libsdl.org/SDL3/SDL_CreateWindowWithProperties.

| Property | Value |
| --- | --- |
| `SDL_PROP_WINDOW_CREATE_TITLE_STRING` | `title`. |
| `SDL_PROP_WINDOW_CREATE_X_NUMBER`, `SDL_PROP_WINDOW_CREATE_Y_NUMBER` | The point that `video_get_place` gives: `docs/intern/video_settings.c.md`, *The place*. |
| `SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER`, `SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER` | `width` and `height`. |
| `SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN`, `SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN` | `hidden` and `resizable`. |
| `SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN`, `SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN` | `fullscreen` and `high_dpi`. |
| `SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN` | `true` if the row of the driver gives `SDL3_SURFACE_METAL`. |

`set_flags` writes the five properties of the type `bool`.
On macOS, SDL3 loads OpenGL for a window that names no graphics API, in `SDL_DefaultGraphicsBackends` of `SDL_video.c`.
The property of Metal stops that load, because the module uses only the Metal view.
The value of that property is a comparison, and not a condition. Thus it adds no outcome that only one platform reaches.
The limits of the sides are in `docs/extern/sdl3_video.h.md`, *sdl3:WindowSettings*.

## The close

`video_close_window` destroys the Metal view, then the window, as https://wiki.libsdl.org/SDL3/SDL_Metal_DestroyView tells. Then it clears both in `kept`.
If the module made no window, it does nothing. `SDL_Metal_DestroyView` does nothing with a null view, so one call serves each driver.
`open_window` calls it after a failure, `bridge_window` at each run after the close, and `bridge_shutdown` at the stop.

## Functions

| Function | Result |
| --- | --- |
| `bridge_window` | Opens the window at the first run. Then asks SDL3 for the changed settings, and writes `sdl3:Window`. Gives 0, or 1 when the window does not open. |
| `open_window` | Makes the window, its Metal view and its handles, or writes `no window`. Gives 0, or 1. |
| `create_window` | Makes the window of SDL3 from the settings. Gives the window, or null. |
| `set_flags` | Writes the five properties of the type `bool` of the window. |
| `video_close_window` | Destroys the Metal view and the window that the module made, and forgets them. |
