//
// Created by Sherif Abdou on 9/28/23.
//

#include "ModelBufferGraphicsPipeline.h"
#include "../../core/interfaces/IndirectCallStruct.h"


GraphicsPipeline & ModelBufferGraphicsPipeline::getGraphicsPipeline() {
    return graphicsPipeline;
}

void ModelBufferGraphicsPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    modelBuffer->resetIndex();
    auto vertex_buffers = renderArguments.vertexBuffers;
    graphicsPipeline.prepareRender(renderArguments);
    VkDeviceSize offset_stride = sizeof (IndirectCallStruct);
    for (uint32_t i = 0; i < vertex_buffers.size(); i++) {
        auto vertex_buffer = vertex_buffers[i];
        if (vertex_buffers.size() > 1 && vertex_buffer->mask & VertexBuffer::DeferredQuadBit) {
            continue;
        }
        auto* descriptorSet = descriptors->getDescriptorFor(MODEL_BUFFER_DESCRIPTOR_NAME);
        modelBuffer->attachOffsetToDescriptor(*descriptorSet, 0);
        descriptorSet->bindToCommandBuffer(&renderArguments.commandBuffer, graphicsPipeline.getPipelineLayout());

        graphicsPipeline.renderVertexBuffer({
            renderArguments.commandBuffer,
            renderArguments.imageIndex,
            {vertex_buffer},
            indirectBuffer,
            renderArguments.indirectBufferOffset + i * offset_stride,
        }, vertex_buffer); modelBuffer->nextIndex();
    }
    graphicsPipeline.finishRender(renderArguments);
}

ModelBufferGraphicsPipeline::ModelBufferGraphicsPipeline(GraphicsPipeline &&graphicsPipeline,
                                                         unsigned int size) : graphicsPipeline(
        std::move(graphicsPipeline)), modelBuffer(new ModelBuffer(this->graphicsPipeline.context, size)) {
}

void ModelBufferGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    modelBuffer->writeBuffer(*descriptors->getDescriptorFor(MODEL_BUFFER_DESCRIPTOR_NAME), descriptors->getBindingOf(MODEL_BUFFER_DESCRIPTOR_NAME));
}

ModelBufferGraphicsPipeline::ModelBufferGraphicsPipeline(GraphicsPipeline &&graphicsPipeline,
                                                         ModelBuffer *modelBuffer):
                                                         graphicsPipeline(std::move(graphicsPipeline)), modelBuffer(modelBuffer) {
    // descriptors = new NewDescriptorManager(graphicsPipeline.context);
}

ModelBufferGraphicsPipeline*
ModelBufferGraphicsPipeline::createPipelineFromBuilder(GraphicsPipelineBuilder &&builder, ModelBuffer *modelBuffer,
                                                       DescriptorSet* descriptor) {
    if (builder.descriptorSets.size() == 0) {
        builder.descriptorSets = std::vector<DescriptorSet*> {descriptor};
    }
    auto pipeline = builder.buildGraphicsPipeline();

    auto* final_pipeline = new ModelBufferGraphicsPipeline(std::move(pipeline), modelBuffer);
    // final_pipeline->descriptors->addDescriptorSet("main", descriptor);

    return final_pipeline;
}

void ModelBufferGraphicsPipeline::destroy() {
    getGraphicsPipeline().destroy();
}


void ModelBufferGraphicsPipeline::setDescriptorSet(DescriptorSet* descriptor) {
  // this->descriptors->addDescriptorSet("main", descriptor);
}
