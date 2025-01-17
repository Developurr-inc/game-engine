//
// Created by Vinícius Ferreira Aguiar on 13/01/25.
//

#include <NAYLA/memory.h>

#include "dynamic_array.h"

typedef struct DynamicArray {
    uint64 capacity;
    uint64 length;
    uint64 stride;
    uint8  data[];
} DynamicArray;

void *darray_create2(uint64 stride) {
    DynamicArray *new_darray = n_memory_alloc(sizeof(DynamicArray) + DARRAY_DEFAULT_CAPACITY * stride, MEMORY_TAG_DYNAMIC_ARRAY);

    if (!new_darray) {
        return nullptr;
    }

    new_darray->capacity = DARRAY_DEFAULT_CAPACITY;
    new_darray->length = 0;
    new_darray->stride = stride;

    n_memory_zero(new_darray->data, DARRAY_DEFAULT_CAPACITY * stride);

    return new_darray;
}

void darray_destroy2(DynamicArray *darray) {
    if (!darray) {
        return;
    }

    n_memory_free(darray, sizeof(DynamicArray) + darray->capacity * darray->stride, MEMORY_TAG_DYNAMIC_ARRAY);
}
