//
// Created by Vinícius Ferreira Aguiar on 20/12/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
static const char **getRequiredLayers(const State *state, uint32_t *requiredLayerCount);
static void         checkRequiredLayers(const char **requiredLayers, uint32_t requiredLayerCount);
static const char **getRequiredExtensions(const State *state, uint32_t *requiredExtensionCount);
static void         checkRequiredExtensions(const char **requiredExtensions, uint32_t requiredExtensionCount);
static const char **addLayer(const char **layers, uint32_t *layerCount, const char *layer);
static const char **addExtension(const char **extensions, uint32_t *extensionCount, const char *extension);
static const char **addStringToArray(const char **strings, uint32_t *stringCount, const char *string);

static void createSurface(State *state);

static void               pickPhysicalDevice(State *state);
static bool               isDeviceSuitable(State *state, VkPhysicalDevice device);
static QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice device);
static const char **      getRequiredDeviceExtensions(State *state, VkPhysicalDevice device, uint32_t *requiredDeviceExtensionCount);
static bool               checkRequiredDeviceExtensions(VkPhysicalDevice device, const char **requiredDeviceExtensions, uint32_t requiredDeviceExtensionCount);

static void createLogicalDevice(State *state);

static void                    createSwapChain(State *state);
static SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
static VkSurfaceFormatKHR      chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, uint32_t formatCount);
static VkPresentModeKHR        chooseSwapPresentMode(const VkPresentModeKHR *availablePresentModes, uint32_t presentModeCount);
static VkExtent2D              chooseSwapExtent(State *state, const VkSurfaceCapabilitiesKHR *capabilities);
static uint32_t                clamp(uint32_t value, uint32_t min, uint32_t max);

static void createImageViews(State *state);

static void createGraphicsPipeline(State *state);
static ShaderInfo readFile(const char *filename);
VkShaderModule createShaderModule(State *state, ShaderInfo shader);

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
    createGraphicsPipeline(state);
}

void destroyVulkan(const State *state) {
#ifndef NDEBUG
    DestroyDebugMessenger(state);
#endif // NDEBUG

    for (int i = 0; i < state->swapchain_image_count; i++) {
        vkDestroyImageView(state->device, state->swapchain_image_views[i], state->allocator);
    }
    free(state->swapchain_image_views);
    free(state->swapchain_images);

    vkDestroySwapchainKHR(state->device, state->swapchain, state->allocator);
    vkDestroyDevice(state->device, state->allocator);
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
//     const char **layers = nullptr;
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
    PANIC(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr), "Failed to get instance layer count");

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
        return nullptr;
    }

    uint32_t extensionCount = 0;
    const char **extensions = nullptr;

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
    PANIC(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr), "Failed to get instance extension count");

    VkExtensionProperties *availableExtensions = calloc(availableExtensionCount, sizeof(VkExtensionProperties));
    PANIC(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions), "Failed to get instance extensions");

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
        return nullptr;
    }

    newStrings[*stringCount] = strdup(string);

    if (newStrings[*stringCount] == NULL) {
        PANIC(1, "Failed to allocate memory for string");
        return nullptr;
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

    PANIC(vkEnumeratePhysicalDevices(state->instance, &deviceCount, nullptr), "Couldn't get physical device count");
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
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

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

static const char **getRequiredDeviceExtensions(State *state, VkPhysicalDevice device, uint32_t *requiredDeviceExtensionCount) {
    uint32_t deviceExtensionCount = 0;
    const char **deviceExtensions = nullptr;

    for (int i = 0; i < state->device_extension_count; i++) {
        deviceExtensions = addExtension(deviceExtensions, &deviceExtensionCount, state->device_extensions[i]);
    }

    if (checkRequiredDeviceExtensions(device, deviceExtensions, deviceExtensionCount)) {
        return nullptr;
    }

    *requiredDeviceExtensionCount = deviceExtensionCount;

    return deviceExtensions;
}

static bool checkRequiredDeviceExtensions(VkPhysicalDevice device, const char **requiredDeviceExtensions, uint32_t requiredDeviceExtensionCount) {
    uint32_t availableDeviceExtensionCount;
    PANIC(vkEnumerateDeviceExtensionProperties(device, nullptr, &availableDeviceExtensionCount, nullptr), "Failed to get device extension count");

    VkExtensionProperties *availableDeviceExtensions = calloc(availableDeviceExtensionCount, sizeof(VkExtensionProperties));
    PANIC(vkEnumerateDeviceExtensionProperties(device, nullptr, &availableDeviceExtensionCount, availableDeviceExtensions), "Failed to get available device extensions");

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
        swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    PANIC(vkCreateSwapchainKHR(state->device, &swapChainCreateInfo, state->allocator, &state->swapchain), "Failed to create swap chain");

    vkGetSwapchainImagesKHR(state->device, state->swapchain, &state->swapchain_image_count, nullptr);
    state->swapchain_images = calloc(state->swapchain_image_count, sizeof(VkImage));
    vkGetSwapchainImagesKHR(state->device, state->swapchain, &state->swapchain_image_count, state->swapchain_images);

    state->swapchain_image_format = surfaceFormat.format;

    free(swapChainSupport.formats);
    free(swapChainSupport.presentModes);
}

static SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice) {
    SwapChainSupportDetails details = {};

    PANIC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities), "Failed to get physical device surface capabilities");
    PANIC(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &details.formatCount, nullptr), "Failed to get physical device surface format count");
    PANIC(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &details.presentModeCount, nullptr), "Failed to get physical device surface present mode count");

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

static void createGraphicsPipeline(State *state) {
    ShaderInfo vertShaderCode = readFile("../src/shaders/vert.spv");
    ShaderInfo fragShaderCode = readFile("../src/shaders/frag.spv");

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


    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = (uint32_t) 2,
        .pDynamicStates = dynamicStates
    };

    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();


    vkDestroyShaderModule(state->device, fragShaderModule, state->allocator);
    vkDestroyShaderModule(state->device, vertShaderModule, state->allocator);
}

static ShaderInfo readFile(const char *filename) {
    ShaderInfo shaderInfo = {
        .filename = filename,
        .code = nullptr,
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
        return shaderInfo;
    }

    if (fread(shaderInfo.code, sizeof(char), shaderInfo.length, file) != shaderInfo.length) {
        PANIC(1, "Failed to read file %s", filename);
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
