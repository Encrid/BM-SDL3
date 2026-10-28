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

enum act : uint32_t {
    TITLE = 1,
    SIZE,
    HIDDEN,
    RESIZABLE,
    FULLSCREEN,
    PLACE,
    HIGH_DPI,
    PUSH,
    USER_SIZE,
    MAX_SIZE,
    OTHER,
    DETACH,
    ATTACH
};

typedef struct {
    const char *scene;
    uint32_t round;
    enum act act;
    int64_t a;
    int64_t b;
    int64_t c;
    const char *text;
} step_t;

constexpr uint32_t ROUNDS = 4;
#ifdef DETACHER
constexpr uint32_t DETACHES = 2;
#else
constexpr uint32_t DETACHES = 0;
#endif

[[gnu::section(MANIFEST_SECTION), gnu::used, gnu::retain]]
static const char WHO[] = MANIFEST;

static const char A64[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
static const char A62[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

static const step_t STEPS[] = {
    { "events", 2, PUSH, SDL_EVENT_WINDOW_EXPOSED, 0, 0, nullptr },
    { "events", 2, PUSH, SDL_EVENT_WINDOW_MOVED, 0, 0, nullptr },
    { "close", 2, PUSH, SDL_EVENT_WINDOW_CLOSE_REQUESTED, 0, 0, nullptr },
    { "close", 3, TITLE, 0, 0, 0, "again" },
    { "show", 2, HIDDEN, 0, 0, 0, nullptr },
    { "hide", 2, HIDDEN, 1, 0, 0, nullptr },
    { "title", 2, TITLE, 0, 0, 0, "new title" },
    { "title64", 2, TITLE, 0, 0, 0, A64 },
    { "title62", 2, TITLE, 0, 0, 0, A62 },
    { "size", 2, SIZE, 80, 60, 0, nullptr },
    { "clamp", 2, MAX_SIZE, 70, 50, 0, nullptr },
    { "clamp", 2, SIZE, 80, 60, 0, nullptr },
    { "user", 2, USER_SIZE, 80, 60, 0, nullptr },
    { "resize_on", 2, RESIZABLE, 1, 0, 0, nullptr },
    { "full", 2, FULLSCREEN, 1, 0, 0, nullptr },
    { "move_y", 2, PLACE, SDL3_PLACE_AT, 120, 100, nullptr },
    { "system", 2, PLACE, SDL3_PLACE_SYSTEM, 120, 90, nullptr },
    { "place7", 2, PLACE, 7, 120, 90, nullptr },
    { "center", 2, PLACE, SDL3_PLACE_CENTER, 120, 90, nullptr },
    { "move_xy", 2, PLACE, SDL3_PLACE_CENTER, 130, 100, nullptr },
    { "move_at", 2, PLACE, SDL3_PLACE_AT, 130, 100, nullptr },
    { "dpi", 2, HIGH_DPI, 0, 0, 0, nullptr },
    { "flat", 2, SIZE, 64, 0, 0, nullptr },
    { "tall", 2, SIZE, 64, 40, 0, nullptr },
    { "flat_twice", 2, SIZE, 64, 0, 0, nullptr },
    { "flat_twice", 3, SIZE, 64, 48, 0, nullptr },
    { "flat_twice", 4, SIZE, 64, 0, 0, nullptr },
    { "wide", 2, SIZE, 2147483648, 48, 0, nullptr },
    { "grow_center", 2, SIZE, 80, 60, 0, nullptr },
    { "grow_center", 2, PLACE, SDL3_PLACE_CENTER, 120, 90, nullptr },
    { "center_grow", 2, PLACE, SDL3_PLACE_CENTER, 120, 90, nullptr },
    { "center_grow", 3, SIZE, 80, 60, 0, nullptr },
    { "detach_window", 2, DETACH, 0, 0, 0, nullptr },
    { "detach_window", 2, PUSH, SDL_EVENT_WINDOW_EXPOSED, 0, 0, nullptr },
    { "reattach", 2, DETACH, 0, 0, 0, nullptr },
    { "reattach", 3, ATTACH, 0, 0, 0, nullptr },
    { "detach_settings", 2, DETACH, 1, 0, 0, nullptr },
    { "detach_settings", 2, PUSH, SDL_EVENT_WINDOW_EXPOSED, 0, 0, nullptr },
    { "other", 2, OTHER, 1, 0, 0, nullptr },
    { "other_quiet", 2, OTHER, 0, 0, 0, nullptr }
};

static uint32_t scene_slot;
static uint32_t settings_slot;
static uint32_t window_slot;
static uint32_t round_now;
static SDL_Window *own_window;

static void say(const bridge_api_t *api, const char *label, uint32_t a0,
    uint32_t a1)
{
    char text[128];
    bridge_line_t line = { text, 0, a0, a1 };

    snprintf(text, sizeof(text), "r%u %s", round_now, label);
    line.len = (uint32_t)strlen(text);
    bridge_native_log(api, BRIDGE_LOG_INFO, &line);
}

static uint32_t count_fields(const sdl3_window_t *w)
{
    const uint64_t fields[] = { w->surface, w->width, w->height, w->closed,
        w->fullscreen, w->hidden, w->hinstance, w->hwnd, w->layer,
        w->x11_display, w->x11_window, w->wl_display, w->wl_surface };
    uint32_t set = 0;

    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); i++)
        set += fields[i] != 0;
    return (set);
}

