//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef TIME_H
#define TIME_H

#include <PAL/export.h>
#include <PAL/types.h>

PAL_EXPORT float64 pal_get_absolute_time();
PAL_EXPORT void    pal_sleep            (uint64 milliseconds);

#endif //TIME_H
