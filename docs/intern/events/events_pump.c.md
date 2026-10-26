# src/events_pump.c

This file holds the system `pump`. It is the only code of the SDL3 modules that pumps the queue of SDL3.

## A round

`bridge_pump` follows the steps of `docs/extern/sdl3_events.h.md`, *The system pump*.
Step 1 is `SDL_FlushEvents` from `SDL_EVENT_FIRST` to `SDL_EVENT_LAST`, so the queue holds only the events of this round.
Step 2 is `destroy_drops`, which reads the entity of each row in the spans of `sdl3:Drop`, and destroys it.
Step 5 uses `SDL_HasEvent`, so the event of the quit stays in the queue for the other modules of the round.

## The drops

`take_drops` takes at most `drops` drops of files, one at a time, and counts the drops that stay:

```pseudo
PROCEDURE take_drops(most : U32)
   taken <- 0
   WHILE taken < most AND SDL_PeepEvents(e, 1, SDL_GETEVENT, SDL_EVENT_DROP_FILE, SDL_EVENT_DROP_FILE) = 1
      CALL keep_drop(e.drop.data)
      taken <- taken + 1
   ENDWHILE
   rest <- count_drops()
   IF rest > 0 THEN
      CALL say_left(rest, most)
   ENDIF
ENDPROCEDURE
```

`count_drops` calls `SDL_PeepEvents` with `SDL_PEEKEVENT` and no buffer for the events. SDL3 then gives the count of the events in the range, and takes none.

`keep_drop` leaves out a drop with no path. A path of 1024 bytes or more does not fit, so `keep_drop` writes the message of the long path.
Else it spawns an entity, and attaches a `sdl3:Drop` that holds a copy of the path.
SDL3 keeps the text of an event until the next pump, so the copy happens in the same round.
`say_left` writes the message of the drops that are left out. Both messages go through the helper `bridge_native_say` of BB-Native.

## Functions

| Function | Result |
| --- | --- |
| `bridge_pump` | Pumps the queue, makes the drops of the round, and gives `BRIDGE_END` on a quit. Else gives 0. |
| `destroy_drops` | Destroys each `sdl3:Drop` of the round before. |
| `take_drops` | Takes at most `most` drops of files, and counts the rest. |
| `count_drops` | Gives the count of the drops of files that stay in the queue. |
| `keep_drop` | Spawns and attaches a `sdl3:Drop` for a path that fits. Leaves out the other drops. |
| `say_left` | Writes how many drops are left out in the round. |
