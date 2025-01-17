//
// Created by Vinícius Ferreira Aguiar on 13/01/25.
//

#include <NAYLA/event.h>
#include <NAYLA/memory.h>
#include "_event.h"

#define MAX_EVENT_CODES  16384

typedef struct RegisteredEvent {
    void *listener;
    PFN_on_event on_event;
} RegisteredEvent;

typedef struct EventCodeEntry {
    RegisteredEvent *events;
} EventCodeEntry;

typedef struct EventSystemState {
    EventCodeEntry registered[MAX_EVENT_CODES];
} EventSystemState;

static bool1            g_isInitialized = false;
static EventSystemState g_eventState;

bool1 event_create() {
    if (g_isInitialized) {
        return false;
    }

    n_memory_zero(&g_eventState, sizeof(EventSystemState));
    g_isInitialized = true;

    return true;
}

void event_destroy() {
    if (! g_isInitialized) {
        return;
    }

    for (uint16 i = 0; i < MAX_EVENT_CODES; ++i) {
        if (g_eventState.registered[i].events) {
            n_memory_free(g_eventState.registered[i].events);

            g_eventState.registered[i].events = nullptr;
        }
    }

    n_memory_zero(&g_eventState, sizeof(EventSystemState));
    g_isInitialized = false;
}

bool1 event_register(const uint16 code, void *listener, PFN_on_event on_event) {
    if (! g_isInitialized) {
        return false;
    }

    if (code >= MAX_EVENT_CODES) {
        return false;
    }

    EventCodeEntry *entry = &g_eventState.registered[code];
    if (entry->events) {
        // TODO: warn
        return false;
    }

    entry->events = n_memory_alloc(sizeof(RegisteredEvent), MEMORY_TAG_DARRAY);
    RegisteredEvent *event = &entry->events[n_memory_size(entry->events) - 1];
    event->listener = listener;
    event->on_event = on_event;

    return true;
}

bool1 event_unregister(const uint16 code, void *listener, PFN_on_event on_event) {
    if (! g_isInitialized) {
        return false;
    }

    if (code >= MAX_EVENT_CODES) {
        return false;
    }

    EventCodeEntry *entry = &g_eventState.registered[code];
    if (! entry->events) {
        // TODO: warn
        return false;
    }

    for (uint64 i = 0; i < n_memory_size(entry->events); ++i) {
        RegisteredEvent *event = &entry->events[i];
        if (event->listener == listener && event->on_event == on_event) {
            // Found one, remove it
            n_memory_free(event, sizeof(RegisteredEvent), MEMORY_TAG_DARRAY);

            return true;
        }
    }

    // not found
    return false;
}