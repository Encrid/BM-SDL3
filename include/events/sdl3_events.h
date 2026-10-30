/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BM_SDL3_EVENTS_H
    #define BM_SDL3_EVENTS_H

    #include <stdint.h>

    #include "bridge_schema.h"

    #define SDL3_EVENTS_SETTINGS_FIELDS { uint32_t drops; bool quit; }
    #define SDL3_DROP_FIELDS { char path[1024]; }

BRIDGE_SCHEMA_STRUCT_TYPE(sdl3_events_settings_t, SDL3_EVENTS_SETTINGS_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(sdl3_events_settings_t, SDL3_EVENTS_SETTINGS_FIELDS);
BRIDGE_SCHEMA_STRUCT_TYPE(sdl3_drop_t, SDL3_DROP_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(sdl3_drop_t, SDL3_DROP_FIELDS);

#endif
