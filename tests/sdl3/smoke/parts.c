/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

typedef struct {
    const char *name;
    SDL_WindowFlags flags;
    bool external;
} window_mode_t;

static const window_mode_t MODES[] = {
    { "plain", SDL_WINDOW_HIDDEN, false },
    { "external", SDL_WINDOW_HIDDEN, true },
    { "gl", SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL, false },
    { "vulkan", SDL_WINDOW_HIDDEN | SDL_WINDOW_VULKAN, false },
    { "metal", SDL_WINDOW_HIDDEN, true },
};

static int list_drivers(void)
{
    for (int i = 0; i < SDL_GetNumRenderDrivers(); i++)
        printf("render %s\n", SDL_GetRenderDriver(i));
    for (int i = 0; i < SDL_GetNumGPUDrivers(); i++)
        printf("gpu %s\n", SDL_GetGPUDriver(i));
    return (0);
}

static SDL_Window *open_window(const window_mode_t *mode)
{
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_Window *window;

    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 64);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 64);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER,
        (Sint64)mode->flags);
    SDL_SetBooleanProperty(props,
        SDL_PROP_WINDOW_CREATE_EXTERNAL_GRAPHICS_CONTEXT_BOOLEAN,
        mode->external);
    window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);
    return (window);
}

static void say_window(SDL_Window *window, bool metal)
{
    SDL_WindowFlags flags = SDL_GetWindowFlags(window);
    SDL_MetalView view = nullptr;

    if (!metal) {
        printf("window gl %d vulkan %d\n", (flags & SDL_WINDOW_OPENGL) != 0,
            (flags & SDL_WINDOW_VULKAN) != 0);
        return;
    }
    view = SDL_Metal_CreateView(window);
    printf("metal layer %d\n", view && SDL_Metal_GetLayer(view));
    SDL_Metal_DestroyView(view);
}

int main(int argc, char **argv)
{
    const char *name = argc > 1 ? argv[1] : "plain";
    SDL_Window *window = nullptr;

    if (strcmp(name, "drivers") == 0)
        return (list_drivers());
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("no video: %s\n", SDL_GetError());
        return (1);
    }
    for (size_t i = 0; i < sizeof(MODES) / sizeof(MODES[0]); i++)
        if (strcmp(name, MODES[i].name) == 0)
            window = open_window(&MODES[i]);
    if (!window) {
        printf("no window: %s\n", SDL_GetError());
        SDL_Quit();
        return (1);
    }
    say_window(window, strcmp(name, "metal") == 0);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return (0);
}
