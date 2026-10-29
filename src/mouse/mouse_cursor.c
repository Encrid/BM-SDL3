/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "mouse_inside.h"

static mouse_kept_t kept;

static void change_shape(uint32_t shape)
{
    SDL_Cursor *made = SDL_CreateSystemCursor((SDL_SystemCursor)shape);

    if (!made)
        return;
    SDL_SetCursor(made);
    SDL_DestroyCursor(kept.made);
    kept.made = made;
    kept.shape = shape;
}

static void set_relative(bool relative)
{
    int count = 0;
    SDL_Window **windows = SDL_GetWindows(&count);

    for (int i = 0; i < count; i++)
        SDL_SetWindowRelativeMouseMode(windows[i], relative);
    SDL_free(windows);
}

void mouse_apply_cursor(const sdl3_cursor_t *c)
{
    if (c->shape < SDL_SYSTEM_CURSOR_COUNT
        && (!kept.made || c->shape != kept.shape))
        change_shape(c->shape);
    if (c->hidden)
        SDL_HideCursor();
    else
        SDL_ShowCursor();
    set_relative(c->relative);
}

void mouse_free_cursor(void)
{
    SDL_DestroyCursor(kept.made);
    kept.made = nullptr;
}
