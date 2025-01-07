//
// Created by Vinícius Ferreira Aguiar on 06/01/25.
//

#include "../platform.h"

#ifdef N_PLATFORM_LINUX

# include "platform_linux.h"

# include <core/logger.h>

# include <xcb/xcb.h>
# include <x11/keysym.h>
# include <x11/XKBlib.h>
# include <x11/Xlib.h>
# include <x11/Xlib-xcb.h>
# include <sys/time.h>

# if _POSIX_C_SOURCE >= 199309L
#  include <time.h>
# else
#  include <unistd.h>
# endif

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

bool1 platform_create(
    PlatformState *platform_state,
    const char *application_name,
    const int32 x,
    const int32 y,
    const int32 width,
    const int32 height)
{
    InternalState *internal_state = platform_allocate(sizeof(InternalState), false);
    if (!internal_state)
    {
        return false;
    }

    platform_state->internal_state = internal_state;

    // Connecting to X
    internal_state->display = XOpenDisplay(NULL);
    if (!internal_state->display)
    {
        platform_free(internal_state, false);

        N_FATAL("Failed to open display");
        return false;
    }

    XAutoRepeatOff(internal_state->display);

    // Retrieve the XCB connection
    internal_state->connection = XGetXCBConnection(internal_state->display);
    if (xcb_connection_has_error(internal_state->connection))
    {
        platform_free(internal_state, false);

        N_FATAL("Failed to connect to X server via XCB");
        return false;
    }

    // Get data from the X Server
    const struct xcb_setup_t *setup = xcb_get_setup(internal_state->connection);

    // Loop through the screens
    xcb_iterator_t iter = xcb_setup_roots_iterator(setup);
    int32 screen_number = 0;
    for (int32 i = screen_number; i > 0; i--)
    {
        xcb_screen_next(&iter);
    }

    // After the loop, iter.data will be the screen we want
    internal_state->screen = iter.data;

    // Allocate a XID for the window
    internal_state->window = xcb_generate_id(internal_state->connection);

    // Register event types
    // XCB_CW_BACK_PIXEL = filling the window with a single color
    // XCB_CW_EVENT_MASK is required
    const uint32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    // Listen for keyboard and mouse buttons
    const uint32 event_values = XCB_EVENT_MASK_EXPOSURE
        | XCB_EVENT_MASK_KEY_PRESS
        | XCB_EVENT_MASK_KEY_RELEASE
        | XCB_EVENT_MASK_BUTTON_PRESS
        | XCB_EVENT_MASK_BUTTON_RELEASE
        | XCB_EVENT_MASK_POINTER_MOTION
        | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    // Values to be sent over XCB (by color, events)
    uint32 values[] = {
        internal_state->screen->black_pixel,
        event_values
    };

    // Create the window
    xcb_void_cookie_t window_cookie = xcb_create_window(
        internal_state->connection,
        XCB_COPY_FROM_PARENT,
        internal_state->window,
        internal_state->screen->root,
        x,
        y,
        width,
        height,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        internal_state->screen->root_visual,
        event_mask,
        values
    );

    // Change the title
    xcb_change_property(
        internal_state->connection,
        XCB_PROP_MODE_REPLACE,
        internal_state->window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        strlen(application_name),
        application_name
    );

    // Tell the server to notify when the window manager wants to close the window
    xcb_intern_atom_cookie_t wm_delete_window_cookie = xcb_intern_atom(
        internal_state->connection,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW"
    );
    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(
        internal_state->connection,
        0,
        strlen("WM_PROTOCOLS"),
        "WM_PROTOCOLS"
    );

    xcb_intern_atom_reply_t *wm_delete_window_reply = xcb_intern_atom_reply(
        internal_state->connection,
        wm_delete_window_cookie,
        NULL
    );
    xcb_internal_atom_reply_t *wm_protocols_reply = xcb_intern_atom_reply(
        internal_state->connection,
        wm_protocols_cookie,
        NULL
    );

    internal_state->wm_delete_window = wm_delete_window_reply->atom;
    internal_state->wm_protocols = wm_protocols_reply->atom;

    xcb_change_property(
        internal_state->connection,
        XCB_PROP_MODE_REPLACE,
        internal_state->window,
        internal_state->wm_protocols,
        XCB_ATOM_ATOM,
        32,
        1,
        &internal_state->wm_delete_window
    );

    // Map window to the screen
    xcb_map_window(internal_state->connection, internal_state->window);

    // Flush the stream
    int32 stream_result = xcb_flush(internal_state->connection);
    if (stream_result <= 0)
    {
        platform_free(internal_state, false);

        N_FATAL("Failed to flush the stream");
        return false;
    }

    return true;
}

