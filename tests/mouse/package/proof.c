/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <sdl3_mouse.h>

int main(void)
{
    if (sizeof(sdl3_mouse_settings_t) != 1 || sizeof(sdl3_cursor_t) != 8)
        return (1);
    if (sizeof(sdl3_mouse_t) != 36)
        return (1);
    return (sizeof(sdl3_mouse_settings_t_desc) > 1
        && sizeof(sdl3_cursor_t_desc) > 1
        && sizeof(sdl3_mouse_t_desc) > 1 ? 0 : 1);
}
