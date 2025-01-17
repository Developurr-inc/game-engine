//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#include "_asserts.h"

#include <NAYLA/logger.h>

#include <string.h>

void report_assertion_failure(const char *expression, const char *error, const char *filepath, const int32 line) {
    const char *filename = strrchr(filepath, '/');
    filename = filename ? filename + 1 : filepath;

    if (strlen(error)) {
        n_logger_message(
            LOG_LEVEL_FATAL,
            "Assertion failure: %s, message '%s', in file: %s, line: %d",
            expression,
            error,
            filename,
            line
        );
    } else {
        n_logger_message(
            LOG_LEVEL_FATAL,
            "Assertion failure: %s, in file: %s, line: %d",
            expression,
            filename,
            line
        );
    }
}
