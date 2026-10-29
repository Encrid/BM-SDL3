/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <sdl3_events.h>

int main(void)
{
    sdl3_events_settings_t settings = { .drops = 16, .quit = true };
    sdl3_drop_t drop = { .path = "dropped/a.txt" };

    if (settings.drops != 16 || !settings.quit)
        return (1);
    if (sizeof(drop.path) != 1024 || drop.path[0] != 'd')
        return (1);
    return (sizeof(sdl3_drop_t_desc) > 1
        && sizeof(sdl3_events_settings_t_desc) > 1 ? 0 : 1);
}
