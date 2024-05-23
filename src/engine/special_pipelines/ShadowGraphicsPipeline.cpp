//
// Created by Sherif Abdou on 9/15/23.
//

#include "ShadowGraphicsPipeline.h"

void ShadowGraphicsPipeline::renderPipeline(GraphicsPipeline::RenderArguments renderArguments) {
    BasePipeline::tryBindLocalDescriptor(&renderArguments.commandBuffer);
    pipeline->renderPipeline(renderArguments);
}

ShadowGraphicsPipeline::ShadowGraphicsPipeline(std::unique_ptr<ModelBufferGraphicsPipeline> pipeline) : pipeline(std::move(pipeline)), lightUniformBuffer(this->pipeline->getGraphicsPipeline().context) {
    this->descriptors = this->pipeline->descriptors;
    lightUniformBuffer.allocateBuffer();
    lightUniformBuffer.updateBuffer(lightUBO);

    pipeline_name = "shadow_pipeline";
}

GraphicsPipeline &ShadowGraphicsPipeline::getPipeline() {
    return pipeline->getGraphicsPipeline();
}

GraphicsPipeline &ShadowGraphicsPipeline::getGraphicsPipeline() {
  return getPipeline();
}

void ShadowGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    BasePipeline::tryLoadLocalDescriptor();

    pipeline->prepareRender(renderArguments);
    lightUniformBuffer.updateBuffer(lightUBO);
    lightUniformBuffer.writeToDescriptor(*local_descriptor, descriptors->getBindingOf("shadow_ubo", this->pipeline_name));
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

vk::Image ShadowGraphicsPipeline::getDepthImage() {
    return this->pipeline->getGraphicsPipeline().ownedImages[0].imageAllocation.image;
}

glm::mat4 ShadowGraphicsPipeline::exportLightProjView() {
    return lightUBO.proj * lightUBO.view;
}
