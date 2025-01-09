//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include <PAL/console.h>

#ifdef N_PLATFORM_APPLE

# include <stdio.h>

static void platform_console_write(FILE *stream, const char *message, uint8 color);
static void platform_console_write_output(const char *message, uint8 color);
static void platform_console_write_error(const char *message, uint8 color);

Console *platform_console_create() {
    // TODO: Dynamic allocation
    static Console console;

    console.write_output = platform_console_write_output;
    console.write_error  = platform_console_write_error;

    return &console;
}

void platform_console_destroy(const Console *console) {
    // TODO: Free dynamic allocation
    (void) console;
}

static void platform_console_write_output(const char *message, const uint8 color) {
    platform_console_write(stdout, message, color);
}

static void platform_console_write_error(const char *message, const uint8 color) {
    platform_console_write(stderr, message, color);
}

static void platform_console_write(FILE *stream, const char *message, const uint8 color) {
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