//
// Created by Vinícius Ferreira Aguiar on 07/01/25.
//

#ifndef GAME_TYPES_H
# define GAME_TYPES_H

# include <core/application.h>

typedef struct game {
    n_ApplicationConfig app_config;

    void *state;

    bool1 (* initialize) (struct game *game);
    bool1 (* update)     (struct game *game, float32 delta_time);
    bool1 (* render)     (struct game *game, float32 delta_time);
    void  (* on_resize)  (struct game *game, uint32 width, uint32 height);
} Game;

#endif //GAME_TYPES_H
