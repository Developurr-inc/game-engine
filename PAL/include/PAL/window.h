//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef WINDOW_H
# define WINDOW_H

# include <platform.h>
# include <PAL/types.h>

/**********************************************************************************************************************
 *****                                                   STRUCTS                                                  *****
 **********************************************************************************************************************/

typedef struct Window {
    void *data;

    bool1 (* pump_messages)(struct Window *window);
} Window;

/**********************************************************************************************************************
 *****                                              PUBLIC FUNCTIONS                                              *****
 **********************************************************************************************************************/

PLAT_API Window *platform_window_create (const char *application_name, int32 x, int32 y, int32 width, int32 height);
PLAT_API void    platform_window_destroy(Window *window);

// PLAT_API bool1 platform_window_pump_messages(Window *window);

#endif //WINDOW_H
