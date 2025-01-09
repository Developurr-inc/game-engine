//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include <core/logger.h>

#include "n_memory.h"
#include "../platform/platform.h"

void *n_memory_alloc(uint64 size, MemoryTag tag) {
    return nullptr;
}

void n_memory_free(void *block, uint64 size, MemoryTag tag) {

}

void *n_memory_copy(void *destination, void *source, uint64 size) {
    return nullptr;
}

void *n_memory_zero(void *block, uint64 size) {
    return nullptr;
}

void *n_memory_set(void *block, uint8 value, uint64 size) {
    return nullptr;
}

char *n_memory_get_usage() {
    return nullptr;
}
