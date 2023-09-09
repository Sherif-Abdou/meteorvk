//
// Created by Sherif Abdou on 8/5/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSCOMMANDBUFFER_H
#define VULKAN_ENGINE_GRAPHICSCOMMANDBUFFER_H

#include "../VulkanContext.h"
#include "GraphicsPipeline.h"
#include "../storage/UniformBuffer.h"
#include <vulkan/vulkan.hpp>

class GraphicsCommandBuffer {
private:
    VulkanContext& context;
    vk::raii::CommandPool pool= nullptr;
    vk::raii::CommandBuffer commandBuffer= nullptr;
    vk::raii::Semaphore imageAvailableSemaphore= nullptr;
    vk::raii::Semaphore renderFinishedSemaphore= nullptr;
    vk::raii::Fence inFlightFence= nullptr;
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
    struct DescriptorPipelineBinding {
        DescriptorSet* descriptorSet;
        vk::raii::PipelineLayout* layout;
        uint32_t set = 0;
    };
    std::vector<DescriptorPipelineBinding> bindings {};
    std::vector<VertexBuffer> vertexBuffers {};
    std::vector<GraphicsPipeline> pipelines {};

    explicit GraphicsCommandBuffer(VulkanContext &context);

    void init();
    void renderToSwapchain();
    void bindDescriptorSet(DescriptorSet &descriptor_set, vk::raii::PipelineLayout &, uint32_t set_number = 0);
    void destroy();

    void beginCommandBuffer() const;

    void beginSwapchainRender();

    void finishSwapchainRender();
};


#endif //VULKAN_ENGINE_GRAPHICSCOMMANDBUFFER_H
