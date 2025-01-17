//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include <NAYLA/logger.h>
#include <NAYLA/memory.h>

#include <PAL/memory.h>

#include "_memory.h"

#include <stdio.h>
#include <string.h>

struct memory_stats {
    uint64 total_allocated;
    uint64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static struct memory_stats stats;

char *memory_tag_to_string(const EMemoryTag i) {
    switch (i) {
        case MEMORY_TAG_ARRAY:
            return "Array";

        case MEMORY_TAG_DARRAY:
            return "Dynamic array";

        case MEMORY_TAG_DICT:
            return "Dictionary";

        case MEMORY_TAG_RING_QUEUE:
            return "Ring Queue";

        case MEMORY_TAG_BST:
            return "Binary Search Tree";

        case MEMORY_TAG_STRING:
            return "String";

        case MEMORY_TAG_APPLICATION:
            return "Application";

        case MEMORY_TAG_JOB:
            return "Job";

        case MEMORY_TAG_TEXTURE:
            return "Texture";

        case MEMORY_TAG_MATERIAL_INSTANCE:
            return "Material Instance";

        case MEMORY_TAG_RENDERER:
            return "Renderer";

        case MEMORY_TAG_GAME:
            return "Game";

        case MEMORY_TAG_TRANSFORM:
            return "Transform";

        case MEMORY_TAG_ENTITY:
            return "Entity";

        case MEMORY_TAG_ENTITY_NODE:
            return "Entity Node";

        case MEMORY_TAG_SCENE:
            return "Scene";

        default:
            return "Unknown";
    }
}

bool1 memory_create() {
    stats.total_allocated = 0;
    for (uint32 i = 0; i < MEMORY_TAG_MAX_TAGS; i++) {
        stats.tagged_allocations[i] = 0;
    }

    return true;
}

void memory_destroy() {
    if (stats.total_allocated != 0) {
        N_WARNING("memory_destroy: Memory leak detected.");
    }
}

void *n_memory_alloc(const uint64 size, const EMemoryTag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        N_WARNING("n_memory_alloc: Called using MEMORY_TAG_UNKNOWN. Re-class the allocation.");
    }

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;

    void *block = pal_memory_alloc(size, false);
    if (block == nullptr) {
        N_ERROR("n_memory_alloc: Failed to allocate memory.");
        return nullptr;
    }

    pal_memory_zero(block, size);

    return block;
}

void n_memory_free(void *block, const uint64 size, const EMemoryTag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        N_WARNING("n_memory_free: Called using MEMORY_TAG_UNKNOWN. Re-class the allocation.");
    }

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

    pal_memory_free(block, size);
}

void *n_memory_copy(void *destination, const void *source, const uint64 size) {
    return pal_memory_copy(destination, source, size);
}

void *n_memory_zero(void *block, const uint64 size) {
    return n_memory_set(block, 0, size);
}

void *n_memory_set(void *block, const uint8 value, const uint64 size) {
    return pal_memory_set(block, value, size);
}

char *n_memory_get_usage() {
    const uint64 gib = 1024 * 1024 * 1024;
    const uint64 mib = 1024 * 1024;
    const uint64 kib = 1024;

    static char buffer[1024] = "System memory use (tagged):\n";

    uint64 offset = strlen(buffer);

    for (uint32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i) {
        char unit[4] = "XiB";
        float32 amount = 1.0f;

        if (stats.tagged_allocations[i] > gib) {
            amount = (float32) stats.tagged_allocations[i] / (float32) gib;
            unit[0] = 'G';
        } else if (stats.tagged_allocations[i] > mib) {
            amount = (float32) stats.tagged_allocations[i] / (float32) mib;
            unit[0] = 'M';
        } else if (stats.tagged_allocations[i] > kib) {
            amount = (float32) stats.tagged_allocations[i] / (float32) kib;
            unit[0] = 'K';
        } else {
            amount = (float32) stats.tagged_allocations[i];
            unit[0] = 'B';
            unit[1] = '\0';
        }

        int32 length = snprintf(buffer + offset, 8000, "\t%18s: %.2f %s\n", memory_tag_to_string(i), amount, unit);
        offset += length;
    }

    return buffer;
}
