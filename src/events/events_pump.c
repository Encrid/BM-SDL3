/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "events_inside.h"

static void destroy_drops(const bridge_api_t *api)
{
    const bridge_header_t *h = bridge_native_get_header(api);
    const bridge_query_t *q = bridge_native_get_query(api, h->system);
    const uint32_t *blocks = (const uint32_t *)(api->private_base + h->blocks);
    const uint32_t *span = nullptr;
    const uint64_t *ids = nullptr;

    for (uint32_t s = 0; s < q->spans; s++) {
        span = bridge_native_get_span(api, q, s, 2);
        ids = (const uint64_t *)(api->shared_base + blocks[span[0]]);
        for (uint32_t r = 0; r < span[1]; r++)
            api->destroy(api, ids[r]);
    }
}

static void keep_drop(const bridge_api_t *api, const char *path)
{
    sdl3_drop_t drop = {};
    size_t len = 0;

    if (!path || !*path)
        return;
    len = strlen(path);
    if (len >= sizeof(drop.path)) {
        bridge_native_say(api, BRIDGE_LOG_WARN,
            "a dropped path of %zu bytes is left out, and 1023 is the most",
            len);
        return;
    }
    memcpy(drop.path, path, len);
    BRIDGE_NATIVE_ATTACH(api, api->spawn(api), DROP, drop);
}

static void say_left(const bridge_api_t *api, int rest, uint32_t most)
{
    bridge_native_say(api, BRIDGE_LOG_WARN,
        "the dropped files past %u in a round are left out: %d", most, rest);
}

static void take_drops(const bridge_api_t *api, uint32_t most)
{
    SDL_Event e = {};
    uint32_t taken = 0;
    int rest = 0;

    while (taken < most && SDL_PeepEvents(&e, 1, SDL_GETEVENT,
        SDL_EVENT_DROP_FILE, SDL_EVENT_DROP_FILE) == 1) {
        keep_drop(api, e.drop.data);
        taken++;
    }
    rest = SDL_PeepEvents(nullptr, 0, SDL_PEEKEVENT, SDL_EVENT_DROP_FILE,
        SDL_EVENT_DROP_FILE);
    if (rest > 0)
        say_left(api, rest, most);
}

BRIDGE_NATIVE(pump)
{
    const sdl3_events_settings_t *settings = bridge_native_get_single(api,
        SETTINGS);

    SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
    destroy_drops(api);
    SDL_PumpEvents();
    if (!settings)
        return (0);
    take_drops(api, settings->drops);
    if (settings->quit && SDL_HasEvent(SDL_EVENT_QUIT))
        return (BRIDGE_END);
    return (0);
}
