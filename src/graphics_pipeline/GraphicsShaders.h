//
// Created by Sherif Abdou on 8/5/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSSHADERS_H
#define VULKAN_ENGINE_GRAPHICSSHADERS_H

#include <string>
#include <shaderc/shaderc.hpp>
#include "../VulkanContext.h"

class GraphicsShaders {
public:
    VulkanContext& context;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    GraphicsShaders(VulkanContext &context, const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
    std::vector<vk::PipelineShaderStageCreateInfo> getShaderStage();

private:
    static std::vector<uint32_t> compileShader(const std::string&, shaderc_shader_kind);
};


#endif //VULKAN_ENGINE_GRAPHICSSHADERS_H
