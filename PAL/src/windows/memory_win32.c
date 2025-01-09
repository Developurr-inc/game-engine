//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include <PAL/memory.h>

#ifdef N_PLATFORM_WINDOWS

# include <windows.h>
# include <stdlib.h>
# include <string.h>

static void *platform_allocate   (uint64 size, bool1 is_aligned);
static void  platform_free       (void *block, bool1 is_aligned);
static void *platform_zero_memory(void *block, uint64 size);
static void *platform_copy_memory(void *destination, const void *source, uint64 size);
static void *platform_set_memory (void *destination, int32 value, uint64 size);

Memory *platform_memory_create() {
    Memory *memory = platform_allocate(sizeof(Memory), false);

    memory->allocate = platform_allocate;
    memory->free     = platform_free;
    memory->zero     = platform_zero_memory;
    memory->copy     = platform_copy_memory;
    memory->set      = platform_set_memory;

    return memory;
}

void platform_memory_destroy(Memory *memory) {
    platform_free(memory, false);
}

static void *platform_allocate(const uint64 size, const bool1 is_aligned)
{
    return malloc(size);
}

static void platform_free(void *block, const bool1 is_aligned) {
    if (block) {
        free(block);
    }
}

static void *platform_zero_memory(void *block, const uint64 size) {
    return platform_set_memory(block, 0, size);
}

static void *platform_copy_memory(void *destination, const void *source, const uint64 size) {
    return memcpy(destination, source, size);
}

static void *platform_set_memory(void *destination, const int32 value, const uint64 size) {
    return memset(destination, value, size);
}

#endif
