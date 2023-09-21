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

#include "src/VulkanContext.h"
#include "src/storage/VertexBuffer.h"
#include "src/graphics_pipeline/GraphicsRenderPass.h"
#include "src/graphics_pipeline/GraphicsPipeline.h"
#include "src/graphics_pipeline/GraphicsCommandBuffer.h"
#include "src/storage/UniformBuffer.h"
#include "src/graphics_pipeline/GraphicsPipelineBuilder.h"
#include "src/storage/DescriptorSampler.h"
#include "src/storage/OBJFile.h"
#include "src/graphics_pipeline/special_pipelines/ShadowGraphicsPipeline.h"
#include "src/graphics_pipeline/special_pipelines/ForwardRenderedGraphicsPipeline.h"

VertexBuffer createVertexBuffer(VulkanContext& context) {
    VertexBuffer buffer(*context.allocator);
    OBJFile file = OBJFile::fromFilePath("./models/super_backpack.obj");
    auto raw = file.createVulkanBuffer();
    buffer.vertices = std::move(raw);
//    buffer.vertices.push_back(Vertex::positionOnly({0, 0, 0}));
//    buffer.vertices.push_back(Vertex::positionOnly({1, 0, 0}));
//    buffer.vertices.push_back(Vertex::positionOnly({0, 1, 0}));

    return buffer;
}


struct ImageViewPair {
    VulkanAllocator::VulkanImageAllocation image;
    vk::ImageView imageView;
};

DescriptorSampler createSampler(VulkanContext &context);

using UBO = ForwardRenderedGraphicsPipeline::UBO;

GraphicsPipeline createShadowPipeline(VulkanContext& context, DescriptorSet* descriptorSet) {
    GraphicsRenderPass renderPass(context);
    renderPass.useColor = false;
    renderPass.storeDepth = true;

    vk::SubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = {};
    dependency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;


    renderPass.init();
    auto shaders = GraphicsShaders(context, "shaders/basic.vert", "shaders/shadow.frag");
    auto builder = GraphicsPipelineBuilder(context, shaders, renderPass);
    builder.addDepthImage();


    if (descriptorSet != nullptr) {
        builder.descriptorSets.push_back(descriptorSet);
    }
    auto pipeline = builder.buildGraphicsPipeline();

    return std::move(pipeline);
}

VulkanAllocator::VulkanBufferAllocation createUniformBuffer(VulkanContext& context) {
    vk::BufferCreateInfo bufferCreateInfo {};
    bufferCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
    bufferCreateInfo.setSize(sizeof (UBO));
    bufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);

    VkBufferCreateInfo rawCreateInfo = bufferCreateInfo;

    VulkanAllocator::VulkanBufferAllocation buffer;
    context.allocator->allocateBuffer(&rawCreateInfo, VMA_MEMORY_USAGE_AUTO, &buffer);
    return buffer;
}

DescriptorSet createUniformBindings(VulkanContext& context, DescriptorSampler& descriptorSampler) {
    vk::DescriptorSetLayoutBinding bufferBinding {};
    bufferBinding.setBinding(0);
    bufferBinding.setDescriptorCount(1);
    bufferBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
    bufferBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);



    vk::DescriptorSetLayoutBinding imageBinding {};
    imageBinding.setBinding(1);
    imageBinding.setDescriptorCount(1);
    imageBinding.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    imageBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
    imageBinding.setImmutableSamplers(*descriptorSampler.getSampler());

    DescriptorSet descriptorSet(context, {bufferBinding, imageBinding});
    descriptorSet.buildDescriptor();

    return std::move(descriptorSet);
}

