//
// Created by Vinícius Ferreira Aguiar on 04/01/25.
//

#include <core/application.h>
#include <core/logger.h>
#include <core/asserts.h>

int main() {
    N_FATAL("Fatal message");
    N_ERROR("Error message");
    N_WARNING("Warning message");
    N_INFO("Info message");
    N_DEBUG("Debug message");
    N_TRACE("Trace message");

    // application_create();
    // application_run();
    // application_destroy();

    return 0;
}
