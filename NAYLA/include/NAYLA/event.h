//
// Created by Vinícius Ferreira Aguiar on 13/01/25.
//

#ifndef NAYLA_EVENT_H
#define NAYLA_EVENT_H

#include <NAYLA/export.h>
#include <NAYLA/types.h>

typedef struct EventContext {
    union {
        int64   int64[2];
        uint64  uint64[2];
        float64 float64[2];

        int32   int32[4];
        uint32  uint32[4];
        float32 float32[4];

        int16  int16[8];
        uint16 uint16[8];

        char int8[16];
    } data;
} EventContext;

// Should return true if handled
typedef bool1 (* PFN_on_event)(uint16 code, void *sender, void *listener_instance, EventContext data);

N_EXPORT bool1 event_register  (uint16 code, void *listener, PFN_on_event on_event);
N_EXPORT bool1 event_unregister(uint16 code, void *listener, PFN_on_event on_event);
N_EXPORT bool1 event_fire      (uint16 code, void *sender, EventContext data);

#endif //NAYLA_EVENT_H
