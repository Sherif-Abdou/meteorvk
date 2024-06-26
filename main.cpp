#include <iostream>

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_SILENT_WARNINGS
//#define GLM_FORCE_PLATFORM_UNKNOWN
//#define GLM_FORCE_COMPILER_UNKNOWN
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define VMA_IMPLEMENTATION
#define VMA_DEBUG_LOG
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include <unistd.h>

#include "src/core/VulkanContext.h"
#include "src/core/compute_pipeline/ComputeCommandBuffer.h"
#include "src/core/compute_pipeline/ComputePipelineBuilder.h"
#include "src/core/VulkanContext.h"
#include "src/core/storage/CombinedDescriptorSampler.h"
#include "src/core/storage/VertexBuffer.h"
#include "src/core/graphics_pipeline/GraphicsRenderPass.h"
#include "src/core/graphics_pipeline/GraphicsPipeline.h"
#include "src/core/graphics_pipeline/GraphicsCommandBuffer.h"
#include "src/core/graphics_pipeline/GraphicsPipelineBuilder.h"
#include "src/core/storage/CombinedDescriptorSampler.h"
#include "src/core/storage/OBJFile.h"
#include "src/engine/special_pipelines/ShadowGraphicsPipeline.h"
#include "src/engine/special_pipelines/ForwardRenderedGraphicsPipeline.h"
#include "src/core/shared_pipeline/PipelineBarrierBuilder.h"
#include "src/core/shared_pipeline/TransferQueue.h"
#include "src/core/storage/StorageBuffer.h"
#include "src/engine/special_pipelines/SSAOGraphicsPipeline.h"
#include "src/engine/storage/ImageTextureLoader.h"
#include "src/engine/special_pipelines/DepthOnlyPipeline.h"
#include "src/engine/programs/BackpackRenderer.h"
#include "src/engine/special_pipelines/CullingComputePipeline.h"

CombinedDescriptorSampler createSampler(VulkanContext* context) {
    auto descriptorSampler = CombinedDescriptorSampler(context);
    descriptorSampler.buildSampler();
    return descriptorSampler;
}

int main() {
    VulkanContext context{};
    context.initVulkan();
    const auto use_backpack_render = true;
    if (use_backpack_render) {

        auto renderer = BackpackRenderer();
        renderer.run(&context);
    }
//    else {
//        CullingComputePipeline pipeline(&context);
//        ModelBuffer* buffer = new ModelBuffer(&context, 2);
//        buffer->updateBuffer({glm::identity<glm::mat4>(), Material{glm::vec4(0.0)}}, 0);
//        buffer->updateBuffer({glm::translate(glm::identity<glm::mat4>(), glm::vec3(-1, 0, 0)), Material{glm::vec4(0.0)}}, 1);
//        auto v1 = BackpackRenderer::createVertexBuffer(&context, "./models/super_backpack.obj");
//        v1.canBeStorage = true;
//        v1.init();
//        v1.updateVertexBuffer();
//
//        auto v2 = BackpackRenderer::createVertexBuffer(&context, "./models/floor.obj");
//        v2.canBeStorage = true;
//        v2.init();
//        v2.updateVertexBuffer();
//
//        pipeline.vertex_buffers = {&v1, &v2};
//        pipeline.models = buffer;
//
//        pipeline.init();
//        pipeline.generateIndirects();
//
//        buffer->destroy();
//        v1.destroy();
//        v2.destroy();
//        pipeline.destroy();
//        delete buffer;
//    }
    context.cleanup();
    return 0;
}

