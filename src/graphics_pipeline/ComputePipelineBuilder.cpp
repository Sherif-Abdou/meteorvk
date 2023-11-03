//
// Created by Sherif Abdou on 11/3/23.
//

#include "ComputePipelineBuilder.h"

ComputePipelineBuilder::ComputePipelineBuilder(VulkanContext &context) : context(context) {}

void ComputePipelineBuilder::createPipelineLayout() {
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
    if (this->descriptor.has_value()) {
        pipelineLayoutCreateInfo.setSetLayouts(this->descriptor.value());
    }

    this->layout = context.device.createPipelineLayout(pipelineLayoutCreateInfo);
}

void ComputePipelineBuilder::createPipeline() {
    vk::ComputePipelineCreateInfo createInfo {};
    createInfo.setLayout(*this->layout);
    createInfo.setStage(this->shader_stage);

    this->computePipeline = context.device.createComputePipeline(nullptr, createInfo);
}

const std::optional<vk::DescriptorSetLayout> &ComputePipelineBuilder::getDescriptor() const {
    return descriptor;
}

void ComputePipelineBuilder::setDescriptor(const std::optional<vk::DescriptorSetLayout> &descriptor) {
    ComputePipelineBuilder::descriptor = descriptor;
}

void ComputePipelineBuilder::setShader(ComputeShaders &shaders) {
    this->shader_stage = shaders.getShaderStage();
}

ComputePipeline ComputePipelineBuilder::build() {
    createPipelineLayout();
    createPipeline();

    auto res = ComputePipeline(context, std::move(layout), std::move(computePipeline));
    res.workgroups = this->workgroups;
    return res;
}
