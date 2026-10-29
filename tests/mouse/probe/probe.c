/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "bridge_native.h"
#include "sdl3_mouse.h"

#define SCENE_FIELDS { char scene[32]; char module[512]; }

BRIDGE_SCHEMA_STRUCT_TYPE(test_scene_t, SCENE_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(test_scene_t, SCENE_FIELDS);

typedef struct {
    const char *name;
    uint32_t rounds;
} scene_t;

enum step_kind : uint8_t {
    STEP_PUSH,
    STEP_OPEN,
    STEP_SHOW,
    STEP_OWN,
    STEP_CURSOR,
    STEP_QUEUE,
    STEP_PUMP
};

typedef struct {
    const char *scene;
    uint32_t round;
    enum step_kind kind;
    SDL_EventType type;
    SDL_WindowID window;
    float x;
    float y;
    float xrel;
    float yrel;
    uint8_t button;
    SDL_MouseWheelDirection direction;
    sdl3_cursor_t cursor;
} step_t;

typedef struct {
    const char *scene;
    uint32_t round;
    bool natural;
} write_t;

constexpr SDL_WindowID MINE = 0;
constexpr SDL_WindowID GONE = 999;
constexpr uint32_t WINDOWS = 2;

[[gnu::section(MANIFEST_SECTION), gnu::used, gnu::retain]]
static const char WHO[] = MANIFEST;

static const scene_t SCENES[] = {
    { "quiet", 1 }, { "move", 2 }, { "gone", 1 }, { "click", 4 },
    { "edge", 1 }, { "over", 1 }, { "wheel", 2 }, { "flip", 1 },
    { "range", 1 }, { "own", 1 }, { "zero", 1 }, { "hide", 2 },
    { "relative", 2 }, { "later", 2 }, { "free", 2 }, { "shape", 1 },
    { "same", 2 }, { "new", 2 }, { "count", 2 }, { "natural", 2 },
    { "unnatural", 2 }, { "drop", 2 }, { "keep", 2 }, { "waiting", 2 },
    { "pumped", 1 }
};

static const step_t STEPS[] = {
    { .scene = "move", .round = 1, .kind = STEP_OPEN },
    { .scene = "move", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_MOTION, .window = MINE, .x = 10, .y = 20,
        .xrel = 3, .yrel = 4 },
    { .scene = "move", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_MOTION, .window = MINE, .x = 12, .y = 25,
        .xrel = 2, .yrel = 5 },
    { .scene = "gone", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_MOTION, .window = GONE, .x = 7, .y = 9,
        .xrel = 7, .yrel = 9 },
    { .scene = "click", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_BUTTON_DOWN, .button = 1 },
    { .scene = "click", .round = 3, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_BUTTON_UP, .button = 1 },
    { .scene = "edge", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_BUTTON_DOWN, .button = 32 },
    { .scene = "over", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_BUTTON_DOWN, .button = 0 },
    { .scene = "over", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_BUTTON_DOWN, .button = 33 },
    { .scene = "over", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_BUTTON_UP, .button = 0 },
    { .scene = "over", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_BUTTON_UP, .button = 33 },
    { .scene = "wheel", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_WHEEL, .x = 1, .y = 2,
        .direction = SDL_MOUSEWHEEL_NORMAL },
    { .scene = "wheel", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_WHEEL, .x = 2, .y = 3,
        .direction = SDL_MOUSEWHEEL_NORMAL },
    { .scene = "flip", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_WHEEL, .x = 1, .y = 2,
        .direction = SDL_MOUSEWHEEL_FLIPPED },
    { .scene = "range", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_ADDED },
    { .scene = "range", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_REMOVED },
    { .scene = "own", .round = 1, .kind = STEP_OPEN },
    { .scene = "own", .round = 1, .kind = STEP_OWN },
    { .scene = "zero", .round = 1, .kind = STEP_OPEN },
    { .scene = "zero", .round = 1, .kind = STEP_OWN },
    { .scene = "zero", .round = 1, .kind = STEP_CURSOR, .cursor = {} },
    { .scene = "hide", .round = 1, .kind = STEP_CURSOR,
        .cursor = { .hidden = true } },
    { .scene = "hide", .round = 2, .kind = STEP_CURSOR, .cursor = {} },
    { .scene = "relative", .round = 1, .kind = STEP_OPEN },
    { .scene = "relative", .round = 1, .kind = STEP_OPEN },
    { .scene = "relative", .round = 1, .kind = STEP_CURSOR,
        .cursor = { .relative = true } },
    { .scene = "later", .round = 1, .kind = STEP_OPEN },
    { .scene = "later", .round = 1, .kind = STEP_CURSOR,
        .cursor = { .relative = true } },
    { .scene = "later", .round = 2, .kind = STEP_OPEN },
    { .scene = "free", .round = 1, .kind = STEP_OPEN },
    { .scene = "free", .round = 1, .kind = STEP_OPEN },
    { .scene = "free", .round = 1, .kind = STEP_CURSOR,
        .cursor = { .relative = true } },
    { .scene = "free", .round = 2, .kind = STEP_CURSOR, .cursor = {} },
    { .scene = "shape", .round = 1, .kind = STEP_CURSOR,
        .cursor = { .shape = SDL_SYSTEM_CURSOR_POINTER } },
    { .scene = "same", .round = 1, .kind = STEP_CURSOR,
        .cursor = { .shape = SDL_SYSTEM_CURSOR_POINTER } },
    { .scene = "same", .round = 2, .kind = STEP_CURSOR,
        .cursor = { .shape = SDL_SYSTEM_CURSOR_POINTER } },
    { .scene = "new", .round = 1, .kind = STEP_CURSOR,
        .cursor = { .shape = SDL_SYSTEM_CURSOR_POINTER } },
    { .scene = "new", .round = 2, .kind = STEP_CURSOR,
        .cursor = { .shape = SDL_SYSTEM_CURSOR_CROSSHAIR } },
    { .scene = "count", .round = 1, .kind = STEP_CURSOR,
        .cursor = { .shape = SDL_SYSTEM_CURSOR_POINTER } },
    { .scene = "count", .round = 2, .kind = STEP_CURSOR,
        .cursor = { .shape = SDL_SYSTEM_CURSOR_COUNT } },
    { .scene = "natural", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_WHEEL, .x = 1, .y = 2,
        .direction = SDL_MOUSEWHEEL_FLIPPED },
    { .scene = "natural", .round = 2, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_WHEEL, .x = 1, .y = 2,
        .direction = SDL_MOUSEWHEEL_FLIPPED },
    { .scene = "unnatural", .round = 1, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_WHEEL, .x = 1, .y = 2,
        .direction = SDL_MOUSEWHEEL_FLIPPED },
    { .scene = "unnatural", .round = 2, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_WHEEL, .x = 1, .y = 2,
        .direction = SDL_MOUSEWHEEL_FLIPPED },
    { .scene = "drop", .round = 1, .kind = STEP_SHOW },
    { .scene = "drop", .round = 2, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_MOTION, .window = MINE, .x = 10, .y = 20,
        .xrel = 3, .yrel = 4 },
    { .scene = "drop", .round = 2, .kind = STEP_CURSOR,
        .cursor = { .relative = true } },
    { .scene = "keep", .round = 1, .kind = STEP_SHOW },
    { .scene = "keep", .round = 2, .kind = STEP_PUSH,
        .type = SDL_EVENT_MOUSE_MOTION, .window = MINE, .x = 10, .y = 20,
        .xrel = 3, .yrel = 4 },
    { .scene = "keep", .round = 2, .kind = STEP_CURSOR,
        .cursor = { .relative = false } },
    { .scene = "waiting", .round = 1, .kind = STEP_QUEUE,
        .type = SDL_EVENT_MOUSE_MOTION, .window = GONE, .x = 7, .y = 9,
        .xrel = 3, .yrel = 4 },
    { .scene = "waiting", .round = 2, .kind = STEP_PUMP },
    { .scene = "pumped", .round = 1, .kind = STEP_QUEUE,
        .type = SDL_EVENT_MOUSE_MOTION, .window = GONE, .x = 7, .y = 9,
        .xrel = 3, .yrel = 4 },
    { .scene = "pumped", .round = 1, .kind = STEP_PUMP }
};

static uint32_t scene_slot;
static uint32_t cursor_slot;
static uint32_t mouse_slot;
static uint32_t round_now;
static SDL_Window *windows[WINDOWS];
static uint32_t window_count;
static SDL_Cursor *seen;

#ifdef WRITE
constexpr uint32_t DRIVE_TERMS = 3;

static const write_t WRITES[] = { { "natural", 2, true } };

static uint32_t settings_slot;

static void write_settings(const bridge_api_t *api, const test_scene_t *on)
{
    sdl3_mouse_settings_t *settings = bridge_native_get_single(api,
        settings_slot);

    for (size_t i = 0; settings && i < sizeof(WRITES) / sizeof(WRITES[0]); i++)
        if (WRITES[i].round == round_now && !strcmp(WRITES[i].scene, on->scene))
            settings->natural = WRITES[i].natural;
}
#else
constexpr uint32_t DRIVE_TERMS = 2;
#endif

static void say(const bridge_api_t *api, const char *label, uint32_t a0,
    uint32_t a1)
{
    char text[128];
    bridge_line_t line = { text, 0, a0, a1 };

    snprintf(text, sizeof(text), "r%u %s", round_now, label);
    line.len = (uint32_t)strlen(text);
    bridge_native_log(api, BRIDGE_LOG_INFO, &line);
}

static void say_pair(const bridge_api_t *api, const char *name, float a,
    float b)
{
    char label[96];

    snprintf(label, sizeof(label), "%s %g %g", name, (double)a, (double)b);
    say(api, label, 0, 0);
}

static void open_window(SDL_WindowFlags flags)
{
    SDL_Window *window = nullptr;

    if (window_count < WINDOWS)
        window = SDL_CreateWindow("probe", 64, 64, flags);
    if (!window)
        return;
    windows[window_count] = window;
    window_count++;
}

static void close_windows(void)
{
    for (uint32_t i = 0; i < window_count; i++)
        SDL_DestroyWindow(windows[i]);
    window_count = 0;
}

static void own_cursor(void)
{
    SDL_HideCursor();
    for (uint32_t i = 0; i < window_count; i++)
        SDL_SetWindowRelativeMouseMode(windows[i], true);
}

static void set_cursor(const bridge_api_t *api, const sdl3_cursor_t *value)
{
    sdl3_cursor_t *cursor = bridge_native_get_single(api, cursor_slot);

    if (cursor)
        *cursor = *value;
    else
        BRIDGE_NATIVE_ATTACH(api, 0, cursor_slot, *value);
}

static SDL_WindowID get_window_id(SDL_WindowID window)
{
    if (window == MINE && window_count)
        return (SDL_GetWindowID(windows[0]));
    return (window);
}

static void push_event(const step_t *s)
{
    SDL_Event ev = { .type = s->type };
    bool down = s->type == SDL_EVENT_MOUSE_BUTTON_DOWN;

    if (s->type == SDL_EVENT_MOUSE_MOTION)
        ev.motion = (SDL_MouseMotionEvent){ .type = s->type,
            .windowID = get_window_id(s->window), .x = s->x, .y = s->y,
            .xrel = s->xrel, .yrel = s->yrel };
    if (down || s->type == SDL_EVENT_MOUSE_BUTTON_UP)
        ev.button = (SDL_MouseButtonEvent){ .type = s->type,
            .button = s->button, .down = down };
    if (s->type == SDL_EVENT_MOUSE_WHEEL)
        ev.wheel = (SDL_MouseWheelEvent){ .type = s->type, .x = s->x,
            .y = s->y, .direction = s->direction };
    SDL_PushEvent(&ev);
}

static void SDLCALL push_queued(void *data)
{
    push_event(data);
}

static int SDLCALL queue_from_helper(void *data)
{
    SDL_RunOnMainThread(push_queued, data, false);
    return (0);
}

static void do_step(const bridge_api_t *api, const step_t *s)
{
    switch (s->kind) {
        case STEP_PUSH:
            push_event(s);
            break;
        case STEP_OPEN:
            open_window(SDL_WINDOW_HIDDEN);
            break;
        case STEP_SHOW:
            open_window(0);
            break;
        case STEP_OWN:
            own_cursor();
            break;
        case STEP_CURSOR:
            set_cursor(api, &s->cursor);
            break;
        case STEP_QUEUE:
            SDL_WaitThread(SDL_CreateThread(queue_from_helper, "probe",
                (void *)s), nullptr);
            break;
        case STEP_PUMP:
            SDL_PumpEvents();
            break;
    }
}

static uint32_t count_set(const void *at, size_t len)
{
    const uint8_t *bytes = at;
    uint32_t set = 0;

    for (size_t i = 0; i < len; i++)
        set += bytes[i] != 0;
    return (set);
}

static void say_relative(const bridge_api_t *api)
{
    int count = 0;
    SDL_Window **list = SDL_GetWindows(&count);
    uint32_t relative = 0;

    for (int i = 0; i < count; i++)
        relative += SDL_GetWindowRelativeMouseMode(list[i]);
    SDL_free(list);
    say(api, "relative", relative, (uint32_t)count);
}

static void say_cursor(const bridge_api_t *api)
{
    SDL_Cursor *now = SDL_GetCursor();

    say(api, "cursor", now != SDL_GetDefaultCursor(), now == seen);
    seen = now;
    say(api, "visible", SDL_CursorVisible(), 0);
    say_relative(api);
}

static void say_mouse(const bridge_api_t *api, const sdl3_mouse_t *m)
{
    uint32_t set = m ? count_set(m, sizeof(*m)) : 0;

    say(api, "mouse", m != nullptr, set);
    if (!m)
        return;
    say_pair(api, "position", m->x, m->y);
    say_pair(api, "motion", m->dx, m->dy);
    say_pair(api, "wheel", m->wheel_x, m->wheel_y);
    say(api, "buttons", m->held, m->pressed);
    say(api, "released", m->released, 0);
    say_cursor(api);
    say(api, "queue", SDL_HasEvents(SDL_EVENT_MOUSE_MOTION,
        SDL_EVENT_MOUSE_REMOVED), 0);
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

BRIDGE_NATIVE(drive)
{
    const test_scene_t *on = bridge_native_get_single(api, scene_slot);

    round_now++;
#ifdef WRITE
    if (on)
        write_settings(api, on);
#endif
    for (size_t i = 0; on && i < sizeof(STEPS) / sizeof(STEPS[0]); i++)
        if (STEPS[i].round == round_now && !strcmp(STEPS[i].scene, on->scene))
            do_step(api, &STEPS[i]);
    return (0);
}

static void say_reload_or_video(const bridge_api_t *api,
    const test_scene_t *on)
{
    if (!strcmp(on->scene, "reload"))
        say(api, "reload", (uint32_t)call_reload(on->module), 0);
    if (!strcmp(on->scene, "video"))
        say(api, "video", SDL_WasInit(SDL_INIT_VIDEO) != 0, 0);
}

BRIDGE_NATIVE(check)
{
    const test_scene_t *on = bridge_native_get_single(api, scene_slot);
    const scene_t *scene = on ? find_scene(on->scene) : nullptr;

    if (on)
        say_reload_or_video(api, on);
    if (scene)
        say_mouse(api, bridge_native_get_single(api, mouse_slot));
    if (scene && round_now < scene->rounds)
        return (0);
    close_windows();
    return (BRIDGE_END);
}

static void declare_types(const bridge_api_t *api)
{
    scene_slot = BRIDGE_NATIVE_COMPONENT(api, "test:Scene", test_scene_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
    cursor_slot = BRIDGE_NATIVE_COMPONENT(api, "sdl3:Cursor", sdl3_cursor_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
    mouse_slot = BRIDGE_NATIVE_COMPONENT(api, "sdl3:Mouse", sdl3_mouse_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON, BRIDGE_ON_DESTROY_REMOVE);
#ifdef WRITE
    settings_slot = BRIDGE_NATIVE_COMPONENT(api, "sdl3:MouseSettings",
        sdl3_mouse_settings_t, BRIDGE_STRIP, BRIDGE_SINGLETON,
        BRIDGE_ON_DESTROY_REMOVE);
#endif
}

BRIDGE_NATIVE(init)
{
    bridge_term_t drive[3] = {};
    bridge_term_t check[2] = {};

#ifdef HOLD
    SDL_InitSubSystem(SDL_INIT_EVENTS);
#else
    SDL_SetHint("BM_PROBE_HINT", "1");
#endif
    declare_types(api);
    drive[0] = (bridge_term_t){ scene_slot, BRIDGE_READ, BRIDGE_OPTIONAL, 0 };
    drive[1] = (bridge_term_t){ cursor_slot, BRIDGE_WRITE, BRIDGE_OPTIONAL,
        0 };
#ifdef WRITE
    drive[2] = (bridge_term_t){ settings_slot, BRIDGE_WRITE, BRIDGE_OPTIONAL,
        0 };
#endif
    check[0] = (bridge_term_t){ scene_slot, BRIDGE_READ, BRIDGE_OPTIONAL, 0 };
    check[1] = (bridge_term_t){ mouse_slot, BRIDGE_READ, BRIDGE_OPTIONAL, 0 };
    seen = SDL_GetCursor();
    BRIDGE_NATIVE_ADD_SYSTEM(api, "drive", "drive", drive, DRIVE_TERMS,
        &cursor_slot, 1, nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "check", "check", check, 2, nullptr, 0,
        nullptr, 0);
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
