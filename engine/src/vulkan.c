//
// Created by Vinícius Ferreira Aguiar on 20/12/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include "vulkan.h"

#ifndef NDEBUG
# include "debug.h"
#endif // NDEBUG

typedef struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
} QueueFamilyIndices;

typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t formatCount;
    VkSurfaceFormatKHR *formats;
    uint32_t presentModeCount;
    VkPresentModeKHR *presentModes;
} SwapChainSupportDetails;

typedef struct ShaderInfo {
    const char *filename;
    char *code;
    long length;
} ShaderInfo;

static void         createInstance(State *state);
// static const char **getRequiredLayers(const State *state, uint32_t *requiredLayerCount);
static void         checkRequiredLayers(const char **requiredLayers, uint32_t requiredLayerCount);
static const char **getRequiredExtensions(const State *state, uint32_t *requiredExtensionCount);
static void         checkRequiredExtensions(const char **requiredExtensions, uint32_t requiredExtensionCount);
// static const char **addLayer(const char **layers, uint32_t *layerCount, const char *layer);
static const char **addExtension(const char **extensions, uint32_t *extensionCount, const char *extension);
static const char **addStringToArray(const char **strings, uint32_t *stringCount, const char *string);

static void createSurface(State *state);

static void               pickPhysicalDevice(State *state);
static bool               isDeviceSuitable(State *state, VkPhysicalDevice device);
static QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice device);
// static const char **      getRequiredDeviceExtensions(State *state, VkPhysicalDevice device, uint32_t *requiredDeviceExtensionCount);
static bool               checkRequiredDeviceExtensions(VkPhysicalDevice device, const char **requiredDeviceExtensions, uint32_t requiredDeviceExtensionCount);

static void createLogicalDevice(State *state);

static void                    createSwapChain(State *state);
static SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
static VkSurfaceFormatKHR      chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, uint32_t formatCount);
static VkPresentModeKHR        chooseSwapPresentMode(const VkPresentModeKHR *availablePresentModes, uint32_t presentModeCount);
static VkExtent2D              chooseSwapExtent(State *state, const VkSurfaceCapabilitiesKHR *capabilities);
static uint32_t                clamp(uint32_t value, uint32_t min, uint32_t max);

static void createImageViews(State *state);

static void createRenderPass(State *state);

static void createGraphicsPipeline(State *state);
static ShaderInfo readFile(const char *filename);
VkShaderModule createShaderModule(State *state, ShaderInfo shader);

static void createFramebuffers(State *state);

static void createCommandPool(State *state);

static void createCommandBuffer(State *state);
static void recordCommandBuffer(State *state, VkCommandBuffer commandBuffer, uint32_t imageIndex);

static void createSyncObjects(State *state);

static void recreateSwapChain(State *state);
static void cleanupSwapChain(State *state);

/*
 * Public Functions
 */

void initVulkan(State *state) {
    createInstance(state);

#ifndef NDEBUG
    setupDebugMessenger(state);
#endif // NDEBUG

    createSurface(state);
    pickPhysicalDevice(state);
    createLogicalDevice(state);
    createSwapChain(state);
    createImageViews(state);
    createRenderPass(state);
    createGraphicsPipeline(state);
    createFramebuffers(state);
    createCommandPool(state);
    createCommandBuffer(state);
    createSyncObjects(state);
}

