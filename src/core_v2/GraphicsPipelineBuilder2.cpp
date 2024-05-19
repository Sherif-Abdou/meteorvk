//
// Created by Sherif Abdou on 5/5/24.
//

#include "GraphicsPipelineBuilder2.h"

GraphicsPipelineBuilder2::GraphicsPipelineBuilder2(VulkanContext *context, NewDescriptorManager* manager): context(context), descriptorManager(manager) {
    if (descriptorManager == nullptr) {
      descriptorManager = new NewDescriptorManager(context);
    }
    initializePipelineStatesDefaults();
}

void GraphicsPipelineBuilder2::initializePipelineStatesDefaults() {
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;


    dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    dynamicStateCreateInfo.setDynamicStates(dynamic_states);

    pipelineVertexInputStateCreateInfo.setVertexBindingDescriptions(bindings);
    pipelineVertexInputStateCreateInfo.setVertexAttributeDescriptions(descriptions);

    pipelineInputAssemblyStateCreateInfo.setPrimitiveRestartEnable(false);
    pipelineInputAssemblyStateCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);

    pipelineViewportStateCreateInfo.setViewportCount(1);
    pipelineViewportStateCreateInfo.setScissorCount(1);

    pipelineRasterizationStateCreateInfo.setCullMode(vk::CullModeFlagBits::eNone);
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

}

void GraphicsPipelineBuilder2::enableMultisampling() {
    multisampleStateCreateInfo.setRasterizationSamples(context->sampleCountFlagBits);
    multisampleStateCreateInfo.setSampleShadingEnable(true);
    multisampleStateCreateInfo.setMinSampleShading(0.2f);
}

void GraphicsPipelineBuilder2::buildDescriptors() {
    descriptorSets = descriptorManager->buildDescriptors();
}

void GraphicsPipelineBuilder2::buildRenderpass() {
    vk::SubpassDescription subpassDescription {};
    uint32_t attachment_index = 0;

    if (options.imageSource == ImageSource::Swapchain) {
        colorDescription.setFormat(context->swapChainImageFormat)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        auto* colorRef = new vk::AttachmentReference {};
        colorRef->setAttachment(attachment_index);
        colorRef->setLayout(vk::ImageLayout::eColorAttachmentOptimal);
        if (options.multisampling) {
            colorDescription
                .setSamples(context->sampleCountFlagBits)
                .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);
        }
        subpassDescription.setColorAttachments(*colorRef);
        attachment_index++;
    } else if (options.imageSource == ImageSource::Custom) {
        colorDescription.setFormat(options.format)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        auto* colorRef = new vk::AttachmentReference {};
        colorRef->setAttachment(attachment_index);
        colorRef->setLayout(vk::ImageLayout::eColorAttachmentOptimal);
        subpassDescription.setColorAttachments(*colorRef);
        attachment_index++;
    }

    if (options.useDepth) {
        vk::AttachmentStoreOp storeOp = options.shouldStoreDepth ? vk::AttachmentStoreOp::eStore : vk::AttachmentStoreOp::eDontCare;
        depthDescription
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
            .setFormat(vk::Format::eD32Sfloat)
            .setSamples(colorDescription.samples)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(storeOp)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        auto* depthRef = new vk::AttachmentReference {};
        depthRef->setAttachment(attachment_index);
        depthRef->setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        subpassDescription.setPDepthStencilAttachment(depthRef);
        attachment_index++;
    }

    if (options.multisampling) {
        colorDescriptionResolve.setFormat(context->swapChainImageFormat)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        auto* resolveRef = new vk::AttachmentReference {};
        resolveRef->setAttachment(attachment_index);
        resolveRef->setLayout(vk::ImageLayout::eColorAttachmentOptimal);
        subpassDescription.setResolveAttachments(*resolveRef);
    }

    subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    vk::SubpassDependency subpassDependency {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    std::vector<vk::AttachmentDescription> attachments;
    if (options.imageSource != ImageSource::None && options.imageSource != ImageSource::Depth) {
        attachments.push_back(colorDescription);
    }
    if (options.useDepth) {
        attachments.push_back(depthDescription);
    }
    if (options.multisampling) {
        attachments.push_back(colorDescriptionResolve);
    }

    vk::RenderPassCreateInfo renderPassCreateInfo {};
    renderPassCreateInfo.setAttachments(attachments);
    renderPassCreateInfo.setSubpasses(subpassDescription);
    renderPassCreateInfo.setDependencies(subpassDependency);

    renderPass = std::make_unique<vk::raii::RenderPass>(context->device.createRenderPass(renderPassCreateInfo));
}

