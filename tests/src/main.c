//
// Created by Vinícius Ferreira Aguiar on 04/01/25.
//

#include <entry.h>
#include <stdlib.h>

#include "game.h"

bool create_game(Game *out_game) {
    out_game->app_config.name = "Nayla Game Engine";
    out_game->app_config.start_pos_x = 100;
    out_game->app_config.start_pos_y = 100;
    out_game->app_config.start_width = 1280;
    out_game->app_config.start_height = 720;

    out_game->initialize = game_initialize;
    out_game->update = game_update;
    out_game->render = game_render;
    out_game->on_resize = game_on_resize;

    out_game->state = malloc(sizeof(GameState));

    return true;
}