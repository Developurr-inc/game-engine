//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef N_MEMORY_H
#define N_MEMORY_H

#include <NAYLA/export.h>
#include <NAYLA/types.h>

typedef enum memory_tag {
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_DARRAY,
    MEMORY_TAG_DICT,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BST,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    MEMORY_TAG_MAX_TAGS
} EMemoryTag;

N_EXPORT void *n_memory_alloc(uint64 size, EMemoryTag tag);
N_EXPORT void  n_memory_free (void *block, uint64 size, EMemoryTag tag);
N_EXPORT void *n_memory_copy (void *destination, const void *source, uint64 size);
N_EXPORT void *n_memory_zero (void *block, uint64 size);
N_EXPORT void *n_memory_set  (void *block, uint8 value, uint64 size);

#endif //N_MEMORY_H