void GraphicsPipelineBuilder2::addDepthImage() {
    VulkanAllocator::VulkanImageAllocation depthImage;
    vk::raii::ImageView depthImageView = nullptr;
    vk::ImageCreateInfo imageCreateInfo {};

    imageCreateInfo.setInitialLayout(vk::ImageLayout::eUndefined);
    imageCreateInfo.setFormat(vk::Format::eD32Sfloat);
    imageCreateInfo.setArrayLayers(1);
    imageCreateInfo.setExtent({extent.width, extent.height, 1});
    imageCreateInfo.setImageType(vk::ImageType::e2D);
    imageCreateInfo.setMipLevels(1);
    imageCreateInfo.setSamples(vk::SampleCountFlagBits::e1);
    imageCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
    imageCreateInfo.setTiling(vk::ImageTiling::eOptimal);
    imageCreateInfo.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled);

    if (options.multisampling) {
        imageCreateInfo.setSamples(context->sampleCountFlagBits);
    }

    VkImageCreateInfo rawImageCreateInfo = imageCreateInfo;
    context->allocator->allocateImage(&rawImageCreateInfo, VMA_MEMORY_USAGE_AUTO, &depthImage);

    vk::ImageViewCreateInfo imageViewCreateInfo {};
    imageViewCreateInfo.setFormat(vk::Format::eD32Sfloat);
    imageViewCreateInfo.setImage(depthImage.image);
    imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
    imageViewCreateInfo.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1));

    depthImageView = context->device.createImageView(imageViewCreateInfo);

    ImagePair depthImagePair { depthImage, std::move(depthImageView) };

    ownedImages.push_back(std::move(depthImagePair));

    attachments.depthTarget = *ownedImages[ownedImages.size()-1].imageView;
}

void GraphicsPipelineBuilder2::addColorImage(vk::Format format, vk::SampleCountFlagBits samples, bool is_target) {
    VulkanAllocator::VulkanImageAllocation colorImage;
    vk::raii::ImageView colorImageView = nullptr;

    vk::ImageCreateInfo imageCreateInfo {};
    imageCreateInfo.setInitialLayout(vk::ImageLayout::eUndefined);
    imageCreateInfo.setFormat(format);
    imageCreateInfo.setArrayLayers(1);
    imageCreateInfo.setExtent({extent.width, extent.height, 1});
    imageCreateInfo.setImageType(vk::ImageType::e2D);
    imageCreateInfo.setMipLevels(1);
    imageCreateInfo.setSamples(samples);
    imageCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
    imageCreateInfo.setTiling(vk::ImageTiling::eOptimal);
    imageCreateInfo.setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);

    VkImageCreateInfo rawImageCreateInfo = imageCreateInfo;
    context->allocator->allocateImage(&rawImageCreateInfo, VMA_MEMORY_USAGE_AUTO, &colorImage);

    vk::ImageViewCreateInfo imageViewCreateInfo {};
    imageViewCreateInfo.setFormat(format);
    imageViewCreateInfo.setImage(colorImage.image);
    imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
    imageViewCreateInfo.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

    colorImageView = context->device.createImageView(imageViewCreateInfo);

    auto colorImagePair = ImagePair {colorImage, std::move(colorImageView)};

    ownedImages.push_back(std::move(colorImagePair));

    if (is_target) {
        targets.push_back(*ownedImages[ownedImages.size() - 1].imageView);
    } else {
        attachments.colorTargets.push_back(*ownedImages[ownedImages.size() - 1].imageView);
    }
}

