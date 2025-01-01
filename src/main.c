#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "glfw.h"
#include "vulkan.h"

void logInfo() {
    uint32_t instance_api_version;

    PANIC(vkEnumerateInstanceVersion(&instance_api_version), "Failed to get Vulkan instance version");

    const uint32_t api_version_variant = VK_API_VERSION_VARIANT(instance_api_version);
    const uint32_t api_version_major = VK_API_VERSION_MAJOR(instance_api_version);
    const uint32_t api_version_minor = VK_API_VERSION_MINOR(instance_api_version);
    const uint32_t api_version_patch = VK_API_VERSION_PATCH(instance_api_version);

    printf("Vulkan API Version: %i.%i.%i.%i\n", api_version_variant, api_version_major, api_version_minor, api_version_patch);
    printf("GLFW Version: %s\n", glfwGetVersionString());
    printf("Engine Version: %i.%i.%i.%i\n", VARIANT, MAJOR, MINOR, PATCH);
}

void init(State *state) {
    logInfo();

    initGlfw(state);
    initVulkan(state);
}

void loop(State *state) {
    while (!glfwWindowShouldClose(state->window)) {
        glfwPollEvents();

        // if (state->recreate_swapchain) {
        //     state->recreate_swapchain = false;
        //     createSwapchain(state);
        // }

        // uint32_t imageIndex;
        // PANIC(vkAcquireNextImageKHR(state->device, state->swapchain, UINT64_MAX, nullptr, nullptr, &imageIndex), "couldn't acquire next image");
        //
        // VkPresentInfoKHR presentInfo = {
        //     .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        //     .swapchainCount = 1,
        //     .pSwapchains = &state->swapchain,
        //     .pImageIndices = &imageIndex,
        // };
        //
        // VkResult result = vkQueuePresentKHR(state->queue, &presentInfo);
        //
        // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        //     state->recreate_swapchain = true;
        // } else if (result != VK_SUCCESS) {
        //     PANIC(result, "Failed to present image");
        // }
    }
}

void cleanup(State *state) {
    destroyVulkan(state);
    destroyGlfw(state);
}

int main(void) {
    State state = {
        .window_title = "Vulkan + GLFW",
        .window_width = 720,
        .window_height = 480,
        .window_resizable = false,
        .window_fullscreen = false,

        .engine_name = "No Engine",

        .api_version = VK_API_VERSION_1_4,
        .engine_version = VK_MAKE_API_VERSION(VARIANT, MAJOR, MINOR, PATCH),
        .application_version = VK_MAKE_API_VERSION(VARIANT, MAJOR, MINOR, PATCH),

        .queue_present_family_index = UINT32_MAX,
        .queue_graphics_family_index = UINT32_MAX,

        .device_extensions = (char *[]) {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        },
        .device_extension_count = 1,

        .extensions = (char *[]) {
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#ifndef NDEBUG
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // NDEBUG
        },
        .extension_count = 3,

        .layers = (char *[]) {
#ifndef NDEBUG
            "VK_LAYER_KHRONOS_validation",
#endif // NDEBUG
        },
        .layer_count = 1,

#ifndef NDEBUG
        .enableValidationLayers = true,
#else
        .enableValidationLayers = false,
#endif // NDEBUG
    };

    init(&state);
    loop(&state);
    cleanup(&state);

    return EXIT_SUCCESS;
}
