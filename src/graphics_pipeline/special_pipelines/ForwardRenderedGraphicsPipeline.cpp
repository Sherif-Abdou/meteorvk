//
// Created by Sherif Abdou on 9/18/23.
//

#include "ForwardRenderedGraphicsPipeline.h"

void ForwardRenderedGraphicsPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    uniformBuffer.updateBuffer(ubo);
    uniformBuffer.writeToDescriptor(*descriptorSet, 0);
    pipeline.renderPipeline(renderArguments);
}

GraphicsPipeline &ForwardRenderedGraphicsPipeline::getPipeline() {
    return pipeline;
}

ForwardRenderedGraphicsPipeline::ForwardRenderedGraphicsPipeline(GraphicsPipeline &&pipeline) : pipeline(
        std::move(pipeline)), uniformBuffer(pipeline.context) {
    uniformBuffer.allocateBuffer();
}
