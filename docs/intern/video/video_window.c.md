# src/video_window.c

This file holds the systems `window` and `present`: the window, its context, its events and its swaps.

## A round

`bridge_window` opens the window once, then reads its events at each round:

```pseudo
FUNCTION bridge_window(s : WindowSettings, w : Window) RETURNS I32
   IF w.closed THEN
      RETURN 0
   ENDIF
   IF w.gl_proc = 0 THEN
      RETURN open_window(s, w)
   ENDIF
   CALL take_events(w)
   RETURN 0
ENDFUNCTION

PROCEDURE take_events(w : Window)
   IF SDL_HasEvent(SDL_EVENT_WINDOW_CLOSE_REQUESTED) THEN
      w.closed <- TRUE
      w.gl_proc <- 0
   ENDIF
   CALL SDL_FlushEvents(SDL_EVENT_WINDOW_FIRST, SDL_EVENT_WINDOW_LAST)
   CALL SDL_GetWindowSizeInPixels(SDL_GL_GetCurrentWindow(), w.width, w.height)
ENDPROCEDURE
```

A window that closed never opens again, because `closed` comes first.
`take_events` takes the whole range of the window, so no event of the window stays in the queue.

## The opening

`open_window` does these steps:

1. `set_attributes` gives the attributes of GL to SDL3, and gives the flags of the window.
2. It makes the window with `title`, `width`, `height` and the flags.
3. It moves the window when `place` is `center` or `at`.
4. It makes the context of GL, which becomes the current context of the thread.
5. It sets the swap interval of `vsync`, and writes `no swap interval` when the driver refuses it.
6. It writes `gl_proc`, the size of the frame in pixels, and `samples`.

If step 2 or step 4 fails, `open_window` destroys what it made, writes `no window` and the error of SDL3, and gives 1.

| Attribute or flag | Value |
| --- | --- |
| `SDL_GL_CONTEXT_MAJOR_VERSION`, `SDL_GL_CONTEXT_MINOR_VERSION` | `gl_major` and `gl_minor`. |
| `SDL_GL_CONTEXT_PROFILE_MASK` | The core profile when `core` is `true`, else the compatibility profile. |
| `SDL_GL_CONTEXT_FLAGS` | Forward compatible when `core` is `true`, because macOS needs it for GL 4.1. |
| `SDL_GL_DEPTH_SIZE`, `SDL_GL_STENCIL_SIZE` | `depth` and `stencil`. |
| `SDL_GL_MULTISAMPLEBUFFERS`, `SDL_GL_MULTISAMPLESAMPLES` | 1 and `samples` when `samples` is more than 0, else 0 and 0. |
| `SDL_GL_DOUBLEBUFFER` | 1, because `present` swaps two buffers. |
| `SDL_WINDOW_OPENGL` | Always. |
| `SDL_WINDOW_HIDDEN`, `SDL_WINDOW_RESIZABLE`, `SDL_WINDOW_FULLSCREEN`, `SDL_WINDOW_HIGH_PIXEL_DENSITY` | When `hidden`, `resizable`, `fullscreen` and `high_dpi` are `true`. |

`samples` of `sdl3:Window` comes from `SDL_GL_GetAttribute`, because a driver can give fewer samples than it was asked.

## The swap

`bridge_present` swaps the buffers of the current window while `gl_proc` is not 0.
In the round of the close, it does not swap, because the renderer stopped its GL calls.

## Functions

| Function | Result |
| --- | --- |
| `bridge_window` | Opens the window at its first run, then writes `sdl3:Window` at each round. Gives 0, or 1 when the window does not open. |
| `take_events` | Reads the close and the size of the window, and takes the events of the window. |
| `open_window` | Makes the window and its context. Gives 0, or 1 when one of them fails. |
| `set_attributes` | Gives the attributes of GL to SDL3, and gives the flags of the window. |
| `bridge_present` | Swaps the buffers while the context is there. Gives 0. |
