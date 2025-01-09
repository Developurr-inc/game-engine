//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef TIME_H
# define TIME_H

# include <platform.h>
# include <PAL/types.h>

/**********************************************************************************************************************
 *****                                                   STRUCTS                                                  *****
 **********************************************************************************************************************/

typedef struct Time {
    void *(* get_absolute_time)(void);
    void  (* sleep)(uint64 milliseconds);
} Time;

/**********************************************************************************************************************
 *****                                              PUBLIC FUNCTIONS                                              *****
 **********************************************************************************************************************/

PLAT_API Time* platform_time_create (void);
PLAT_API void  platform_time_destroy(Time *time);

// PLAT_API float64 platform_get_absolute_time();
// PLAT_API void    platform_sleep            (uint64 milliseconds);

#endif //TIME_H
