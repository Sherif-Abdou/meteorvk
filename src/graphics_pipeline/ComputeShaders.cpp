//
// Created by Sherif Abdou on 11/3/23.
//

#include <shaderc/shaderc.h>
#include "ComputeShaders.h"
#include "GraphicsShaders.h"

ComputeShaders::ComputeShaders(VulkanContext &context, const std::string &computeShaderPath)
    :context(context), computeShaderPath(computeShaderPath) {
}

vk::PipelineShaderStageCreateInfo ComputeShaders::getShaderStage() {
    auto computeShaderModuleCreateInfo = vk::ShaderModuleCreateInfo();
    auto computeShader = GraphicsShaders::compileShader(computeShaderPath, shaderc_glsl_compute_shader);
    computeShaderModuleCreateInfo.setCode(computeShader);
    computeShaderModule = context.device.createShaderModule(computeShaderModuleCreateInfo);

    auto computeShaderStage = vk::PipelineShaderStageCreateInfo();
    computeShaderStage.setModule(*computeShaderModule);
    computeShaderStage.setPName("main");
    computeShaderStage.setStage(vk::ShaderStageFlagBits::eCompute);

    return computeShaderStage;

}

