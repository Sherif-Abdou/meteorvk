//
// Created by sheri on 7/28/2023.
//

#ifndef VULKAN_TEST_VULKANCONTEXT_H
#define VULKAN_TEST_VULKANCONTEXT_H


#include <optional>
#include <vector>
#include <string>
#include <set>
#include <stdexcept>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <algorithm>
#include <iostream>

class VulkanAllocator;

struct VulkanContext {
    GLFWwindow* window = nullptr;
    vk::raii::Context context {};
    vk::raii::Instance instance = VK_NULL_HANDLE;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    vk::raii::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
    vk::raii::Device device = VK_NULL_HANDLE;
    vk::raii::Queue graphicsQueue = VK_NULL_HANDLE;
    vk::raii::Queue presentQueue = VK_NULL_HANDLE;
    vk::raii::SurfaceKHR surface = VK_NULL_HANDLE;
    vk::raii::SwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::raii::ImageView> swapChainImageViews;
    vk::Format swapChainImageFormat;
    vk::Extent2D swapChainExtent;
    VulkanAllocator* allocator;

    VulkanContext();

    void initVulkan();
    void cleanup();

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    QueueFamilyIndices findQueueFamilies(vk::raii::PhysicalDevice device);

    virtual ~VulkanContext();

private:

    void initWindow();

    void createInstance();

    std::vector<const char *> getRequiredExtensions();

    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};

    bool checkValidationLayerSupport();

    void setupDebugMessenger();

    void pickPhysicalDevice();

    bool isDeviceSuitable(vk::raii::PhysicalDevice device);

    void createLogicalDevice();

    void createSurface();

    bool checkDeviceExtensionsSupport(vk::raii::PhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(vk::raii::PhysicalDevice device);

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

    void createSwapChain();

    void createImageViews();

    void createAllocator();
};

#include "VulkanAllocator.h"

#endif //VULKAN_TEST_VULKANCONTEXT_H
