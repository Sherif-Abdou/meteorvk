//
// Created by Sherif Abdou on 8/4/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSPIPELINE_H
#define VULKAN_ENGINE_GRAPHICSPIPELINE_H

#include "../VulkanContext.h"
#include "GraphicsRenderPass.h"
#include "../storage/DescriptorSet.h"
#include "../storage/VertexBuffer.h"
#include "../interfaces/Renderable.h"

/// Core of a pipeline, wrapper over VkPipeline
/// Stores the vulkan pipeline and the images it owns
class GraphicsPipeline: public Renderable {
public:
    struct ImagePair {
        VulkanAllocator::VulkanImageAllocation imageAllocation;
        vk::raii::ImageView imageView;
    };

    VulkanContext* context;
    std::unique_ptr<GraphicsRenderPass> ownedRenderPass = nullptr;
    std::unique_ptr<vk::raii::RenderPass> ownedDirectRenderPass = nullptr;

    std::vector<vk::raii::Framebuffer> targetFramebuffers {};

    explicit GraphicsPipeline(VulkanContext *context, std::unique_ptr<GraphicsRenderPass> renderPass);
    explicit GraphicsPipeline(VulkanContext *context, std::unique_ptr<vk::raii::RenderPass> renderPass);
    GraphicsPipeline(GraphicsPipeline&&) = default;

    void init();
    vk::raii::PipelineLayout pipelineLayout = nullptr;
    std::vector<ImagePair> ownedImages {};
    vk::raii::PipelineLayout &getPipelineLayout();
    vk::raii::Semaphore & getPipelineSemaphore();
    std::vector<vk::ClearValue> clearValues;

    vk::Extent2D extent;

    vk::raii::Fence & getPipelineFence();

    void renderPipeline(Renderable::RenderArguments renderArguments) override;

    virtual ~GraphicsPipeline();

    void destroy();

    void renderVertexBuffer(RenderArguments renderArguments, VertexBuffer *vbo) const;

private:
    vk::raii::Pipeline pipeline = nullptr;
public:
    const vk::raii::Pipeline &getPipeline() const;

    void setPipeline(vk::raii::Pipeline && pipeline);

    void prepareRender(RenderArguments &renderArguments);

    void finishRender(const RenderArguments &renderArguments) const;
private:
    vk::raii::Semaphore pipelineSemaphore = nullptr;
    void createSyncObjects();
    vk::raii::Fence pipelineFence = nullptr;

};


#endif //VULKAN_ENGINE_GRAPHICSPIPELINE_H
