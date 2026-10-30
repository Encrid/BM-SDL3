/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>

#include <SDL3/SDL.h>

#include "bridge_native.h"
#include "shared_subsystem.h"

[[gnu::section(MANIFEST_SECTION), gnu::used, gnu::retain]]
static const char WHO[] = MANIFEST;

BRIDGE_NATIVE(init)
{
    SDL_SetHint("BM_PROBE_HINT", "1");
#ifdef HOLD
    SDL_InitSubSystem(SDL_INIT_EVENTS);
#endif
#ifdef EVENTS
    return (shared_start_events(api) ? 0 : 1);
#else
    return (shared_start_video(api) ? 0 : 1);
#endif
}

BRIDGE_NATIVE(shutdown)
{
#ifdef EVENTS
    shared_stop_subsystem(SDL_INIT_EVENTS);
#else
    shared_stop_subsystem(SDL_INIT_VIDEO);
#endif
    printf("sdl quit %d\n", SDL_GetHint("BM_PROBE_HINT") == nullptr);
    fflush(stdout);
#ifdef HOLD
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    SDL_Quit();
#endif
    return (0);
}

BRIDGE_NATIVE(reload)
{
    return (1);
}
