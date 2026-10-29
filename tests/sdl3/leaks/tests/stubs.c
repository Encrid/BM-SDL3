/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_metal.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

static char held[8];

SDL_Window *SDL_CreateWindow([[maybe_unused]] const char *title,
    [[maybe_unused]] int w, [[maybe_unused]] int h,
    [[maybe_unused]] SDL_WindowFlags flags)
{
    return ((SDL_Window *)held);
}

SDL_Window *SDL_CreateWindowWithProperties(
    [[maybe_unused]] SDL_PropertiesID props)
{
    return ((SDL_Window *)held);
}

void SDL_DestroyWindow([[maybe_unused]] SDL_Window *window)
{
}

SDL_Window **SDL_GetWindows(int *count)
{
    if (count)
        *count = 0;
    return ((SDL_Window **)held);
}

void SDL_free([[maybe_unused]] void *mem)
{
}

SDL_MetalView SDL_Metal_CreateView([[maybe_unused]] SDL_Window *window)
{
    return (held);
}

void SDL_Metal_DestroyView([[maybe_unused]] SDL_MetalView view)
{
}

SDL_GLContext SDL_GL_CreateContext([[maybe_unused]] SDL_Window *window)
{
    return ((SDL_GLContext)held);
}

bool SDL_GL_DestroyContext([[maybe_unused]] SDL_GLContext context)
{
    return (true);
}

SDL_Cursor *SDL_CreateSystemCursor([[maybe_unused]] SDL_SystemCursor id)
{
    return ((SDL_Cursor *)held);
}

void SDL_DestroyCursor([[maybe_unused]] SDL_Cursor *cursor)
{
}

bool SDL_InitSubSystem(SDL_InitFlags flags)
{
    return (flags != SDL_INIT_HAPTIC);
}

void SDL_QuitSubSystem([[maybe_unused]] SDL_InitFlags flags)
{
}

SDL_PropertiesID SDL_CreateProperties(void)
{
    return (1);
}

void SDL_DestroyProperties([[maybe_unused]] SDL_PropertiesID props)
{
}
