# src/mouse_cursor.c

This file applies `sdl3:Cursor`, and keeps the cursor that the module made.

## The state

A native module gets only its API at each call, so the cursor that it made has no other place between two rounds.
The file keeps it in one static struct: the cursor of SDL3, and the shape that it shows.
A `[[style.exception]]` of the project excuses this struct from the rule G4, with that reason.

## The cursor

`mouse_apply_cursor` applies the three fields of `sdl3:Cursor` at each round:

```pseudo
PROCEDURE mouse_apply_cursor(c : Cursor)
   IF c.shape < SDL_SYSTEM_CURSOR_COUNT AND (NOT kept.made OR c.shape <> kept.shape) THEN
      CALL change_shape(c.shape)
   ENDIF
   IF c.hidden THEN
      CALL SDL_HideCursor()
   ELSE
      CALL SDL_ShowCursor()
   ENDIF
   CALL set_relative(c.relative)
ENDPROCEDURE
```

`change_shape` makes the system cursor of the shape, shows it, and then destroys the cursor that the module made before.
If SDL3 cannot make the cursor, the cursor stays as it is, and the next round tries again.
A `shape` of `SDL_SYSTEM_CURSOR_COUNT` or more changes nothing.
When SDL3 destroys the cursor that it shows, it shows its default cursor again. So `mouse_free_cursor` needs no other step.

`set_relative` gets the windows from `SDL_GetWindows`, so a window that opens later gets the mode at the next round.

## Functions

| Function | Result |
| --- | --- |
| `mouse_apply_cursor` | Shows the shape of `sdl3:Cursor`, hides or shows the cursor, and sets the relative mode. |
| `change_shape` | Shows a new system cursor, and destroys the cursor before it. Keeps the old cursor when SDL3 refuses. |
| `set_relative` | Sets the relative mode of each window. |
| `mouse_free_cursor` | Destroys the cursor that the module made, if there is one. |
