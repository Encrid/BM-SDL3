/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "bridge_native.h"
#include "sdl3_keyboard.h"

#define SCENE_FIELDS { char scene[32]; char module[512]; }

BRIDGE_SCHEMA_STRUCT_TYPE(test_scene_t, SCENE_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(test_scene_t, SCENE_FIELDS);

typedef struct {
    const char *name;
    uint32_t rounds;
    uint32_t code;
} scene_t;

typedef struct {
    const char *scene;
    uint32_t round;
    SDL_EventType type;
    SDL_Scancode code;
    SDL_Keymod mod;
    bool repeat;
    const char *text;
} step_t;

typedef struct {
    const char *scene;
    uint32_t round;
    bool text;
    bool repeat;
} write_t;

constexpr SDL_EventType OPEN = SDL_EVENT_FIRST;
constexpr SDL_EventType START = (SDL_EventType)1;
constexpr SDL_EventType QUEUE = (SDL_EventType)2;
constexpr SDL_EventType PUMP = (SDL_EventType)3;
constexpr uint32_t WINDOWS = 2;

[[gnu::section(MANIFEST_SECTION), gnu::used, gnu::retain]]
static const char WHO[] = MANIFEST;

static const char B40[] = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
static const char B30[] = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
static const char B23[] = "bbbbbbbbbbbbbbbbbbbbbbb";
static const char A62E[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\xc3\xa9";

static const scene_t SCENES[] = {
    { "quiet", 1, 4 }, { "keys", 4, 4 }, { "mods", 1, 4 },
    { "edge", 1, 511 }, { "over", 1, 511 }, { "range", 1, 4 },
    { "windows", 1, 4 }, { "later", 2, 4 }, { "typed", 2, 4 },
    { "cut", 4, 4 }, { "started", 1, 4 }, { "stop", 2, 4 },
    { "stay", 2, 4 }, { "repeat", 2, 4 }, { "held", 2, 4 },
    { "waiting", 2, 4 }, { "pumped", 1, 4 }
};

static const step_t STEPS[] = {
    { "keys", 1, SDL_EVENT_KEY_DOWN, 4, 0, false, nullptr },
    { "keys", 3, SDL_EVENT_KEY_DOWN, 4, 0, true, nullptr },
    { "keys", 4, SDL_EVENT_KEY_UP, 4, 0, false, nullptr },
    { "mods", 1, SDL_EVENT_KEY_DOWN, 4, SDL_KMOD_LCTRL, false, nullptr },
    { "mods", 1, SDL_EVENT_KEY_UP, 5, SDL_KMOD_LSHIFT, false, nullptr },
    { "edge", 1, SDL_EVENT_KEY_DOWN, 511, SDL_KMOD_LSHIFT, false, nullptr },
    { "over", 1, SDL_EVENT_KEY_DOWN, 512, SDL_KMOD_LSHIFT, false, nullptr },
    { "range", 1, SDL_EVENT_TEXT_EDITING, 0, 0, false, "x" },
    { "range", 1, SDL_EVENT_KEYMAP_CHANGED, 0, 0, false, nullptr },
    { "range", 1, SDL_EVENT_KEYBOARD_ADDED, 0, 0, false, nullptr },
    { "range", 1, SDL_EVENT_SCREEN_KEYBOARD_HIDDEN, 0, 0, false, nullptr },
    { "windows", 1, OPEN, 0, 0, false, nullptr },
    { "windows", 1, OPEN, 0, 0, false, nullptr },
    { "later", 1, OPEN, 0, 0, false, nullptr },
    { "later", 2, OPEN, 0, 0, false, nullptr },
    { "typed", 1, SDL_EVENT_TEXT_INPUT, 0, 0, false, "caf" },
    { "typed", 1, SDL_EVENT_TEXT_INPUT, 0, 0, false, "\xc3\xa9" },
    { "cut", 1, SDL_EVENT_TEXT_INPUT, 0, 0, false, B40 },
    { "cut", 1, SDL_EVENT_TEXT_INPUT, 0, 0, false, B30 },
    { "cut", 2, SDL_EVENT_TEXT_INPUT, 0, 0, false, A62E },
    { "cut", 3, SDL_EVENT_TEXT_INPUT, 0, 0, false, A62E },
    { "cut", 3, SDL_EVENT_TEXT_INPUT, 0, 0, false, "c" },
    { "cut", 4, SDL_EVENT_TEXT_INPUT, 0, 0, false, B40 },
    { "cut", 4, SDL_EVENT_TEXT_INPUT, 0, 0, false, B23 },
    { "cut", 4, SDL_EVENT_TEXT_INPUT, 0, 0, false, "c" },
    { "started", 1, OPEN, 0, 0, false, nullptr },
    { "started", 1, OPEN, 0, 0, false, nullptr },
    { "started", 1, START, 0, 0, false, nullptr },
    { "stop", 1, OPEN, 0, 0, false, nullptr },
    { "stop", 1, OPEN, 0, 0, false, nullptr },
    { "stay", 1, OPEN, 0, 0, false, nullptr },
    { "stay", 1, OPEN, 0, 0, false, nullptr },
    { "repeat", 1, SDL_EVENT_KEY_DOWN, 4, 0, false, nullptr },
    { "repeat", 2, SDL_EVENT_KEY_DOWN, 4, 0, true, nullptr },
    { "held", 1, SDL_EVENT_KEY_DOWN, 4, 0, false, nullptr },
    { "held", 2, SDL_EVENT_KEY_DOWN, 4, 0, true, nullptr },
    { "waiting", 1, QUEUE, 4, 0, false, nullptr },
    { "waiting", 2, PUMP, 0, 0, false, nullptr },
    { "pumped", 1, QUEUE, 4, 0, false, nullptr },
    { "pumped", 1, PUMP, 0, 0, false, nullptr }
};

static uint32_t scene_slot;
static uint32_t keyboard_slot;
static uint32_t round_now;
static SDL_Window *windows[WINDOWS];
static uint32_t window_count;

#ifdef WRITE
constexpr uint32_t DRIVE_TERMS = 2;

static const write_t WRITES[] = {
    { "stop", 2, false, false }, { "repeat", 2, true, true }
};

static uint32_t settings_slot;

static void write_settings(const bridge_api_t *api, const test_scene_t *on)
{
    sdl3_keyboard_settings_t *settings = bridge_native_get_single(api,
        settings_slot);

    for (size_t i = 0; settings && i < sizeof(WRITES) / sizeof(WRITES[0]); i++)
        if (WRITES[i].round == round_now && !strcmp(WRITES[i].scene, on->scene))
            *settings = (sdl3_keyboard_settings_t){ .text = WRITES[i].text,
                .repeat = WRITES[i].repeat };
}
#else
constexpr uint32_t DRIVE_TERMS = 1;
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

static void open_window(void)
{
    SDL_Window *window = nullptr;

    if (window_count < WINDOWS)
        window = SDL_CreateWindow("probe", 64, 64, SDL_WINDOW_HIDDEN);
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

static void start_windows(void)
{
    for (uint32_t i = 0; i < window_count; i++)
        SDL_StartTextInput(windows[i]);
}

static void push_step(const step_t *s)
{
    SDL_Event ev = { .type = s->type };

    if (s->type == SDL_EVENT_KEY_DOWN || s->type == SDL_EVENT_KEY_UP)
        ev.key = (SDL_KeyboardEvent){ .type = s->type, .scancode = s->code,
            .mod = s->mod, .down = s->type == SDL_EVENT_KEY_DOWN,
            .repeat = s->repeat };
    if (s->type == SDL_EVENT_TEXT_INPUT)
        ev.text.text = s->text;
    if (s->type == SDL_EVENT_TEXT_EDITING)
        ev.edit.text = s->text;
    SDL_PushEvent(&ev);
}

static void SDLCALL push_queued(void *data)
{
    const step_t *s = data;
    SDL_Event ev = { .key = { .type = SDL_EVENT_KEY_DOWN,
        .scancode = s->code, .down = true } };

    SDL_PushEvent(&ev);
}

static int SDLCALL queue_from_helper(void *data)
{
    SDL_RunOnMainThread(push_queued, data, false);
    return (0);
}

static void do_step(const step_t *s)
{
    if (s->type == OPEN)
        open_window();
    if (s->type == START)
        start_windows();
    if (s->type == QUEUE)
        SDL_WaitThread(SDL_CreateThread(queue_from_helper, "probe",
            (void *)s), nullptr);
    if (s->type == PUMP)
        SDL_PumpEvents();
    if (s->type > PUMP)
        push_step(s);
}

static uint32_t count_set(const void *at, size_t len)
{
    const uint8_t *bytes = at;
    uint32_t set = 0;

    for (size_t i = 0; i < len; i++)
        set += bytes[i] != 0;
    return (set);
}

static void say_input(const bridge_api_t *api)
{
    int count = 0;
    SDL_Window **list = SDL_GetWindows(&count);
    uint32_t active = 0;

    for (int i = 0; i < count; i++)
        active += SDL_TextInputActive(list[i]);
    SDL_free(list);
    say(api, "input", active, (uint32_t)count);
}

static void say_keyboard(const bridge_api_t *api, const sdl3_keyboard_t *kb,
    uint32_t code)
{
    char label[96];
    uint32_t set = kb ? count_set(kb, sizeof(*kb)) : 0;

    say(api, "keyboard", kb != nullptr, set);
    if (!kb)
        return;
    snprintf(label, sizeof(label), "key%u", code);
    say(api, label, kb->key[code], kb->hit[code]);
    say(api, "mods", kb->mods, count_set(kb->key, sizeof(kb->key)));
    snprintf(label, sizeof(label), "text \"%.63s\"", kb->text);
    say(api, label, kb->text_len,
        (uint32_t)SDL_strnlen(kb->text, sizeof(kb->text)));
    say_input(api);
    say(api, "queue", SDL_HasEvents(SDL_EVENT_KEY_DOWN,
        SDL_EVENT_SCREEN_KEYBOARD_HIDDEN), 0);
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
            do_step(&STEPS[i]);
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
        say_keyboard(api, bridge_native_get_single(api, keyboard_slot),
            scene->code);
    if (scene && round_now < scene->rounds)
        return (0);
    close_windows();
    return (BRIDGE_END);
}

BRIDGE_NATIVE(init)
{
    bridge_term_t drive[2] = { { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { 0, BRIDGE_WRITE, BRIDGE_OPTIONAL, 0 } };
    bridge_term_t check[2] = { { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 } };

#ifdef HOLD
    SDL_InitSubSystem(SDL_INIT_EVENTS);
#else
    SDL_SetHint("BM_PROBE_HINT", "1");
#endif
    scene_slot = BRIDGE_NATIVE_COMPONENT(api, "test:Scene", test_scene_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON);
    keyboard_slot = BRIDGE_NATIVE_COMPONENT(api, "sdl3:Keyboard",
        sdl3_keyboard_t, BRIDGE_STRIP, BRIDGE_SINGLETON);
#ifdef WRITE
    settings_slot = BRIDGE_NATIVE_COMPONENT(api, "sdl3:KeyboardSettings",
        sdl3_keyboard_settings_t, BRIDGE_STRIP, BRIDGE_SINGLETON);
    drive[1].slot = settings_slot;
#endif
    drive[0].slot = scene_slot;
    check[0].slot = scene_slot;
    check[1].slot = keyboard_slot;
    BRIDGE_NATIVE_ADD_SYSTEM(api, "drive", "drive", drive, DRIVE_TERMS,
        nullptr, 0, nullptr, 0);
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
