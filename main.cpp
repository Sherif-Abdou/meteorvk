#include <iostream>

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define VMA_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "src/core/VulkanContext.h"
#include "src/core/compute_pipeline/ComputeCommandBuffer.h"
#include "src/core/compute_pipeline/ComputePipelineBuilder.h"
#include "src/core/VulkanContext.h"
#include "src/core/storage/DescriptorSampler.h"
#include "src/core/storage/VertexBuffer.h"
#include "src/core/graphics_pipeline/GraphicsRenderPass.h"
#include "src/core/graphics_pipeline/GraphicsPipeline.h"
#include "src/core/graphics_pipeline/GraphicsCommandBuffer.h"
#include "src/core/graphics_pipeline/GraphicsPipelineBuilder.h"
#include "src/core/storage/DescriptorSampler.h"
#include "src/core/storage/OBJFile.h"
#include "src/engine/special_pipelines/ShadowGraphicsPipeline.h"
#include "src/engine/special_pipelines/ForwardRenderedGraphicsPipeline.h"
#include "src/core/shared_pipeline/PipelineBarrierBuilder.h"
#include "src/core/storage/StorageBuffer.h"

VertexBuffer createVertexBuffer(VulkanContext& context, const char* path) {
    VertexBuffer buffer(*context.allocator);
    OBJFile file = OBJFile::fromFilePath(path);
    auto raw = file.createVulkanBuffer();
    buffer.vertices = std::move(raw);
    return buffer;
}


struct ImageViewPair {
    VulkanAllocator::VulkanImageAllocation image;
    vk::ImageView imageView;
};

DescriptorSampler createSampler(VulkanContext&context);

void oldMain(VulkanContext&context);

using UBO = ForwardRenderedGraphicsPipeline::UBO;

GraphicsPipeline createShadowPipeline(VulkanContext&context, DescriptorSet* descriptorSet) {
    GraphicsRenderPass renderPass(context);
    renderPass.useColor = false;
    renderPass.storeDepth = true;

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

    renderPass.init();
    auto shaders = GraphicsShaders(context, "shaders/basic.vert", "shaders/shadow.frag");
    auto builder = GraphicsPipelineBuilder(context, shaders, renderPass);
    builder.setExtent({1024, 1024});
    builder.setPipelineRasterizationStateCreateInfo(pipelineRasterizationStateCreateInfo);
    builder.addDepthImage();


    if (descriptorSet != nullptr) {
        builder.descriptorSets.push_back(descriptorSet);
    }
    auto pipeline = builder.buildGraphicsPipeline();

    return std::move(pipeline);
}

DescriptorSet createUniformBindings(VulkanContext&context, DescriptorSampler&descriptorSampler) {
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
    imageBinding.setImmutableSamplers(*descriptorSampler.getSampler());

    vk::DescriptorSetLayoutBinding dynamicBufferBinding{};
    dynamicBufferBinding.setBinding(2);
    dynamicBufferBinding.setDescriptorCount(1);
    dynamicBufferBinding.setDescriptorType(vk::DescriptorType::eUniformBufferDynamic);
    dynamicBufferBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    DescriptorSet descriptorSet(context, {bufferBinding, imageBinding, dynamicBufferBinding});
    descriptorSet.buildDescriptor();

    return descriptorSet;
}

DescriptorSampler createSampler(VulkanContext&context) {
    auto descriptorSampler = DescriptorSampler(context);
    descriptorSampler.buildSampler();
    return descriptorSampler;
}

UBO initialBuffer() {
    glm::mat4 proj = glm::perspective(glm::radians(90.0), 1920.0 / 1080.0, 0.1, 100.0);
    glm::mat4 view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0));
    view = glm::translate(view, glm::vec3(0, 0, -4));
    auto model = glm::identity<glm::mat4>();

    return {
        proj,
        view,
        model,
    };
}

