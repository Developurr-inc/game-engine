//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef PAL_MEMORY_H
#define PAL_MEMORY_H

#include <PAL/export.h>
#include <PAL/types.h>

PAL_EXPORT void *pal_memory_alloc(uint64 size, bool1 is_aligned);
PAL_EXPORT void  pal_memory_free (void *block, bool1 is_aligned);
PAL_EXPORT void *pal_memory_zero (void *block, uint64 size);
PAL_EXPORT void *pal_memory_copy (void *destination, const void *source, uint64 size);
PAL_EXPORT void *pal_memory_set  (void *destination, int32 value, uint64 size);

#endif //PAL_MEMORY_H
