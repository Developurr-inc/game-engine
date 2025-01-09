//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

# include <PAL/console.h>

#ifdef N_PLATFORM_WINDOWS

#include "PAL/memory.h"

# include <windows.h>

static void platform_console_write_output(const char *message, uint8 color);
static void platform_console_write_error(const char *message, uint8 color);
static void platform_console_write(HANDLE *handle, const char *message, uint8 color);

Console *platform_console_create() {
    Memory *memory = platform_memory_create();

    Console *new_console = memory->allocate(sizeof(Console), false);

    platform_memory_destroy(memory);

    new_console->write_output = platform_console_write_output;
    new_console->write_error  = platform_console_write_error;

    return new_console;
}

void platform_console_destroy(const Console *console) {
    Memory *memory = platform_memory_create();

    memory->free((void *) console, false);

    platform_memory_destroy(memory);
}

static void platform_console_write_output(const char *message, const uint8 color) {
    platform_console_write(GetStdHandle(STD_OUTPUT_HANDLE), message, color);
}

static void platform_console_write_error(const char *message, const uint8 color) {
    platform_console_write(GetStdHandle(STD_ERROR_HANDLE), message, color);
}

static void platform_console_write(HANDLE *handle, const char *message, const uint8 color) {
    static uint8 color_levels[6] = {
        8,
        1,
        2,
        6,
        4,
        64,
    };

    uint64 message_length = strlen(message);
    LPDWORD number_of_characters_written = nullptr;

    SetConsoleTextAttribute(handle, color_levels[color]);

    OutputDebugStringA(message);
    WriteConsoleA(handle, message, (DWORD) message_length, number_of_characters_written, nullptr);
}

#endif
