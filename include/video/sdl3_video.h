/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BM_SDL3_VIDEO_H
    #define BM_SDL3_VIDEO_H

    #include <stdint.h>

    #include "bridge_schema.h"

    #define SDL3_PLACE_VALUES { SDL3_PLACE_SYSTEM = 1, SDL3_PLACE_CENTER = 2, \
        SDL3_PLACE_AT = 3 }
    #define SDL3_SURFACE_VALUES { SDL3_SURFACE_NONE = 1, \
        SDL3_SURFACE_WINDOWS = 2, SDL3_SURFACE_METAL = 3, \
        SDL3_SURFACE_X11 = 4, SDL3_SURFACE_WAYLAND = 5 }
    #define SDL3_WINDOW_SETTINGS_FIELDS { char title[64]; uint32_t width; \
        uint32_t height; bool hidden; bool resizable; bool fullscreen; \
        sdl3_place_t place; int32_t x; int32_t y; bool high_dpi; }
    #define SDL3_WINDOW_FIELDS { sdl3_surface_t surface; uint32_t width; \
        uint32_t height; bool closed; bool fullscreen; bool hidden; \
        uint64_t hinstance; uint64_t hwnd; uint64_t layer; \
        uint64_t x11_display; uint64_t x11_window; uint64_t wl_display; \
        uint64_t wl_surface; }

BRIDGE_SCHEMA_ENUM_TYPE(sdl3_place_t, uint32_t, SDL3_PLACE_VALUES);
BRIDGE_SCHEMA_ENUM_DESC(sdl3_place_t, uint32_t, SDL3_PLACE_VALUES);
BRIDGE_SCHEMA_ENUM_TYPE(sdl3_surface_t, uint32_t, SDL3_SURFACE_VALUES);
BRIDGE_SCHEMA_ENUM_DESC(sdl3_surface_t, uint32_t, SDL3_SURFACE_VALUES);
BRIDGE_SCHEMA_STRUCT_TYPE(sdl3_window_settings_t, SDL3_WINDOW_SETTINGS_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(sdl3_window_settings_t, SDL3_WINDOW_SETTINGS_FIELDS);
BRIDGE_SCHEMA_STRUCT_TYPE(sdl3_window_t, SDL3_WINDOW_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(sdl3_window_t, SDL3_WINDOW_FIELDS);

#endif
