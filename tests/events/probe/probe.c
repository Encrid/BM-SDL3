/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "bridge_native.h"
#include "sdl3_events.h"

#define SCENE_FIELDS { char scene[32]; char module[512]; }

BRIDGE_SCHEMA_STRUCT_TYPE(test_scene_t, SCENE_FIELDS);
BRIDGE_SCHEMA_STRUCT_DESC(test_scene_t, SCENE_FIELDS);

typedef struct {
    const char *name;
    uint32_t rounds;
} scene_t;

typedef struct {
    const char *scene;
    uint32_t round;
    SDL_EventType type;
    const char *path;
    uint32_t count;
} step_t;

[[gnu::section(MANIFEST_SECTION), gnu::used, gnu::retain]]
static const char WHO[] = MANIFEST;

static char edge_path[1024];
static char over_path[1025];

static const scene_t SCENES[] = {
    { "quiet", 1 }, { "flush", 2 }, { "files", 3 }, { "twice", 3 },
    { "nopath", 2 }, { "empty", 2 }, { "others", 3 }, { "edge", 2 },
    { "over", 2 }, { "many", 2 }, { "three", 2 }, { "twenty", 2 },
    { "quit", 3 }
};

static const step_t STEPS[] = {
    { "flush", 1, SDL_EVENT_USER, nullptr, 1 },
    { "files", 1, SDL_EVENT_DROP_FILE, "dropped/a.txt", 1 },
    { "files", 1, SDL_EVENT_DROP_FILE, "dropped/caf\xc3\xa9.txt", 1 },
    { "twice", 1, SDL_EVENT_DROP_FILE, "dropped/a.txt", 1 },
    { "twice", 2, SDL_EVENT_DROP_FILE, "dropped/a.txt", 1 },
    { "nopath", 1, SDL_EVENT_DROP_FILE, nullptr, 1 },
    { "nopath", 1, SDL_EVENT_DROP_FILE, "dropped/a.txt", 1 },
    { "empty", 1, SDL_EVENT_DROP_FILE, "", 1 },
    { "empty", 1, SDL_EVENT_DROP_FILE, "dropped/a.txt", 1 },
    { "others", 1, SDL_EVENT_DROP_BEGIN, nullptr, 1 },
    { "others", 1, SDL_EVENT_DROP_POSITION, nullptr, 1 },
    { "others", 1, SDL_EVENT_DROP_TEXT, "dropped text", 1 },
    { "others", 1, SDL_EVENT_DROP_FILE, "dropped/a.txt", 1 },
    { "others", 1, SDL_EVENT_DROP_COMPLETE, nullptr, 1 },
    { "edge", 1, SDL_EVENT_DROP_FILE, edge_path, 1 },
    { "over", 1, SDL_EVENT_DROP_FILE, over_path, 1 },
    { "many", 1, SDL_EVENT_DROP_FILE, "dropped/a.txt", 18 },
    { "three", 1, SDL_EVENT_DROP_FILE, "dropped/a.txt", 3 },
    { "twenty", 1, SDL_EVENT_DROP_FILE, "dropped/a.txt", 20 },
    { "quit", 1, SDL_EVENT_QUIT, nullptr, 1 }
};

constexpr size_t STEP_COUNT = sizeof(STEPS) / sizeof(STEPS[0]);

static uint32_t scene_slot;
static uint32_t drop_slot;
static uint32_t check_slot;
static uint32_t round_now;

#ifdef STARVE
static SDL_malloc_func real_malloc;
static SDL_calloc_func real_calloc;
static SDL_realloc_func real_realloc;
static SDL_free_func real_free;

static void *SDLCALL refuse_malloc([[maybe_unused]] size_t size)
{
    return (nullptr);
}

static void *SDLCALL refuse_calloc([[maybe_unused]] size_t count,
    [[maybe_unused]] size_t size)
{
    return (nullptr);
}

static void *SDLCALL refuse_realloc([[maybe_unused]] void *at,
    [[maybe_unused]] size_t size)
{
    return (nullptr);
}

static void starve_sdl(void)
{
    SDL_SetError("the error buffer of this thread is made before the starve");
    SDL_GetOriginalMemoryFunctions(&real_malloc, &real_calloc, &real_realloc,
        &real_free);
    SDL_SetMemoryFunctions(refuse_malloc, refuse_calloc, refuse_realloc,
        real_free);
}

static void feed_sdl(void)
{
    SDL_SetMemoryFunctions(real_malloc, real_calloc, real_realloc, real_free);
}
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

static bool is_now(const step_t *s, const test_scene_t *on)
{
    return (s->round == round_now && !strcmp(s->scene, on->scene));
}

static void SDLCALL push_step(void *data)
{
    const step_t *s = data;
    SDL_Event ev = { .drop = { .type = s->type, .data = s->path } };

    SDL_PushEvent(&ev);
}

