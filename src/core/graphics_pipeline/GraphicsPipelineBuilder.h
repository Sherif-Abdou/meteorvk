//
// Created by Sherif Abdou on 9/12/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSPIPELINEBUILDER_H
#define VULKAN_ENGINE_GRAPHICSPIPELINEBUILDER_H

#include "core/VulkanContext.h"
#include "GraphicsShaders.h"
#include "GraphicsRenderPass.h"
#include "GraphicsPipeline.h"

/// Handles construction of GraphicsPipeline, including building images
class GraphicsPipelineBuilder {
private:
    using ImagePair = GraphicsPipeline::ImagePair;
public:
    bool enable_multisampling = false;
    GraphicsPipelineBuilder(VulkanContext *context, std::unique_ptr<GraphicsShaders> shaders, std::unique_ptr<GraphicsRenderPass> renderPass);

    void setViewport(const vk::Viewport &viewport);

    void setDynamicStateCreateInfo(const vk::PipelineDynamicStateCreateInfo &dynamicStateCreateInfo);

    void setPipelineVertexInputStateCreateInfo(
            const vk::PipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo);

    void setPipelineInputAssemblyStateCreateInfo(
            const vk::PipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo);

    void setPipelineViewportStateCreateInfo(const vk::PipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo);

    void setPipelineRasterizationStateCreateInfo(
            const vk::PipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo);

    void setDepthStencilStateCreateInfo(const vk::PipelineDepthStencilStateCreateInfo &depthStencilStateCreateInfo);

    void setColorBlendStateCreateInfo(const vk::PipelineColorBlendStateCreateInfo &colorBlendStateCreateInfo);

    void setMultisampleStateCreateInfo(const vk::PipelineMultisampleStateCreateInfo &multisampleStateCreateInfo);


    uint32_t subPassIndex = 0;
    std::vector<vk::ImageView> targetImageViews {}; // External image view to output to, usually swapchain
    std::vector<DescriptorSet*> descriptorSets {};

    const std::vector<DescriptorSet *> &getDescriptorSets() const;

    void setDescriptorSets(const std::vector<DescriptorSet *> &descriptorSets);
    // Descriptor sets to be used by pipeline

    std::vector<GraphicsPipeline::ImagePair> colorImageAttachments {}; // Color Images to attach to pipeline
    std::optional<GraphicsPipeline::ImagePair> depthImageAttachment {}; // Depth Image to attach to pipeline

    void addDepthImage();
    void addColorImage(vk::Format format, vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1);

    void enableMultisampling();

    GraphicsPipeline buildGraphicsPipeline();
private:
    VulkanContext* context;
    std::unique_ptr<GraphicsShaders> shaders;
    std::unique_ptr<GraphicsRenderPass> renderPass;
    vk::raii::PipelineLayout pipelineLayout = nullptr;

    vk::Viewport viewport {};
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo {};
    vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo {};
    vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo {};
    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo {};
    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo {};
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo {};
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo {};
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo {};
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo {};
    vk::Extent2D extent = context->swapChainExtent;
public:
    const vk::Extent2D &getExtent() const;

    void setExtent(const vk::Extent2D &extent);

private:

    std::vector<vk::DynamicState> dynamic_states {};
    std::vector<vk::PipelineShaderStageCreateInfo> stages {};
    std::vector<vk::VertexInputBindingDescription> bindings = {Vertex::bindingDescription()};
    std::array<vk::VertexInputAttributeDescription, Vertex::VERTEX_NUM> descriptions = Vertex::attributeDescriptions();
    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments = {};

    void initializeDefaults();
    void createPipelineLayout();

    void attachPipelineParameters(const std::vector<vk::PipelineShaderStageCreateInfo> &stages);
};


#endif //VULKAN_ENGINE_GRAPHICSPIPELINEBUILDER_H
