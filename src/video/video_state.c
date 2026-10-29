/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "video_inside.h"

static const video_driver_t DRIVERS[] = {
    { "windows", SDL3_SURFACE_WINDOWS },
    { "cocoa", SDL3_SURFACE_METAL },
    { "x11", SDL3_SURFACE_X11 },
    { "wayland", SDL3_SURFACE_WAYLAND } };

const video_driver_t *video_find_driver(void)
{
    const char *name = SDL_GetCurrentVideoDriver();

    for (size_t i = 0; i < sizeof(DRIVERS) / sizeof(DRIVERS[0]); i++)
        if (!strcmp(DRIVERS[i].name, name))
            return (&DRIVERS[i]);
    SDL_SetError("the video driver %s gives no surface", name);
    return (nullptr);
}

void video_write_handles(const video_driver_t *row, SDL_Window *window,
    SDL_MetalView view, sdl3_window_t *w)
{
    SDL_PropertiesID p = SDL_GetWindowProperties(window);

    w->surface = row->surface;
    w->hinstance = (uintptr_t)SDL_GetPointerProperty(p,
        SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
    w->hwnd = (uintptr_t)SDL_GetPointerProperty(p,
        SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    w->layer = (uintptr_t)SDL_Metal_GetLayer(view);
    w->x11_display = (uintptr_t)SDL_GetPointerProperty(p,
        SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
    w->x11_window = (uint64_t)SDL_GetNumberProperty(p,
        SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    w->wl_display = (uintptr_t)SDL_GetPointerProperty(p,
        SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
    w->wl_surface = (uintptr_t)SDL_GetPointerProperty(p,
        SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
}

bool video_has_handle(const sdl3_window_t *w)
{
    return ((w->hinstance | w->hwnd | w->layer | w->x11_display
        | w->x11_window | w->wl_display | w->wl_surface) != 0);
}

void video_take_events(SDL_Window *window, sdl3_window_t *w)
{
    SDL_WindowFlags flags = 0;
    int width = 0;
    int height = 0;

    if (SDL_HasEvent(SDL_EVENT_WINDOW_CLOSE_REQUESTED))
        *w = (sdl3_window_t){ .surface = SDL3_SURFACE_NONE, .closed = true };
    SDL_FlushEvents(SDL_EVENT_WINDOW_FIRST, SDL_EVENT_WINDOW_LAST);
    if (w->closed)
        return;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    flags = SDL_GetWindowFlags(window);
    w->width = (uint32_t)width;
    w->height = (uint32_t)height;
    w->fullscreen = (flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_HIDDEN))
        == SDL_WINDOW_FULLSCREEN;
    w->hidden = (flags & SDL_WINDOW_HIDDEN) != 0;
}
