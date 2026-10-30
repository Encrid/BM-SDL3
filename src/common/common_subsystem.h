/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef COMMON_SUBSYSTEM_H
    #define COMMON_SUBSYSTEM_H

    #include <SDL3/SDL_init.h>

    #include "bridge_native.h"

bool common_start_video(const bridge_api_t *api);
bool common_start_events(const bridge_api_t *api);
void common_stop_subsystem(SDL_InitFlags flags);

#endif
