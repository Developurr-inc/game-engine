//
// Created by Vinícius Ferreira Aguiar on 13/01/25.
//

#ifndef NAYLA__EVENT_H
#define NAYLA__EVENT_H

#include <NAYLA/types.h>

typedef enum ESystemEventCode {
    EVENT_CODE_APPLICATION_QUIT = 0x01,
    EVENT_CODE_KEY_PRESSED      = 0x02,
    EVENT_CODE_KEY_RELEASED     = 0x03,
    EVENT_CODE_BUTTON_PRESSED   = 0x04,
    EVENT_CODE_BUTTON_RELEASED  = 0x05,
    EVENT_CODE_MOUSE_MOVED      = 0x06,
    EVENT_CODE_MOUSE_WHEEL      = 0x07,
    EVENT_CODE_RESIZED          = 0x08,

    EVENT_CODE_MAX              = 0xFF
} ESystemEventCode;

bool1 event_create ();
void  event_destroy();

#endif //NAYLA__EVENT_H
