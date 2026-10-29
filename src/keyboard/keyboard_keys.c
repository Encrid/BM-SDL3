/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "keyboard_inside.h"

static void set_text(bool text)
{
    int count = 0;
    SDL_Window **windows = SDL_GetWindows(&count);

    for (int i = 0; i < count; i++) {
        if (SDL_TextInputActive(windows[i]) == text)
            continue;
        if (text)
            SDL_StartTextInput(windows[i]);
        else
            SDL_StopTextInput(windows[i]);
    }
    SDL_free(windows);
}

static void apply_key(sdl3_keyboard_t *k, const SDL_KeyboardEvent *key,
    uint8_t down, bool repeats)
{
    if (key->scancode >= sizeof(k->key))
        return;
    k->mods = key->mod;
    k->key[key->scancode] = down;
    if (down == 1 && (!key->repeat || repeats))
        k->hit[key->scancode] = 1;
}

static void apply_event(sdl3_keyboard_t *k, const SDL_Event *e, bool repeats)
{
    switch (e->type) {
        case SDL_EVENT_KEY_DOWN:
            apply_key(k, &e->key, 1, repeats);
            break;
        case SDL_EVENT_KEY_UP:
            apply_key(k, &e->key, 0, repeats);
            break;
        case SDL_EVENT_TEXT_INPUT:
            keyboard_append_text(k, e->text.text);
            break;
    }
}

BRIDGE_NATIVE(keys)
{
    const sdl3_keyboard_settings_t *s = bridge_native_get_single(api,
        SETTINGS);
    sdl3_keyboard_t *k = bridge_native_get_single(api, KEYBOARD);
    SDL_Event e = {};

    if (!s || !k)
        return (0);
    set_text(s->text);
    memset(k->hit, 0, sizeof(k->hit));
    memset(k->text, 0, sizeof(k->text));
    k->text_len = 0;
    while (SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_EVENT_KEY_DOWN,
        SDL_EVENT_SCREEN_KEYBOARD_HIDDEN) > 0)
        apply_event(k, &e, s->repeat);
    return (0);
}
