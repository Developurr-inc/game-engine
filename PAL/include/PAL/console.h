//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef CONSOLE_H
# define CONSOLE_H

# include <platform.h>
# include <PAL/types.h>

/**********************************************************************************************************************
 *****                                                   STRUCTS                                                  *****
 **********************************************************************************************************************/

typedef struct Console {
    void (* write_output)(const char *message, uint8 color);
    void (* write_error)(const char *message, uint8 color);
} Console;

/**********************************************************************************************************************
 *****                                              PUBLIC FUNCTIONS                                              *****
 **********************************************************************************************************************/

PLAT_API Console* platform_console_create (void);
PLAT_API void     platform_console_destroy(const Console *console);

// PLAT_API void platform_console_write_message(const char *message, uint8 color);
// PLAT_API void platform_console_write_error  (const char *message, uint8 color);

#endif //CONSOLE_H
