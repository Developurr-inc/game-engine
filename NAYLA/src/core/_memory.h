//
// Created by Vinícius Ferreira Aguiar on 12/01/25.
//

#ifndef _MEMORY_H
#define _MEMORY_H

#include <PAL/types.h>

bool1 memory_create();
void  memory_destroy();

char *n_memory_get_usage();

#endif //_MEMORY_H
