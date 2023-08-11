#include <iostream>

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define VMA_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "src/VulkanContext.h"
#include "src/storage/VertexBuffer.h"
#include "src/graphics_pipeline/GraphicsRenderPass.h"
#include "src/graphics_pipeline/GraphicsPipeline.h"
#include "src/graphics_pipeline/GraphicsCommandBuffer.h"

VertexBuffer createVertexBuffer(VulkanContext& context) {
    VertexBuffer buffer(*context.allocator);
    buffer.vertices.push_back(Vertex::positionOnly({0, 0, 0}));
    buffer.vertices.push_back(Vertex::positionOnly({1, 0, 0}));
    buffer.vertices.push_back(Vertex::positionOnly({0, 1, 0}));

    return buffer;
}

int main() {
    VulkanContext context {};
    context.initVulkan();
    GraphicsRenderPass renderPass(context);
    renderPass.init();
    GraphicsPipeline pipeline(context, std::move(renderPass));
    for (auto& swapChainImage: context.swapChainImageViews) {
        pipeline.targetImageViews.push_back(*swapChainImage);
    }
    pipeline.init();
    GraphicsCommandBuffer commandBuffer(context);

    commandBuffer.pipelines.push_back(std::move(pipeline));
    commandBuffer.init();
    auto vertexbuffer = createVertexBuffer(context);
    vertexbuffer.init();
    commandBuffer.vertexBuffers.push_back(std::move(vertexbuffer));

    commandBuffer.renderToSwapchain();
    glfwShowWindow(context.window);
    while (!glfwWindowShouldClose(context.window)) {
        glfwPollEvents();
    }


    context.device.waitIdle();
    commandBuffer.destroy();
    context.cleanup();
    return 0;
}
