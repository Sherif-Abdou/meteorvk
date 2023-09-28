//
// Created by Sherif Abdou on 9/12/23.
//

#include "GraphicsPipelineBuilder.h"
#include "../storage/Vertex.h"


void GraphicsPipelineBuilder::initializeDefaults() {
    createPipelineLayout();

    stages = shaders.getShaderStage();

    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)context.swapChainExtent.width;
    viewport.height = (float)context.swapChainExtent.height;


    dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    dynamicStateCreateInfo.setDynamicStates(dynamic_states);

    pipelineVertexInputStateCreateInfo.setVertexBindingDescriptions(bindings);
    pipelineVertexInputStateCreateInfo.setVertexAttributeDescriptions(descriptions);

    pipelineInputAssemblyStateCreateInfo.setPrimitiveRestartEnable(false);
    pipelineInputAssemblyStateCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);

    pipelineViewportStateCreateInfo.setViewportCount(1);
    pipelineViewportStateCreateInfo.setScissorCount(1);

    pipelineRasterizationStateCreateInfo.setCullMode(vk::CullModeFlagBits::eFront);
    pipelineRasterizationStateCreateInfo.setRasterizerDiscardEnable(false);
    pipelineRasterizationStateCreateInfo.setLineWidth(1.0f);
    pipelineRasterizationStateCreateInfo.setPolygonMode(vk::PolygonMode::eFill);
    pipelineRasterizationStateCreateInfo.setFrontFace(vk::FrontFace::eClockwise);
    pipelineRasterizationStateCreateInfo.setDepthBiasEnable(false);

    depthStencilStateCreateInfo.setStencilTestEnable(false);
    depthStencilStateCreateInfo.setDepthTestEnable(true);
    depthStencilStateCreateInfo.setDepthWriteEnable(true);
    depthStencilStateCreateInfo.setDepthCompareOp(vk::CompareOp::eLess);
    depthStencilStateCreateInfo.setMinDepthBounds(0.0f);
    depthStencilStateCreateInfo.setMinDepthBounds(1.0f);

    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState {};
    colorBlendAttachmentState.setBlendEnable(false);
    colorBlendAttachmentState.setColorBlendOp(vk::BlendOp::eAdd);
    colorBlendAttachmentState.setAlphaBlendOp(vk::BlendOp::eAdd);
    colorBlendAttachmentState.setSrcColorBlendFactor(vk::BlendFactor::eDstAlpha);
    colorBlendAttachmentState.setDstColorBlendFactor(vk::BlendFactor::eOneMinusDstAlpha);
    colorBlendAttachmentState.setSrcAlphaBlendFactor(vk::BlendFactor::eDstAlpha);
    colorBlendAttachmentState.setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusDstAlpha);
    colorBlendAttachmentState.setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR);

    colorBlendAttachments = {colorBlendAttachmentState};
    colorBlendStateCreateInfo.setAttachments(colorBlendAttachments);
    colorBlendStateCreateInfo.setLogicOpEnable(false);

    attachPipelineParameters(stages);
}

void GraphicsPipelineBuilder::attachPipelineParameters(const std::vector<vk::PipelineShaderStageCreateInfo> &stages) {
    createPipelineLayout();

    this->stages = stages;

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
    pipelineCreateInfo.setStages(this->stages);
}

void GraphicsPipelineBuilder::createPipelineLayout() {
    vk::PipelineLayoutCreateInfo layoutCreateInfo {};
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts {};
    descriptorSetLayouts.reserve(descriptorSets.size());
    for (auto descriptorSet: descriptorSets) {
        descriptorSetLayouts.push_back(*descriptorSet->getDescriptorSetLayout());
    }
    layoutCreateInfo.setSetLayouts(descriptorSetLayouts);

    this->pipelineLayout = context.device.createPipelineLayout(layoutCreateInfo);
}

GraphicsPipelineBuilder::GraphicsPipelineBuilder(VulkanContext &context, GraphicsShaders &shaders,
                                                 GraphicsRenderPass &renderPass) : context(context), shaders(shaders),
                                                                                   renderPass(renderPass) {
    initializeDefaults();
}

