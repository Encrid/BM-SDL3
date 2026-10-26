# src/mouse_events.c

This file holds the system `mouse`. It applies `sdl3:Cursor`, and turns the mouse events of a round into `sdl3:Mouse`.

## A round

`bridge_mouse` does these steps:

1. If the app attached `sdl3:Cursor`, it gives it to `mouse_apply_cursor`.
2. It clears the motion, the wheel, `pressed` and `released`. The position and `held` keep their values.
3. It takes each event from `SDL_EVENT_MOUSE_MOTION` to `SDL_EVENT_MOUSE_REMOVED`, and gives it to `apply_event`.

Step 3 takes the whole range of the mouse, so no event of the mouse stays in the queue.

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
   d <- get_density(e.windowID)
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

SDL3 gives the position and the motion in the units of the window. `get_density` gives the pixels in one unit, so each value comes in pixels of the frame.
`get_density` gives 1 when SDL3 gives 0, for example for a window that is gone.
`apply_button` keeps a button from 1 to 32 as its bit of `SDL_BUTTON_MASK`, and leaves out each other button.
A button that goes down sets its bit in `held` and in `pressed`. A button that goes up clears its bit in `held`, and sets it in `released`.

## Functions

| Function | Result |
| --- | --- |
| `bridge_mouse` | Applies the cursor, then writes `sdl3:Mouse` for the round. Gives 0. |
| `apply_event` | Gives each event of the mouse to the function of its type. |
| `apply_motion` | Writes the position, and adds the motion, in pixels of the frame. |
| `apply_button` | Writes the bit of a button in `held`, `pressed` or `released`. |
| `apply_wheel` | Adds the turn of the wheel, in the direction that `natural` tells. |
| `get_density` | Gives the pixel density of the window of an event, or 1. |
