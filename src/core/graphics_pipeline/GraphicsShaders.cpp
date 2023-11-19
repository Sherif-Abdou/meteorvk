//
// Created by Sherif Abdou on 8/5/23.
//

#include "GraphicsShaders.h"

#include <fstream>
#include <sstream>

static std::string readFile(const std::string& path) {
    std::fstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

std::vector<uint32_t> GraphicsShaders::compileShader(const std::string & path, shaderc_shader_kind kind) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    std::string source = readFile(path);

    options.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_zero);

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, path.c_str(), options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw std::runtime_error(result.GetErrorMessage());
    }

    return {result.cbegin(), result.cend()};
}

std::vector<vk::PipelineShaderStageCreateInfo> GraphicsShaders::getShaderStage() {
    auto vertexShaderModuleCreateInfo = vk::ShaderModuleCreateInfo();
    auto vertexShader = compileShader(vertexShaderPath, shaderc_glsl_vertex_shader);
    vertexShaderModuleCreateInfo.setCode(vertexShader);
    vertexShaderModule = context.device.createShaderModule(vertexShaderModuleCreateInfo);

    auto fragmentShaderModuleCreateInfo = vk::ShaderModuleCreateInfo();
    auto fragmentShader = compileShader(fragmentShaderPath, shaderc_glsl_fragment_shader);
    fragmentShaderModuleCreateInfo.setCode(fragmentShader);
    fragmentShaderModule = context.device.createShaderModule(fragmentShaderModuleCreateInfo);

    auto vertexShaderStage = vk::PipelineShaderStageCreateInfo();
    vertexShaderStage.setModule(*vertexShaderModule);
    vertexShaderStage.setPName("main");
    vertexShaderStage.setStage(vk::ShaderStageFlagBits::eVertex);

    auto fragmentShaderStage = vk::PipelineShaderStageCreateInfo();
    fragmentShaderStage.setModule(*fragmentShaderModule);
    fragmentShaderStage.setPName("main");
    fragmentShaderStage.setStage(vk::ShaderStageFlagBits::eFragment);

    return {vertexShaderStage, fragmentShaderStage};
}

GraphicsShaders::GraphicsShaders(VulkanContext &context, const std::string &vertexShaderPath,
                                 const std::string &fragmentShaderPath) : context(context),
                                                                          vertexShaderPath(vertexShaderPath),
                                                                          fragmentShaderPath(fragmentShaderPath) {}
