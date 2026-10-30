/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "video_inside.h"

[[gnu::section(MANIFEST_SECTION), gnu::used, gnu::retain]]
static const char WHO[] = MANIFEST;

static void declare_types(const bridge_api_t *api)
{
    BRIDGE_NATIVE_DECLARE(api, BRIDGE_ENUM, "sdl3:sdl3_place_t", sdl3_place_t,
        BRIDGE_STRIP, BRIDGE_TABLE, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_DECLARE(api, BRIDGE_ENUM, "sdl3:sdl3_surface_t",
        sdl3_surface_t, BRIDGE_STRIP, BRIDGE_TABLE, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_COMPONENT(api, "sdl3:WindowSettings", sdl3_window_settings_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_COMPONENT(api, "sdl3:Window", sdl3_window_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
}

BRIDGE_NATIVE(init)
{
    const bridge_term_t terms[2] = {
        { SETTINGS, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { WINDOW, BRIDGE_WRITE, BRIDGE_OPTIONAL, 0 } };
    sdl3_window_t none = { .surface = SDL3_SURFACE_NONE };

    if (!common_start_video(api))
        return (1);
    declare_types(api);
    BRIDGE_NATIVE_ATTACH(api, 0, WINDOW, none);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "window", "input", terms, 2, nullptr, 0,
        nullptr, 0);
    return (0);
}

BRIDGE_NATIVE(shutdown)
{
    video_close_window();
    common_stop_subsystem(SDL_INIT_VIDEO);
    return (0);
}

BRIDGE_NATIVE(reload)
{
    return (1);
}
