//
// Created by Sherif Abdou on 8/4/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSPIPELINE_H
#define VULKAN_ENGINE_GRAPHICSPIPELINE_H

#include "../VulkanContext.h"
#include "GraphicsRenderPass.h"
#include "../DescriptorSet.h"

class GraphicsPipeline {
public: // Inputs
    VulkanContext& context;

    std::vector<vk::raii::ImageView> targetImageViews;
    GraphicsRenderPass renderPass;
    unsigned int subPassIndex = 0;
    std::optional<DescriptorSet> descriptorSet;

    explicit GraphicsPipeline(VulkanContext &context, GraphicsRenderPass&& renderPass);

    void init();
private:
    void createPipeline();
    void createSyncObjects();
    void createFramebuffers();
    void createDepthImage();
    std::vector<vk::raii::Framebuffer> targetFramebuffers;
    vk::raii::PipelineLayout pipelineLayout = nullptr;
    vk::raii::Pipeline pipeline = nullptr;
    vk::raii::Semaphore waitSemaphore = nullptr;
    vk::raii::Semaphore signalSemaphore = nullptr;
    vk::raii::Fence pipelineFence = nullptr;
    VulkanAllocator::VulkanImageAllocation depthImage;
    vk::raii::ImageView depthImageView = nullptr;

    void createPipelineLayout();
};


#endif //VULKAN_ENGINE_GRAPHICSPIPELINE_H
