/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BM_SDL3_MOUSE_H
    #define BM_SDL3_MOUSE_H

    #include <stdint.h>

    #include "bridge_schema.h"

    #define SDL3_MOUSE_SETTINGS_FIELDS { bool natural; }
    #define SDL3_CURSOR_FIELDS { uint32_t shape; bool hidden; bool relative; }
    #define SDL3_MOUSE_FIELDS { float x; float y; float dx; float dy; \
        float wheel_x; float wheel_y; uint32_t held; uint32_t pressed; \
        uint32_t released; }

BRIDGE_SCHEMA_STRUCT_TYPE(sdl3_mouse_settings_t, SDL3_MOUSE_SETTINGS_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(sdl3_mouse_settings_t, SDL3_MOUSE_SETTINGS_FIELDS);
BRIDGE_SCHEMA_STRUCT_TYPE(sdl3_cursor_t, SDL3_CURSOR_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(sdl3_cursor_t, SDL3_CURSOR_FIELDS);
BRIDGE_SCHEMA_STRUCT_TYPE(sdl3_mouse_t, SDL3_MOUSE_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(sdl3_mouse_t, SDL3_MOUSE_FIELDS);

#endif
