//
// Created by Sherif Abdou on 9/18/23.
//

#include "ForwardRenderedGraphicsPipeline.h"

void ForwardRenderedGraphicsPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    pipeline.renderPipeline(renderArguments);
}

GraphicsPipeline &ForwardRenderedGraphicsPipeline::getPipeline() {
    return pipeline.getGraphicsPipeline();
}

ForwardRenderedGraphicsPipeline::ForwardRenderedGraphicsPipeline(ModelBufferGraphicsPipeline &pipeline) : pipeline(
        pipeline), uniformBuffer(pipeline.getGraphicsPipeline().context) {
    uniformBuffer.allocateBuffer();

    ubo.proj = glm::perspective(glm::radians(90.0), 1920.0 / 1080.0, 0.1, 100.0);
    ubo.view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0));
    ubo.view = glm::translate(ubo.view, glm::vec3(0, 0, -4));
}

void ForwardRenderedGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    pipeline.prepareRender(renderArguments);
    uniformBuffer.updateBuffer(ubo);
    uniformBuffer.writeToDescriptor(*descriptorSet, 0);
}

ForwardRenderedGraphicsPipeline::~ForwardRenderedGraphicsPipeline() {
    uniformBuffer.destroy();
}

ForwardRenderedGraphicsPipeline
ForwardRenderedGraphicsPipeline::createFromModelPipeline(ModelBufferGraphicsPipeline &pipeline) {
    auto descriptorSet = pipeline.descriptorSet;
    auto result =  ForwardRenderedGraphicsPipeline(pipeline);
    result.descriptorSet = descriptorSet;

    return result;
}
