/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <sdl3_video.h>

int main(void)
{
    sdl3_window_settings_t settings = { .place = SDL3_PLACE_SYSTEM };
    sdl3_window_t window = { .surface = SDL3_SURFACE_NONE };

    if (settings.place != SDL3_PLACE_SYSTEM)
        return (1);
    if (window.surface != SDL3_SURFACE_NONE)
        return (1);
    return (sizeof(sdl3_window_t_desc) > 1 ? 0 : 1);
}