static std::string readFile(const std::string& path) {
    std::fstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

std::vector<uint32_t> GraphicsPipelineBuilder2::compileShader(const std::string &path, shaderc_shader_kind kind) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions compile_options;

    compile_options.AddMacroDefinition("CUSTOM_BINDINGS");
    for (auto layout_name: descriptorManager->getLayoutNames()) {
        uint32_t setNum = descriptorManager->getSetOf(layout_name);
        uint32_t bindingNum = descriptorManager->getBindingOf(layout_name);
        std::transform(layout_name.begin(), layout_name.end(), layout_name.begin(), ::toupper);
        compile_options.AddMacroDefinition(layout_name + "_BINDING", std::to_string(bindingNum));
        compile_options.AddMacroDefinition(layout_name + "_SET", std::to_string(setNum));
    }

    compile_options.SetOptimizationLevel(shaderc_optimization_level_zero);

    std::string source = readFile(path);

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, path.c_str(), compile_options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw std::runtime_error(result.GetErrorMessage());
    }

    return {result.cbegin(), result.cend()};
}

void GraphicsPipelineBuilder2::buildShaderStages() {
    auto vertexShaderModuleCreateInfo = vk::ShaderModuleCreateInfo();
    auto vertexShader = compileShader(options.vertexShaderPath, shaderc_glsl_vertex_shader);
    vertexShaderModuleCreateInfo.setCode(vertexShader);
    vertexShaderModule = context->device.createShaderModule(vertexShaderModuleCreateInfo);

    auto fragmentShaderModuleCreateInfo = vk::ShaderModuleCreateInfo();
    auto fragmentShader = compileShader(options.fragmentShaderPath, shaderc_glsl_fragment_shader);
    fragmentShaderModuleCreateInfo.setCode(fragmentShader);
    fragmentShaderModule = context->device.createShaderModule(fragmentShaderModuleCreateInfo);

    auto vertexShaderStage = vk::PipelineShaderStageCreateInfo();
    vertexShaderStage.setModule(*vertexShaderModule);
    vertexShaderStage.setPName("main");
    vertexShaderStage.setStage(vk::ShaderStageFlagBits::eVertex);

    auto fragmentShaderStage = vk::PipelineShaderStageCreateInfo();
    fragmentShaderStage.setModule(*fragmentShaderModule);
    fragmentShaderStage.setPName("main");
    fragmentShaderStage.setStage(vk::ShaderStageFlagBits::eFragment);

    this->shader_stages = {vertexShaderStage, fragmentShaderStage};
}

