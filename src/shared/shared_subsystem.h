/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef SHARED_SUBSYSTEM_H
    #define SHARED_SUBSYSTEM_H

    #include <SDL3/SDL_init.h>

    #include "bridge_native.h"

bool shared_start_video(const bridge_api_t *api);
bool shared_start_events(const bridge_api_t *api);
void shared_stop_subsystem(SDL_InitFlags flags);

#endif
