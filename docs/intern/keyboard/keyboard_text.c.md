# src/keyboard_text.c

This file holds the typed text of `sdl3:Keyboard`. It adds each typed text of a round, and cuts a text that does not fit.

## The text

`keyboard_append_text` adds a typed text after the text of the round, and keeps a zero after it.
`fitting` gives how many bytes of the typed text fit in the 63 bytes.
If a cut falls inside a character of UTF-8, it moves back to the start of that character:

```pseudo
FUNCTION fitting(used : U32, typed : STRING) RETURNS U32
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
```

A byte from 128 to 191 continues a character, so the cut cannot fall before it.
After a cut, `keyboard_append_text` leaves out each later text of the round.

## Functions

| Function | Result |
| --- | --- |
| `keyboard_append_text` | Adds a typed text to `text`, or leaves it out after a cut in the round. |
| `fitting` | Gives the bytes of a typed text that fit, at the end of a whole character. |
