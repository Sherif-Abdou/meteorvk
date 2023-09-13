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

VertexBuffer createVertexBuffer(VulkanContext& context) {
    VertexBuffer buffer(*context.allocator);
    buffer.vertices.push_back(Vertex::positionOnly({0, 0, 0}));
    buffer.vertices.push_back(Vertex::positionOnly({1, 0, 0}));
    buffer.vertices.push_back(Vertex::positionOnly({0, 1, 0}));

    return buffer;
}


struct UBO {
    glm::mat4 proj;
    glm::mat4 view;
    glm::mat4 model;
};

struct ImageViewPair {
    VulkanAllocator::VulkanImageAllocation image;
    vk::ImageView imageView;
};

GraphicsPipeline createShadowPipeline(VulkanContext& context, DescriptorSet* descriptorSet) {
    GraphicsRenderPass renderPass(context);
    renderPass.useColor = false;
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

DescriptorSet createUniformBindings(VulkanContext& context) {
    vk::DescriptorSetLayoutBinding bufferBinding {};
    bufferBinding.setBinding(0);
    bufferBinding.setDescriptorCount(1);
    bufferBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
    bufferBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
    DescriptorSet descriptorSet(context, {bufferBinding});
    descriptorSet.buildDescriptor();
    return std::move(descriptorSet);
}

UBO initialBuffer() {
    glm::mat4 proj = glm::perspectiveFov(glm::radians(90.0), 1920.0, 1080.0, 0.1, 100.0);
    glm::mat4 view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0));
    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(0, 0, -2));

    return {
        proj,
        view,
        model,
    };
}

int main() {
    VulkanContext context {};
    context.initVulkan();
    auto descriptor = createUniformBindings(context);
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
    builder.descriptorSets = {&descriptor};
    builder.addDepthImage();
    auto pipeline = builder.buildGraphicsPipeline();
    auto shadow_pipeline = createShadowPipeline(context, &descriptor);
    GraphicsCommandBuffer commandBuffer(context);

    commandBuffer.pipelines.push_back(std::move(shadow_pipeline));
    commandBuffer.pipelines.push_back(std::move(pipeline));


    commandBuffer.init();
    auto vertexbuffer = createVertexBuffer(context);
    vertexbuffer.init();
    commandBuffer.vertexBuffers.push_back(std::move(vertexbuffer));

    auto ubo_buffer = UniformBuffer<UBO>(context, descriptor.getDescriptorSet());
    ubo_buffer.allocateBuffer();
    auto initial_ubo = initialBuffer();
    ubo_buffer.updateBuffer(initial_ubo);
    ubo_buffer.writeToDescriptor();


    auto* layout = &(commandBuffer.pipelines[0].getPipelineLayout());
    commandBuffer.bindings.push_back({
         &descriptor,
         layout,
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

        if (glfwGetKey(context.window, GLFW_KEY_W)) {
            initial_ubo.view = glm::translate(initial_ubo.view, glm::vec3(0, 0, speed * delta));
            ubo_buffer.updateBuffer(initial_ubo);
            ubo_buffer.writeToDescriptor();
        }

        if (glfwGetKey(context.window, GLFW_KEY_S)) {
            initial_ubo.view = glm::translate(initial_ubo.view, glm::vec3(0, 0, -speed * delta));
            ubo_buffer.updateBuffer(initial_ubo);
            ubo_buffer.writeToDescriptor();
        }

        if (glfwGetKey(context.window, GLFW_KEY_D)) {
            initial_ubo.view = glm::translate(initial_ubo.view, glm::vec3(-speed * delta, 0, 0));
            ubo_buffer.updateBuffer(initial_ubo);
            ubo_buffer.writeToDescriptor();
        }

        if (glfwGetKey(context.window, GLFW_KEY_A)) {
            initial_ubo.view = glm::translate(initial_ubo.view, glm::vec3(speed * delta, 0, 0));
            ubo_buffer.updateBuffer(initial_ubo);
            ubo_buffer.writeToDescriptor();
        }

        if (glfwGetKey(context.window, GLFW_KEY_Q)) {
            initial_ubo.view = glm::rotate(initial_ubo.view, 1.0f * (float)delta, glm::vec3(0,1,0));
            ubo_buffer.updateBuffer(initial_ubo);
            ubo_buffer.writeToDescriptor();
        }

        if (glfwGetKey(context.window, GLFW_KEY_E)) {
            initial_ubo.view = glm::rotate(initial_ubo.view, -1.0f * (float)delta, glm::vec3(0,1,0));
            ubo_buffer.updateBuffer(initial_ubo);
            ubo_buffer.writeToDescriptor();
        }

        last_time = glfwGetTime();
        commandBuffer.finishSwapchainRender();
    }


    context.device.waitIdle();
    ubo_buffer.destroy();
    commandBuffer.destroy();
    context.cleanup();
    return 0;
}
