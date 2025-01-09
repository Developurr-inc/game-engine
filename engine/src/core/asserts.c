//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#include "core/logger.h"

void report_assertion_failure(const char *expression, const char *message, const char *filename, const int32 line)
{
    log_message(
        LOG_LEVEL_FATAL,
        "Assertion failure: %s, message '%s', in file: %s, line: %d\n",
        expression,
        message,
        filename,
        line
    );
}