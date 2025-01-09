//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include <PAL/window.h>

#ifdef N_PLATFORM_WINDOWS

#include "PAL/memory.h"

# include <windows.h>
# include <windowsx.h>

/**********************************************************************************************************************
 *****                                                   STRUCTS                                                  *****
 **********************************************************************************************************************/

typedef struct InternalState {
    HINSTANCE h_instance;
    HWND h_window;
} InternalState;

/**********************************************************************************************************************
 *****                                              PRIVATE FUNCTIONS                                             *****
 **********************************************************************************************************************/

static LRESULT CALLBACK win32_process_messages(HWND window, UINT message, WPARAM w_param, LPARAM l_param);

Window *platform_window_create(
    const char *application_name,
    const int32 x,
    const int32 y,
    const int32 width,
    const int32 height
) {
    Memory *memory = platform_memory_create();

    Window *window = memory->allocate(sizeof(Window), false);
    if (!window) {
        // N_FATAL("Failed to allocate memory for window");
        return nullptr;
    }

    InternalState *internal_state = memory->allocate(sizeof(InternalState), false);
    if (!internal_state) {
        memory->free(window, false);

        platform_memory_destroy(memory);

        // N_FATAL("Failed to allocate memory for internal state");
        return nullptr;
    }

    internal_state->h_instance = GetModuleHandleA(nullptr);

    HICON icon = LoadIcon(nullptr, IDI_APPLICATION);
    WNDCLASSA window_class = {
        .style = CS_DBLCLKS,
        .lpfnWndProc = win32_process_messages,
        .hInstance = internal_state->h_instance,
        .lpszClassName = "Nayla window class",
        .hIcon = icon,
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .cbClsExtra = 0,
        .cbWndExtra = 0
    };

    if (!RegisterClassA(&window_class))
    {
        memory->free(window, false);
        memory->free(internal_state, false);

        platform_memory_destroy(memory);

        MessageBoxA(nullptr, "Failed to register window class", "Error", MB_ICONEXCLAMATION | MB_OK);
        return nullptr;
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

    uint32 window_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
    uint32 window_ex_style = WS_EX_APPWINDOW;

    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    window_x += border_rect.left;
    window_y += border_rect.top;

    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND h_window = CreateWindowExA(
        window_ex_style,
        "Nayla window class",
        application_name,
        window_style,
        window_x,
        window_y,
        window_width,
        window_height,
        nullptr,
        nullptr,
        internal_state->h_instance,
        nullptr
    );

    if (!h_window)
    {
        memory->free(window, false);
        memory->free(internal_state, false);

        platform_memory_destroy(memory);

        MessageBoxA(nullptr, "Failed to create window", "Error", MB_ICONEXCLAMATION | MB_OK);

        // N_FATAL("Failed to create window");
        return nullptr;
    }

    internal_state->h_window = h_window;

    bool1 is_window_visible = true;
    int32 show_window_flags = is_window_visible ? SW_SHOW : SW_SHOWNOACTIVATE;

    ShowWindow(h_window, show_window_flags);

    platform_memory_destroy(memory);

    return window;
}

void platform_window_destroy(const Window *window)
{
    InternalState *internal_state = window->data;

    if (internal_state->h_window)
    {
        DestroyWindow(internal_state->h_window);
    }

    if (internal_state->h_instance)
    {
        UnregisterClassA("Nayla window class", internal_state->h_instance);
    }

    Memory *memory = platform_memory_create();

    memory->free(internal_state, false);

    platform_memory_destroy(memory);
}

static bool1 platform_pump_messages(const Window *window)
{
    MSG message;

    while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return true;
}

static LRESULT CALLBACK win32_process_messages(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
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
        default: break;
    }

    return DefWindowProcA(window, message, w_param, l_param);
}

#endif
