//
// Created by Vinícius Ferreira Aguiar on 07/01/25.
//

#include <core/application.h>

#include <PAL/window.h>

#include "../platform/platform.h"
#include "core/logger.h"

typedef struct ApplicationState {
    bool1 is_running;
    bool1 isSuspended;

    Window *window;

    Game *game_instance;
    // PlatformState platform_state;

    int16 width;
    int16 height;

    float64 lastTime;
} ApplicationState;

static bool1 g_isInitialized = false;
static ApplicationState g_appState;

bool1 n_application_create(Game *game_instance) {
    if (g_isInitialized) {
        N_ERROR("n_application_create: Application already initialized");
        return false;
    }

    if (! logger_create()) {
        // N_ERROR("n_application_create: Failed to create logger");
        return false;
    }

    g_appState.game_instance = game_instance;
    g_appState.is_running = true;
    g_appState.isSuspended = false;

    Window *window = platform_window_create(
        game_instance->app_config.name,
        game_instance->app_config.start_pos_x,
        game_instance->app_config.start_pos_y,
        game_instance->app_config.start_width,
        game_instance->app_config.start_height
    );
    if (! window) {
        N_ERROR("n_application_create: Failed to create window");
        return false;
    }

    if (! game_instance->initialize(game_instance)) {
        N_FATAL("n_application_create: Failed to initialize game");
        return false;
    }

    g_appState.window = window;
    g_appState.game_instance->on_resize(g_appState.game_instance, g_appState.width, g_appState.height);

    g_isInitialized = true;

    return true;
}

bool1 n_application_run() {
    while (g_appState.is_running) {
        if (! g_appState.window->pump_messages(g_appState.window)) {
            N_ERROR("n_application_run: Failed to pump messages");
            g_appState.is_running = false;
        }

        if (! g_appState.isSuspended) {
            if (! g_appState.game_instance->update(g_appState.game_instance, (float32) 0.0)) {
                N_FATAL("n_application_run: Failed to update game");
                g_appState.is_running = false;
                break;
            }

            if (! g_appState.game_instance->render(g_appState.game_instance, (float32) 0.0)) {
                N_FATAL("n_application_run: Failed to render game");
                g_appState.is_running = false;
                break;
            }
        }
    }

    g_appState.is_running = false;

    platform_window_destroy(g_appState.window);

    return true;
}

void n_application_destroy() {
}