void platform_destroy(const PlatformState *platform_state)
{
    InternalState *internal_state = platform_state->internal_state;

    // Turn on auto repeat again
    XAutoRepeatOn(internal_state->display);

    // Close the display
    xcb_destroy_window(internal_state->connection, internal_state->window);

    // Free the internal state
    platform_free(internal_state, false);
}

bool1 platform_pump_messages(PlatformState *platform_state) {
    InternalState *internal_state = platform_state->internal_state;

    xcb_generic_event_t *event;

    while (event != 0)
    {
        event = xcb_poll_for_event(internal_state->connection);
        if (!event == 0)
        {
            break;
        }

        switch (event->response_type & ~0x80)
        {
            case XCB_EXPOSE:
            {
                // Redraw the window
                break;
            }

            case XCB_KEY_PRESS:
            {
                // xcb_key_press_event_t *key_event = (xcb_key_press_event_t *)event;

                // Get the key code
                // KeySym key = XkbKeycodeToKeysym(internal_state->display, key_event->detail, 0, key_event->state & ShiftMask ? 1 : 0);

                // Handle the key press
                break;
            }

            case XCB_KEY_RELEASE:
            {
                // xcb_key_release_event_t *key_event = (xcb_key_release_event_t *)event;

                // Get the key code
                // KeySym key = XkbKeycodeToKeysym(internal_state->display, key_event->detail, 0, key_event->state & ShiftMask ? 1 : 0);

                // Handle the key release
                break;
            }

            case XCB_BUTTON_PRESS:
            {
                // xcb_button_press_event_t *button_event = (xcb_button_press_event_t *)event;

                // Handle the button press
                break;
            }

            case XCB_BUTTON_RELEASE:
            {
                // xcb_button_release_event_t *button_event = (xcb_button_release_event_t *)event;

                // Handle the button release
                break;
            }

            case XCB_MOTION_NOTIFY:
            {
                // xcb_motion_notify_event_t *motion_event = (xcb_motion_notify_event_t *)event;

                // Handle the mouse movement
                break;
            }

            case XCB_CLIENT_MESSAGE:
            {
                // xcb_client_message_event_t *client_message_event = (xcb_client_message_event_t *) event;
                //
                // if (client_message_event->data.data32[0] == internal_state->wm_delete_window)
                // {
                //     return false;
                // }

                break;
            }

            case XCB_DESTROY_NOTIFY:
            {
                return false;
            }

            default:
            {
                break;
            }
        }

        free(event);
    }

    return true;
}

void *platform_allocate(const uint64 size, const bool1 is_aligned)
{
    void *new_block = malloc(size);
    if (!new_block)
    {
        N_FATAL("Failed to allocate memory");
        return NULL;
    }

    return new_block;
}

void platform_free(void *block, const bool1 is_aligned) {
    if (block)
    {
        free(block);
    }
}

void *platform_zero_memory(void *block, const uint64 size) {
    return platform_set_memory(block, 0, size);
}

void *platform_copy_memory(void *destination, const void *source, const uint64 size) {
    return memcpy(destination, source, size);
}

void *platform_set_memory(void *destination, const int32 value, const uint64 size) {
    return memset(destination, value, size);
}

void platform_console_write(const char *message, const uint8 color) {
    const char *reset_color = "\033[0m";
    const char *color_strings[6] = {
        "\033[1;30m",
        "\033[1;34m",
        "\033[1;32m",
        "\033[1;33m",
        "\033[1;31m",
        "\033[0;41m",
    };

    fprintf(stdout, "%s%s%s", color_strings[color], message, reset_color);
}

void platform_console_error(const char *message, const uint8 color) {
    const char *reset_color = "\033[0m";
    const char *color_strings[6] = {
        "\033[1;30m",
        "\033[1;34m",
        "\033[1;32m",
        "\033[1;33m",
        "\033[1;31m",
        "\033[0;41m",
    };

    fprintf(stderr, "%s%s%s", color_strings[color], message, reset_color);
}

float64 platform_get_absolute_time() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);

    return (float64)time.tv_sec + (float64)time.tv_nsec / 1000000000.0;
}

void platform_sleep(uint64 milliseconds) {
# if _POSIX_C_SOURCE >= 199309L
    struct timespec time;

    time.tv_sec = milliseconds / 1000;
    time.tv_nsec = (milliseconds % 1000) * 1000 * 1000;

    nanosleep(&time, 0);
# else
    if (milliseconds >= 1000)
    {
        sleep(milliseconds / 1000);
    }

    usleep(milliseconds % 1000 * 1000);
#endif
}

#endif