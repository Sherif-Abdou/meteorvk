//
// Created by Sherif Abdou on 9/28/23.
//

#include "ModelBufferGraphicsPipeline.h"

GraphicsPipeline & ModelBufferGraphicsPipeline::getGraphicsPipeline() {
    return graphicsPipeline;
}

void ModelBufferGraphicsPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    modelBuffer->resetIndex();
    auto vertex_buffers = renderArguments.vertexBuffers;
    graphicsPipeline.prepareRender(renderArguments);
    for (uint32_t i = 0; i < vertex_buffers.size(); i++) {
        auto vertex_buffer = vertex_buffers[i];
        modelBuffer->attachOffsetToDescriptor(*descriptorSet, 0);
        descriptorSet->bindToCommandBuffer(renderArguments.commandBuffer, graphicsPipeline.getPipelineLayout());

        graphicsPipeline.renderVertexBuffer({
            renderArguments.commandBuffer,
            renderArguments.imageIndex,
            {vertex_buffer}
        }, vertex_buffer);
        modelBuffer->nextIndex();
    }
    graphicsPipeline.finishRender(renderArguments);
}

ModelBufferGraphicsPipeline::ModelBufferGraphicsPipeline(GraphicsPipeline &&graphicsPipeline,
                                                         unsigned int size) : graphicsPipeline(
        std::move(graphicsPipeline)), modelBuffer(new ModelBuffer(this->graphicsPipeline.context, size)) {
}

void ModelBufferGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    modelBuffer->writeBuffer(*descriptorSet, 2);
}

ModelBufferGraphicsPipeline::ModelBufferGraphicsPipeline(GraphicsPipeline &&graphicsPipeline,
                                                         ModelBuffer *modelBuffer):
                                                         graphicsPipeline(std::move(graphicsPipeline)), modelBuffer(modelBuffer) {

}

ModelBufferGraphicsPipeline
ModelBufferGraphicsPipeline::createPipelineFromBuilder(GraphicsPipelineBuilder &&builder, ModelBuffer *modelBuffer,
                                                       DescriptorSet* descriptor) {
    if (builder.descriptorSets.size() == 0) {
        builder.descriptorSets = std::vector<DescriptorSet*> {descriptor};
    }
    auto pipeline = builder.buildGraphicsPipeline();

    auto final_pipeline = ModelBufferGraphicsPipeline(std::move(pipeline), modelBuffer);
    final_pipeline.descriptorSet = descriptor;

    return final_pipeline;
}

