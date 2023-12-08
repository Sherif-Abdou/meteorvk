//
// Created by Sherif Abdou on 9/15/23.
//

#include "ShadowGraphicsPipeline.h"

void ShadowGraphicsPipeline::renderPipeline(GraphicsPipeline::RenderArguments renderArguments) {
    pipeline.renderPipeline(renderArguments);
}

ShadowGraphicsPipeline::ShadowGraphicsPipeline(ModelBufferGraphicsPipeline &pipeline) : pipeline(pipeline),
                                                                              lightUniformBuffer(this->pipeline.getGraphicsPipeline().context) {
    lightUniformBuffer.allocateBuffer();
    lightUniformBuffer.updateBuffer(lightUBO);
}

GraphicsPipeline &ShadowGraphicsPipeline::getPipeline() {
    return pipeline.getGraphicsPipeline();
}

void ShadowGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    pipeline.prepareRender(renderArguments);
    lightUniformBuffer.updateBuffer(lightUBO);
    lightUniformBuffer.writeToDescriptor(*descriptorSet, binding);
}

ShadowGraphicsPipeline::~ShadowGraphicsPipeline() {
}

ShadowGraphicsPipeline ShadowGraphicsPipeline::createFromPipeline(ModelBufferGraphicsPipeline &&pipeline) {
    auto descriptor = pipeline.descriptorSet;
    auto new_pipeline = ShadowGraphicsPipeline(pipeline);
    new_pipeline.descriptorSet = descriptor;

    return new_pipeline;
}

void ShadowGraphicsPipeline::destroy() {
    lightUniformBuffer.destroy();
    pipeline.destroy();
}
