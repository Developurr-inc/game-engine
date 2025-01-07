//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#ifndef PLATFORM_H
# define PLATFORM_H

# include "../../src/defines.h"

/**********************************************************************************************************************
 *****                                                   STRUCTS                                                  *****
 **********************************************************************************************************************/

typedef struct PlatformState {
    void *internal_state;
} PlatformState;

/**********************************************************************************************************************
 *****                                             PROTECTED FUNCTIONS                                            *****
 **********************************************************************************************************************/

bool platform_create(PlatformState *platform_state, const char *application_name, int32 x, int32 y, int32 width, int32 height);
void platform_destroy(const PlatformState *platform_state);
bool platform_pump_messages(PlatformState *platform_state);

void *platform_allocate(uint64 size, bool is_aligned);
void platform_free(void *block, bool is_aligned);
void *platform_zero_memory(void *block, uint64 size);
void *platform_copy_memory(void *destination, const void *source, uint64 size);
void *platform_set_memory(void *destination, int32 value, uint64 size);

void platform_console_write(const char *message, uint8 color);
void platform_console_error(const char *message, uint8 color);

float64 platform_get_absolute_time();

void platform_sleep(uint64 milliseconds);

/**********************************************************************************************************************
 *****                                              PUBLIC FUNCTIONS                                              *****
 **********************************************************************************************************************/

#endif //PLATFORM_H