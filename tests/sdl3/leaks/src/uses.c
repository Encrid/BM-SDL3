/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_metal.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

static void use_pairs(bool give)
{
    SDL_Window *window = SDL_CreateWindow("leaks", 64, 64, 0);
    SDL_MetalView view = SDL_Metal_CreateView(window);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_Cursor *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    SDL_Window **windows = SDL_GetWindows(nullptr);

    if (give) {
        SDL_free(windows);
        SDL_DestroyCursor(cursor);
        SDL_GL_DestroyContext(context);
        SDL_Metal_DestroyView(view);
    }
    SDL_DestroyWindow(window);
}

static void use_windows(bool both)
{
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_Window *one = SDL_CreateWindow("leaks", 64, 64, 0);
    SDL_Window *two = SDL_CreateWindowWithProperties(props);

    SDL_DestroyProperties(props);
    SDL_DestroyWindow(one);
    if (both)
        SDL_DestroyWindow(two);
}

static void use_subsystem(SDL_InitFlags flags, bool quit)
{
    if (SDL_InitSubSystem(flags) && quit)
        SDL_QuitSubSystem(flags);
}

static void use_properties(bool destroy)
{
    SDL_PropertiesID props = SDL_CreateProperties();

    if (destroy)
        SDL_DestroyProperties(props);
}

int main(int argc, char **argv)
{
    const char *mode = argc > 1 ? argv[1] : "balanced";
    bool all = strcmp(mode, "balanced") == 0;

    if (all || strcmp(mode, "pairs") == 0)
        use_pairs(all);
    if (all || strcmp(mode, "window") == 0)
        use_windows(all);
    if (all || strcmp(mode, "subsystem") == 0)
        use_subsystem(SDL_INIT_EVENTS, all);
    if (strcmp(mode, "refused") == 0)
        use_subsystem(SDL_INIT_HAPTIC, false);
    if (all || strcmp(mode, "properties") == 0)
        use_properties(all);
    printf("%s\n", mode);
    return (0);
}
