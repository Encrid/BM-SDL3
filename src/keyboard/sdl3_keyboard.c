/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "keyboard_inside.h"

[[gnu::section(MANIFEST_SECTION), gnu::used, gnu::retain]]
static const char WHO[] = MANIFEST;

BRIDGE_NATIVE(init)
{
    const bridge_term_t terms[2] = {
        { SETTINGS, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { KEYBOARD, BRIDGE_WRITE, BRIDGE_OPTIONAL, 0 } };
    sdl3_keyboard_t zero = {};

    if (!shared_start_video(api))
        return (1);
    BRIDGE_NATIVE_COMPONENT(api, "sdl3:KeyboardSettings",
        sdl3_keyboard_settings_t, BRIDGE_STRIP, BRIDGE_SINGLETON,
        BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_COMPONENT(api, "sdl3:Keyboard", sdl3_keyboard_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_ATTACH(api, 0, KEYBOARD, zero);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "keys", "input", terms, 2, nullptr, 0,
        nullptr, 0);
    return (0);
}

BRIDGE_NATIVE(shutdown)
{
    shared_stop_subsystem(SDL_INIT_VIDEO);
    return (0);
}

BRIDGE_NATIVE(reload)
{
    return (1);
}