void GraphicsPipelineBuilder::setViewport(const vk::Viewport &viewport) {
    GraphicsPipelineBuilder::viewport = viewport;
}

void
GraphicsPipelineBuilder::setDynamicStateCreateInfo(const vk::PipelineDynamicStateCreateInfo &dynamicStateCreateInfo) {
    GraphicsPipelineBuilder::dynamicStateCreateInfo = dynamicStateCreateInfo;
}

void GraphicsPipelineBuilder::setPipelineVertexInputStateCreateInfo(
        const vk::PipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo) {
    GraphicsPipelineBuilder::pipelineVertexInputStateCreateInfo = pipelineVertexInputStateCreateInfo;
}

void GraphicsPipelineBuilder::setPipelineInputAssemblyStateCreateInfo(
        const vk::PipelineInputAssemblyStateCreateInfo &pipelineInputAssemblyStateCreateInfo) {
    GraphicsPipelineBuilder::pipelineInputAssemblyStateCreateInfo = pipelineInputAssemblyStateCreateInfo;
}

void GraphicsPipelineBuilder::setPipelineViewportStateCreateInfo(
        const vk::PipelineViewportStateCreateInfo &pipelineViewportStateCreateInfo) {
    GraphicsPipelineBuilder::pipelineViewportStateCreateInfo = pipelineViewportStateCreateInfo;
}

void GraphicsPipelineBuilder::setPipelineRasterizationStateCreateInfo(
        const vk::PipelineRasterizationStateCreateInfo &pipelineRasterizationStateCreateInfo) {
    GraphicsPipelineBuilder::pipelineRasterizationStateCreateInfo = pipelineRasterizationStateCreateInfo;
}

void GraphicsPipelineBuilder::setDepthStencilStateCreateInfo(
        const vk::PipelineDepthStencilStateCreateInfo &depthStencilStateCreateInfo) {
    GraphicsPipelineBuilder::depthStencilStateCreateInfo = depthStencilStateCreateInfo;
}

void GraphicsPipelineBuilder::setColorBlendStateCreateInfo(
        const vk::PipelineColorBlendStateCreateInfo &colorBlendStateCreateInfo) {
    GraphicsPipelineBuilder::colorBlendStateCreateInfo = colorBlendStateCreateInfo;
}

void GraphicsPipelineBuilder::setMultisampleStateCreateInfo(
        const vk::PipelineMultisampleStateCreateInfo &multisampleStateCreateInfo) {
    GraphicsPipelineBuilder::multisampleStateCreateInfo = multisampleStateCreateInfo;
}

void GraphicsPipelineBuilder::addDepthImage() {
    VulkanAllocator::VulkanImageAllocation depthImage;
    vk::raii::ImageView depthImageView = nullptr;
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
    imageCreateInfo.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled);

    VkImageCreateInfo rawImageCreateInfo = imageCreateInfo;
    context.allocator->allocateImage(&rawImageCreateInfo, VMA_MEMORY_USAGE_AUTO, &depthImage);

    vk::ImageViewCreateInfo imageViewCreateInfo {};
    imageViewCreateInfo.setFormat(vk::Format::eD32Sfloat);
    imageViewCreateInfo.setImage(depthImage.image);
    imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
    imageViewCreateInfo.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1));

    depthImageView = context.device.createImageView(imageViewCreateInfo);

    auto depthImagePair = ImagePair {depthImage, std::move(depthImageView)};

    this->depthImageAttachment = std::move(depthImagePair);
}

