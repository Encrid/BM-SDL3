/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef VIDEO_INSIDE_H
    #define VIDEO_INSIDE_H

    #include <SDL3/SDL.h>

    #include "bridge_native.h"
    #include "shared_subsystem.h"
    #include "sdl3_video.h"

constexpr uint32_t PLACE = 0;
constexpr uint32_t SURFACE = 1;
constexpr uint32_t SETTINGS = 2;
constexpr uint32_t WINDOW = 3;

typedef struct {
    SDL_Window *window;
    SDL_MetalView view;
    sdl3_window_settings_t last;
} video_kept_t;

typedef struct {
    const char *name;
    sdl3_surface_t surface;
} video_driver_t;

void video_close_window(void);
void video_apply_settings(const bridge_api_t *api, SDL_Window *window,
    sdl3_window_settings_t *last, const sdl3_window_settings_t *now);
SDL_Point video_get_place(const sdl3_window_settings_t *s);
const video_driver_t *video_find_driver(void);
void video_write_handles(const video_driver_t *row, SDL_Window *window,
    SDL_MetalView view, sdl3_window_t *w);
bool video_has_handle(const sdl3_window_t *w);
void video_take_events(SDL_Window *window, sdl3_window_t *w);

#endif
