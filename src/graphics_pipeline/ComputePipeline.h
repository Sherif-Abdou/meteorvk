//
// Created by Sherif Abdou on 11/3/23.
//

#ifndef VULKAN_ENGINE_COMPUTEPIPELINE_H
#define VULKAN_ENGINE_COMPUTEPIPELINE_H

#include "../VulkanContext.h"
#include <vulkan/vulkan_raii.hpp>

class ComputePipeline {
public:
    explicit ComputePipeline(VulkanContext& context, vk::raii::PipelineLayout &&descriptorLayout, vk::raii::Pipeline &&pipeline);
    uint32_t workgroups = 1;

    void bind(vk::CommandBuffer commandBuffer);
    void dispatch(vk::CommandBuffer commandBuffer);
private:
    vk::raii::PipelineLayout pipelineLayout = nullptr;
    vk::raii::Pipeline pipeline = nullptr;
    VulkanContext& context;
};


#endif //VULKAN_ENGINE_COMPUTEPIPELINE_H
