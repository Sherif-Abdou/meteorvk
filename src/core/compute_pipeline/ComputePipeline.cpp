//
// Created by Sherif Abdou on 11/3/23.
//

#include "ComputePipeline.h"

ComputePipeline::ComputePipeline(VulkanContext *context, vk::raii::PipelineLayout &&pipelineLayout,
                                 vk::raii::Pipeline &&pipeline) : context(context),
                                                                  pipelineLayout(std::move(pipelineLayout)),
                                                                  pipeline(std::move(pipeline)) {}

void ComputePipeline::dispatch(vk::CommandBuffer commandBuffer) {
    commandBuffer.dispatch(workgroups, 1, 1);
}

vk::raii::PipelineLayout& ComputePipeline::getPipelineLayout() {
    return this->pipelineLayout;
}


void ComputePipeline::bind(vk::CommandBuffer commandBuffer) {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, *pipeline);
}
