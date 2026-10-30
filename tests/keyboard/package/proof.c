/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <sdl3_keyboard.h>

int main(void)
{
    if (sizeof(sdl3_keyboard_settings_t) != 2)
        return (1);
    if (sizeof(sdl3_keyboard_t) != 1096)
        return (1);
    return (sizeof(sdl3_keyboard_settings_t_desc) > 1
        && sizeof(sdl3_keyboard_t_desc) > 1 ? 0 : 1);
}
