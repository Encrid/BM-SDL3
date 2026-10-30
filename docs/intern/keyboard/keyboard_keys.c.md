# src/keyboard/keyboard_keys.c

This file holds the system `keys`. It turns the keyboard events of a round into `sdl3:Keyboard`.

## A round

`bridge_keys` does these steps:

1. If `sdl3:KeyboardSettings` or `sdl3:Keyboard` is gone, it gives 0 at once.
2. It calls `set_text` with `text`.
3. It clears `hit`, `text` and `text_len`. `key` and `mods` keep their values.
4. It takes each event from `SDL_EVENT_KEY_DOWN` to `SDL_EVENT_SCREEN_KEYBOARD_HIDDEN`, the whole range of the keyboard, and gives it to `apply_event`.

`bridge_native_get_single` gives null for `sdl3:KeyboardSettings` or `sdl3:Keyboard` after a module detaches it: https://github.com/Encrid/BB-Native/blob/v0.2.0/docs/extern/bridge_native_kept.h.md#functions.

`set_text` gets the windows from `SDL_GetWindows`.
With `text` at `true`, it calls `SDL_StartTextInput` on each window where `SDL_TextInputActive` gives `false`.
With `text` at `false`, it calls `SDL_StopTextInput` on each window where it gives `true`.
Thus no event of the keyboard stays in the queue after step 4.
The state comes from the events, and not from `SDL_GetKeyboardState`. Thus an event that a case pushes also counts.

## An event

`apply_event` keeps the key events and the text events, and leaves out the other events of the range:

```pseudo
PROCEDURE apply_event(k : Keyboard, e : Event, repeats : BOOLEAN)
   CASE OF e.type
      SDL_EVENT_KEY_DOWN : CALL apply_key(k, e.key, 1, repeats)
      SDL_EVENT_KEY_UP : CALL apply_key(k, e.key, 0, repeats)
      SDL_EVENT_TEXT_INPUT : CALL keyboard_append_text(k, e.text.text)
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

`repeats` is the setting `repeat`. `keyboard_append_text` of `src/keyboard/keyboard_text.c` adds a typed text: `docs/intern/keyboard/keyboard_text.c.md`.

## Functions

| Function | Result |
| --- | --- |
| `bridge_keys` | Writes `sdl3:Keyboard` for the round, or does nothing while a singleton is gone. Gives 0. |
| `set_text` | Starts or stops the text input on each window, as `text` tells. |
| `apply_event` | Gives a key event to `apply_key`, and a text event to `keyboard_append_text`. |
| `apply_key` | Writes the state of one key, its hit and the modifiers. |
