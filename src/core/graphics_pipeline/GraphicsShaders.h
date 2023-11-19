//
// Created by Sherif Abdou on 8/5/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSSHADERS_H
#define VULKAN_ENGINE_GRAPHICSSHADERS_H

#include <string>
#include <shaderc/shaderc.hpp>
#include "../VulkanContext.h"

/// Handles the compilation of glsl shaders
class GraphicsShaders {
public:
    VulkanContext& context;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    vk::raii::ShaderModule vertexShaderModule = nullptr;
    vk::raii::ShaderModule fragmentShaderModule = nullptr;
    GraphicsShaders(VulkanContext &context, const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
    std::vector<vk::PipelineShaderStageCreateInfo> getShaderStage();

    static std::vector<uint32_t> compileShader(const std::string&, shaderc_shader_kind);
};


#endif //VULKAN_ENGINE_GRAPHICSSHADERS_H
