//
// Created by Sherif Abdou on 8/4/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSPIPELINE_H
#define VULKAN_ENGINE_GRAPHICSPIPELINE_H

#include "../VulkanContext.h"
#include "GraphicsRenderPass.h"
#include "../storage/DescriptorSet.h"
#include "../storage/VertexBuffer.h"

class GraphicsPipeline {
public:
    struct ImagePair {
        VulkanAllocator::VulkanImageAllocation imageAllocation;
        vk::raii::ImageView imageView;
    };
    struct RenderArguments {
        vk::raii::CommandBuffer& commandBuffer;
        uint32_t imageIndex;
        std::vector<VertexBuffer>& vertexBuffers;
    };
    VulkanContext& context;
    GraphicsRenderPass renderPass;
    std::vector<vk::raii::Framebuffer> targetFramebuffers {};
    std::optional<DescriptorSet*> descriptorSet;

    explicit GraphicsPipeline(VulkanContext &context, GraphicsRenderPass&& renderPass);
    GraphicsPipeline(GraphicsPipeline&&) = default;

    void init();
    vk::raii::PipelineLayout pipelineLayout = nullptr;
    std::vector<ImagePair> ownedImages {};
    vk::raii::PipelineLayout &getPipelineLayout();
    vk::raii::Semaphore & getPipelineSemaphore();

    vk::raii::Fence & getPipelineFence();

    void renderPipeline(GraphicsPipeline::RenderArguments renderArguments);

    virtual ~GraphicsPipeline();

    void destroy();
private:
    vk::raii::Pipeline pipeline = nullptr;
public:
    const vk::raii::Pipeline &getPipeline() const;

    void setPipeline(vk::raii::Pipeline && pipeline);

private:
    vk::raii::Semaphore pipelineSemaphore = nullptr;
    void createSyncObjects();
    vk::raii::Fence pipelineFence = nullptr;

};


#endif //VULKAN_ENGINE_GRAPHICSPIPELINE_H