void drawFrame(State *state) {
    // Waiting for the previous frame
    PANIC(vkWaitForFences(state->device, 1, &state->inFlightFence[state->currentFrame], VK_TRUE, UINT64_MAX), "Failed to wait for fence");

    // Acquiring an image from the swap chain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(state->device, state->swapchain, UINT64_MAX, state->imageAvailableSemaphore[state->currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain(state);
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        PANIC(result, "Failed to acquire swap chain image");
    }

    // Only reset the fence if we are submitting work
    PANIC(vkResetFences(state->device, 1, &state->inFlightFence[state->currentFrame]), "Failed to reset fence");

    // Recording the command buffer
    vkResetCommandBuffer(state->commandBuffer[state->currentFrame], 0);
    recordCommandBuffer(state, state->commandBuffer[state->currentFrame], imageIndex);

    // Submitting the command buffer
    VkSemaphore waitSemaphores[] = {
        state->imageAvailableSemaphore[state->currentFrame]
    };

    VkSemaphore signalSemaphores[] = {
        state->renderFinishedSemaphore[state->currentFrame]
    };

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &state->commandBuffer[state->currentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    PANIC(vkQueueSubmit(state->presentQueue, 1, &submitInfo, state->inFlightFence[state->currentFrame]), "Failed to submit draw command buffer");

    // Presentation
    VkSwapchainKHR swapChains[] = {
        state->swapchain
    };

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &imageIndex,
        .pResults = NULL, // Optional
    };

    result = vkQueuePresentKHR(state->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || state->recreate_swapchain) {
        state->recreate_swapchain = false;
        recreateSwapChain(state);
    } else if (result != VK_SUCCESS) {
        PANIC(result, "Failed to present swap chain image");
    }

    state->currentFrame = (state->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void destroyVulkan(State *state) {
    cleanupSwapChain(state);

    vkDestroyPipeline(state->device, state->graphicsPipeline, state->allocator);
    vkDestroyPipelineLayout(state->device, state->pipelineLayout, state->allocator);
    vkDestroyRenderPass(state->device, state->renderPass, state->allocator);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(state->device, state->imageAvailableSemaphore[i], state->allocator);
        vkDestroySemaphore(state->device, state->renderFinishedSemaphore[i], state->allocator);
        vkDestroyFence(state->device, state->inFlightFence[i], state->allocator);
    }

    vkDestroyCommandPool(state->device, state->commandPool, state->allocator);
    vkDestroyDevice(state->device, state->allocator);

#ifndef NDEBUG
    DestroyDebugMessenger(state);
#endif // NDEBUG

    vkDestroySurfaceKHR(state->instance, state->surface, state->allocator);
    vkDestroyInstance(state->instance, state->allocator);
}

/*
 * Private Functions
 */

/*
 *  The very first thing you need to do is initialize the Vulkan library by creating an instance. The instance is the
 *  connection between your application and the Vulkan library and creating it involves specifying some details about
 *  your application to the driver.
 */
static void createInstance(State *state) {
    // uint32_t requiredLayerCount = 0;
    // const char **requiredLayers = getRequiredLayers(state, &requiredLayerCount);

    uint32_t requiredExtensionCount = 0;
    const char **requiredExtensions = getRequiredExtensions(state, &requiredExtensionCount);

    checkRequiredLayers(state->layers, state->layer_count);
    checkRequiredExtensions(requiredExtensions, requiredExtensionCount);

    const VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = state->window_title,
        .applicationVersion = state->application_version,
        .pEngineName = state->engine_name,
        .engineVersion = state->engine_version,
        .apiVersion = state->api_version,
    };

    const VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = state->layer_count,
        .ppEnabledLayerNames = state->layers,
        .enabledExtensionCount = requiredExtensionCount,
        .ppEnabledExtensionNames = requiredExtensions,
    };

    PANIC(vkCreateInstance(&instanceCreateInfo, state->allocator, &state->instance), "Failed to create Vulkan instance");

    // for (int i = 0; i < requiredLayerCount; i++) {
    //     free((void *) requiredLayers[i]);
    // }
    // free(requiredLayers);

    for (int i = 0; i < requiredExtensionCount; i++) {
        free((void *) requiredExtensions[i]);
    }
    free(requiredExtensions);
}

