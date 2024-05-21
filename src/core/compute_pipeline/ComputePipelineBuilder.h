//
// Created by Sherif Abdou on 11/3/23.
//

#ifndef VULKAN_ENGINE_COMPUTEPIPELINEBUILDER_H
#define VULKAN_ENGINE_COMPUTEPIPELINEBUILDER_H

#include "core/VulkanContext.h"
#include "ComputeShaders.h"
#include "ComputePipeline.h"

class ComputePipelineBuilder {
public:
    explicit ComputePipelineBuilder(VulkanContext *context);
    std::optional<vk::DescriptorSetLayout> descriptor;

    const std::optional<vk::DescriptorSetLayout> &getDescriptor() const;

    void setDescriptor(const std::optional<vk::DescriptorSetLayout> &descriptor);

    vk::PipelineShaderStageCreateInfo shader_stage {};

    void setShader(ComputeShaders& shaders);

    ComputePipeline build();

    uint32_t workgroups = 1;
private:
    VulkanContext* context;
    vk::raii::Pipeline computePipeline = nullptr;
    vk::raii::PipelineLayout layout = nullptr;
    void createPipelineLayout();
    void createPipeline();
};


#endif //VULKAN_ENGINE_COMPUTEPIPELINEBUILDER_H
