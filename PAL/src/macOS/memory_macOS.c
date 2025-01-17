//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include "../platform_detection.h"

#ifdef PLATFORM_MACOS
#include <PAL/memory.h>

#include <stdlib.h>
#include <string.h>

void *pal_memory_alloc(const uint64 size, const bool1 is_aligned) {
    return malloc(size);
}

void pal_memory_free(void *block, const bool1 is_aligned) {
    free(block);
}

void *pal_memory_zero(void *block, const uint64 size) {
    return pal_memory_set(block, 0, size);
}

void *pal_memory_copy(void *destination, const void *source, const uint64 size) {
    return memcpy(destination, source, size);
}

void *pal_memory_set(void *destination, const int32 value, const uint64 size) {
    return memset(destination, value, size);
}

#endif