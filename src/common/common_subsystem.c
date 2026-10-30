/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <SDL3/SDL_error.h>

#include "common_subsystem.h"

bool common_start_video(const bridge_api_t *api)
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO))
        return (true);
    bridge_native_say(api, BRIDGE_LOG_ERR, "no video: %s", SDL_GetError());
    return (false);
}

bool common_start_events(const bridge_api_t *api)
{
    if (SDL_InitSubSystem(SDL_INIT_EVENTS))
        return (true);
    bridge_native_say(api, BRIDGE_LOG_ERR, "no events: %s", SDL_GetError());
    return (false);
}

void common_stop_subsystem(SDL_InitFlags flags)
{
    SDL_QuitSubSystem(flags);
    if (!SDL_WasInit(0))
        SDL_Quit();
}