static SDL_Window *find_window(uint32_t *count)
{
    int all = 0;
    SDL_Window **list = SDL_GetWindows(&all);
    SDL_Window *found = nullptr;
    uint32_t theirs = 0;

    for (int i = 0; list && i < all; i++) {
        if (list[i] == own_window)
            continue;
        found = list[i];
        theirs++;
    }
    SDL_free(list);
    if (count)
        *count = theirs;
    return (found);
}

static bool is_scene(const char *name)
{
    if (!strcmp(name, "open"))
        return (true);
    for (size_t i = 0; i < sizeof(STEPS) / sizeof(STEPS[0]); i++)
        if (!strcmp(STEPS[i].scene, name))
            return (true);
    return (false);
}

static void push_event(Uint32 type, SDL_Window *window)
{
    SDL_Event ev = { .type = type };

    ev.window.windowID = window ? SDL_GetWindowID(window) : 0;
    SDL_PushEvent(&ev);
}

static void open_other(bool close)
{
    own_window = SDL_CreateWindow("other", 32, 32, SDL_WINDOW_HIDDEN);
    if (own_window && close)
        push_event(SDL_EVENT_WINDOW_CLOSE_REQUESTED, own_window);
}

static void write_title(sdl3_window_settings_t *s, const char *text)
{
    size_t len = strlen(text);

    if (len > sizeof(s->title))
        len = sizeof(s->title);
    memcpy(s->title, text, len);
    if (len < sizeof(s->title))
        s->title[len] = 0;
}

static void write_setting(sdl3_window_settings_t *s, const step_t *step)
{
    if (step->act == TITLE)
        write_title(s, step->text);
    if (step->act == SIZE) {
        s->width = (uint32_t)step->a;
        s->height = (uint32_t)step->b;
    }
    if (step->act == HIDDEN)
        s->hidden = step->a != 0;
    if (step->act == RESIZABLE)
        s->resizable = step->a != 0;
    if (step->act == FULLSCREEN)
        s->fullscreen = step->a != 0;
    if (step->act == PLACE) {
        s->place = (sdl3_place_t)step->a;
        s->x = (int32_t)step->b;
        s->y = (int32_t)step->c;
    }
    if (step->act == HIGH_DPI)
        s->high_dpi = !s->high_dpi;
}

static void attach_zeros(const bridge_api_t *api)
{
    sdl3_window_t zeros = {};

    BRIDGE_NATIVE_ATTACH(api, 0, window_slot, zeros);
}

static void do_step(const bridge_api_t *api, const step_t *step,
    sdl3_window_settings_t *s, SDL_Window *window)
{
    if (step->act == PUSH)
        push_event((Uint32)step->a, window);
    if (step->act == USER_SIZE && window)
        SDL_SetWindowSize(window, (int)step->a, (int)step->b);
    if (step->act == MAX_SIZE && window)
        SDL_SetWindowMaximumSize(window, (int)step->a, (int)step->b);
    if (step->act == OTHER)
        open_other(step->a != 0);
    if (step->act == DETACH)
        api->detach(api, 0, step->a ? settings_slot : window_slot);
    if (step->act == ATTACH)
        attach_zeros(api);
    if (step->act <= HIGH_DPI && s)
        write_setting(s, step);
}

static bool has_handles(const sdl3_window_t *w, SDL_Window *window)
{
    SDL_PropertiesID p = SDL_GetWindowProperties(window);
    const uint64_t want[] = {
        (uintptr_t)SDL_GetPointerProperty(p,
            SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr),
        (uintptr_t)SDL_GetPointerProperty(p,
            SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr),
        (uintptr_t)SDL_GetPointerProperty(p,
            SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr),
        (uint64_t)SDL_GetNumberProperty(p,
            SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0),
        (uintptr_t)SDL_GetPointerProperty(p,
            SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr),
        (uintptr_t)SDL_GetPointerProperty(p,
            SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr) };
    const uint64_t got[] = { w->hinstance, w->hwnd, w->x11_display,
        w->x11_window, w->wl_display, w->wl_surface };
    uint64_t any = w->layer;

    for (size_t i = 0; i < sizeof(got) / sizeof(got[0]); i++)
        any |= got[i];
    return (!memcmp(want, got, sizeof(want)) && any != 0
        && (w->layer != 0) == (w->surface == SDL3_SURFACE_METAL));
}

