//
// Created by Vinícius Ferreira Aguiar on 17/12/24.
//

#ifndef MAIN_H
#define MAIN_H

#include <signal.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_beta.h>

#define MAX_FRAMES_IN_FLIGHT 2

#define VARIANT 0
#define MAJOR   1
#define MINOR   0
#define PATCH   0

#define PANIC(ERROR, FORMAT, ...)                               \
    {                                                           \
        if (ERROR) {                                            \
            fprintf(                                            \
                stderr,                                         \
                "%s -> %s -> %i -> Error(%i):\n\t" FORMAT "\n", \
                __FILE_NAME__,                                  \
                __FUNCTION__,                                   \
                __LINE__,                                       \
                ERROR,                                          \
                ##__VA_ARGS__);                                 \
            raise(SIGABRT);                                     \
        }                                                       \
    }

typedef struct {
    // glfw config
    char *window_title;
    int window_width, window_height;
    bool window_resizable;
    bool window_fullscreen;

    // glfw
    int framebuffer_width;
    int framebuffer_height;

    GLFWmonitor *window_monitor;
    GLFWwindow *window;

    // vulkan config
    char *engine_name;
    uint32_t api_version;
    uint32_t engine_version;
    uint32_t application_version;
    bool recreate_swapchain;

    // vulkan
    VkAllocationCallbacks *allocator;
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkSurfaceKHR surface;
    VkDevice device;
    VkQueue presentQueue;
    uint32_t queue_present_family_index;
    VkQueue graphicsQueue;
    uint32_t queue_graphics_family_index;

    VkSwapchainKHR swapchain;
    VkImage *swapchain_images;
    VkImageView *swapchain_image_views;
    uint32_t swapchain_image_count;

    VkFormat swapchain_image_format;
    VkExtent2D swapchain_extent;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkFramebuffer *swapChainFramebuffers;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer[MAX_FRAMES_IN_FLIGHT];

    VkSemaphore imageAvailableSemaphore[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
    VkFence inFlightFence[MAX_FRAMES_IN_FLIGHT];

    int currentFrame;

    const char **device_extensions;
    const uint32_t device_extension_count;

    const char **layers;
    const uint32_t layer_count;

    const char **extensions;
    uint32_t extension_count;

    bool enableValidationLayers;
    VkDebugUtilsMessengerEXT debugMessenger;
} State;

#endif //MAIN_H
