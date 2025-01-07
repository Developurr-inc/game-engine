//
// Created by Vinícius Ferreira Aguiar on 06/01/25.
//

#ifndef PLATFORM_LINUX_H
# define PLATFORM_LINUX_H

# ifdef N_PLATFORM_LINUX

/**********************************************************************************************************************
 *****                                                   STRUCTS                                                  *****
 **********************************************************************************************************************/

typedef struct InternalState {
    Display *display;
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_window;
} InternalState;

# endif

#endif //PLATFORM_LINUX_H