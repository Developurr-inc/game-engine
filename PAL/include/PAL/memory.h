//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef MEMORY_H
# define MEMORY_H

# include <platform.h>
# include <PAL/types.h>

/**********************************************************************************************************************
 *****                                                   STRUCTS                                                  *****
 **********************************************************************************************************************/

typedef struct Memory {
    bool1 used;

    void *(* allocate)(uint64 size, bool1 is_aligned);
    void  (* free)(void *block, bool1 is_aligned);
    void *(* zero)(void *block, uint64 size);
    void *(* copy)(void *destination, const void *source, uint64 size);
    void *(* set)(void *destination, int32 value, uint64 size);
} Memory;

/**********************************************************************************************************************
 *****                                              PUBLIC FUNCTIONS                                              *****
 **********************************************************************************************************************/

PLAT_API Memory* platform_memory_create (void);
PLAT_API void    platform_memory_destroy(Memory *memory);

// PLAT_API void *platform_allocate   (uint64 size, bool1 is_aligned);
// PLAT_API void  platform_free       (void *block, bool1 is_aligned);
// PLAT_API void *platform_zero_memory(void *block, const uint64 size);
// PLAT_API void *platform_copy_memory(void *destination, const void *source, const uint64 size);
// PLAT_API void *platform_set_memory (void *destination, const int32 value, const uint64 size);

#endif //MEMORY_H
