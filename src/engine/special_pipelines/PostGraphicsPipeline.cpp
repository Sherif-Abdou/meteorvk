//
// Created by Sherif Abdou on 1/19/24.
//

#include "PostGraphicsPipeline.h"

void PostGraphicsPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    Renderable::RenderArguments new_arguments = renderArguments;
    std::vector<VertexBuffer*> v_buffer {};
    for (auto buffer : new_arguments.vertexBuffers) {
        if (buffer->mask & VertexBuffer::DeferredQuadBit) {
            v_buffer.push_back(buffer);
        }
    }
    pipeline->renderPipeline(renderArguments);
}

PostGraphicsPipeline::PostGraphicsPipeline(std::unique_ptr<ModelBufferGraphicsPipeline> pipeline):
    pipeline(std::move(pipeline)), uniformBuffer(pipeline->getGraphicsPipeline().context) {
    uniformBuffer.allocateBuffer();

    ubo.proj = glm::perspective(glm::radians(90.0), 1920.0 / 1080.0, 0.1, 100.0);
    ubo.view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0));
}

void PostGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    Renderable::prepareRender(renderArguments);
    uniformBuffer.updateBuffer(ubo);
    uniformBuffer.writeToDescriptor(*descriptorSet, 0);
}
