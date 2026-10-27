/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "bridge_native.h"
#include "sdl3_video.h"

#define SCENE_FIELDS { char scene[32]; char module[512]; }

BRIDGE_SCHEMA_STRUCT_TYPE(test_scene_t, SCENE_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(test_scene_t, SCENE_FIELDS);

typedef struct {
    const char *name;
    uint32_t rounds;
    uint32_t steal;
    uint32_t depth;
    uint32_t stencil;
} scene_t;

typedef struct {
    const char *scene;
    uint32_t round;
    SDL_EventType type;
} step_t;

typedef SDL_FunctionPointer (*get_proc_t)(const char *name);
typedef const uint8_t *(*get_string_t)(uint32_t name);
typedef void (*get_integer_t)(uint32_t name, int32_t *value);
typedef uint32_t (*get_error_t)(void);

constexpr uint32_t VERSION = 0x1F02;
constexpr uint32_t PROFILE_MASK = 0x9126;
constexpr int32_t CORE_BIT = 0x1;
constexpr uint32_t PROFILE_FROM = 32;

[[gnu::section(MANIFEST_SECTION), gnu::used, gnu::retain]]
static const char WHO[] = MANIFEST;

static const scene_t SCENES[] = {
    { "open", 4, 0, 24, 8 }, { "events", 2, 0, 24, 8 },
    { "close", 4, 0, 24, 8 }, { "shut", 3, 2, 24, 8 },
    { "steal", 3, 2, 24, 8 }, { "deeper", 1, 0, 32, 16 }
};

static const step_t STEPS[] = {
    { "events", 2, SDL_EVENT_WINDOW_EXPOSED },
    { "events", 2, SDL_EVENT_WINDOW_MOVED },
    { "close", 2, SDL_EVENT_WINDOW_CLOSE_REQUESTED },
    { "shut", 2, SDL_EVENT_WINDOW_CLOSE_REQUESTED }
};

static uint32_t scene_slot;
static uint32_t window_slot;
static uint32_t round_now;
static SDL_Window *taken_window;
static SDL_GLContext taken_context;

static void say(const bridge_api_t *api, const char *label, uint32_t a0,
    uint32_t a1)
{
    char text[128];
    bridge_line_t line = { text, 0, a0, a1 };

    snprintf(text, sizeof(text), "r%u %s", round_now, label);
    line.len = (uint32_t)strlen(text);
    bridge_native_log(api, BRIDGE_LOG_INFO, &line);
}

static uint32_t count_set(const void *at, size_t len)
{
    const uint8_t *bytes = at;
    uint32_t set = 0;

    for (size_t i = 0; i < len; i++)
        set += bytes[i] != 0;
    return (set);
}

static void push_event(SDL_EventType type)
{
    SDL_Event ev = { .type = type };

    ev.window.windowID = SDL_GetWindowID(SDL_GL_GetCurrentWindow());
    SDL_PushEvent(&ev);
}

static void take_context(void)
{
    taken_window = SDL_GL_GetCurrentWindow();
    taken_context = SDL_GL_GetCurrentContext();
    if (taken_context)
        SDL_GL_MakeCurrent(nullptr, nullptr);
}

static void give_context(const bridge_api_t *api)
{
    say(api, "swap", SDL_GetError()[0] != 0, 0);
    SDL_GL_MakeCurrent(taken_window, taken_context);
    taken_context = nullptr;
}

static uint32_t get_proc_kind(const sdl3_window_t *w)
{
    get_proc_t proc = (get_proc_t)(uintptr_t)w->gl_proc;
    SDL_FunctionPointer want = nullptr;

    if (!proc)
        return (0);
    want = SDL_GL_GetProcAddress("glGetString");
    return (want && proc("glGetString") == want ? 1 : 2);
}

static void say_state(const bridge_api_t *api, const sdl3_window_t *w)
{
    int count = 0;
    SDL_Window **list = SDL_GetWindows(&count);

    SDL_free(list);
    say(api, "window", get_proc_kind(w), w->closed);
    say(api, "frame", w->width, w->height);
    say(api, "windows", (uint32_t)count, SDL_GL_GetCurrentContext() != nullptr);
    say(api, "queue", SDL_HasEvents(SDL_EVENT_WINDOW_FIRST,
        SDL_EVENT_WINDOW_LAST), 0);
}

static void say_flags(const bridge_api_t *api, SDL_Window *window)
{
    SDL_WindowFlags flags = SDL_GetWindowFlags(window);
    const char *title = SDL_GetWindowTitle(window);
    char label[96];

    say(api, "hidden", (flags & SDL_WINDOW_HIDDEN) != 0, 0);
    say(api, "resizable", (flags & SDL_WINDOW_RESIZABLE) != 0, 0);
    say(api, "fullscreen", (flags & SDL_WINDOW_FULLSCREEN) != 0, 0);
    say(api, "high_dpi", (flags & SDL_WINDOW_HIGH_PIXEL_DENSITY) != 0, 0);
    snprintf(label, sizeof(label), "title \"%.63s\"", title);
    say(api, label, (uint32_t)strlen(title), 0);
}

static void say_place(const bridge_api_t *api, SDL_Window *window)
{
    SDL_DisplayID display = SDL_GetDisplayForWindow(window);
    SDL_Rect area = {};
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    SDL_GetWindowPosition(window, &x, &y);
    SDL_GetWindowSize(window, &w, &h);
    if (!SDL_GetDisplayUsableBounds(display, &area) || w > area.w
        || h > area.h)
        SDL_GetDisplayBounds(display, &area);
    say(api, "at", (uint32_t)x, (uint32_t)y);
    say(api, "center", x == area.x + (area.w - w) / 2,
        y == area.y + (area.h - h) / 2);
}

static void say_profile(const bridge_api_t *api)
{
    int profile = 0;
    int flags = 0;

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &flags);
    say(api, "profile", (uint32_t)profile, (uint32_t)flags);
}

