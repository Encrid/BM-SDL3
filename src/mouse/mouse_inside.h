/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef MOUSE_INSIDE_H
    #define MOUSE_INSIDE_H

    #include <SDL3/SDL.h>

    #include "bridge_native.h"
    #include "common_subsystem.h"
    #include "sdl3_mouse.h"

constexpr uint32_t SETTINGS = 0;
constexpr uint32_t CURSOR = 1;
constexpr uint32_t MOUSE = 2;

typedef struct {
    SDL_Cursor *made;
    uint32_t shape;
} mouse_kept_t;

void mouse_apply_cursor(const sdl3_cursor_t *c);
void mouse_free_cursor(void);

#endif
