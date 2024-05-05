//
// Created by Sherif Abdou on 9/18/23.
//

#include "ForwardRenderedGraphicsPipeline.h"

void ForwardRenderedGraphicsPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    pipeline->renderPipeline(renderArguments);
}

GraphicsPipeline &ForwardRenderedGraphicsPipeline::getPipeline() {
    return pipeline->getGraphicsPipeline();
}

GraphicsPipeline &ForwardRenderedGraphicsPipeline::getGraphicsPipeline() {
  return pipeline->getGraphicsPipeline();
}

ForwardRenderedGraphicsPipeline::ForwardRenderedGraphicsPipeline(std::unique_ptr<BasePipeline> pipeline) : pipeline(
        std::move(pipeline)), uniformBuffer(pipeline->getGraphicsPipeline().context) {
    uniformBuffer.allocateBuffer();

    ubo.proj = glm::perspective(glm::radians(90.0), 1920.0 / 1080.0, 0.1, 100.0);
    ubo.view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0));
    ubo.view = glm::translate(ubo.view, glm::vec3(0, 0, -4));

    descriptors = this->pipeline->descriptors;
}

void ForwardRenderedGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    pipeline->prepareRender(renderArguments);
    uniformBuffer.updateBuffer(ubo);
    uniformBuffer.writeToDescriptor(*descriptors->getDescriptorSet("main"), 0);
}

ForwardRenderedGraphicsPipeline*
ForwardRenderedGraphicsPipeline::createFromModelPipeline(std::unique_ptr<ModelBufferGraphicsPipeline> pipeline) {
    auto* result = new ForwardRenderedGraphicsPipeline(std::move(pipeline));

    return result;
}

void ForwardRenderedGraphicsPipeline::destroy() {
    uniformBuffer.destroy();
    pipeline->destroy();
}

void ForwardRenderedGraphicsPipeline::setDescriptorSet(DescriptorSet* descriptor) {
  descriptors->addDescriptorSet("main", descriptor);
}
