# src/keyboard_text.c

This file holds the typed text of `sdl3:Keyboard`. It adds each typed text of a round, and cuts a text that does not fit.

## The text

`keyboard_append_text` adds a typed text after the text of the round, and keeps a zero after it.
`get_fitting` gives how many bytes of the typed text fit in the 63 bytes.
If a cut falls inside a character of UTF-8, it moves back to the start of that character:

```pseudo
FUNCTION get_fitting(used : U32, typed : STRING) RETURNS U32
   n <- LENGTH(typed)
   IF used + n <= 63 THEN
      RETURN n
   ENDIF
   n <- 63 - used
   WHILE n > 0 AND ASC(MID(typed, n + 1, 1)) DIV 64 = 2
      n <- n - 1
   ENDWHILE
   RETURN n
ENDFUNCTION

PROCEDURE keyboard_append_text(k : Keyboard, typed : STRING)
   n <- get_fitting(k.text_len, typed)
   k.text <- k.text & LEFT(typed, n)
   k.text_len <- k.text_len + n
   IF n < LENGTH(typed) THEN
      CALL SDL_FlushEvent(SDL_EVENT_TEXT_INPUT)
   ENDIF
ENDPROCEDURE
```

A byte from 128 to 191 continues a character, so the cut cannot fall before it.
At a cut, `keyboard_append_text` calls `SDL_FlushEvent` with `SDL_EVENT_TEXT_INPUT`. The flush takes the later texts of the round out of the queue, so `text` gets none of them.
A text that a different thread pushes after the flush, while `keys` runs, can still come into `text`.

## Functions

| Function | Result |
| --- | --- |
| `keyboard_append_text` | Adds to `text` the bytes of a typed text that fit. At a cut, it removes the later texts of the round from the queue. |
| `get_fitting` | Gives the bytes of a typed text that fit, at the end of a whole character. |
