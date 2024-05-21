//
// Created by Sherif Abdou on 8/5/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSCOMMANDBUFFER_H
#define VULKAN_ENGINE_GRAPHICSCOMMANDBUFFER_H

#include "core/VulkanContext.h"
#include "GraphicsPipeline.h"
#include "core/storage/UniformBuffer.h"
#include <vulkan/vulkan.hpp>
#include <unordered_map>

#include "core/shared_pipeline/PipelineBarrier.h"

/// Command buffer that runs pipelines
class GraphicsCommandBuffer {
private:
    uint32_t current_frame = 0;
    constexpr static uint32_t FRAMES_IN_FLIGHT = 2;
    VulkanContext* context;
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
    /// The pipeline layout to bind to a descriptor set
    struct DescriptorPipelineBinding {
        DescriptorSet* descriptorSet;
        vk::raii::PipelineLayout* layout;
        uint32_t set = 0;
    };

    std::vector<DescriptorPipelineBinding> bindings {}; /// Descriptor set to bind for each pipeline
    std::vector<VertexBuffer*> vertexBuffers {}; /// All vertex buffers to render
    std::vector<Renderable*> pipelines {}; /// Pipelines to run in order
    std::unordered_map<uint32_t, std::vector<PipelineBarrier>> dependencies {}; /// Dependency to bind per pipeline

    explicit GraphicsCommandBuffer(VulkanContext *context);

    void init();
    void renderToSwapchain();
    void destroy();

    void beginCommandBuffer() const;

    void beginSwapchainRender();

    void finishSwapchainRender();
};


#endif //VULKAN_ENGINE_GRAPHICSCOMMANDBUFFER_H
