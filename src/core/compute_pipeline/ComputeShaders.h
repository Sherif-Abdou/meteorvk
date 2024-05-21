//
// Created by Sherif Abdou on 11/3/23.
//

#ifndef VULKAN_ENGINE_COMPUTESHADERS_H
#define VULKAN_ENGINE_COMPUTESHADERS_H

#include "core/VulkanContext.h"

class ComputeShaders {
private:
    VulkanContext& context;
    std::string computeShaderPath;
    vk::raii::ShaderModule computeShaderModule = nullptr;
public:
    ComputeShaders(VulkanContext &context, const std::string &computeShaderPath);
    vk::PipelineShaderStageCreateInfo getShaderStage();
};


#endif //VULKAN_ENGINE_COMPUTESHADERS_H
