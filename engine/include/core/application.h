//
// Created by Vinícius Ferreira Aguiar on 07/01/25.
//

#ifndef APPLICATION_H
# define APPLICATION_H

# include "../src/defines.h"

typedef struct n_ApplicationConfig {
    int16 start_pos_x;
    int16 start_pos_y;
    int16 start_width;
    int16 start_height;

    char *name;
} n_ApplicationConfig;

#include <game_types.h>

N_API bool1 n_application_create(Game *game_instance);
N_API bool1 n_application_run();

#endif //APPLICATION_H
