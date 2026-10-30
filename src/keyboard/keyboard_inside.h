/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef KEYBOARD_INSIDE_H
    #define KEYBOARD_INSIDE_H

    #include <SDL3/SDL.h>

    #include "bridge_native.h"
    #include "sdl3_keyboard.h"

constexpr uint32_t SETTINGS = 0;
constexpr uint32_t KEYBOARD = 1;

void keyboard_append_text(sdl3_keyboard_t *k, const char *typed);

#endif
