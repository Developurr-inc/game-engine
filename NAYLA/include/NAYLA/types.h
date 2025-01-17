//
// Created by Vinícius Ferreira Aguiar on 13/01/25.
//

#ifndef NAYLA_TYPES_H
#define NAYLA_TYPES_H

#include <PAL/types.h>

typedef struct ApplicationConfig {
    int16 start_pos_x;
    int16 start_pos_y;
    int16 start_width;
    int16 start_height;

    char *name;
} ApplicationConfig;

typedef struct Game {
    ApplicationConfig app_config;

    void *state;

    bool1 (* initialize) (struct Game *game);
    bool1 (* update)     (struct Game *game, float32 delta_time);
    bool1 (* render)     (struct Game *game, float32 delta_time);
    void  (* on_resize)  (struct Game *game, uint32 width, uint32 height);
} Game;

#endif //NAYLA_TYPES_H
