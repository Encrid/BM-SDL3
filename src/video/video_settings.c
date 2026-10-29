/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "video_inside.h"

static void report(const bridge_api_t *api, bool done, const char *setting)
{
    if (!done)
        bridge_native_say(api, BRIDGE_LOG_WARN, "no change of %s: %s",
            setting, SDL_GetError());
}

static bool set_hidden(SDL_Window *window, bool hidden)
{
    if (hidden)
        return (SDL_HideWindow(window));
    return (SDL_ShowWindow(window));
}

static void apply_flags(const bridge_api_t *api, SDL_Window *window,
    const sdl3_window_settings_t *now, const sdl3_window_settings_t *last)
{
    if (now->hidden != last->hidden)
        report(api, set_hidden(window, now->hidden), "hidden");
    if (now->resizable != last->resizable)
        report(api, SDL_SetWindowResizable(window, now->resizable),
            "resizable");
    if (now->fullscreen != last->fullscreen)
        report(api, SDL_SetWindowFullscreen(window, now->fullscreen),
            "fullscreen");
}

void video_apply_settings(const bridge_api_t *api, SDL_Window *window,
    sdl3_window_settings_t *last, const sdl3_window_settings_t *now)
{
    SDL_Point at = video_get_place(now);
    SDL_Point was = video_get_place(last);

    if (strcmp(now->title, last->title))
        report(api, SDL_SetWindowTitle(window, now->title), "title");
    if (now->width != last->width || now->height != last->height)
        report(api, SDL_SetWindowSize(window, (int)now->width,
            (int)now->height), "size");
    apply_flags(api, window, now, last);
    if (at.x != was.x || at.y != was.y)
        report(api, SDL_SetWindowPosition(window, at.x, at.y), "place");
    if (now->high_dpi != last->high_dpi)
        bridge_native_say(api, BRIDGE_LOG_WARN,
            "no change of high_dpi: it acts only when the window opens");
    *last = *now;
}

SDL_Point video_get_place(const sdl3_window_settings_t *s)
{
    if (s->place == SDL3_PLACE_CENTER)
        return ((SDL_Point){ SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED });
    if (s->place == SDL3_PLACE_AT)
        return ((SDL_Point){ s->x, s->y });
    return ((SDL_Point){ SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED });
}
