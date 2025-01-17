//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#include <NAYLA/logger.h>

#include <PAL/console.h>

#include "_logger.h"

#include <stdio.h>
#include <stdarg.h>

#define MAX_BUFFER  32000

/**********************************************************************************************************************
 *****                                             PROTECTED FUNCTIONS                                            *****
 **********************************************************************************************************************/

bool1 logger_create() {
    // TODO: Implement logger initialization
    return true;
}

void logger_destroy() {
    // TODO: Implement logger closing / write queued up messages
}

/**********************************************************************************************************************
 *****                                              PUBLIC FUNCTIONS                                              *****
 **********************************************************************************************************************/

void n_logger_message(const ELogLevel level, const char *message, ...) {
    static const char *level_strings[6] = {
        "TRC",
        "DBG",
        "INF",
        "WRN",
        "ERR",
        "FAT",
    };

    char buffer[MAX_BUFFER] = {0};
    char output[MAX_BUFFER] = {0};

    __builtin_va_list arg_ptr;

    va_start(arg_ptr, message);
    (void) vsnprintf(buffer, MAX_BUFFER, message, arg_ptr);
    va_end(arg_ptr);

    (void) snprintf(output, MAX_BUFFER, "[%s] - %s\n", level_strings[level], buffer);

    if (level >= LOG_LEVEL_ERROR) {
        pal_console_error(output, (uint8) level);
    } else {
        pal_console_message(output, (uint8) level);
    }
}
