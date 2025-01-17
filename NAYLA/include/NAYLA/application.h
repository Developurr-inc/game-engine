//
// Created by Vinícius Ferreira Aguiar on 07/01/25.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <NAYLA/export.h>
#include <NAYLA/types.h>

N_EXPORT bool1 n_application_create (Game *game_instance);
N_EXPORT bool1 n_application_run    ();
N_EXPORT void  n_application_destroy();

#endif //APPLICATION_H
