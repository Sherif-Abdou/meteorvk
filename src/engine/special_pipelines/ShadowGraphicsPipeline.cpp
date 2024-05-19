//
// Created by Sherif Abdou on 9/15/23.
//

#include "ShadowGraphicsPipeline.h"

void ShadowGraphicsPipeline::renderPipeline(GraphicsPipeline::RenderArguments renderArguments) {
    pipeline->renderPipeline(renderArguments);
}

ShadowGraphicsPipeline::ShadowGraphicsPipeline(std::unique_ptr<ModelBufferGraphicsPipeline> pipeline) : pipeline(std::move(pipeline)), lightUniformBuffer(this->pipeline->getGraphicsPipeline().context) {
    this->descriptors = this->pipeline->descriptors;
    lightUniformBuffer.allocateBuffer();
    lightUniformBuffer.updateBuffer(lightUBO);
}

GraphicsPipeline &ShadowGraphicsPipeline::getPipeline() {
    return pipeline->getGraphicsPipeline();
}

GraphicsPipeline &ShadowGraphicsPipeline::getGraphicsPipeline() {
  return getPipeline();
}

void ShadowGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    pipeline->prepareRender(renderArguments);
    lightUniformBuffer.updateBuffer(lightUBO);
    lightUniformBuffer.writeToDescriptor(*descriptors->getDescriptorFor("global_ubo"), descriptors->getBindingOf("global_ubo"));
}

ShadowGraphicsPipeline::~ShadowGraphicsPipeline() {
}

void ShadowGraphicsPipeline::destroy() {
    lightUniformBuffer.destroy();
    pipeline->destroy();
}


void ShadowGraphicsPipeline::setDescriptorSet(DescriptorSet* descriptor) {
}

vk::ImageView ShadowGraphicsPipeline::getDepthImageView() {
    return *this->pipeline->getGraphicsPipeline().ownedImages[0].imageView;
}
