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
    unsigned int current_frame = 0;
    static constexpr unsigned int FRAMES_IN_FLIGHT = 2;
    VulkanContext& context;
    vk::raii::CommandPool pool = nullptr;
    std::array<vk::raii::CommandBuffer, FRAMES_IN_FLIGHT> commandBuffer = {nullptr, nullptr};
    std::array<vk::raii::Semaphore, FRAMES_IN_FLIGHT> imageAvailableSemaphore = {nullptr, nullptr};
    std::array<vk::raii::Semaphore, FRAMES_IN_FLIGHT> renderFinishedSemaphore = {nullptr, nullptr};
    std::array<vk::raii::Fence, FRAMES_IN_FLIGHT> inFlightFence = {nullptr, nullptr};
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

    struct Dependency {
        vk::ImageMemoryBarrier imageBarrier;
        vk::PipelineStageFlags srcStageMask;
        vk::PipelineStageFlags dstStageMask;
    };
    std::vector<DescriptorPipelineBinding> bindings {};
    std::vector<VertexBuffer*> vertexBuffers {};
    std::vector<Renderable*> pipelines {};
    std::vector<Dependency> dependencies {};

    explicit GraphicsCommandBuffer(VulkanContext &context);

    void init();
    void renderToSwapchain();
    void destroy();

    void beginCommandBuffer() const;

    void beginSwapchainRender();

    void finishSwapchainRender();
};


#endif //VULKAN_ENGINE_GRAPHICSCOMMANDBUFFER_H
