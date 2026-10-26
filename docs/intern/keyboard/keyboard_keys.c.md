# src/keyboard_keys.c

This file holds the system `keys`. It turns the keyboard events of a round into `sdl3:Keyboard`.

## A round

`bridge_keys` does these steps:

1. If `text` is `true`, it calls `start_text`.
2. It clears `hit`, `text` and `text_len`. `key` and `mods` keep their values.
3. It takes each event from `SDL_EVENT_KEY_DOWN` to `SDL_EVENT_SCREEN_KEYBOARD_HIDDEN`, and gives it to `apply_event`.

`start_text` gets the windows from `SDL_GetWindows`, and starts the text input on each window that does not have it.
Step 3 takes the whole range of the keyboard, so no event of the keyboard stays in the queue.
The state comes from the events, and not from `SDL_GetKeyboardState`. Thus an event that a case pushes also counts.

## An event

`apply_event` keeps the key events and the text events, and leaves out the other events of the range:

```pseudo
PROCEDURE apply_event(k : Keyboard, e : Event, repeats : BOOLEAN)
   CASE OF e.type
      SDL_EVENT_KEY_DOWN : CALL apply_key(k, e.key, 1, repeats)
      SDL_EVENT_KEY_UP : CALL apply_key(k, e.key, 0, repeats)
      SDL_EVENT_TEXT_INPUT : CALL append_text(k, e.text.text)
   ENDCASE
ENDPROCEDURE

PROCEDURE apply_key(k : Keyboard, key : KeyEvent, down : U8, repeats : BOOLEAN)
   IF key.scancode < 512 THEN
      k.mods <- key.mod
      k.key[key.scancode] <- down
      IF down = 1 AND (NOT key.repeat OR repeats) THEN
         k.hit[key.scancode] <- 1
      ENDIF
   ENDIF
ENDPROCEDURE
```

`repeats` is the setting `repeat`.

## The text

`append_text` adds a typed text after the text of the round, and keeps a zero after it.
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
After a cut, `append_text` leaves out each later text of the round.

## Functions

| Function | Result |
| --- | --- |
| `bridge_keys` | Writes `sdl3:Keyboard` for the round. Gives 0. |
| `start_text` | Starts the text input on each window that does not have it. |
| `apply_event` | Gives a key event to `apply_key`, and a text event to `append_text`. |
| `apply_key` | Writes the state of one key, its hit and the modifiers. |
| `append_text` | Adds a typed text to `text`, or leaves it out after a cut in the round. |
| `fitting` | Gives the bytes of a typed text that fit, at the end of a whole character. |
