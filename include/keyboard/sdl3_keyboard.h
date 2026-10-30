/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BM_SDL3_KEYBOARD_H
    #define BM_SDL3_KEYBOARD_H

    #include <stdint.h>

    #include "bridge_schema.h"

    #define SDL3_KEYBOARD_SETTINGS_FIELDS { bool text; bool repeat; }
    #define SDL3_KEYBOARD_FIELDS { uint8_t key[512]; uint8_t hit[512]; \
        uint32_t mods; char text[64]; uint32_t text_len; }

BRIDGE_SCHEMA_STRUCT_TYPE(sdl3_keyboard_settings_t,
    SDL3_KEYBOARD_SETTINGS_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(sdl3_keyboard_settings_t,
    SDL3_KEYBOARD_SETTINGS_FIELDS);
BRIDGE_SCHEMA_STRUCT_TYPE(sdl3_keyboard_t, SDL3_KEYBOARD_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(sdl3_keyboard_t, SDL3_KEYBOARD_FIELDS);

#endif
