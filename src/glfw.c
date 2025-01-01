//
// Created by Vinícius Ferreira Aguiar on 30/12/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "glfw.h"

static void setupErrorHandling();
static void exit_callback();
static void glfw_error_callback(const int error_code, const char* description);
static void createWindow(State *state);
static void framebufferSizeCallback(GLFWwindow *window, const int width, const int height);

/*
 * Public Functions
 */

void initGlfw(State *state) {
    setupErrorHandling();
    createWindow(state);
}

void destroyGlfw(State *state) {
    glfwDestroyWindow(state->window);
    state->window = nullptr;
    glfwTerminate();
}

/*
 * Private Functions
 */

static void setupErrorHandling() {
    glfwSetErrorCallback(glfw_error_callback);
    atexit(exit_callback);
}

static void exit_callback() {
    glfwTerminate();
}

static void glfw_error_callback(const int error_code, const char* description) {
    PANIC(error_code, "GLFW: %s", description);
}

static void createWindow(State *state) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    if (!glfwVulkanSupported()) {
        fprintf(stderr, "Vulkan is not supported on this system\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, state->window_resizable);

    if (state->window_fullscreen) {
        state->window_monitor = glfwGetPrimaryMonitor();

        const GLFWvidmode *mode = glfwGetVideoMode(state->window_monitor);
        state->window_width = mode->width;
        state->window_height = mode->height;
    }

    state->window = glfwCreateWindow(state->window_width, state->window_height, state->window_title, state->window_monitor, nullptr);

    glfwSetWindowUserPointer(state->window, state);
    glfwSetFramebufferSizeCallback(state->window, framebufferSizeCallback);

    int width, height;
    glfwGetFramebufferSize(state->window, &width, &height);

    framebufferSizeCallback(state->window, width, height);
}

static void framebufferSizeCallback(GLFWwindow *window, const int width, const int height) {
    State *state = glfwGetWindowUserPointer(window);

    state->recreate_swapchain = true;
    state->framebuffer_width = width;
    state->framebuffer_height = height;
}
