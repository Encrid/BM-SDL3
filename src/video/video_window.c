/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "video_inside.h"

static video_kept_t kept;

static void set_flags(SDL_PropertiesID p, const sdl3_window_settings_t *now,
    const video_driver_t *row)
{
    SDL_SetBooleanProperty(p, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN,
        now->hidden);
    SDL_SetBooleanProperty(p, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN,
        now->resizable);
    SDL_SetBooleanProperty(p, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN,
        now->fullscreen);
    SDL_SetBooleanProperty(p, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN,
        now->high_dpi);
    SDL_SetBooleanProperty(p, SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN,
        row->surface == SDL3_SURFACE_METAL);
}

static SDL_Window *create_window(const sdl3_window_settings_t *now,
    const video_driver_t *row)
{
    SDL_PropertiesID p = SDL_CreateProperties();
    SDL_Point at = video_get_place(now);
    SDL_Window *made = nullptr;

    SDL_SetStringProperty(p, SDL_PROP_WINDOW_CREATE_TITLE_STRING, now->title);
    SDL_SetNumberProperty(p, SDL_PROP_WINDOW_CREATE_X_NUMBER, at.x);
    SDL_SetNumberProperty(p, SDL_PROP_WINDOW_CREATE_Y_NUMBER, at.y);
    SDL_SetNumberProperty(p, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, now->width);
    SDL_SetNumberProperty(p, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER,
        now->height);
    set_flags(p, now, row);
    made = SDL_CreateWindowWithProperties(p);
    SDL_DestroyProperties(p);
    return (made);
}

void video_close_window(void)
{
    if (!kept.window)
        return;
    SDL_Metal_DestroyView(kept.view);
    SDL_DestroyWindow(kept.window);
    kept.view = nullptr;
    kept.window = nullptr;
}

static int32_t open_window(const bridge_api_t *api,
    const sdl3_window_settings_t *now, sdl3_window_t *w)
{
    const video_driver_t *row = video_find_driver();
    sdl3_window_t made = {};

    kept.last = *now;
    if (row)
        kept.window = create_window(now, row);
    if (kept.window) {
        kept.view = SDL_Metal_CreateView(kept.window);
        video_write_handles(row, kept.window, kept.view, &made);
    }
    if (video_has_handle(&made)) {
        *w = made;
        return (0);
    }
    video_close_window();
    bridge_native_say(api, BRIDGE_LOG_ERR, "no window: %s", SDL_GetError());
    return (1);
}

BRIDGE_NATIVE(window)
{
    const sdl3_window_settings_t *s = bridge_native_get_single(api, SETTINGS);
    sdl3_window_t *w = bridge_native_get_single(api, WINDOW);
    sdl3_window_settings_t now = {};

    if (!s || !w)
        return (0);
    if (w->closed) {
        video_close_window();
        return (0);
    }
    now = *s;
    now.title[sizeof(now.title) - 1] = 0;
    if (!kept.window && open_window(api, &now, w))
        return (1);
    video_apply_settings(api, kept.window, &kept.last, &now);
    video_take_events(kept.window, w);
    return (0);
}
