//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef CONSOLE_H
#define CONSOLE_H

#include <PAL/export.h>
#include <PAL/types.h>

PAL_EXPORT void pal_console_message(const char *message, uint8 color);
PAL_EXPORT void pal_console_error  (const char *message, uint8 color);

#endif //CONSOLE_H
