# src/mouse_events.c

This file holds the system `mouse`. It applies `sdl3:Cursor`, and turns the mouse events of a round into `sdl3:Mouse`.

## A round

`bridge_mouse` does these steps:

1. If `sdl3:MouseSettings` or `sdl3:Mouse` is gone, it gives 0 and does nothing more.
2. If `sdl3:Cursor` is there, it gives it to `mouse_apply_cursor`.
3. It clears the motion, the wheel, `pressed` and `released`. The position and `held` keep their values.
4. It takes each event from `SDL_EVENT_MOUSE_MOTION` to `SDL_EVENT_MOUSE_REMOVED`, and gives it to `apply_event`.

`bridge_native_get_single` gives null for a singleton that a module detached, so step 1 finds it: `docs/extern/sdl3_mouse.h.md`, *The system mouse*.
It also gives null for `sdl3:Cursor` before the app attaches it. So step 2 changes no cursor while `sdl3:Cursor` is gone.
Step 4 takes the whole range of the mouse, so no event of the mouse stays in the queue.

## An event

```pseudo
PROCEDURE apply_event(m : Mouse, e : Event, natural : BOOLEAN)
   CASE OF e.type
      SDL_EVENT_MOUSE_MOTION : CALL apply_motion(m, e.motion)
      SDL_EVENT_MOUSE_BUTTON_DOWN : CALL apply_button(m, e.button.button, TRUE)
      SDL_EVENT_MOUSE_BUTTON_UP : CALL apply_button(m, e.button.button, FALSE)
      SDL_EVENT_MOUSE_WHEEL : CALL apply_wheel(m, e.wheel, natural)
   ENDCASE
ENDPROCEDURE

PROCEDURE apply_motion(m : Mouse, e : MotionEvent)
   d <- SDL_GetWindowPixelDensity(SDL_GetWindowFromID(e.windowID))
   IF d = 0 THEN
      d <- 1
   ENDIF
   m.x <- e.x * d
   m.y <- e.y * d
   m.dx <- m.dx + e.xrel * d
   m.dy <- m.dy + e.yrel * d
ENDPROCEDURE

PROCEDURE apply_wheel(m : Mouse, e : WheelEvent, natural : BOOLEAN)
   sign <- 1
   IF e.direction = SDL_MOUSEWHEEL_FLIPPED AND NOT natural THEN
      sign <- -1
   ENDIF
   m.wheel_x <- m.wheel_x + e.x * sign
   m.wheel_y <- m.wheel_y + e.y * sign
ENDPROCEDURE
```

SDL3 gives the position and the motion in the units of the window. `SDL_GetWindowPixelDensity` gives the pixels in one unit, so each value comes in pixels of the frame.
`apply_motion` uses 1 when SDL3 gives 0, for example for a window that is gone.
`apply_button` keeps a button from 1 to 32 as its bit of `SDL_BUTTON_MASK`, and leaves out each other button.
A button that goes down sets its bit in `held` and in `pressed`. A button that goes up clears its bit in `held`, and sets it in `released`.

## Functions

| Function | Result |
| --- | --- |
| `bridge_mouse` | Applies the cursor, then writes `sdl3:Mouse` for the round. Does nothing while `sdl3:MouseSettings` or `sdl3:Mouse` is gone. Gives 0. |
| `apply_event` | Gives each event of the mouse to the function of its type. |
| `apply_motion` | Writes the position, and adds the motion, in pixels of the frame. |
| `apply_button` | Writes the bit of a button in `held`, `pressed` or `released`. |
| `apply_wheel` | Adds the turn of the wheel, in the direction that `natural` tells. |