// static const char **getRequiredLayers(const State *state, uint32_t *requiredLayerCount) {
//     uint32_t layerCount = 0;
//     const char **layers = NULL;
//
//     for (int i = 0; i < state->layer_count; i++) {
//         layers = addLayer(layers, &layerCount, state->layers[i]);
//     }
//
//     checkRequiredLayers(layers, layerCount);
//
//     *requiredLayerCount = layerCount;
//
//     return layers;
// }
//
// static const char **addLayer(const char **layers, uint32_t *layerCount, const char *layer) {
//     return addStringToArray(layers, layerCount, layer);
// }

static void checkRequiredLayers(const char **requiredLayers, uint32_t requiredLayerCount) {
    uint32_t availableLayerCount;
    PANIC(vkEnumerateInstanceLayerProperties(&availableLayerCount, NULL), "Failed to get instance layer count");

    VkLayerProperties *availableLayers = calloc(availableLayerCount, sizeof(VkLayerProperties));
    PANIC(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers), "Failed to get instance layers");

    for (int i = 0; i < requiredLayerCount; i++) {
        bool found = false;

        for (int j = 0; j < availableLayerCount; j++) {
            if (strcmp(requiredLayers[i], availableLayers[j].layerName) == 0) {
                found = true;
                break;
            }
        }

        PANIC(!found, "Required layer %s not found", requiredLayers[i]);
    }

    free(availableLayers);
}

static const char **getRequiredExtensions(const State *state, uint32_t *requiredExtensionCount) {
    uint32_t glfwExtensionCount;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    if (glfwExtensions == NULL) {
        PANIC(1, "Failed to get required GLFW extensions");
        return NULL;
    }

    uint32_t extensionCount = 0;
    const char **extensions = NULL;

    for (int i = 0; i < glfwExtensionCount; i++) {
        extensions = addExtension(extensions, &extensionCount, glfwExtensions[i]);
    }

    for (int i = 0; i < state->extension_count; i++) {
        extensions = addExtension(extensions, &extensionCount, state->extensions[i]);
    }

    *requiredExtensionCount = extensionCount;

    return extensions;
}

static void checkRequiredExtensions(const char **requiredExtensions, uint32_t requiredExtensionCount) {
    uint32_t availableExtensionCount;
    PANIC(vkEnumerateInstanceExtensionProperties(NULL, &availableExtensionCount, NULL), "Failed to get instance extension count");

    VkExtensionProperties *availableExtensions = calloc(availableExtensionCount, sizeof(VkExtensionProperties));
    PANIC(vkEnumerateInstanceExtensionProperties(NULL, &availableExtensionCount, availableExtensions), "Failed to get instance extensions");

    for (int i = 0; i < requiredExtensionCount; i++) {
        bool found = false;

        for (int j = 0; j < availableExtensionCount; j++) {
            if (strcmp(requiredExtensions[i], availableExtensions[j].extensionName) == 0) {
                found = true;
                break;
            }
        }

        PANIC(!found, "Required layer %s not found", requiredExtensions[i]);
    }

    free(availableExtensions);
}

static const char **addExtension(const char **extensions, uint32_t *extensionCount, const char *extension) {
    return addStringToArray(extensions, extensionCount, extension);
}

static const char**addStringToArray(const char **strings, uint32_t *stringCount, const char *string) {
    for (int i = 0; i < *stringCount; i++) {
        if (strcmp(strings[i], string) == 0) {
            return strings;
        }
    }

    const char **newStrings = realloc(strings, sizeof(char *) * (*stringCount + 1));

    if (newStrings == NULL) {
        free(strings);
        PANIC(1, "Failed to reallocate memory for strings");
        return NULL;
    }

    newStrings[*stringCount] = strdup(string);

    if (newStrings[*stringCount] == NULL) {
        PANIC(1, "Failed to allocate memory for string");
        return NULL;
    }

    *stringCount += 1;

    return newStrings;
}

