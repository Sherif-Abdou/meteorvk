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
#include "src/VertexBuffer.h"
#include "src/graphics_pipeline/GraphicsRenderPass.h"
#include "src/graphics_pipeline/GraphicsPipeline.h"

int main() {
    VulkanContext context {};
    context.initVulkan();
    GraphicsRenderPass renderPass(context);
    renderPass.init();
    GraphicsPipeline pipeline(context, std::move(renderPass));
    pipeline.init();


    char buffer[32];
    std::cin >> buffer;

    std::cout << "Hello, World!" << std::endl;
    context.cleanup();
    return 0;
}