int main() {
    VulkanContext context{};
    context.initVulkan();
    const auto use_old = true;
    if (use_old) {
        oldMain(context);
    }
    else {
        auto sb1_binding = vk::DescriptorSetLayoutBinding()
                .setBinding(0)
                .setDescriptorCount(1)
                .setDescriptorType(vk::DescriptorType::eStorageBuffer)
                .setStageFlags(vk::ShaderStageFlagBits::eCompute);

        auto sb2_binding = vk::DescriptorSetLayoutBinding()
                .setBinding(1)
                .setDescriptorCount(1)
                .setDescriptorType(vk::DescriptorType::eStorageBuffer)
                .setStageFlags(vk::ShaderStageFlagBits::eCompute);

        struct A {
            glm::vec4 a[256];
        };

        auto compute_pipeline_builder = ComputePipelineBuilder(context);
        auto shaders = ComputeShaders(context, "shaders/basic.comp");

        auto storage1 = StorageBuffer<A>(context);
        auto storage2 = StorageBuffer<A>(context);
        storage1.allocateBuffer();
        storage2.allocateBuffer();

        auto descriptor = DescriptorSet(context, {sb1_binding, sb2_binding});
        descriptor.buildDescriptor();

        storage1.writeToDescriptor(descriptor, 0);
        storage2.writeToDescriptor(descriptor, 1);

        compute_pipeline_builder.setShader(shaders);
        compute_pipeline_builder.setDescriptor(descriptor.getDescriptorSetLayout());
        compute_pipeline_builder.workgroups = 16;
        auto pipeline = compute_pipeline_builder.build();

        auto compute_command = ComputeCommandBuffer(context);
        compute_command.init();
        compute_command.begin();
        compute_command.bindAndDispatch(pipeline, &descriptor);
        compute_command.end();
        compute_command.submit();

        A* ptr1 = (A *)storage1.mapMemory();
        A* ptr2 = (A *)storage2.mapMemory();

        storage1.unMapMemory();
        storage2.unMapMemory();
        context.device.waitIdle();
        storage1.destroy();
        storage2.destroy();
    }
    context.cleanup();
    return 0;
}

