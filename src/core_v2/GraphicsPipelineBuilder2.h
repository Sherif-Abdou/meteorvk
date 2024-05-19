//
// Created by Sherif Abdou on 5/5/24.
//

#ifndef VULKAN_ENGINE_GRAPHICSPIPELINEBUILDER2_H
#define VULKAN_ENGINE_GRAPHICSPIPELINEBUILDER2_H

#include <memory>
#include <fstream>
#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.hpp>
#include "../core/VulkanContext.h"
#include "../core/storage/Vertex.h"
#include "../core/graphics_pipeline/GraphicsPipeline.h"
#include "NewDescriptorManager.h"

class GraphicsPipelineBuilder2 {
    using ImagePair = GraphicsPipeline::ImagePair;

    VulkanContext* context;
    vk::raii::PipelineLayout pipelineLayout = nullptr;

    std::vector<DescriptorSet*> descriptorSets;

    // Pipeline creation structs
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
    std::vector<vk::DynamicState> dynamic_states {};
    std::vector<vk::PipelineShaderStageCreateInfo> stages {};
    std::vector<vk::VertexInputBindingDescription> bindings = {Vertex::bindingDescription()};
    std::array<vk::VertexInputAttributeDescription, Vertex::VERTEX_NUM> descriptions = Vertex::attributeDescriptions();
    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments = {};

    // Renderpass creation structs
    struct Subpass {
        vk::SubpassDescription description;
        vk::SubpassDependency dependency;
    };
    vk::AttachmentDescription colorDescription {};
    vk::AttachmentDescription depthDescription {};
    vk::AttachmentDescription colorDescriptionResolve {};
    Subpass subpass;

    // Shader creation structs
    vk::raii::ShaderModule vertexShaderModule = nullptr;
    vk::raii::ShaderModule fragmentShaderModule = nullptr;
    std::vector<vk::PipelineShaderStageCreateInfo> shader_stages {};

    std::unique_ptr<vk::raii::RenderPass> renderPass = nullptr;
public:
    enum class ImageSource {
        Swapchain,
        Custom,
        Depth,
        None,
    };
    struct GraphicsPipelineBuilderOptions {
        bool multisampling;
        ImageSource imageSource;
        bool useDepth;
        bool shouldStoreDepth;

        vk::Format format = vk::Format::eUndefined;
        std::string vertexShaderPath;
        std::string fragmentShaderPath;

    };
    struct ImageTargets {
        std::vector<vk::ImageView> colorTargets;
        std::optional<vk::ImageView> depthTarget;
        std::vector<vk::ImageView> colorResolveTargets;
    };

    GraphicsPipelineBuilderOptions options;
    // Resultants to be rendered too
    std::vector<vk::ImageView> targets;
    // All attached to each framebuffer
    ImageTargets attachments;

    NewDescriptorManager* descriptorManager;

    NewDescriptorManager* getDescriptorManager();

    std::vector<ImagePair> ownedImages;

    explicit GraphicsPipelineBuilder2(VulkanContext* context, NewDescriptorManager* descriptorManager = nullptr);

    GraphicsPipeline build();
private:
    void initializePipelineStatesDefaults();
    void enableMultisampling();
    void buildDescriptors();
    void createPipelineLayout();

    void buildRenderpass();

    void addDepthImage();
    void addColorImage(vk::Format format, vk::SampleCountFlagBits, bool is_target=true);

    std::vector<uint32_t> compileShader(const std::string & path, shaderc_shader_kind kind);
    void buildShaderStages();

    void attach();
};


#endif //VULKAN_ENGINE_GRAPHICSPIPELINEBUILDER2_H
