//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include "../platform_detection.h"

#ifdef PLATFORM_MACOS
#include <PAL/console.h>

#include <stdio.h>

static void pal_console_write(FILE *stream, const char *message, uint8 color);

void pal_console_message(const char *message, const uint8 color) {
    pal_console_write(stdout, message, color);
}

void pal_console_error(const char *message, const uint8 color) {
    pal_console_write(stderr, message, color);
}

static void pal_console_write(FILE *stream, const char *message, const uint8 color) {
    const char *reset_color = "\033[0m";
    const char *color_strings[6] = {
        "\033[1;30m",
        "\033[1;34m",
        "\033[1;32m",
        "\033[1;33m",
        "\033[1;31m",
        "\033[0;41m",
    };

    fprintf(stream, "%s%s%s\n", color_strings[color], message, reset_color);
}

#endif
