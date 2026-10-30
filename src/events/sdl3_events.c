/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "events_inside.h"

[[gnu::section(MANIFEST_SECTION), gnu::used, gnu::retain]]
static const char WHO[] = MANIFEST;

BRIDGE_NATIVE(init)
{
    const bridge_term_t terms[2] = {
        { DROP, BRIDGE_READ, BRIDGE_AND, 0 },
        { SETTINGS, BRIDGE_READ, BRIDGE_OPTIONAL, 0 } };
    const uint32_t changes[1] = { DROP };

    if (!shared_start_events(api))
        return (1);
    BRIDGE_NATIVE_COMPONENT(api, "sdl3:Drop", sdl3_drop_t, BRIDGE_STRIP,
        BRIDGE_TABLE, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_COMPONENT(api, "sdl3:EventsSettings", sdl3_events_settings_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "pump", "events", terms, 2, changes, 1,
        nullptr, 0);
    return (0);
}

BRIDGE_NATIVE(shutdown)
{
    shared_stop_subsystem(SDL_INIT_EVENTS);
    return (0);
}

BRIDGE_NATIVE(reload)
{
    return (1);
}
