//
// Created by Vinícius Ferreira Aguiar on 06/01/25.
//

#ifndef PLATFORM_WIN32_H
# define PLATFORM_WIN32_H

# ifdef N_PLATFORM_WINDOWS

# include <windows.h>

/**********************************************************************************************************************
 *****                                                   STRUCTS                                                  *****
 **********************************************************************************************************************/

typedef struct InternalState {
    HINSTANCE h_instance;
    HWND h_window;
} InternalState;

/**********************************************************************************************************************
 *****                                              PRIVATE FUNCTIONS                                              *****
 **********************************************************************************************************************/

static LRESULT CALLBACK win32_process_messages(HWND window, UINT message, WPARAM w_param, LPARAM l_param);

# endif

#endif //PLATFORM_WIN32_H
