//
// Created by Vinícius Ferreira Aguiar on 07/01/25.
//

#ifndef GAME_H
# define GAME_H

# include <NAYLA/types.h>

typedef struct GameState {
    float32 delta_time;
} GameState;

bool1 game_initialize(Game *game);

bool1 game_update(Game *game, float32 delta_time);

bool1 game_render(Game *game, float32 delta_time);

void game_on_resize(Game *game, uint32 width, uint32 height);

#endif //GAME_H
