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

        auto compute_pipeline_builder = ComputePipelineBuilder(&context);
        auto shaders = ComputeShaders(context, "shaders/basic.comp");

        auto storage1 = StorageBuffer<A>(&context);
        auto storage2 = StorageBuffer<A>(&context);
        storage1.allocateBuffer();
        storage2.allocateBuffer();

        auto descriptor = DescriptorSet(&context, {sb1_binding, sb2_binding});
        descriptor.buildDescriptor();

        storage1.writeToDescriptor(descriptor, 0);
        storage2.writeToDescriptor(descriptor, 1);

        compute_pipeline_builder.setShader(shaders);
        compute_pipeline_builder.setDescriptor(descriptor.getDescriptorSetLayout());
        compute_pipeline_builder.workgroups = 16;
        auto pipeline = compute_pipeline_builder.build();

        auto compute_command = ComputeCommandBuffer(&context);
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

