# src/shared/shared_subsystem.c

This file starts and stops the subsystems of SDL3 for the modules.
The archive `shared.a` holds it, and each module links the archive. Thus each module holds its own copy, and builds, runs and releases alone.

## The start

`shared_start_video` and `shared_start_events` start the video or the events of SDL3.
If SDL3 refuses, they write `no video` or `no events` with the text of `SDL_GetError`, and give `false`: `readme.md`, *The modules*.
Then `bridge_init` of the module gives 1. Bridge calls no `shutdown` after an `init` that failed, so no subsystem stays started.

## The stop

`shared_stop_subsystem` stops the subsystem of its flags, and follows the rule of `readme.md`, *SDL3*:

```pseudo
PROCEDURE shared_stop_subsystem(flags : U32)
   CALL SDL_QuitSubSystem(flags)
   IF SDL_WasInit(0) = 0 THEN
      CALL SDL_Quit()
   ENDIF
ENDPROCEDURE
```

`SDL_WasInit(0)` gives the subsystems that a module still holds. Thus the last module that stops quits SDL3.

## The visibility

The archive compiles with `-fvisibility=hidden`. Thus the functions stay out of the exports of each module, and two modules never share a copy.

## Functions

| Function | Result |
| --- | --- |
| `shared_start_video` | Starts the video of SDL3. Gives `true`, or `false` after it writes `no video`. |
| `shared_start_events` | Starts the events of SDL3. Gives `true`, or `false` after it writes `no events`. |
| `shared_stop_subsystem` | Stops a subsystem, then SDL3 after the last subsystem. |
