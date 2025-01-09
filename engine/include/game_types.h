//
// Created by Vinícius Ferreira Aguiar on 07/01/25.
//

#ifndef GAME_TYPES_H
# define GAME_TYPES_H

typedef struct game Game;

# include <core/application.h>

struct game {
    n_ApplicationConfig app_config;

    void *state;

    bool1 (* initialize) (Game *game);
    bool1 (* update)     (Game *game, float32 delta_time);
    bool1 (* render)     (Game *game, float32 delta_time);
    void  (* on_resize)  (Game *game, uint32 width, uint32 height);
} ;

#endif //GAME_TYPES_H
