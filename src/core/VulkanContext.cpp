//
// Created by sheri on 7/28/2023.
//

#include "VulkanContext.h"

constexpr int WIDTH = 2560.0f / 2;
constexpr int HEIGHT = 1440.0f / 2;

void VulkanContext::initVulkan() {
    initWindow();
    createInstance();
    if (checkValidationLayerSupport()) {
        setupDebugMessenger();
    }
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createAllocator();
}

void VulkanContext::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
//    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

//    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void VulkanContext::createInstance() {
    context = vk::raii::Context();

    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Test";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    appInfo.pEngineName = "Test Vulkan Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

#ifdef __APPLE__
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    std::vector<const char *> requiredExtensions = getRequiredInstanceExtensions();

    auto enableValidationLayers = checkValidationLayerSupport();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    instance = vk::raii::Instance(context, createInfo);
}

std::vector<const char *> VulkanContext::getRequiredInstanceExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> requiredExtensions;

    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        requiredExtensions.emplace_back(glfwExtensions[i]);
    }

    if (checkValidationLayerSupport()) {
        requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

#ifdef __APPLE__
    requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
    requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    return requiredExtensions;
}

bool VulkanContext::checkValidationLayerSupport() {
    if (validationLayers.size() == 0) {
        return false;
    }
    unsigned int layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (auto layerName : validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL ContextDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult ContextCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanContext::setupDebugMessenger() {
    if (!checkValidationLayerSupport()) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = ContextDebugCallback;
    createInfo.pUserData = nullptr; // Optional

    debugMessenger = instance.createDebugUtilsMessengerEXT(createInfo, nullptr);
}

void VulkanContext::pickPhysicalDevice() {
    std::vector<vk::raii::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            return;
        }
    }

}


bool VulkanContext::isDeviceSuitable(vk::raii::PhysicalDevice device) {
    vk::PhysicalDeviceProperties properties = device.getProperties();
    vk::PhysicalDeviceFeatures features = device.getFeatures();
    auto indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionsSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        auto swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

VulkanContext::QueueFamilyIndices VulkanContext::findQueueFamilies(vk::raii::PhysicalDevice device) {
    QueueFamilyIndices indices {};

    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & (vk::QueueFlagBits::eGraphics ^ vk::QueueFlagBits::eCompute)) {
            VkBool32 presentSupport = device.getSurfaceSupportKHR(i, *surface);
            if (presentSupport) {
                indices.presentFamily = i;
            }
            indices.graphicsFamily = i;
            indices.computeFamily = i;
        }
        if (indices.isComplete()) break;
        i++;
    }

    return indices;
}

void VulkanContext::createLogicalDevice() {
    auto indices = findQueueFamilies(physicalDevice);


    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.computeFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.sampleRateShading = VK_TRUE;

    VkPhysicalDeviceSynchronization2Features deviceFeatures2 {};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    deviceFeatures2.synchronization2 = true;


    VkPhysicalDeviceDescriptorIndexingFeatures descriptorFeatures {};
    descriptorFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    descriptorFeatures.descriptorBindingPartiallyBound = true;
    descriptorFeatures.descriptorBindingSampledImageUpdateAfterBind = true;
    descriptorFeatures.descriptorBindingVariableDescriptorCount = true;
    descriptorFeatures.descriptorBindingStorageBufferUpdateAfterBind = true;
    descriptorFeatures.runtimeDescriptorArray = true;


    deviceFeatures2.pNext = &descriptorFeatures;

    VkDeviceCreateInfo createInfo{};
    createInfo.pNext = &deviceFeatures2;
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    if (checkValidationLayerSupport()) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledLayerCount = 0;
    }

    device = physicalDevice.createDevice(createInfo);
    graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
    presentQueue = device.getQueue(indices.presentFamily.value(), 0);
    computeQueue = device.getQueue(indices.computeFamily.value(), 0);
}

void VulkanContext::createSurface() {
    VkSurfaceKHR localSurface;
    if (glfwCreateWindowSurface(*instance, window, nullptr, &localSurface) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create window surface");
    }
    surface = vk::raii::SurfaceKHR(instance, localSurface);
}

bool VulkanContext::checkDeviceExtensionsSupport(vk::raii::PhysicalDevice device) {
    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

    std::set<std::string> required_extensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension: availableExtensions) {
        required_extensions.erase(extension.extensionName);
    }
    return required_extensions.empty();
}

VulkanContext::SwapChainSupportDetails VulkanContext::querySwapChainSupport(vk::raii::PhysicalDevice device) {
    SwapChainSupportDetails details;

    details.capabilities = device.getSurfaceCapabilitiesKHR(*surface);


    details.formats = device.getSurfaceFormatsKHR(*surface);

    details.presentModes = device.getSurfacePresentModesKHR(*surface);

    return details;
}

vk::SurfaceFormatKHR VulkanContext::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR VulkanContext::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto& availablePresentMode: availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanContext::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height),
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void VulkanContext::createSwapChain() {
    auto swapChainSupport = querySwapChainSupport(physicalDevice);

    auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    auto extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    vk::SwapchainCreateInfoKHR createInfo {};
    createInfo.surface = *surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    auto indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;


    swapChain = device.createSwapchainKHR(createInfo);

    swapChainImages = swapChain.getImages();

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void VulkanContext::createImageViews() {
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        vk::ImageViewCreateInfo createInfo {};
        createInfo.image = swapChainImages[i];
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        swapChainImageViews.push_back(device.createImageView(createInfo));
    }
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void VulkanContext::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

VulkanContext::VulkanContext() {
}

void VulkanContext::createAllocator() {
    allocator = new VulkanAllocator();
    allocator->context = this;
    allocator->init();
}

VulkanContext::~VulkanContext() {
    allocator->destroy();
    delete allocator;
}



