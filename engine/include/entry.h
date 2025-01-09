//
// Created by Vinícius Ferreira Aguiar on 07/01/25.
//

#ifndef ENTRY_H
# define ENTRY_H

#include "../src/defines.h"
#include "core/application.h"
#include "core/logger.h"

extern bool1 create_game(Game *out_game);

int main (const int argc, const char **argv) {
    Game game = {0};

    if (!create_game(&game)) {
        N_FATAL("main: Could not create game");
        return -1;
    }

    if (game.initialize == nullptr) {
        N_FATAL("main: Game has no initialize function");
        return -2;
    }

    if (game.render == nullptr) {
        N_FATAL("main: Game has no render function");
        return -3;
    }

    if (game.update == nullptr) {
        N_FATAL("main: Game has no update function");
        return -4;
    }

    if (game.on_resize == nullptr) {
        N_FATAL("main: Game has no on_resize function");
        return -5;
    }

    if (! n_application_create(&game)) {
        N_INFO("main: Could not create application");
        return 1;
    }

    if (! n_application_run()) {
        N_INFO("main: Could not run application");
        return 2;
    }

    return 0;
}

#endif //ENTRY_H