DescriptorSampler createSampler(VulkanContext &context) {
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
    VulkanContext context {};
    context.initVulkan();
    DescriptorSampler sampler = createSampler(context);
    auto descriptor = createUniformBindings(context, sampler);
    auto forward_descriptor = createUniformBindings(context, sampler);
    GraphicsRenderPass renderPass(context);
    renderPass.init();

//    GraphicsPipeline pipeline(context, std::move(renderPass));
//    pipeline.descriptorSet = &descriptor;
//    for (auto& swapChainImage: context.swapChainImageViews) {
//        pipeline.targetImageViews.push_back(*swapChainImage);
//    }
//    pipeline.init();
    auto shaders = GraphicsShaders(context, "shaders/basic.vert", "shaders/basic.frag");
    GraphicsPipelineBuilder builder = GraphicsPipelineBuilder(context, shaders, renderPass);
    builder.targetImageViews = {};
    for (auto& targetSwapchainImageView: context.swapChainImageViews) {
        builder.targetImageViews.push_back(*targetSwapchainImageView);
    }
    builder.descriptorSets = {&forward_descriptor};
    builder.addDepthImage();
    auto pipeline = ForwardRenderedGraphicsPipeline(builder.buildGraphicsPipeline());
    auto shadow_pipeline = ShadowGraphicsPipeline(createShadowPipeline(context, &descriptor));
    pipeline.descriptorSet = &forward_descriptor;
    shadow_pipeline.descriptorSet = &descriptor;
    GraphicsCommandBuffer commandBuffer(context);

    commandBuffer.pipelines.push_back(&shadow_pipeline);
    commandBuffer.pipelines.push_back(&pipeline);

    sampler.targetImageView = *shadow_pipeline.getPipeline().ownedImages[0].imageView;
    sampler.updateSampler(forward_descriptor, 1);


    commandBuffer.init();
    auto vertexbuffer = createVertexBuffer(context);
    vertexbuffer.init();
    commandBuffer.vertexBuffers.push_back(&vertexbuffer);

    auto initial_ubo = initialBuffer();

    vk::ImageMemoryBarrier2 imageMemoryBarrier {};
    auto depthImage = shadow_pipeline.getPipeline().ownedImages[0].imageAllocation.image;
    imageMemoryBarrier.setImage(depthImage);
    imageMemoryBarrier.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1));
    imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits2::eDepthStencilAttachmentWrite);
    imageMemoryBarrier.setDstAccessMask(vk::AccessFlagBits2::eShaderRead);
    imageMemoryBarrier.setSrcStageMask(vk::PipelineStageFlagBits2::eLateFragmentTests);
    imageMemoryBarrier.setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader);

    vk::DependencyInfoKHR dependencyInfo {};
    dependencyInfo.setImageMemoryBarriers(imageMemoryBarrier);

    commandBuffer.dependencies.push_back(dependencyInfo);

    commandBuffer.bindings.push_back({
        &descriptor,
        &shadow_pipeline.getPipeline().getPipelineLayout(),
    });
    commandBuffer.bindings.push_back({
         &forward_descriptor,
         &pipeline.getPipeline().getPipelineLayout(),
    });
//    commandBuffer.layout = layout;
//    commandBuffer.descriptorSet = &descriptor;
//    commandBuffer.bindDescriptorSet(descriptor, commandBuffer.pipelines[0].getPipelineLayout(), 0);

    glfwShowWindow(context.window);
    auto last_time = glfwGetTime();
    float speed = 0.4;
    while (!glfwWindowShouldClose(context.window)) {
        glfwPollEvents();
        commandBuffer.beginSwapchainRender();
        auto delta = glfwGetTime() - last_time;

        pipeline.ubo.lightProjView = shadow_pipeline.lightUBO.proj * shadow_pipeline.lightUBO.view;
        if (glfwGetKey(context.window, GLFW_KEY_W)) {
            pipeline.ubo.view = glm::translate(pipeline.ubo.view, glm::vec3(0, 0, speed * delta));
        }

        if (glfwGetKey(context.window, GLFW_KEY_S)) {
            pipeline.ubo.view = glm::translate(pipeline.ubo.view, glm::vec3(0, 0, -speed * delta));
        }

        if (glfwGetKey(context.window, GLFW_KEY_D)) {
            pipeline.ubo.view = glm::translate(pipeline.ubo.view, glm::vec3(-speed * delta, 0, 0));
        }

        if (glfwGetKey(context.window, GLFW_KEY_A)) {
            pipeline.ubo.view = glm::translate(pipeline.ubo.view, glm::vec3(speed * delta, 0, 0));
        }

        if (glfwGetKey(context.window, GLFW_KEY_Q)) {
            pipeline.ubo.view = glm::rotate(pipeline.ubo.view, 1.0f * (float)delta, glm::vec3(0,1,0));
        }

        if (glfwGetKey(context.window, GLFW_KEY_E)) {
            pipeline.ubo.view = glm::rotate(pipeline.ubo.view, -1.0f * (float)delta, glm::vec3(0,1,0));
        }

        sampler.updateSampler(forward_descriptor, 1);
        last_time = glfwGetTime();
        commandBuffer.finishSwapchainRender();
    }


    context.device.waitIdle();
    shadow_pipeline.getPipeline().destroy();
    pipeline.getPipeline().destroy();
    commandBuffer.destroy();
    context.cleanup();
    return 0;
}
