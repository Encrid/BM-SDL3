/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "mouse_inside.h"

static void apply_motion(sdl3_mouse_t *m, const SDL_MouseMotionEvent *e)
{
    float d = SDL_GetWindowPixelDensity(SDL_GetWindowFromID(e->windowID));

    if (d == 0)
        d = 1;
    m->x = e->x * d;
    m->y = e->y * d;
    m->dx += e->xrel * d;
    m->dy += e->yrel * d;
}

static void apply_button(sdl3_mouse_t *m, uint8_t button, bool down)
{
    uint32_t bit = 0;

    if (button == 0 || button > 32)
        return;
    bit = SDL_BUTTON_MASK(button);
    if (down) {
        m->held |= bit;
        m->pressed |= bit;
    } else {
        m->held &= ~bit;
        m->released |= bit;
    }
}

static void apply_wheel(sdl3_mouse_t *m, const SDL_MouseWheelEvent *e,
    bool natural)
{
    float sign = 1;

    if (e->direction == SDL_MOUSEWHEEL_FLIPPED && !natural)
        sign = -1;
    m->wheel_x += e->x * sign;
    m->wheel_y += e->y * sign;
}

static void apply_event(sdl3_mouse_t *m, const SDL_Event *e, bool natural)
{
    switch (e->type) {
        case SDL_EVENT_MOUSE_MOTION:
            apply_motion(m, &e->motion);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            apply_button(m, e->button.button, true);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            apply_button(m, e->button.button, false);
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            apply_wheel(m, &e->wheel, natural);
            break;
    }
}

BRIDGE_NATIVE(mouse)
{
    const sdl3_mouse_settings_t *s = bridge_native_get_single(api, SETTINGS);
    const sdl3_cursor_t *c = bridge_native_get_single(api, CURSOR);
    sdl3_mouse_t *m = bridge_native_get_single(api, MOUSE);
    SDL_Event e = {};

    if (!s || !m)
        return (0);
    if (c)
        mouse_apply_cursor(c);
    *m = (sdl3_mouse_t){ .x = m->x, .y = m->y, .held = m->held };
    while (SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_EVENT_MOUSE_MOTION,
        SDL_EVENT_MOUSE_REMOVED) > 0)
        apply_event(m, &e, s->natural);
    return (0);
}
