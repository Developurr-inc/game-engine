//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

# include <PAL/console.h>

#ifdef N_PLATFORM_WINDOWS

# include <windows.h>

static void platform_console_write_output(const char *message, uint8 color);
static void platform_console_write_error(const char *message, uint8 color);
static void platform_console_write(HANDLE *handle, const char *message, uint8 color);

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
    platform_console_write(GetStdHandle(STD_OUTPUT_HANDLE), message, color);
}

static void platform_console_write_error(const char *message, const uint8 color) {
    platform_console_write(GetStdHandle(STD_ERROR_HANDLE), message, color);
}

static void platform_console_write(HANDLE *handle, const char *message, uint8 color) {
    static uint8 color_levels[6] = {
        8,
        1,
        2,
        6,
        4,
        64,
    };

    uint64 message_length = strlen(message);
    LPDWORD number_of_characters_written = 0;

    SetConsoleTextAttribute(handle, color_levels[color]);

    OutputDebugStringA(message);
    WriteConsoleA(handle, message, (DWORD) message_length, number_of_characters_written, 0);
}

#endif