static void say_value(const bridge_api_t *api, const sdl3_window_t *w,
    SDL_Window *window)
{
    say(api, "state", w->surface, w->closed);
    say(api, "fields", count_fields(w), 0);
    say(api, "frame", w->width, w->height);
    say(api, "flags", w->fullscreen, w->hidden);
    if (window)
        say(api, "surface", w->surface, has_handles(w, window));
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
    say(api, "graphics", (flags & SDL_WINDOW_OPENGL) != 0,
        (flags & SDL_WINDOW_METAL) != 0);
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

static void say_window(const bridge_api_t *api,
    const sdl3_window_settings_t *s, const sdl3_window_t *w)
{
    uint32_t count = 0;
    SDL_Window *window = find_window(&count);

    if (w)
        say_value(api, w, window);
    say(api, "windows", count, 0);
    say(api, "queue", SDL_HasEvents(SDL_EVENT_WINDOW_FIRST,
        SDL_EVENT_WINDOW_LAST), 0);
    if (s)
        say(api, "wanted", s->width, s->height);
    if (!window)
        return;
    say_flags(api, window);
    say_place(api, window);
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

static void close_other(void)
{
    if (own_window)
        SDL_DestroyWindow(own_window);
    own_window = nullptr;
}

BRIDGE_NATIVE(drive)
{
    const test_scene_t *on = bridge_native_get_single(api, scene_slot);
    sdl3_window_settings_t *s = bridge_native_get_single(api, settings_slot);
    const sdl3_window_t *w = bridge_native_get_single(api, window_slot);
    SDL_Window *window = find_window(nullptr);

    round_now++;
    if (window)
        SDL_SyncWindow(window);
    SDL_PumpEvents();
    say(api, "before", w ? w->surface : 0, w ? count_fields(w) : 0);
    for (size_t i = 0; on && i < sizeof(STEPS) / sizeof(STEPS[0]); i++)
        if (STEPS[i].round == round_now && !strcmp(STEPS[i].scene, on->scene))
            do_step(api, &STEPS[i], s, window);
    return (0);
}

BRIDGE_NATIVE(check)
{
    const test_scene_t *on = bridge_native_get_single(api, scene_slot);
    bool known = on && is_scene(on->scene);

    if (on)
        say_reload_or_video(api, on);
    if (known)
        say_window(api, bridge_native_get_single(api, settings_slot),
            bridge_native_get_single(api, window_slot));
    if (known && round_now < ROUNDS)
        return (0);
    close_other();
    return (BRIDGE_END);
}

static void declare_types(const bridge_api_t *api)
{
    scene_slot = BRIDGE_NATIVE_COMPONENT(api, "test:Scene", test_scene_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_DECLARE(api, BRIDGE_ENUM, "sdl3:sdl3_place_t",
        sdl3_place_t, BRIDGE_STRIP, BRIDGE_TABLE, BRIDGE_ON_DESTROY_REMOVE);
    settings_slot = BRIDGE_NATIVE_COMPONENT(api, "sdl3:WindowSettings",
        sdl3_window_settings_t, BRIDGE_STRIP, BRIDGE_SINGLETON,
        BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_DECLARE(api, BRIDGE_ENUM, "sdl3:sdl3_surface_t",
        sdl3_surface_t, BRIDGE_STRIP, BRIDGE_TABLE, BRIDGE_ON_DESTROY_REMOVE);
    window_slot = BRIDGE_NATIVE_COMPONENT(api, "sdl3:Window", sdl3_window_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
}

BRIDGE_NATIVE(init)
{
    bridge_term_t drive[3] = { { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { 0, BRIDGE_WRITE, BRIDGE_OPTIONAL, 0 },
        { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 } };
    bridge_term_t check[3] = { { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 } };
    uint32_t changes[2] = {};

#ifdef HOLD
    SDL_InitSubSystem(SDL_INIT_EVENTS);
#else
    SDL_SetHint("BM_PROBE_HINT", "1");
#endif
    declare_types(api);
    drive[0].slot = scene_slot;
    drive[1].slot = settings_slot;
    drive[2].slot = window_slot;
    check[0].slot = scene_slot;
    check[1].slot = settings_slot;
    check[2].slot = window_slot;
    changes[0] = settings_slot;
    changes[1] = window_slot;
    BRIDGE_NATIVE_ADD_SYSTEM(api, "drive", "drive", drive, 3, changes,
        DETACHES);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "check", "check", check, 3, nullptr, 0);
    return (0);
}

BRIDGE_NATIVE(shutdown)
{
    close_other();
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