static void say_version(const bridge_api_t *api)
{
    get_string_t get_string = (get_string_t)SDL_GL_GetProcAddress(
        "glGetString");
    get_integer_t get_integer = (get_integer_t)SDL_GL_GetProcAddress(
        "glGetIntegerv");
    get_error_t get_error = (get_error_t)SDL_GL_GetProcAddress("glGetError");
    int asked[2] = {};
    uint32_t got[2] = {};
    int32_t mask = 0;

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &asked[0]);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &asked[1]);
    sscanf((const char *)get_string(VERSION), "%u.%u", &got[0], &got[1]);
    if (got[0] * 10 + got[1] >= PROFILE_FROM)
        get_integer(PROFILE_MASK, &mask);
    get_error();
    say(api, "asked", (uint32_t)asked[0], (uint32_t)asked[1]);
    say(api, "got", got[0] * 10 + got[1]
        >= (uint32_t)(asked[0] * 10 + asked[1]), (mask & CORE_BIT) != 0);
}

static void say_buffers(const bridge_api_t *api, const sdl3_window_t *w,
    const scene_t *scene)
{
    int depth = 0;
    int stencil = 0;
    int samples = 0;
    int twice = 0;
    int interval = 0;
    bool known = SDL_GL_GetSwapInterval(&interval);

    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil);
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &samples);
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &twice);
    say(api, "depth", (uint32_t)depth >= scene->depth,
        (uint32_t)stencil >= scene->stencil);
    say(api, "samples", w->samples, w->samples == (uint32_t)samples);
    say(api, "double", (uint32_t)twice, 0);
    say(api, "interval", (uint32_t)interval, known);
}

static void say_window(const bridge_api_t *api, const sdl3_window_t *w,
    const scene_t *scene)
{
    SDL_Window *window = SDL_GL_GetCurrentWindow();

    say_state(api, w);
    if (!window || !SDL_GL_GetCurrentContext())
        return;
    say_flags(api, window);
    say_place(api, window);
    say_profile(api);
    say_version(api);
    say_buffers(api, w, scene);
}

static const scene_t *find_scene(const char *name)
{
    for (size_t i = 0; i < sizeof(SCENES) / sizeof(SCENES[0]); i++)
        if (!strcmp(SCENES[i].name, name))
            return (&SCENES[i]);
    return (nullptr);
}

static int call_reload(const char *path)
{
    SDL_SharedObject *lib = SDL_LoadObject(path);
    SDL_FunctionPointer reload = nullptr;
    int got = -1;

    if (lib)
        reload = SDL_LoadFunction(lib, "bridge_reload");
    if (reload)
        got = ((int (*)(const bridge_api_t *))reload)(nullptr);
    if (lib)
        SDL_UnloadObject(lib);
    return (got);
}

static void say_reload_or_video(const bridge_api_t *api,
    const test_scene_t *on)
{
    if (!strcmp(on->scene, "reload"))
        say(api, "reload", (uint32_t)call_reload(on->module), 0);
    if (!strcmp(on->scene, "video"))
        say(api, "video", SDL_WasInit(SDL_INIT_VIDEO) != 0, 0);
}

BRIDGE_NATIVE(drive)
{
    const test_scene_t *on = bridge_native_get_single(api, scene_slot);
    const sdl3_window_t *w = bridge_native_get_single(api, window_slot);

    round_now++;
    if (taken_context)
        give_context(api);
    say(api, "before", w != nullptr, w ? count_set(w, sizeof(*w)) : 0);
    for (size_t i = 0; on && i < sizeof(STEPS) / sizeof(STEPS[0]); i++)
        if (STEPS[i].round == round_now && !strcmp(STEPS[i].scene, on->scene))
            push_event(STEPS[i].type);
    return (0);
}

BRIDGE_NATIVE(check)
{
    const test_scene_t *on = bridge_native_get_single(api, scene_slot);
    const scene_t *scene = on ? find_scene(on->scene) : nullptr;
    const sdl3_window_t *w = bridge_native_get_single(api, window_slot);

    if (on)
        say_reload_or_video(api, on);
    if (scene && w)
        say_window(api, w, scene);
    if (scene && scene->steal == round_now)
        take_context();
    SDL_ClearError();
    if (scene && round_now < scene->rounds)
        return (0);
    return (BRIDGE_END);
}

BRIDGE_NATIVE(init)
{
    bridge_term_t terms[2] = { { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 } };

#ifdef HOLD
    SDL_InitSubSystem(SDL_INIT_EVENTS);
#else
    SDL_SetHint("BM_PROBE_HINT", "1");
#endif
    scene_slot = BRIDGE_NATIVE_COMPONENT(api, "test:Scene", test_scene_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON);
    window_slot = BRIDGE_NATIVE_COMPONENT(api, "sdl3:Window", sdl3_window_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON);
    terms[0].slot = scene_slot;
    terms[1].slot = window_slot;
    BRIDGE_NATIVE_ADD_SYSTEM(api, "drive", "drive", terms, 2, nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "check", "check", terms, 2, nullptr, 0);
    return (0);
}

BRIDGE_NATIVE(shutdown)
{
#ifdef HOLD
    printf("events still started %d\n", SDL_WasInit(SDL_INIT_EVENTS) != 0);
    fflush(stdout);
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    SDL_Quit();
#else
    printf("sdl quit %d\n", SDL_GetHint("BM_PROBE_HINT") == nullptr);
    fflush(stdout);
#endif
    return (0);
}

BRIDGE_NATIVE(reload)
{
    return (1);
}
