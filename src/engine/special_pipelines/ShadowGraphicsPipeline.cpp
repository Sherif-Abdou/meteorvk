//
// Created by Sherif Abdou on 9/15/23.
//

#include "ShadowGraphicsPipeline.h"

void ShadowGraphicsPipeline::renderPipeline(GraphicsPipeline::RenderArguments renderArguments) {
    pipeline->renderPipeline(renderArguments);
}

ShadowGraphicsPipeline::ShadowGraphicsPipeline(std::unique_ptr<ModelBufferGraphicsPipeline> pipeline) : pipeline(std::move(pipeline)),
                                                                              lightUniformBuffer(this->pipeline->getGraphicsPipeline().context) {
    lightUniformBuffer.allocateBuffer();
    lightUniformBuffer.updateBuffer(lightUBO);
}

GraphicsPipeline &ShadowGraphicsPipeline::getPipeline() {
    return pipeline->getGraphicsPipeline();
}

void ShadowGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    pipeline->prepareRender(renderArguments);
    lightUniformBuffer.updateBuffer(lightUBO);
    lightUniformBuffer.writeToDescriptor(*descriptorSet, binding);
}

ShadowGraphicsPipeline::~ShadowGraphicsPipeline() {
}

void ShadowGraphicsPipeline::destroy() {
    lightUniformBuffer.destroy();
    pipeline->destroy();
}