static void queue_step(const step_t *s)
{
    for (uint32_t i = 0; i < s->count; i++)
        SDL_RunOnMainThread(push_step, (void *)s, false);
}

static int SDLCALL queue_steps(void *data)
{
    const test_scene_t *on = data;

    for (size_t i = 0; i < STEP_COUNT; i++)
        if (is_now(&STEPS[i], on) && STEPS[i].type != SDL_EVENT_USER)
            queue_step(&STEPS[i]);
    return (0);
}

static void say_rows(const bridge_api_t *api, const sdl3_drop_t *drops,
    uint32_t rows)
{
    char label[48];

    for (uint32_t r = 0; r < rows; r++) {
        snprintf(label, sizeof(label), "drop \"%.32s\"", drops[r].path);
        say(api, label, (uint32_t)SDL_strnlen(drops[r].path,
            sizeof(drops[r].path)), 0);
    }
}

static void say_round(const bridge_api_t *api)
{
    const bridge_query_t *q = bridge_native_get_query(api, check_slot);
    const bridge_header_t *h = bridge_native_get_header(api);
    const uint32_t *bases = (const uint32_t *)(api->private_base
        + h->column_base);
    const uint32_t *span;
    uint32_t count = 0;

    for (uint32_t s = 0; s < q->spans; s++) {
        span = bridge_native_get_span(api, q, s, 2);
        say_rows(api, (const sdl3_drop_t *)(api->shared_base
            + bases[span[0] * h->types + drop_slot]), span[1]);
        count += span[1];
    }
    say(api, "drops", count, 0);
    say(api, "others", (uint32_t)SDL_PeepEvents(nullptr, 0, SDL_PEEKEVENT,
        SDL_EVENT_DROP_TEXT, SDL_EVENT_DROP_POSITION), 0);
    say(api, "user", SDL_HasEvent(SDL_EVENT_USER), 0);
    say(api, "quit", SDL_HasEvent(SDL_EVENT_QUIT), 0);
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

static void say_reload_or_events(const bridge_api_t *api,
    const test_scene_t *on)
{
    if (!strcmp(on->scene, "reload"))
        say(api, "reload", (uint32_t)call_reload(on->module), 0);
    if (!strcmp(on->scene, "events"))
        say(api, "events", SDL_WasInit(SDL_INIT_EVENTS) != 0, 0);
}

BRIDGE_NATIVE(drive)
{
    const test_scene_t *on = bridge_native_get_single(api, scene_slot);
    SDL_Thread *helper = nullptr;

#ifdef STARVE
    feed_sdl();
#endif
    round_now++;
    for (size_t i = 0; on && i < STEP_COUNT; i++)
        if (is_now(&STEPS[i], on) && STEPS[i].type == SDL_EVENT_USER)
            push_step((void *)&STEPS[i]);
    if (on)
        helper = SDL_CreateThread(queue_steps, "probe", (void *)on);
    SDL_WaitThread(helper, nullptr);
    return (0);
}

BRIDGE_NATIVE(check)
{
    const test_scene_t *on = bridge_native_get_single(api, scene_slot);
    const scene_t *scene = on ? find_scene(on->scene) : nullptr;

    if (on)
        say_reload_or_events(api, on);
    if (scene)
        say_round(api);
    if (scene && round_now < scene->rounds)
        return (0);
    return (BRIDGE_END);
}

BRIDGE_NATIVE(init)
{
    bridge_term_t drive[1] = { { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 } };
    bridge_term_t check[2] = { { 0, BRIDGE_READ, BRIDGE_OPTIONAL, 0 },
        { 0, BRIDGE_READ, BRIDGE_AND, 0 } };

#ifdef HOLD
    SDL_InitSubSystem(SDL_INIT_EVENTS);
#else
    SDL_SetHint("BM_PROBE_HINT", "1");
#endif
#ifdef STARVE
    starve_sdl();
#endif
    memset(edge_path, 'a', sizeof(edge_path) - 1);
    memset(over_path, 'a', sizeof(over_path) - 1);
    scene_slot = BRIDGE_NATIVE_COMPONENT(api, "test:Scene", test_scene_t,
        BRIDGE_STRIP, BRIDGE_SINGLETON);
    drop_slot = BRIDGE_NATIVE_COMPONENT(api, "sdl3:Drop", sdl3_drop_t,
        BRIDGE_STRIP, BRIDGE_TABLE);
    drive[0].slot = scene_slot;
    check[0].slot = scene_slot;
    check[1].slot = drop_slot;
    BRIDGE_NATIVE_ADD_SYSTEM(api, "drive", "drive", drive, 1, nullptr, 0);
    check_slot = BRIDGE_NATIVE_ADD_SYSTEM(api, "check", "check", check, 2,
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
