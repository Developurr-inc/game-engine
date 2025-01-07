//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#include "../platform.h"

#ifdef N_PLATFORM_WINDOWS

# include "platform_win32.h"

#include <core/logger.h>

# include <stdlib.h>
# include <string.h>

# include <windowsx.h>

bool1 platform_create(
    PlatformState *platform_state,
    const char *application_name,
    const int32 x,
    const int32 y,
    const int32 width,
    const int32 height)
{
    platform_state->internal_state = platform_allocate(sizeof(InternalState), false);
    if (!platform_state->internal_state)
    {
        return false;
    }

    InternalState *internal_state = platform_state->internal_state;

    internal_state->h_instance = GetModuleHandleA(0);

    HICON icon = LoadIcon(0, IDI_APPLICATION);
    WNDCLASSA window_class = {
        .style = CS_DBLCLKS,
        .lpfnWndProc = win32_process_messages,
        .hInstance = internal_state->h_instance,
        .lpszClassName = "Nayla window class",
        .hIcon = icon,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = 0,
        .lpszMenuName = 0,
        .cbClsExtra = 0,
        .cbWndExtra = 0
    };

    if (!RegisterClassA(&window_class))
    {
        MessageBoxA(0, "Failed to register window class", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // Create window

    uint32 client_x = x;
    uint32 client_y = y;
    uint32 client_width = width;
    uint32 client_height = height;

    uint32 window_x = client_x;
    uint32 window_y = client_y;
    uint32 window_width = client_width;
    uint32 window_height = client_height;

    uint32 window_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_TICKFRAME;
    uint32 window_ex_style = WS_EX_APPWINDOW;

    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    window_x += border_rect.left;
    window_y += border_rect.top;

    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND window = CreateWindowExA(
        window_ex_style,
        "Nayla window class",
        application_name,
        window_style,
        window_x,
        window_y,
        window_width,
        window_height,
        0,
        0,
        internal_state->h_instance,
        0
    );

    if (!window)
    {
        platform_free(internal_state, false);

        MessageBoxA(0, "Failed to create window", "Error", MB_ICONEXCLAMATION | MB_OK);

        N_FATAL("Failed to create window");
        return false;
    }

    internal_state->h_window = window;

    bool1 is_window_visible = true;
    int32 show_window_flags = is_window_visible ? SW_SHOW : SW_SHOWNOACTIVATE;

    ShowWindow(window, show_window_flags);

    return true;
}

void platform_destroy(const PlatformState *platform_state)
{
    InternalState *internal_state = platform_state->internal_state;

    if (internal_state->h_window)
    {
        DestroyWindow(internal_state->h_window);
    }

    if (internal_state->h_instance)
    {
        UnregisterClassA("Nayla window class", internal_state->h_instance);
    }

    platform_free(internal_state, false);
}

bool1 platform_pump_messages(PlatformState *platform_state)
{
    MSG message;

    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return true;
}

void *platform_allocate(uint64 size, bool1 is_aligned)
{
    void *new_block = malloc(size);

    if (!new_block)
    {
        N_FATAL("Failed to allocate memory");
    }

    return new_block;
}

void platform_free(void *block, bool1 is_aligned)
{
    if (block)
    {
        free(block);
    }
}

void *platform_zero_memory(void *block, const uint64 size)
{
    return platform_set_memory(block, 0, size);
}

void *platform_copy_memory(void *destination, const void *source, const uint64 size)
{
    return memcpy(destination, source, size);
}

void *platform_set_memory(void *destination, const int32 value, const uint64 size)
{
    return memset(destination, value, size);
}

void platform_console_write(const char *message, const uint8 color)
{
    static uint8 color_levels[6] = {
        64,
        4,
        6,
        2,
        1,
        8,
    };

    uint64 message_length = strlen(message);
    LPDWORD number_of_characters_written = 0;

    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console_handle, color_levels[color]);

    OutputDebugStringA(message);
    WriteConsoleA(console_handle, message, (DWORD) message_length, number_of_characters_written, 0);
}

void platform_console_error(const char *message, const uint8 color)
{
    static uint8 color_levels[6] = {
        64,
        4,
        6,
        2,
        1,
        8,
    };

    uint64 message_length = strlen(message);
    LPDWORD number_of_characters_written = 0;

    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    SetConsoleTextAttribute(console_handle, color_levels[color]);

    OutputDebugStringA(message);
    WriteConsoleA(console_handle, message, (DWORD) message_length, number_of_characters_written, 0);
}

float64 platform_get_absolute_time()
{
    LARGE_INTEGER performance_counter;
    LARGE_INTEGER performance_frequency;

    QueryPerformanceCounter(&performance_counter);
    QueryPerformanceFrequency(&performance_frequency);

    return (float64) performance_counter.QuadPart / (float64) performance_frequency.QuadPart;
}

void platform_sleep(const uint64 milliseconds)
{
    Sleep((DWORD) milliseconds);
}

LRESULT CALLBACK win32_process_messages(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message)
    {
        case WM_ERASEBKGND:
        {
            // Notify the OS that the background will be erased by the application to prevent flickering
            return 1;
        }

        case WM_CLOSE:
        {
            // TODO: Fire an event for the application to quit
            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        case WM_SIZE:
        {
            // RECT client_rect = {};
            // GetClientRect(window, &client_rect);

            // uint32 client_width = client_rect.right - client_rect.left;
            // uint32 client_height = client_rect.bottom - client_rect.top;

            // TODO: Fire an event for the application to resize

            break;
        }

        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            // bool1 pressed = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);

            // TODO: input processing

            break;
        }

        case WM_MOUSEMOVE:
        {
            // int32 x_position = GET_X_LPARAM(l_param);
            // int32 y_position = GET_Y_LPARAM(l_param);

            // TODO: input processing

            break;
        }

        case WM_MOUSEWHEEL:
        {
            // int32 wheel_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            //
            // if (wheel_delta != 0)
            // {
            //     wheel_delta = wheel_delta < 0 ? -1 : 1;
            //
            //     TODO: input processing
            // }

            break;
        }

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
            // bool1 pressed = (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN);

            // TODO: input processing

            break;
        }
    }

    return DefWindowProcA(window, message, w_param, l_param);
}

#endif