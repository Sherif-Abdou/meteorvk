//
// Created by Sherif Abdou on 9/15/23.
//

#include "ShadowGraphicsPipeline.h"

void ShadowGraphicsPipeline::renderPipeline(GraphicsPipeline::RenderArguments renderArguments) {
    lightUniformBuffer.updateBuffer(lightUBO);
    lightUniformBuffer.writeToDescriptor(*descriptorSet, binding);
    pipeline.renderPipeline(renderArguments);
}

ShadowGraphicsPipeline::ShadowGraphicsPipeline(VulkanContext &context, GraphicsRenderPass &&renderPass)
        : pipeline(context, std::move(renderPass)), lightUniformBuffer(context) {
    lightUniformBuffer.allocateBuffer();
    lightUniformBuffer.updateBuffer(lightUBO);
}

ShadowGraphicsPipeline::ShadowGraphicsPipeline(GraphicsPipeline &&pipeline) : pipeline(std::move(pipeline)),
                                                                              lightUniformBuffer(this->pipeline.context) {
    lightUniformBuffer.allocateBuffer();
    lightUniformBuffer.updateBuffer(lightUBO);
}

GraphicsPipeline &ShadowGraphicsPipeline::getPipeline() {
    return pipeline;
}
