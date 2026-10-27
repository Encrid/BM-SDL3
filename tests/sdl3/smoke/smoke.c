/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

typedef void (*clear_color_t)(float red, float green, float blue, float alpha);
typedef void (*clear_t)(uint32_t mask);
typedef void (*read_pixels_t)(int32_t x, int32_t y, int32_t width,
    int32_t height, uint32_t format, uint32_t type, void *pixels);
typedef const uint8_t *(*get_string_t)(uint32_t name);

constexpr uint32_t COLOR_BUFFER_BIT = 0x4000;
constexpr uint32_t RGBA = 0x1908;
constexpr uint32_t UNSIGNED_BYTE = 0x1401;
constexpr uint32_t VERSION = 0x1F02;

static SDL_Window *open_window(void)
{
    SDL_Window *window;

    if (!SDL_Init(SDL_INIT_VIDEO))
        return (nullptr);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    window = SDL_CreateWindow("smoke", 64, 64,
        SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!window || !SDL_GL_CreateContext(window))
        return (nullptr);
    return (window);
}

static int read_back(float red)
{
    clear_color_t clear_color = (clear_color_t)SDL_GL_GetProcAddress(
        "glClearColor");
    clear_t clear = (clear_t)SDL_GL_GetProcAddress("glClear");
    read_pixels_t read_pixels = (read_pixels_t)SDL_GL_GetProcAddress(
        "glReadPixels");
    get_string_t get_string = (get_string_t)SDL_GL_GetProcAddress(
        "glGetString");
    uint8_t pixel[4] = {};

    if (!clear_color || !clear || !read_pixels || !get_string)
        return (1);
    printf("version %s\n", (const char *)get_string(VERSION));
    clear_color(red, 0.4f, 0.6f, 1.0f);
    clear(COLOR_BUFFER_BIT);
    read_pixels(1, 1, 1, 1, RGBA, UNSIGNED_BYTE, pixel);
    printf("pixel %u %u %u\n", pixel[0], pixel[1], pixel[2]);
    return (0);
}

static int print_version(void)
{
    int version = SDL_GetVersion();

    printf("sdl %d.%d.%d\n", SDL_VERSIONNUM_MAJOR(version),
        SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
    return (0);
}

int main(int argc, char **argv)
{
    SDL_Window *window;
    int status;

    if (argc > 1 && strcmp(argv[1], "version") == 0)
        return (print_version());
    window = open_window();
    if (!window) {
        printf("no window: %s\n", SDL_GetError());
        return (1);
    }
    status = read_back(argc > 1 ? 0.8f : 0.2f);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return (status);
}
