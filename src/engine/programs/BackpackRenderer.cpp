//
// Created by Sherif Abdou on 12/10/23.
//

#include "BackpackRenderer.h"
#include "../special_pipelines/ShadowGraphicsPipeline.h"
#include "../../core/graphics_pipeline/GraphicsCommandBuffer.h"
#include "../../core/shared_pipeline/PipelineBarrierBuilder.h"
#include "../special_pipelines/CullingComputePipeline.h"

void BackpackRenderer::run(VulkanContext *context) {
    auto vertexbuffer1 = createVertexBuffer(context, model_path_1);
    vertexbuffer1.init();
    auto vertexbuffer2 = createVertexBuffer(context, "./models/floor.obj");
    vertexbuffer2.init();

    // Create Descriptors for pipeline stages
    VulkanAllocator::VulkanImageAllocation red_image;
    auto red_sampler = load_texture_from_file(context, red_image);
    red_sampler.sampler.targetImageView = *red_sampler.view;

    CombinedDescriptorSampler sampler = createSampler(context);
    auto shadow_descriptor = createUniformBindings(context);
    auto forward_descriptor = createUniformBindings(context);
    auto renderPass = std::make_unique<GraphicsRenderPass>(context);
    renderPass->multisampling = true;
    renderPass->init();

    // Build graphics pipelines
    auto shaders = std::make_unique<GraphicsShaders>(context, "shaders/basic.vert", "shaders/basic.frag");
    GraphicsPipelineBuilder builder = GraphicsPipelineBuilder(context, std::move(shaders), std::move(renderPass));
    for (auto& targetSwapchainImageView: context->swapChainImageViews) {
        builder.targetImageViews.push_back(*targetSwapchainImageView);
    }
    builder.descriptorSets = {&forward_descriptor};
    builder.enableMultisampling();
    builder.addDepthImage();
    auto modelPipeline = std::make_unique<ModelBufferGraphicsPipeline>(builder.buildGraphicsPipeline(), 16);
    modelPipeline->descriptorSet = &forward_descriptor;
    auto* modelBuffer = modelPipeline->modelBuffer;
    CullingComputePipeline cullingComputePipeline(context);
    cullingComputePipeline.models = modelBuffer;
    cullingComputePipeline.vertex_buffers = {&vertexbuffer1, &vertexbuffer2};
    cullingComputePipeline.init();
//    modelPipeline->indirectBuffer = cullingComputePipeline.output_buffer->getBuffer();
    auto pipeline = ForwardRenderedGraphicsPipeline(std::move(modelPipeline));
    auto shadowModelPipeline = std::make_unique<ModelBufferGraphicsPipeline>(createShadowPipeline(context, &shadow_descriptor),
                                                           modelBuffer);
    shadowModelPipeline->descriptorSet = &shadow_descriptor;
    auto shadow_pipeline = ShadowGraphicsPipeline(std::move(shadowModelPipeline));
    pipeline.descriptorSet = &forward_descriptor;
    shadow_pipeline.descriptorSet = &shadow_descriptor;

    auto depth_descriptors = createUniformBindings(context);
    auto depth_pipeline = createDepthOnlyPipeline(context, modelBuffer, &depth_descriptors);

    auto ssao_descriptors = createUniformBindings(context);
    auto ssao_pipeline = createSSAOPipeline(context, &ssao_descriptors, modelBuffer);

    auto depth_sampler = CombinedDescriptorSampler(context);
    depth_sampler.targetImageView = depth_pipeline.getDepthImageView();
    depth_sampler.targetImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    depth_sampler.buildSampler();

    ssao_pipeline->setDepthSampler(&depth_sampler);
//    ssao_pipeline->depth_sampler = &depth_sampler;


    GraphicsCommandBuffer commandBuffer(context);
    commandBuffer.pipelines.push_back(&shadow_pipeline);
    commandBuffer.pipelines.push_back(&depth_pipeline);
    commandBuffer.pipelines.push_back(ssao_pipeline.get());
    commandBuffer.pipelines.push_back(&pipeline);

    sampler.targetImageView = *shadow_pipeline.getPipeline().ownedImages[0].imageView;
    sampler.updateSampler(forward_descriptor, 1);



    commandBuffer.init();
    commandBuffer.vertexBuffers.push_back(&vertexbuffer1);
    commandBuffer.vertexBuffers.push_back(&vertexbuffer2);
    modelBuffer->updateBuffer({
                                                    glm::identity<glm::mat4>(),
                                                    Material(glm::vec4(0.8f, 0.1f, 0.26f, 1.0f))
                                            }, 0);
    modelBuffer->updateBuffer({
                                                    glm::translate(
                                                            glm::scale(glm::identity<glm::mat4>(), glm::vec3(2.3, 0.3, 2.3)),
                                                            glm::vec3(0, -8, 0)),
                                                    Material(glm::vec4(127.0f, 255.0f, 212.0f, 256.0f) / glm::vec4(256.f))
                                            }, 1);

    vertexbuffer1.updateVertexBuffer();
    vertexbuffer2.updateVertexBuffer();

    // Create dependency barrier between graphics pipelines
    auto shadowDepthImage = shadow_pipeline.getPipeline().ownedImages[0].imageAllocation.image;
    auto depthImage = depth_pipeline.getDepthImage();
    auto occlusionImage = ssao_pipeline->getOcclusionImage();
    auto shadow_image_barrier = PipelineBarrierBuilder();
    shadow_image_barrier
            .waitFor(vk::PipelineStageFlagBits2::eLateFragmentTests | vk::PipelineStageFlagBits2::eEarlyFragmentTests)
            .whichUses(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
            .beforeDoing(vk::PipelineStageFlagBits2::eFragmentShader)
            .whichUses(vk::AccessFlagBits2::eShaderRead)
            .forImage(shadowDepthImage, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1))
            .withInitialLayout(vk::ImageLayout::eDepthAttachmentOptimal)
            .withFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    auto depth_image_barrier = PipelineBarrierBuilder();
    depth_image_barrier
            .waitFor(vk::PipelineStageFlagBits2::eLateFragmentTests | vk::PipelineStageFlagBits2::eEarlyFragmentTests)
            .whichUses(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
            .beforeDoing(vk::PipelineStageFlagBits2::eFragmentShader)
            .whichUses(vk::AccessFlagBits2::eShaderRead)
            .forImage(depthImage, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1))
            .withInitialLayout(vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal)
            .withFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    auto occlusion_image_barrier = PipelineBarrierBuilder();
    occlusion_image_barrier
            .waitFor(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
            .whichUses(vk::AccessFlagBits2::eColorAttachmentWrite)
            .beforeDoing(vk::PipelineStageFlagBits2::eFragmentShader)
            .whichUses(vk::AccessFlagBits2::eShaderRead)
            .forImage(occlusionImage, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
            .withInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .withFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    auto shadow_image_barrier_back = PipelineBarrierBuilder();
    shadow_image_barrier_back
            .waitFor(vk::PipelineStageFlagBits2::eFragmentShader)
            .whichUses(vk::AccessFlagBits2::eShaderRead)
            .beforeDoing(vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests)
            .whichUses(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
            .forImage(shadowDepthImage, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1))
            .withInitialLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .withFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    auto cull_indirect_barrier = PipelineBarrierBuilder();
    cull_indirect_barrier
        .waitFor(vk::PipelineStageFlagBits2::eComputeShader)
        .whichUses(vk::AccessFlagBits2::eShaderStorageWrite | vk::AccessFlagBits2::eShaderStorageRead | vk::AccessFlagBits2::eUniformRead)
        .beforeDoing(vk::PipelineStageFlagBits2::eDrawIndirect)
        .whichUses(vk::AccessFlagBits2::eIndirectCommandRead)
        .forBuffer(*cullingComputePipeline.output_buffer->getBuffer(), 2 * sizeof(IndirectCallStruct), 0);


    commandBuffer.dependencies[0] = {cull_indirect_barrier.build()};
    commandBuffer.dependencies[3] = {occlusion_image_barrier.build()};
//    commandBuffer.dependencies[1] = shadow_image_barrier.build();
    commandBuffer.dependencies[2] = {depth_image_barrier.build()};

    commandBuffer.bindings.push_back({
                                             &shadow_descriptor,
                                             &shadow_pipeline.getPipeline().getPipelineLayout(),
                                     });
    commandBuffer.bindings.push_back({
                                             &depth_descriptors,
                                             &depth_pipeline.getPipeline().getPipelineLayout(),
                                     });
    commandBuffer.bindings.push_back({
                                             &ssao_descriptors,
                                             &ssao_pipeline->getPipeline().getPipelineLayout(),
                                     });
    commandBuffer.bindings.push_back({
                                             &forward_descriptor,
                                             &pipeline.getPipeline().getPipelineLayout(),
                                     });
    //    commandBuffer.layout = layout;
    //    commandBuffer.descriptorSet = &shadow_descriptor;
    //    commandBuffer.bindDescriptorSet(shadow_descriptor, commandBuffer.pipelines[0].getPipelineLayout(), 0);

    glfwShowWindow(context->window);
    auto last_time = glfwGetTime();
    float speed = 0.4;
    auto position = glm::zero<glm::vec3>();
    position.z = -5.f;
    auto rotation = glm::zero<glm::vec3>();
    float backpack_rotation = 0;
    auto t = 0.0f;

    CombinedDescriptorSampler occlusionSampler(context);
    occlusionSampler.targetImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    occlusionSampler.targetImageView = ssao_pipeline->getOcclusionImageView();
    occlusionSampler.buildSampler();
    cullingComputePipeline.generateIndirects();


    while (!glfwWindowShouldClose(context->window)) {
        glfwPollEvents();
        commandBuffer.beginSwapchainRender();
        auto delta = glfwGetTime() - last_time;
        t += delta;
//        backpack_rotation += delta * 1.0f;
        auto rot = glm::rotate(glm::scale(glm::identity<glm::mat4>(), glm::vec3(2)), backpack_rotation + 3.14f / 4.0f, glm::vec3(0, 1.0f, 0));
        auto model = glm::translate(rot, glm::vec3(0,-1.0f,0));
        modelBuffer->updateBuffer({
                                                        model,
                                                        Material {glm::vec4(1.0, 0.0, 0.0, 1.0)}
                                                }, 0);


        pipeline.ubo.lightProjView = shadow_pipeline.lightUBO.proj * shadow_pipeline.lightUBO.view;
        if (glfwGetKey(context->window, GLFW_KEY_W)) {
            position += glm::vec3(0, 0, speed * delta);
        }

        if (glfwGetKey(context->window, GLFW_KEY_S)) {
            position += glm::vec3(0, 0, -speed * delta);
        }

        if (glfwGetKey(context->window, GLFW_KEY_D)) {
            position += glm::vec3(-speed * delta, 0, 0);
        }

        if (glfwGetKey(context->window, GLFW_KEY_A)) {
            position += glm::vec3(speed * delta, 0, 0);
        }

        if (glfwGetKey(context->window, GLFW_KEY_Q)) {
            rotation += glm::vec3(0, -1.0f * (float)delta, 0);
        }

        if (glfwGetKey(context->window, GLFW_KEY_E)) {
            rotation += glm::vec3(0, 1.0f * (float)delta, 0);
        }


        pipeline.ubo.view = glm::translate(glm::rotate(glm::identity<glm::mat4>(), rotation.y, glm::vec3(0, 1, 0)),
                                           position);
        cullingComputePipeline.ubo.proj = pipeline.ubo.proj;
        cullingComputePipeline.ubo.view = pipeline.ubo.view;
        depth_pipeline.getUBO().ubo.view = pipeline.ubo.view;
        ssao_pipeline->ubo->view = pipeline.ubo.view;

        if (glfwGetKey(context->window, GLFW_KEY_0)) {
            cullingComputePipeline.generateIndirects();
        }

        sampler.updateSampler(forward_descriptor, 1);
//        sampler.updateSampler(ssao_descriptors, 1);
        sampler.updateSampler(depth_descriptors, 1);
        red_sampler.sampler.updateSampler(forward_descriptor, 3);
        occlusionSampler.updateSampler(forward_descriptor, 4);
        red_sampler.sampler.updateSampler(shadow_descriptor, 3);
        red_sampler.sampler.updateSampler(ssao_descriptors, 3);
        last_time = glfwGetTime();
        commandBuffer.finishSwapchainRender();
    }


    context->device.waitIdle();
    cullingComputePipeline.destroy();
    red_image.destroy();
    modelBuffer->destroy();
    pipeline.destroy();
    shadow_pipeline.destroy();
    depth_pipeline.destroy();
    ssao_pipeline->destroy();
    commandBuffer.destroy();

    // delete depth_model_pipeline;
    // delete depth_forward_pipeline;
    // delete ssao_model_pipeline;
}

VertexBuffer BackpackRenderer::createVertexBuffer(VulkanContext *context, const char *path) {
    VertexBuffer buffer(context, true);
    OBJFile file = OBJFile::fromFilePath(path);
    auto raw = file.createVulkanBufferIndexed();
    buffer.vertices = std::move(raw.vertices);
    buffer.indices = std::move(raw.indices);
    return buffer;
}

DepthOnlyPipeline BackpackRenderer::createDepthOnlyPipeline(VulkanContext *context, ModelBuffer *modelBuffer,
                                                            DescriptorSet *descriptorSet) {
    auto renderPass = std::make_unique<GraphicsRenderPass>(context);
    renderPass->useColor = false;
    renderPass->storeDepth = true;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = {};
    dependency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;


    auto shaders = std::make_unique<GraphicsShaders>(context, "shaders/basic.vert", "shaders/shadow.frag");
    renderPass->init();
    auto builder = GraphicsPipelineBuilder(context, std::move(shaders), std::move(renderPass));
    builder.descriptorSets = {descriptorSet};
    builder.setExtent(context->swapChainExtent);
    builder.addDepthImage();

    depth_model_pipeline = std::make_unique<ModelBufferGraphicsPipeline>(builder.buildGraphicsPipeline(), modelBuffer);
    depth_model_pipeline->descriptorSet = descriptorSet;
    depth_forward_pipeline = std::make_unique<ForwardRenderedGraphicsPipeline>(std::move(depth_model_pipeline));
    depth_forward_pipeline->descriptorSet = descriptorSet;
    return DepthOnlyPipeline(*depth_forward_pipeline);
}

GraphicsPipeline BackpackRenderer::createShadowPipeline(VulkanContext *context, DescriptorSet *descriptorSet) {
    auto renderPass = std::make_unique<GraphicsRenderPass>(context);
    renderPass->useColor = false;
    renderPass->storeDepth = true;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = {};
    dependency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;


    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
    pipelineRasterizationStateCreateInfo.setCullMode(vk::CullModeFlagBits::eBack);
    pipelineRasterizationStateCreateInfo.setRasterizerDiscardEnable(false);
    pipelineRasterizationStateCreateInfo.setLineWidth(1.0f);
    pipelineRasterizationStateCreateInfo.setPolygonMode(vk::PolygonMode::eFill);
    pipelineRasterizationStateCreateInfo.setFrontFace(vk::FrontFace::eClockwise);
    pipelineRasterizationStateCreateInfo.setDepthBiasEnable(false);

    renderPass->init();
    auto shaders = std::make_unique<GraphicsShaders>(context, "shaders/basic.vert", "shaders/shadow.frag");
    auto builder = GraphicsPipelineBuilder(context, std::move(shaders), std::move(renderPass));
    builder.setExtent({2048, 2048});
    builder.setPipelineRasterizationStateCreateInfo(pipelineRasterizationStateCreateInfo);
    builder.addDepthImage();


    if (descriptorSet != nullptr) {
        builder.descriptorSets.push_back(descriptorSet);
    }
    auto pipeline = builder.buildGraphicsPipeline();

    return std::move(pipeline);
}

DescriptorSet BackpackRenderer::createUniformBindings(VulkanContext *context) {
    vk::DescriptorSetLayoutBinding bufferBinding{};
    bufferBinding.setBinding(0);
    bufferBinding.setDescriptorCount(1);
    bufferBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
    bufferBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);


    vk::DescriptorSetLayoutBinding imageBinding{};
    imageBinding.setBinding(1);
    imageBinding.setDescriptorCount(1);
    imageBinding.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    imageBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
//    imageBinding.setImmutableSamplers(*descriptorSampler.getSampler());

    vk::DescriptorSetLayoutBinding dynamicBufferBinding{};
    dynamicBufferBinding.setBinding(2);
    dynamicBufferBinding.setDescriptorCount(1);
    dynamicBufferBinding.setDescriptorType(vk::DescriptorType::eUniformBufferDynamic);
    dynamicBufferBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    vk::DescriptorSetLayoutBinding imageBinding2{};
    imageBinding2.setBinding(3);
    imageBinding2.setDescriptorCount(1);
    imageBinding2.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    imageBinding2.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    vk::DescriptorSetLayoutBinding imageBinding3{};
    imageBinding3.setBinding(4);
    imageBinding3.setDescriptorCount(1);
    imageBinding3.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    imageBinding3.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    DescriptorSet descriptorSet(context, {bufferBinding, imageBinding, dynamicBufferBinding, imageBinding2, imageBinding3});
    descriptorSet.buildDescriptor();

    return descriptorSet;
}

CombinedDescriptorSampler BackpackRenderer::createSampler(VulkanContext *context) {
    auto descriptorSampler = CombinedDescriptorSampler(context);
    descriptorSampler.buildSampler();
    return descriptorSampler;
}

BackpackRenderer::TextureResult
BackpackRenderer::load_texture_from_file(VulkanContext *context, VulkanAllocator::VulkanImageAllocation &red_image) {
    auto loader = ImageTextureLoader(context);
    red_image = loader.loadImageFromFile("./textures/1001_albedo.jpg");
    vk::ImageViewCreateInfo image_view_create_info {};
    image_view_create_info.components = vk::ComponentMapping();
    image_view_create_info.format = vk::Format::eR8G8B8A8Srgb;
    image_view_create_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    image_view_create_info.image = red_image.image;
    image_view_create_info.viewType = vk::ImageViewType::e2D;
    vk::raii::ImageView view = context->device.createImageView(image_view_create_info);

    CombinedDescriptorSampler sampler(context);
    sampler.targetImageView = *view;
    sampler.targetImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    sampler.buildSampler();

    return { std::move(sampler), std::move(view) };
}

std::unique_ptr<SSAOGraphicsPipeline>
BackpackRenderer::createSSAOPipeline(VulkanContext *context, DescriptorSet *descriptorSet, ModelBuffer *buffer) {
    auto renderPass = std::make_unique<GraphicsRenderPass>(context);
    renderPass->useColor = true;
    renderPass->storeDepth = true;
    renderPass->useCustomColor(vk::Format::eR16Sfloat, vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = {};
    dependency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;


    renderPass->init();
    auto shaders = std::make_unique<GraphicsShaders>(context, "shaders/basic.vert", "shaders/ssao.frag");
    auto builder = GraphicsPipelineBuilder(context, std::move(shaders), std::move(renderPass));
    builder.setExtent(context->swapChainExtent);
    builder.addDepthImage();
    builder.addColorImage(vk::Format::eR16Sfloat);


    if (descriptorSet != nullptr) {
        builder.descriptorSets.push_back(descriptorSet);
    }
    auto pipeline = builder.buildGraphicsPipeline();
    ssao_model_pipeline = std::make_unique<ModelBufferGraphicsPipeline> (std::move(pipeline), buffer);

    ssao_model_pipeline->descriptorSet = descriptorSet;
    ssao_model_pipeline->modelBuffer = buffer;

    std::unique_ptr<SSAOGraphicsPipeline> res = std::make_unique<SSAOGraphicsPipeline>(context, std::move(ssao_model_pipeline), descriptorSet);
    res->descriptor_set = descriptorSet;
    res->init();

    return res;
}
