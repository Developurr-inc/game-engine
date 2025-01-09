//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#include <PAL/console.h>

#include <core/logger.h>

#include <stdio.h>
#include <stdarg.h>

/**********************************************************************************************************************
 *****                                             PROTECTED FUNCTIONS                                            *****
 **********************************************************************************************************************/

bool1 logger_create()
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

    char buffer[32000] = {0};
    __builtin_va_list arg_ptr;

    va_start(arg_ptr, message);
    (void) vsnprintf(buffer, 32000, message, arg_ptr);
    va_end(arg_ptr);

    char output[32000] = {0};
    (void) snprintf(output, 32000, "[%s] - %s\n", level_strings[level], buffer);

    Console *console = platform_console_create();

    if (level > LOG_LEVEL_WARNING)
    {
        console->write_error(output, (uint8) level);
    }
    else
    {
        console->write_output(output, (uint8) level);
    }

    platform_console_destroy(console);
}