/*
 * The window surface needs to be created right after the instance creation, because it can actually influence the
 * physical device selection. The reason we postponed this is that window surfaces are part of the larger topic of
 * render targets and presentation for which the explanation would have cluttered the basic setup. It should also be
 * noted that window surfaces are an entirely optional component in Vulkan, if you just need off-screen rendering.
 * Vulkan allows you to do that without hacks like creating an invisible window (necessary for OpenGL).
 */
static void createSurface(State *state) {
    PANIC(glfwCreateWindowSurface(state->instance, state->window, state->allocator, &state->surface), "Failed to create window surface!");
}

/*
 * After initializing the Vulkan library through a VkInstance we need to look for and select a graphics card in the
 * system that supports the features we need. In fact, we can select any number of graphics cards and use them
 * simultaneously, but in this tutorial we'll stick to the first graphics card that suits our needs.
 */
static void pickPhysicalDevice(State *state) {
    uint32_t deviceCount = 0;

    PANIC(vkEnumeratePhysicalDevices(state->instance, &deviceCount, NULL), "Couldn't get physical device count");
    if (deviceCount == 0) {
        PANIC(1, "Failed to find GPUs with Vulkan support!");
        return;
    }

    VkPhysicalDevice *devices = calloc(deviceCount, sizeof(VkPhysicalDevice));
    if (devices == NULL) {
        PANIC(1, "Failed to allocate memory for physical devices");
        return;
    }

    PANIC(vkEnumeratePhysicalDevices(state->instance, &deviceCount, devices), "Couldn't get physical devices");

    for (int i = 0; i < deviceCount; i++) {
        if (isDeviceSuitable(state, devices[i])) {
            state->physical_device = devices[i];
            break;
        }
    }

    free(devices);

    if (state->physical_device == VK_NULL_HANDLE) {
        PANIC(1, "Failed to find a suitable GPU!");
    }
}

static bool isDeviceSuitable(State *state, VkPhysicalDevice device) {
    // VkPhysicalDeviceProperties deviceProperties;
    // vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // VkPhysicalDeviceFeatures deviceFeatures;
    // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;

    QueueFamilyIndices indices = findQueueFamilies(state->surface, device);
    const bool queueFamiliesFound = indices.graphicsFamily != UINT32_MAX && indices.presentFamily != UINT32_MAX;

    const bool extensionsSupported = checkRequiredDeviceExtensions(device, state->device_extensions, state->device_extension_count);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(state->surface, device);
        swapChainAdequate = swapChainSupport.formatCount && swapChainSupport.presentModeCount;

        free(swapChainSupport.formats);
        free(swapChainSupport.presentModes);
    }

    return queueFamiliesFound && extensionsSupported && swapChainAdequate;
}

static QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice) {
    QueueFamilyIndices indices = {
        .graphicsFamily = UINT32_MAX,
        .presentFamily = UINT32_MAX,
    };

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

    VkQueueFamilyProperties *queue_family_properties = calloc(queueFamilyCount, sizeof(VkQueueFamilyProperties));
    if (queue_family_properties == NULL) {
        PANIC(1, "Failed to allocate memory for queue family properties");
        return indices;
    }

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queue_family_properties);

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        VkBool32 presentSupport = false;
        PANIC(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport), "Failed to get physical device surface support");

        if (presentSupport) {
            indices.presentFamily = i;
        }

        // && glfwGetPhysicalDevicePresentationSupport(state->instance, physicalDevice, i)
        if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if (indices.graphicsFamily != UINT32_MAX && indices.presentFamily != UINT32_MAX) {
            break;
        }
    }

    free(queue_family_properties);

    return indices;
}

// static const char **getRequiredDeviceExtensions(State *state, VkPhysicalDevice device, uint32_t *requiredDeviceExtensionCount) {
//     uint32_t deviceExtensionCount = 0;
//     const char **deviceExtensions = NULL;
//
//     for (int i = 0; i < state->device_extension_count; i++) {
//         deviceExtensions = addExtension(deviceExtensions, &deviceExtensionCount, state->device_extensions[i]);
//     }
//
//     if (checkRequiredDeviceExtensions(device, deviceExtensions, deviceExtensionCount)) {
//         return NULL;
//     }
//
//     *requiredDeviceExtensionCount = deviceExtensionCount;
//
//     return deviceExtensions;
// }