GraphicsPipeline GraphicsPipelineBuilder2::build() {
    if (options.multisampling) {
        enableMultisampling();
    }
    if (options.imageSource == ImageSource::Swapchain) {
        if (options.multisampling) {
            // A multisampled image is an attachment that gets resolved to in the renderpass
            addColorImage(context->swapChainImageFormat, context->sampleCountFlagBits, false);
        }
        for (auto& swapchain_image : context->swapChainImageViews) {
            targets.push_back(*swapchain_image);
        }
    } else if (options.imageSource == ImageSource::Custom) {
        addColorImage(options.format, options.multisampling ? context->sampleCountFlagBits : vk::SampleCountFlagBits::e1);
    } 
    if (options.useDepth || options.imageSource == ImageSource::Depth) {
        addDepthImage();
    } 
    if (options.imageSource == ImageSource::Depth) {
        targets.push_back(*this->attachments.depthTarget);
    }


    buildRenderpass();
    createPipelineLayout();
    buildShaderStages();


    std::vector<vk::raii::Framebuffer> framebuffers {};
    std::vector<vk::ClearValue> clearValues {};
    bool clearValuesInitialized = false;
    for (auto& target : targets) {
        vk::FramebufferCreateInfo createInfo {};
        auto attachments = std::vector<vk::ImageView>();

        // Target is first when there isn't multisampling
        if (!options.multisampling && options.imageSource != ImageSource::Depth) {
            attachments.push_back(target);
            if (!clearValuesInitialized)
                clearValues.emplace_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
        }

        for (auto& attachment: this->attachments.colorTargets) {
            attachments.push_back(attachment);

            if (!clearValuesInitialized)
                clearValues.emplace_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
        }
        if (this->attachments.depthTarget.has_value() || options.imageSource == ImageSource::Depth) {
            attachments.push_back(*this->attachments.depthTarget);

            if (!clearValuesInitialized)
                clearValues.emplace_back(vk::ClearDepthStencilValue(1.0f, 0));
        }

        // Target is last when multisampling is enabled
        if (options.multisampling) {
            attachments.push_back(target);
            if (!clearValuesInitialized)
                clearValues.emplace_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
        }

        createInfo.setRenderPass(**renderPass);
        createInfo.setWidth(extent.width);
        createInfo.setHeight(extent.height);
        createInfo.setAttachments(attachments);
        createInfo.setLayers(1);

        framebuffers.push_back(context->device.createFramebuffer(createInfo));
        clearValuesInitialized = true;
    }

    attach();
    auto pipeline = GraphicsPipeline(context, std::move(renderPass));
    pipeline.extent = this->extent;
    pipeline.targetFramebuffers = std::move(framebuffers);
    pipeline.pipelineLayout = std::move(pipelineLayout);
    pipeline.clearValues = clearValues;
    pipeline.ownedImages = std::move(ownedImages);
    auto actual_pipeline = context->device.createGraphicsPipeline(nullptr, pipelineCreateInfo);
    pipeline.setPipeline(std::move(actual_pipeline));

    pipeline.init();

    return pipeline;
}

void GraphicsPipelineBuilder2::createPipelineLayout() {
    buildDescriptors();

    vk::PipelineLayoutCreateInfo layoutCreateInfo {};
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts {};
    descriptorSetLayouts.reserve(descriptorSets.size());

    for (auto descriptorSet: descriptorSets) {
        descriptorSetLayouts.push_back(descriptorSet->getDescriptorSetLayout());
    }

    layoutCreateInfo.setSetLayouts(descriptorSetLayouts);

    this->pipelineLayout = context->device.createPipelineLayout(layoutCreateInfo);
}

void GraphicsPipelineBuilder2::attach() {
    pipelineCreateInfo.setStages(shader_stages);
    pipelineCreateInfo.setRenderPass(**renderPass);
    pipelineCreateInfo.setSubpass(0);
    pipelineCreateInfo.setLayout(*pipelineLayout);
    pipelineCreateInfo.setPDynamicState(&dynamicStateCreateInfo);
    pipelineCreateInfo.setPDepthStencilState(&depthStencilStateCreateInfo);
    pipelineCreateInfo.setPInputAssemblyState(&pipelineInputAssemblyStateCreateInfo);
    pipelineCreateInfo.setPViewportState(&pipelineViewportStateCreateInfo);
    pipelineCreateInfo.setPRasterizationState(&pipelineRasterizationStateCreateInfo);
    pipelineCreateInfo.setPColorBlendState(&colorBlendStateCreateInfo);
    pipelineCreateInfo.setPVertexInputState(&pipelineVertexInputStateCreateInfo);
    pipelineCreateInfo.setPMultisampleState(&multisampleStateCreateInfo);
    pipelineCreateInfo.setStages(this->shader_stages);
}

NewDescriptorManager* GraphicsPipelineBuilder2::getDescriptorManager() {
  return this->descriptorManager;
}
