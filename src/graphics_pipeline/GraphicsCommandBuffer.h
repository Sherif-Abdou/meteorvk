//
// Created by Sherif Abdou on 8/5/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSCOMMANDBUFFER_H
#define VULKAN_ENGINE_GRAPHICSCOMMANDBUFFER_H

#include "../VulkanContext.h"
#include "GraphicsPipeline.h"
#include <vulkan/vulkan.hpp>

class GraphicsCommandBuffer {
private:
    VulkanContext& context;
    vk::raii::CommandPool pool= nullptr;
    vk::raii::CommandBuffer commandBuffer= nullptr;
    vk::raii::Semaphore imageAvailableSemaphore= nullptr;
    vk::raii::Semaphore renderFinishedSemaphore= nullptr;
    vk::raii::Fence inFlightFence= nullptr;

private:
    uint32_t swapChainImageIndex;

    void createCommandPool();
    void createCommandBuffer();
    void createSyncObjects();

    void waitForFence();
    void fetchSwapchain();
    void recordCommandBuffer();
    void submitCommandBuffer();
    void sendToSwapchain();

public:
    std::vector<VertexBuffer> vertexBuffers {};
    std::vector<GraphicsPipeline> pipelines {};

    explicit GraphicsCommandBuffer(VulkanContext &context);

    void init();
    void renderToSwapchain();
};


#endif //VULKAN_ENGINE_GRAPHICSCOMMANDBUFFER_H