static bool checkRequiredDeviceExtensions(VkPhysicalDevice device, const char **requiredDeviceExtensions, uint32_t requiredDeviceExtensionCount) {
    uint32_t availableDeviceExtensionCount;
    PANIC(vkEnumerateDeviceExtensionProperties(device, NULL, &availableDeviceExtensionCount, NULL), "Failed to get device extension count");

    VkExtensionProperties *availableDeviceExtensions = calloc(availableDeviceExtensionCount, sizeof(VkExtensionProperties));
    PANIC(vkEnumerateDeviceExtensionProperties(device, NULL, &availableDeviceExtensionCount, availableDeviceExtensions), "Failed to get available device extensions");

    for (int i = 0; i < requiredDeviceExtensionCount; i++) {
        bool found = false;

        for (int j = 0; j < availableDeviceExtensionCount; j++) {
            if (strcmp(requiredDeviceExtensions[i], availableDeviceExtensions[j].extensionName) == 0) {
                found = true;
                break;
            }
        }

        if (! found) {
            free(availableDeviceExtensions);
            return false;
        }
    }

    free(availableDeviceExtensions);

    return true;
}

static void createLogicalDevice(State *state) {
    QueueFamilyIndices indices = findQueueFamilies(state->surface, state->physical_device);

    if (indices.graphicsFamily == UINT32_MAX || indices.presentFamily == UINT32_MAX) {
        PANIC(1, "Failed to find queue families");
        return;
    }

    uint32_t queueCount = 0;
    VkDeviceQueueCreateInfo *queueCreateInfos = calloc(2, sizeof(VkDeviceQueueCreateInfo));
    if (queueCreateInfos == NULL) {
        PANIC(1, "Failed to allocate memory for queue create info");
        return;
    }

    float queuePriority = 1.0f;
    const VkDeviceQueueCreateInfo queueGraphicsCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueCount = 1,
        .queueFamilyIndex = indices.graphicsFamily,
        .pQueuePriorities = &queuePriority,
    };

    queueCreateInfos[0] = queueGraphicsCreateInfo;
    ++queueCount;

    if (indices.graphicsFamily != indices.presentFamily) {
        const VkDeviceQueueCreateInfo queuePresentCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount = 1,
            .queueFamilyIndex = indices.presentFamily,
            .pQueuePriorities = &queuePriority,
        };

        queueCreateInfos[1] = queuePresentCreateInfo;
        ++queueCount;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};

    if (!checkRequiredDeviceExtensions(state->physical_device, state->device_extensions, state->device_extension_count)) {
        PANIC(1, "Failed to find required device extensions");
        return;
    }

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = queueCount,
        .pQueueCreateInfos = queueCreateInfos,
        .pEnabledFeatures = &deviceFeatures,
        .enabledExtensionCount = state->device_extension_count,
        .ppEnabledExtensionNames = state->device_extensions,
        .enabledLayerCount = state->layer_count,
        .ppEnabledLayerNames = state->layers,
    };

    PANIC(vkCreateDevice(state->physical_device, &deviceCreateInfo, state->allocator, &state->device), "Failed to create logical device");

    vkGetDeviceQueue(state->device, indices.presentFamily, 0, &state->presentQueue);
    state->queue_present_family_index = indices.presentFamily;

    if (indices.graphicsFamily != indices.presentFamily) {
        vkGetDeviceQueue(state->device, indices.graphicsFamily, 0, &state->graphicsQueue);
        state->queue_graphics_family_index = indices.graphicsFamily;
    }

    free(queueCreateInfos);
}

