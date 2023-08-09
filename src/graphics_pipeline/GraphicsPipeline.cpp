//
// Created by Sherif Abdou on 8/4/23.
//

#include "GraphicsPipeline.h"
#include "../storage/Vertex.h"
#include "GraphicsShaders.h"

void GraphicsPipeline::createFramebuffers() {
    for (auto& imageView: targetImageViews) {
        vk::FramebufferCreateInfo createInfo {};
        auto attachments = {imageView, *depthImageView};

        createInfo.setRenderPass(*renderPass.getRenderPass());
        createInfo.setWidth(context.swapChainExtent.width);
        createInfo.setHeight(context.swapChainExtent.height);
        createInfo.setAttachments(attachments);
        createInfo.setLayers(1);

        targetFramebuffers.push_back(context.device.createFramebuffer(createInfo));
    }
}

void GraphicsPipeline::createDepthImage() {
    vk::ImageCreateInfo imageCreateInfo {};
    imageCreateInfo.setInitialLayout(vk::ImageLayout::eUndefined);
    imageCreateInfo.setFormat(vk::Format::eD32Sfloat);
    imageCreateInfo.setArrayLayers(1);
    imageCreateInfo.setExtent({context.swapChainExtent.width, context.swapChainExtent.height, 1});
    imageCreateInfo.setImageType(vk::ImageType::e2D);
    imageCreateInfo.setMipLevels(1);
    imageCreateInfo.setSamples(vk::SampleCountFlagBits::e1);
    imageCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
    imageCreateInfo.setTiling(vk::ImageTiling::eOptimal);
    imageCreateInfo.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);

    VkImageCreateInfo rawImageCreateInfo = imageCreateInfo;
    context.allocator->allocateImage(&rawImageCreateInfo, VMA_MEMORY_USAGE_AUTO, &depthImage);

    vk::ImageViewCreateInfo imageViewCreateInfo {};
    imageViewCreateInfo.setFormat(vk::Format::eD32Sfloat);
    imageViewCreateInfo.setImage(depthImage.image);
    imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
    imageViewCreateInfo.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1));

    depthImageView = context.device.createImageView(imageViewCreateInfo);
}

void GraphicsPipeline::init() {
    createSyncObjects();
    createDepthImage();
    createFramebuffers();
    createPipeline();
}

void GraphicsPipeline::createSyncObjects() {
    vk::SemaphoreCreateInfo semaphoreCreateInfo {};
    vk::FenceCreateInfo fenceCreateInfo {};
    pipelineSemaphore = context.device.createSemaphore(semaphoreCreateInfo);
    pipelineFence = context.device.createFence(fenceCreateInfo);
}

