/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "mouse_inside.h"

[[gnu::section(MANIFEST_SECTION), gnu::used, gnu::retain]]
static const char WHO[] = MANIFEST;

BRIDGE_NATIVE(init)
{
    const bridge_term_t terms[3] = {
        { SETTINGS, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { CURSOR, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { MOUSE, BRIDGE_WRITE, BRIDGE_OPTIONAL, 0 } };
    sdl3_mouse_t zero = {};

    if (!shared_start_video(api))
        return (1);
    BRIDGE_NATIVE_COMPONENT(api, "sdl3:MouseSettings", sdl3_mouse_settings_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_COMPONENT(api, "sdl3:Cursor", sdl3_cursor_t, BRIDGE_STRIP,
        BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_COMPONENT(api, "sdl3:Mouse", sdl3_mouse_t, BRIDGE_STRIP,
        BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_ATTACH(api, 0, MOUSE, zero);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "mouse", "input", terms, 3, nullptr, 0,
        nullptr, 0);
    return (0);
}

BRIDGE_NATIVE(shutdown)
{
    mouse_free_cursor();
    shared_stop_subsystem(SDL_INIT_VIDEO);
    return (0);
}

BRIDGE_NATIVE(reload)
{
    return (1);
}