static void createSwapChain(State *state) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(state->surface, state->physical_device);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.formatCount);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes, swapChainSupport.presentModeCount);
    state->swapchain_extent = chooseSwapExtent(state, &swapChainSupport.capabilities);

    state->swapchain_image_count = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && state->swapchain_image_count > swapChainSupport.capabilities.maxImageCount) {
        state->swapchain_image_count = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = state->surface,
        .minImageCount = state->swapchain_image_count,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = state->swapchain_extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = swapChainSupport.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    QueueFamilyIndices indices = findQueueFamilies(state->surface, state->physical_device);
    const uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
        swapChainCreateInfo.pQueueFamilyIndices = NULL; // Optional
    }

    PANIC(vkCreateSwapchainKHR(state->device, &swapChainCreateInfo, state->allocator, &state->swapchain), "Failed to create swap chain");

    vkGetSwapchainImagesKHR(state->device, state->swapchain, &state->swapchain_image_count, NULL);
    state->swapchain_images = calloc(state->swapchain_image_count, sizeof(VkImage));
    vkGetSwapchainImagesKHR(state->device, state->swapchain, &state->swapchain_image_count, state->swapchain_images);

    state->swapchain_image_format = surfaceFormat.format;

    free(swapChainSupport.formats);
    free(swapChainSupport.presentModes);
}

static SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice) {
    SwapChainSupportDetails details = {};

    PANIC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities), "Failed to get physical device surface capabilities");
    PANIC(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &details.formatCount, NULL), "Failed to get physical device surface format count");
    PANIC(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &details.presentModeCount, NULL), "Failed to get physical device surface present mode count");

    if (details.formatCount != 0) {
        details.formats = calloc(details.formatCount, sizeof(VkSurfaceFormatKHR));
        if (details.formats == NULL) {
            PANIC(1, "Failed to allocate memory for physical device surface formats");
            return details;
        }
        PANIC(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &details.formatCount, details.formats), "Failed to get physical device surface formats");
    }

    if (details.presentModeCount != 0) {
        details.presentModes = calloc(details.presentModeCount, sizeof(VkPresentModeKHR));
        if (details.formats == NULL) {
            PANIC(1, "Failed to allocate memory for physical device surface formats");
            return details;
        }
        PANIC(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &details.presentModeCount, details.presentModes), "Failed to get physical device surface present modes");
    }

    return details;
}

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, uint32_t formatCount) {
    for (int i = 0; i < formatCount; i++) {
        if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormats[i];
        }
    }

    return availableFormats[0];
}

static VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR *availablePresentModes, uint32_t presentModeCount) {
    for (int i = 0; i < presentModeCount; i++) {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentModes[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D chooseSwapExtent(State *state, const VkSurfaceCapabilitiesKHR *capabilities) {
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(state->window, &width, &height);

    VkExtent2D actualExtent = {
        .width = clamp(width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width),
        .height = clamp(height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height),
    };

    return actualExtent;
}

static uint32_t clamp(const uint32_t value, const uint32_t min, const uint32_t max) {
    return value < min ? min : value > max ? max : value;
}

void createImageViews(State *state) {
    if (state->swapchain_images == NULL) {
        PANIC(1, "Swapchain images are null");
        return;
    }

    state->swapchain_image_views = calloc(state->swapchain_image_count, sizeof(VkImageView));
    if (state->swapchain_image_views == NULL) {
        PANIC(1, "Failed to allocate memory for image views");
        return;
    }

    for (int i = 0; i < state->swapchain_image_count; i++) {
        if (state->swapchain_images[i] == VK_NULL_HANDLE) {
            PANIC(1, "Swapchain image %i is null handle", i);
            return;
        }

        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = state->swapchain_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = state->swapchain_image_format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        PANIC(vkCreateImageView(state->device, &imageViewCreateInfo, state->allocator, &state->swapchain_image_views[i]), "Failed to create image view");
    }
}

static void createRenderPass(State *state) {
    VkAttachmentDescription colorAttachment = {
        .format = state->swapchain_image_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference colorAttachmentRef = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    PANIC(vkCreateRenderPass(state->device, &renderPassInfo, state->allocator, &state->renderPass), "Failed to create render pass");
}


static void createGraphicsPipeline(State *state) {
    ShaderInfo vertShaderCode = readFile("../../engine/shaders/vert.spv");
    ShaderInfo fragShaderCode = readFile("../../engine/shaders/frag.spv");

    // Shader Modules
    VkShaderModule vertShaderModule = createShaderModule(state, vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(state, fragShaderCode);

    free(vertShaderCode.code);
    free(fragShaderCode.code);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageInfo,
        fragShaderStageInfo
    };

    // Dynamic state
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = (uint32_t) 2,
        .pDynamicStates = dynamicStates
    };

    // Vertex Input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = NULL, // Optional
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = NULL, // Optional

    };

    // Input Assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    // Viewports and Scissors
    VkPipelineViewportStateCreateInfo viewportState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f, // Optional
        .depthBiasClamp = 0.0f, // Optional
        .depthBiasSlopeFactor = 0.0f, // Optional
    };

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f, // Optional
        .pSampleMask = NULL, // Optional
        .alphaToCoverageEnable = VK_FALSE, // Optional
        .alphaToOneEnable = VK_FALSE, // Optional
    };

    // Depth and stencil testing

    // Color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
        .colorBlendOp = VK_BLEND_OP_ADD, // Optional
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
        .alphaBlendOp = VK_BLEND_OP_ADD, // Optional
    };

    // VkPipelineColorBlendAttachmentState colorBlendAttachment = {
    //     .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    //     .blendEnable = VK_TRUE,
    //     .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    //     .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    //     .colorBlendOp = VK_BLEND_OP_ADD, // Optional
    //     .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
    //     .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
    //     .alphaBlendOp = VK_BLEND_OP_ADD, // Optional
    // };

    VkPipelineColorBlendStateCreateInfo colorBlending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY, // Optional
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}, // Optional
    };

    // Pipeline Layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0, // Optional
        .pSetLayouts = NULL, // Optional
        .pushConstantRangeCount = 0, // Optional
        .pPushConstantRanges = NULL, // Optional
    };

    PANIC(vkCreatePipelineLayout(state->device, &pipelineLayoutInfo, state->allocator, &state->pipelineLayout), "Failed to create pipeline layout");

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = NULL, // Optional
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState, // Optional
        .layout = state->pipelineLayout,
        .renderPass = state->renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE, // Optional
        .basePipelineIndex = -1, // Optional
    };

    PANIC(vkCreateGraphicsPipelines(state->device, VK_NULL_HANDLE, 1, &pipelineInfo, state->allocator, &state->graphicsPipeline), "Failed to create graphics pipeline");

    vkDestroyShaderModule(state->device, fragShaderModule, state->allocator);
    vkDestroyShaderModule(state->device, vertShaderModule, state->allocator);
}

static ShaderInfo readFile(const char *filename) {
    ShaderInfo shaderInfo = {
        .filename = filename,
        .code = NULL,
        .length = 0,
    };

    FILE *file = fopen(shaderInfo.filename, "rb");
    if (file == NULL) {
        PANIC(1, "Failed to open file %s", filename);
        return shaderInfo;
    }

    fseek(file, 0, SEEK_END);
    shaderInfo.length = ftell(file);
    rewind(file);

    shaderInfo.code = calloc(shaderInfo.length + 1, sizeof(char));
    if (shaderInfo.code == NULL) {
        PANIC(1, "Failed to allocate memory for file content");
        fclose(file);
        return shaderInfo;
    }

    if (fread(shaderInfo.code, sizeof(char), shaderInfo.length, file) != shaderInfo.length) {
        PANIC(1, "Failed to read file %s", filename);
        fclose(file);
        free(shaderInfo.code);
        return shaderInfo;
    }

    fclose(file);

    return shaderInfo;
}