void GraphicsPipeline::createPipeline() {
    createPipelineLayout();

    auto shaders = GraphicsShaders(context, "shaders/basic.vert", "shaders/basic.frag");
    auto stages = shaders.getShaderStage();

    vk::Viewport viewport {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = context.swapChainExtent.width;
    viewport.height = context.swapChainExtent.height;


    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo {};
    auto dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    dynamicStateCreateInfo.setDynamicStates(dynamic_states);

    vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo {};
    auto bindings = {Vertex::bindingDescription()};
    auto descriptions = Vertex::attributeDescriptions();
    pipelineVertexInputStateCreateInfo.setVertexBindingDescriptions(bindings);
    pipelineVertexInputStateCreateInfo.setVertexAttributeDescriptions(descriptions);

    vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo {};
    pipelineInputAssemblyStateCreateInfo.setPrimitiveRestartEnable(false);
    pipelineInputAssemblyStateCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);

    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo {};
    pipelineViewportStateCreateInfo.setViewportCount(1);
    pipelineViewportStateCreateInfo.setScissorCount(1);

    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo {};
    pipelineRasterizationStateCreateInfo.setCullMode(vk::CullModeFlagBits::eNone);
    pipelineRasterizationStateCreateInfo.setRasterizerDiscardEnable(false);
    pipelineRasterizationStateCreateInfo.setLineWidth(1.0f);
    pipelineRasterizationStateCreateInfo.setPolygonMode(vk::PolygonMode::eFill);
    pipelineRasterizationStateCreateInfo.setFrontFace(vk::FrontFace::eClockwise);
    pipelineRasterizationStateCreateInfo.setDepthBiasEnable(false);

    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo {};
    depthStencilStateCreateInfo.setStencilTestEnable(false);
    depthStencilStateCreateInfo.setDepthTestEnable(true);
    depthStencilStateCreateInfo.setDepthWriteEnable(true);
    depthStencilStateCreateInfo.setDepthCompareOp(vk::CompareOp::eLess);
    depthStencilStateCreateInfo.setMinDepthBounds(0.0f);
    depthStencilStateCreateInfo.setMinDepthBounds(1.0f);

    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState {};
    colorBlendAttachmentState.setBlendEnable(false);
    colorBlendAttachmentState.setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR);

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo {};
    auto attachments = {colorBlendAttachmentState};
    colorBlendStateCreateInfo.setAttachments(attachments);
    colorBlendStateCreateInfo.setLogicOpEnable(false);

    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo {};
    multisampleStateCreateInfo.setRasterizationSamples(vk::SampleCountFlagBits::e1);
    multisampleStateCreateInfo.setSampleShadingEnable(false);

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo {};
    pipelineCreateInfo.setRenderPass(*renderPass.getRenderPass());
    pipelineCreateInfo.setSubpass(subPassIndex);
    pipelineCreateInfo.setLayout(*pipelineLayout);
    pipelineCreateInfo.setPDynamicState(&dynamicStateCreateInfo);
    pipelineCreateInfo.setPDepthStencilState(&depthStencilStateCreateInfo);
    pipelineCreateInfo.setPInputAssemblyState(&pipelineInputAssemblyStateCreateInfo);
    pipelineCreateInfo.setPViewportState(&pipelineViewportStateCreateInfo);
    pipelineCreateInfo.setPRasterizationState(&pipelineRasterizationStateCreateInfo);
    pipelineCreateInfo.setPColorBlendState(&colorBlendStateCreateInfo);
    pipelineCreateInfo.setPVertexInputState(&pipelineVertexInputStateCreateInfo);
    pipelineCreateInfo.setPMultisampleState(&multisampleStateCreateInfo);
    pipelineCreateInfo.setStages(stages);

    pipeline = context.device.createGraphicsPipeline(nullptr, pipelineCreateInfo);
}

void GraphicsPipeline::createPipelineLayout() {
    vk::PipelineLayoutCreateInfo layoutCreateInfo {};
    if (descriptorSet.has_value()) {
        auto layouts = {*descriptorSet.value().getDescriptorSetLayout()};
        layoutCreateInfo.setSetLayouts(layouts);
    }

    this->pipelineLayout = context.device.createPipelineLayout(layoutCreateInfo);
}

GraphicsPipeline::GraphicsPipeline(VulkanContext &context, GraphicsRenderPass&& renderPass)
        : context(context), renderPass(std::move(renderPass)) {}

vk::raii::Semaphore & GraphicsPipeline::getPipelineSemaphore() {
    return pipelineSemaphore;
}

vk::raii::Fence & GraphicsPipeline::getPipelineFence() {
    return pipelineFence;
}

void GraphicsPipeline::renderPipeline(GraphicsPipeline::RenderArguments renderArguments) {
    auto imageIndex = renderArguments.imageIndex;
    vk::RenderPassBeginInfo beginInfo {};
    imageIndex = std::min((long)imageIndex, (long)targetFramebuffers.size()-1);

    vk::ClearValue clearValues[2] = {};
    clearValues[0].setColor(vk::ClearColorValue(1.0f, 0.0f, 0.0f, 1.0f));
    clearValues[1].setDepthStencil(vk::ClearDepthStencilValue(1.0, 0.0));

    beginInfo.setFramebuffer(*targetFramebuffers[imageIndex]);
    beginInfo.setRenderPass(*renderPass.getRenderPass());
    beginInfo.setClearValues(clearValues);
    auto rect = vk::Rect2D {};
    rect.setOffset({0, 0});
    rect.setExtent(context.swapChainExtent);
    beginInfo.setRenderArea(rect);

    renderArguments.commandBuffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(context.swapChainExtent.height);;
    viewport.width = static_cast<float>(context.swapChainExtent.width);
    viewport.height = -static_cast<float>(context.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    renderArguments.commandBuffer.setViewport(0, viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D {0, 0};
    scissor.extent = context.swapChainExtent;
    renderArguments.commandBuffer.setScissor(0, scissor);
    for (auto& vbo: renderArguments.vertexBuffers) {
        vbo.draw(renderArguments.commandBuffer);
    }
    renderArguments.commandBuffer.endRenderPass();
}

GraphicsPipeline::~GraphicsPipeline() {
    depthImage.destroy();
}
