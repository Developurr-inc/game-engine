//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#include "../platform/platform.h"

#include <core/logger.h>

#include <stdio.h>
#include <stdarg.h>

/**********************************************************************************************************************
 *****                                             PROTECTED FUNCTIONS                                            *****
 **********************************************************************************************************************/

bool logger_create()
{
    // TODO: Implement logger initialization
    return true;
}

void logger_destroy()
{
    // TODO: Implement logger closing / write queued up messages
}

/**********************************************************************************************************************
 *****                                              PUBLIC FUNCTIONS                                              *****
 **********************************************************************************************************************/

void log_message(const ELogLevel level, const char *message, ...)
{
    const char *level_strings[] = {
        "TRC",
        "DBG",
        "INF",
        "WRN",
        "ERR",
        "FAT",
    };

    const int32 buffer_size = 32000;
    char buffer[buffer_size] = {0};
    __builtin_va_list arg_ptr;

    va_start(arg_ptr, message);
    (void) vsnprintf(buffer, buffer_size, message, arg_ptr);
    va_end(arg_ptr);

    char output[buffer_size] = {0};
    (void) snprintf(output, buffer_size, "[%s] - %s\n", level_strings[level], buffer);

    if (level > LOG_LEVEL_WARNING)
    {
        platform_console_error(output, (uint8) level);
    }
    else
    {
        platform_console_write(output, (uint8) level);
    }
}