VkShaderModule createShaderModule(State *state, ShaderInfo shader) {
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = shader.length,
        .pCode = (const uint32_t *) shader.code,
    };

    VkShaderModule shaderModule;
    PANIC(vkCreateShaderModule(state->device, &shaderModuleCreateInfo, state->allocator, &shaderModule), "Failed to create shader module");

    return shaderModule;
}

static void createFramebuffers(State *state) {
    state->swapChainFramebuffers = calloc(state->swapchain_image_count, sizeof(VkFramebuffer));
    if (state->swapChainFramebuffers == NULL) {
        PANIC(1, "Failed to allocate memory for framebuffers");
        return;
    }

    for (int i = 0; i < state->swapchain_image_count; i++) {
        VkImageView attachments[] = {
            state->swapchain_image_views[i],
        };

        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = state->renderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = state->swapchain_extent.width,
            .height = state->swapchain_extent.height,
            .layers = 1,
        };

        PANIC(vkCreateFramebuffer(state->device, &framebufferInfo, state->allocator, &state->swapChainFramebuffers[i]), "Failed to create framebuffer");
    }
}

static void createCommandPool(State *state) {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(state->surface, state->physical_device);

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily,
    };

    PANIC(vkCreateCommandPool(state->device, &poolInfo, state->allocator, &state->commandPool), "Failed to create command pool");
}

static void createCommandBuffer(State *state) {
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = state->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
    };

    PANIC(vkAllocateCommandBuffers(state->device, &allocInfo, state->commandBuffer), "Failed to allocate command buffer");
}

static void recordCommandBuffer(State *state, VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    PANIC(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin recording command buffer");

    VkClearValue clearColor = {
        .color = {
            .float32 = {0.0f, 0.0f, 0.0f, 1.0f}
        }
    };

    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = state->renderPass,
        .framebuffer = state->swapChainFramebuffers[imageIndex],
        .renderArea = {
            .offset = {0, 0},
            .extent = state->swapchain_extent,
        },
        .clearValueCount = 1,
        .pClearValues = &clearColor,
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state->graphicsPipeline);

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) state->swapchain_extent.width,
        .height = (float) state->swapchain_extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = state->swapchain_extent,
    };

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    PANIC(vkEndCommandBuffer(commandBuffer), "Failed to record command buffer");
}

static void createSyncObjects(State *state) {
    VkSemaphoreCreateInfo imageAvailableSemaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkSemaphoreCreateInfo renderFinishedSemaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        PANIC(vkCreateSemaphore(state->device, &imageAvailableSemaphoreInfo, state->allocator, &state->imageAvailableSemaphore[i]), "Failed to create image available semaphore");
        PANIC(vkCreateSemaphore(state->device, &renderFinishedSemaphoreInfo, state->allocator, &state->renderFinishedSemaphore[i]), "Failed to create render finished semaphore");
        PANIC(vkCreateFence(state->device, &fenceInfo, state->allocator, &state->inFlightFence[i]), "Failed to create in flight fence");
    }
}

static void recreateSwapChain(State *state) {
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(state->window, &width, &height);

    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(state->window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(state->device);

    cleanupSwapChain(state);

    createSwapChain(state);
    createImageViews(state);
    createFramebuffers(state);
}

static void cleanupSwapChain(State *state) {
    for (int i = 0; i < state->swapchain_image_count; i++) {
        vkDestroyFramebuffer(state->device, state->swapChainFramebuffers[i], state->allocator);
    }
    free(state->swapChainFramebuffers);

        for (int i = 0; i < state->swapchain_image_count; i++) {
        vkDestroyImageView(state->device, state->swapchain_image_views[i], state->allocator);
    }
    free(state->swapchain_image_views);
    free(state->swapchain_images);

    vkDestroySwapchainKHR(state->device, state->swapchain, state->allocator);
}