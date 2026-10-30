# src/video_state.c

This file finds the surface of the video driver, and writes `sdl3:Window`.
It writes the handles at the open, then the close, the size and the flags at each run.

## The driver

`video_find_driver` finds the name that `SDL_GetCurrentVideoDriver` gives in this table, and gives its row:

| `name` | `surface` |
| --- | --- |
| `windows` | `SDL3_SURFACE_WINDOWS` |
| `cocoa` | `SDL3_SURFACE_METAL` |
| `x11` | `SDL3_SURFACE_X11` |
| `wayland` | `SDL3_SURFACE_WAYLAND` |

SDL3 gives these names to its drivers: https://wiki.libsdl.org/SDL3/SDL_GetCurrentVideoDriver.
For each other driver, it gives null, and gives the reason of a driver with no surface to `SDL_SetError`: `docs/extern/sdl3_video.h.md`, *Messages*.
Thus `open_window` reads that reason from `SDL_GetError`, as for each other failure of the open.
The module holds the video subsystem from `init` to `shutdown`. Thus `SDL_GetCurrentVideoDriver` always gives a name in a run of `window`.

## The handles

`video_write_handles` writes `surface` from the row. It reads each handle from SDL3 on each driver:

| Field | SDL3 gives it by |
| --- | --- |
| `hinstance` | `SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER` |
| `hwnd` | `SDL_PROP_WINDOW_WIN32_HWND_POINTER` |
| `layer` | `SDL_Metal_GetLayer` of the Metal view |
| `x11_display` | `SDL_PROP_WINDOW_X11_DISPLAY_POINTER` |
| `x11_window` | `SDL_PROP_WINDOW_X11_WINDOW_NUMBER`, a number and not a pointer |
| `wl_display` | `SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER` |
| `wl_surface` | `SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER` |

The properties are those of `SDL_GetWindowProperties`: https://wiki.libsdl.org/SDL3/SDL_GetWindowProperties.
A driver sets only its own properties, and `SDL_Metal_GetLayer` gives null with no view. Thus each handle of another kind stays 0, with no condition on the driver.

The module reads the handles one time, at the open. None of them is an object `xdg_*` of Wayland, which SDL3 makes again when the window shows.
`video_has_handle` tells if a value holds a handle that is not 0. With it, `open_window` finds a window that can give no surface.
It ORs the seven handles bit by bit, and compares the result with 0.
As for the property of Metal, a comparison is no condition: `docs/intern/video_window.c.md`, *The window of SDL3*.

## A run

`video_take_events` does these steps at each run of an open window:

1. If SDL3 has a request to close a window, it writes a new value into `sdl3:Window`.
2. It takes each event from `SDL_EVENT_WINDOW_FIRST` to `SDL_EVENT_WINDOW_LAST` with `SDL_FlushEvents`, and reads no field of these events.
3. If the window did not close, it writes the size of `SDL_GetWindowSizeInPixels`, and `fullscreen` and `hidden` of `SDL_GetWindowFlags`.

Step 1 writes the value of a close: `docs/extern/sdl3_video.h.md`, *The system*.
Step 2 takes the whole range of the window, so no event of the window stays in the queue.
`SDL_HasEvent` does not tell which window gets the request. The module covers one window, so it takes each request as a request for that window.

The size and the flags come from SDL3, and not from the settings. Thus `sdl3:Window` is the real state: `docs/extern/sdl3_video.h.md`, *The system*.
`SDL_GetWindowFlags` also gives the requests that wait: a hidden window that asks for the full screen has `SDL_WINDOW_FULLSCREEN`.
Thus `fullscreen` is one comparison of the flags: `SDL_WINDOW_FULLSCREEN` is set, and `SDL_WINDOW_HIDDEN` is clear.
Like `video_has_handle`, it is a comparison, and not a condition.
The source of SDL3 shows the requests that wait: https://github.com/libsdl-org/SDL/blob/release-3.4.18/src/video/SDL_video.c.

## Functions

| Function | Result |
| --- | --- |
| `video_find_driver` | Gives the row of the video driver, or null with the error of SDL3 set. |
| `video_write_handles` | Writes `surface` and the seven handles into a value of `sdl3:Window`. |
| `video_has_handle` | Tells if a value of `sdl3:Window` holds a handle. |
| `video_take_events` | Writes the close, or the size and the flags, and takes the events of the window. |