void GraphicsPipelineBuilder::addColorImage(vk::Format format) {
    VulkanAllocator::VulkanImageAllocation colorImage;
    vk::raii::ImageView colorImageView = nullptr;

    vk::ImageCreateInfo imageCreateInfo {};
    imageCreateInfo.setInitialLayout(vk::ImageLayout::eUndefined);
    imageCreateInfo.setFormat(format);
    imageCreateInfo.setArrayLayers(1);
    imageCreateInfo.setExtent({context.swapChainExtent.width, context.swapChainExtent.height, 1});
    imageCreateInfo.setImageType(vk::ImageType::e2D);
    imageCreateInfo.setMipLevels(1);
    imageCreateInfo.setSamples(vk::SampleCountFlagBits::e1);
    imageCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
    imageCreateInfo.setTiling(vk::ImageTiling::eOptimal);
    imageCreateInfo.setUsage(vk::ImageUsageFlagBits::eColorAttachment);

    VkImageCreateInfo rawImageCreateInfo = imageCreateInfo;
    context.allocator->allocateImage(&rawImageCreateInfo, VMA_MEMORY_USAGE_AUTO, &colorImage);

    vk::ImageViewCreateInfo imageViewCreateInfo {};
    imageViewCreateInfo.setFormat(format);
    imageViewCreateInfo.setImage(colorImage.image);
    imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
    imageViewCreateInfo.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

    colorImageView = context.device.createImageView(imageViewCreateInfo);

    auto colorImagePair = ImagePair {colorImage, std::move(colorImageView)};

    colorImageAttachments.push_back(std::move(colorImagePair));
}

GraphicsPipeline GraphicsPipelineBuilder::buildGraphicsPipeline() {
    attachPipelineParameters(shaders.getShaderStage());

    std::vector<vk::raii::Framebuffer> framebuffers {};
    if (!targetImageViews.empty()) { // Create Framebuffer for each target image view
        for (auto& targetImageView: targetImageViews) {
            vk::FramebufferCreateInfo createInfo {};
            auto attachments = std::vector<vk::ImageView> {targetImageView};
            for (auto& colorImageView : colorImageAttachments) {
                attachments.push_back(*colorImageView.imageView);
            }
            if (depthImageAttachment.has_value()) {
                attachments.push_back(*depthImageAttachment->imageView);
            }

            createInfo.setRenderPass(*renderPass.getRenderPass());
            createInfo.setWidth(context.swapChainExtent.width);
            createInfo.setHeight(context.swapChainExtent.height);
            createInfo.setAttachments(attachments);
            createInfo.setLayers(1);

            framebuffers.push_back(context.device.createFramebuffer(createInfo));
        }
    } else { // Create one framebuffer for the off screen render
        vk::FramebufferCreateInfo createInfo {};
        auto attachments = std::vector<vk::ImageView> {};
        for (auto& colorImageView : colorImageAttachments) {
            attachments.push_back(*colorImageView.imageView);
        }
        if (depthImageAttachment.has_value()) {
            attachments.push_back(*depthImageAttachment->imageView);
        }

        createInfo.setRenderPass(*renderPass.getRenderPass());
        createInfo.setWidth(context.swapChainExtent.width);
        createInfo.setHeight(context.swapChainExtent.height);
        createInfo.setAttachments(attachments);
        createInfo.setLayers(1);

        framebuffers.push_back(context.device.createFramebuffer(createInfo));
    }
    auto pipeline = GraphicsPipeline(context, std::move(renderPass));
    pipeline.targetFramebuffers = std::move(framebuffers);
    pipeline.pipelineLayout = std::move(pipelineLayout);
    auto actual_pipeline = context.device.createGraphicsPipeline(nullptr, pipelineCreateInfo);
    pipeline.setPipeline(std::move(actual_pipeline));

    pipeline.init();

    if (!targetImageViews.empty()) {
        pipeline.clearValues.push_back(vk::ClearValue(vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f)));
    }

    for (auto& colorImageAttachment : colorImageAttachments) {
        pipeline.clearValues.push_back(vk::ClearValue(vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f)));
        pipeline.ownedImages.push_back(std::move(colorImageAttachment));
    }

    if (depthImageAttachment.has_value()) {
        pipeline.clearValues.push_back(vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0)));
        pipeline.ownedImages.push_back(std::move(*depthImageAttachment));
    }


    return pipeline;
}

const std::vector<DescriptorSet *> &GraphicsPipelineBuilder::getDescriptorSets() const {
    return descriptorSets;
}

void GraphicsPipelineBuilder::setDescriptorSets(const std::vector<DescriptorSet *> &descriptorSets) {
    GraphicsPipelineBuilder::descriptorSets = descriptorSets;
}