void oldMain(VulkanContext&context) {
    // Create Descriptors for pipeline stages
    DescriptorSampler sampler = createSampler(context);
    auto shadow_descriptor = createUniformBindings(context, sampler);
    auto forward_descriptor = createUniformBindings(context, sampler);
    GraphicsRenderPass renderPass(context);
    renderPass.init();

    // Build graphics pipelines
    auto shaders = GraphicsShaders(context, "shaders/basic.vert", "shaders/basic.frag");
    GraphicsPipelineBuilder builder = GraphicsPipelineBuilder(context, shaders, renderPass);
    for (auto&targetSwapchainImageView: context.swapChainImageViews) {
        builder.targetImageViews.push_back(*targetSwapchainImageView);
    }
    builder.descriptorSets = {&forward_descriptor};
    builder.addDepthImage();
    auto modelPipeline = ModelBufferGraphicsPipeline(builder.buildGraphicsPipeline(), 16);
    modelPipeline.descriptorSet = &forward_descriptor;
    auto pipeline = ForwardRenderedGraphicsPipeline(modelPipeline);
    auto shadowModelPipeline = ModelBufferGraphicsPipeline(createShadowPipeline(context, &shadow_descriptor),
                                                           modelPipeline.modelBuffer);
    shadowModelPipeline.descriptorSet = &shadow_descriptor;
    auto shadow_pipeline = ShadowGraphicsPipeline(shadowModelPipeline);
    pipeline.descriptorSet = &forward_descriptor;
    shadow_pipeline.descriptorSet = &shadow_descriptor;

    GraphicsCommandBuffer commandBuffer(context);
    commandBuffer.pipelines.push_back(&shadow_pipeline);
    commandBuffer.pipelines.push_back(&pipeline);

    sampler.targetImageView = *shadow_pipeline.getPipeline().ownedImages[0].imageView;
    sampler.updateSampler(forward_descriptor, 1);


    commandBuffer.init();
    auto vertexbuffer1 = createVertexBuffer(context, "./models/super_backpack.obj");
    vertexbuffer1.init();
    commandBuffer.vertexBuffers.push_back(&vertexbuffer1);
    auto vertexbuffer2 = createVertexBuffer(context, "./models/floor.obj");
    vertexbuffer2.init();
    commandBuffer.vertexBuffers.push_back(&vertexbuffer2);
    modelPipeline.modelBuffer->updateBuffer({
                                                glm::identity<glm::mat4>(),
                                                glm::vec4(0.8f, 0.1f, 0.26f, 0.0f)
                                            }, 0);
    modelPipeline.modelBuffer->updateBuffer({
                                                glm::translate(
                                                    glm::scale(glm::identity<glm::mat4>(), glm::vec3(2.3, 0.3, 2.3)),
                                                    glm::vec3(0, -8, 0)),
                                                glm::vec4(127.0f, 255.0f, 212.0f, 0.0f) / glm::vec4(256.f),
                                            }, 1);

    // Create dependency barrier between graphics pipelines
    auto depthImage = shadow_pipeline.getPipeline().ownedImages[0].imageAllocation.image;
    auto barrier_builder = PipelineBarrierBuilder();
    barrier_builder
            .waitFor(vk::PipelineStageFlagBits2::eLateFragmentTests | vk::PipelineStageFlagBits2::eEarlyFragmentTests)
            .whichUses(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
            .beforeDoing(vk::PipelineStageFlagBits2::eFragmentShader)
            .whichUses(vk::AccessFlagBits2::eShaderRead)
            .forImage(depthImage, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1))
            .withInitialLayout(vk::ImageLayout::eDepthAttachmentOptimal)
            .withFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    commandBuffer.dependencies.push_back(barrier_builder.build());

    commandBuffer.bindings.push_back({
        &shadow_descriptor,
        &shadow_pipeline.getPipeline().getPipelineLayout(),
    });
    commandBuffer.bindings.push_back({
        &forward_descriptor,
        &pipeline.getPipeline().getPipelineLayout(),
    });
    //    commandBuffer.layout = layout;
    //    commandBuffer.descriptorSet = &shadow_descriptor;
    //    commandBuffer.bindDescriptorSet(shadow_descriptor, commandBuffer.pipelines[0].getPipelineLayout(), 0);

    glfwShowWindow(context.window);
    auto last_time = glfwGetTime();
    float speed = 0.4;
    auto position = glm::zero<glm::vec3>();
    position.z = -5.f;
    auto rotation = glm::zero<glm::vec3>();
    auto backpack_rotation = 0.0f;
    auto t = 0.0f;
    while (!glfwWindowShouldClose(context.window)) {
        glfwPollEvents();
        commandBuffer.beginSwapchainRender();
        auto delta = glfwGetTime() - last_time;
        t += delta;
        backpack_rotation += delta * 1.0f;
        auto mix_factor = fabs(sin(t));
        modelPipeline.modelBuffer->updateBuffer({
                                                    glm::rotate(glm::identity<glm::mat4>(), backpack_rotation,
                                                                glm::vec3(0, 1, 0)),
                                                    glm::smoothstep(glm::vec4(1.0, 0.0, 0.0, 0),
                                                                    glm::vec4(0.0, 1.0, 1.0, 0), glm::vec4(mix_factor))
                                                }, 0);


        pipeline.ubo.lightProjView = shadow_pipeline.lightUBO.proj * shadow_pipeline.lightUBO.view;
        if (glfwGetKey(context.window, GLFW_KEY_W)) {
            position += glm::vec3(0, 0, speed * delta);
        }

        if (glfwGetKey(context.window, GLFW_KEY_S)) {
            position += glm::vec3(0, 0, -speed * delta);
        }

        if (glfwGetKey(context.window, GLFW_KEY_D)) {
            position += glm::vec3(-speed * delta, 0, 0);
        }

        if (glfwGetKey(context.window, GLFW_KEY_A)) {
            position += glm::vec3(speed * delta, 0, 0);
        }

        if (glfwGetKey(context.window, GLFW_KEY_Q)) {
            rotation += glm::vec3(0, -1.0f * (float)delta, 0);
        }

        if (glfwGetKey(context.window, GLFW_KEY_E)) {
            rotation += glm::vec3(0, 1.0f * (float)delta, 0);
        }

        pipeline.ubo.view = glm::translate(glm::rotate(glm::identity<glm::mat4>(), rotation.y, glm::vec3(0, 1, 0)),
                                           position);

        sampler.updateSampler(forward_descriptor, 1);
        last_time = glfwGetTime();
        commandBuffer.finishSwapchainRender();
    }


    context.device.waitIdle();
    modelPipeline.modelBuffer->destroy();
    shadow_pipeline.getPipeline().destroy();
    pipeline.getPipeline().destroy();
    commandBuffer.destroy();
    context.cleanup();
}
