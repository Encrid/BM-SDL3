/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "keyboard_inside.h"

constexpr uint32_t ROOM = 63;

static uint32_t get_fitting(uint32_t used, const char *typed)
{
    size_t n = strlen(typed);

    if (n <= ROOM - used)
        return ((uint32_t)n);
    n = ROOM - used;
    while (n > 0 && ((uint8_t)typed[n] >> 6) == 2)
        n--;
    return ((uint32_t)n);
}

void keyboard_append_text(sdl3_keyboard_t *k, const char *typed)
{
    uint32_t n = get_fitting(k->text_len, typed);

    memcpy(k->text + k->text_len, typed, n);
    k->text_len += n;
    k->text[k->text_len] = 0;
    if (n < strlen(typed))
        SDL_FlushEvent(SDL_EVENT_TEXT_INPUT);
}
