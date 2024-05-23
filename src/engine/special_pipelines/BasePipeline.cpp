#include "BasePipeline.h"

BasePipeline::BasePipeline(BasePipeline* super) {
  this->descriptors = super->descriptors;
}

void BasePipeline::tryLoadLocalDescriptor() {
    if (descriptors != nullptr && local_descriptor == nullptr) {
        local_descriptor = descriptors->buildPipelineDescriptorSet(pipeline_name);
    }
}

void BasePipeline::tryBindLocalDescriptor(vk::raii::CommandBuffer *commandBuffer) {
    if (local_descriptor != nullptr) {
        local_descriptor->bindToCommandBuffer(commandBuffer, getGraphicsPipeline().getPipelineLayout(), 2);
    }
}
