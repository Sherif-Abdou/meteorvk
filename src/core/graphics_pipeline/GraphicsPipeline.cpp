//
// Created by Sherif Abdou on 8/4/23.
//

#include "GraphicsPipeline.h"
#include "../storage/Vertex.h"
#include "GraphicsShaders.h"

void GraphicsPipeline::init() {
    createSyncObjects();
}

void GraphicsPipeline::createSyncObjects() {
    vk::SemaphoreCreateInfo semaphoreCreateInfo {};
    vk::FenceCreateInfo fenceCreateInfo {};
    pipelineSemaphore = context->device.createSemaphore(semaphoreCreateInfo);
    pipelineFence = context->device.createFence(fenceCreateInfo);
}
GraphicsPipeline::GraphicsPipeline(VulkanContext *context, std::unique_ptr<GraphicsRenderPass> renderPass)
        : context(context), ownedRenderPass(std::move(renderPass)) {}

vk::raii::Semaphore & GraphicsPipeline::getPipelineSemaphore() {
    return pipelineSemaphore;
}

vk::raii::Fence & GraphicsPipeline::getPipelineFence() {
    return pipelineFence;
}

void GraphicsPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    assert(targetFramebuffers.size() > 0);
    prepareRender(renderArguments);

    for (VertexBuffer* vbo: renderArguments.vertexBuffers) {
        renderVertexBuffer(renderArguments, vbo);
    }

    finishRender(renderArguments);
}

void GraphicsPipeline::finishRender(
        const Renderable::RenderArguments &renderArguments) const { renderArguments.commandBuffer.endRenderPass(); }

void GraphicsPipeline::renderVertexBuffer(Renderable::RenderArguments renderArguments, VertexBuffer *vbo) const {
    if (renderArguments.indirectBuffer != nullptr) {
        vbo->draw_indirect(renderArguments.commandBuffer, *renderArguments.indirectBuffer, renderArguments.indirectBufferOffset);
    } else {
        vbo->draw(renderArguments.commandBuffer);
    }
}

void GraphicsPipeline::prepareRender(Renderable::RenderArguments &renderArguments) {
    auto imageIndex = renderArguments.imageIndex;
    vk::RenderPassBeginInfo beginInfo {};
    imageIndex = std::min((long)imageIndex, (long) targetFramebuffers.size() - 1);


    beginInfo.setFramebuffer(*targetFramebuffers[imageIndex]);
    if (ownedRenderPass != nullptr) {
        beginInfo.setRenderPass(*ownedRenderPass->getRenderPass());
    } else if (ownedDirectRenderPass != nullptr) {
        beginInfo.setRenderPass(**ownedDirectRenderPass);
    }
    beginInfo.setClearValues(clearValues);
    auto rect = vk::Rect2D {};
    rect.setOffset({0, 0});
    rect.setExtent(extent);
    beginInfo.setRenderArea(rect);

    renderArguments.commandBuffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
    renderArguments.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
    for (auto& buffer: renderArguments.vertexBuffers) {
        if (buffer == nullptr) continue;
        // buffer->updateVertexBuffer();
        buffer->attachToCommandBuffer(renderArguments.commandBuffer);
    }

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(extent.height);;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = -static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    renderArguments.commandBuffer.setViewport(0, viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D {0, 0};
    scissor.extent = extent;
    renderArguments.commandBuffer.setScissor(0, scissor);
}

GraphicsPipeline::~GraphicsPipeline() {
}

void GraphicsPipeline::destroy() {
    for (auto& ownedImage: ownedImages) {
        ownedImage.imageAllocation.destroy();
    }
}

vk::raii::PipelineLayout &GraphicsPipeline::getPipelineLayout() {
    return pipelineLayout;
}

const vk::raii::Pipeline &GraphicsPipeline::getPipeline() const {
    return pipeline;
}

void GraphicsPipeline::setPipeline(vk::raii::Pipeline && pipeline) {
    GraphicsPipeline::pipeline = std::move(pipeline);
}

GraphicsPipeline::GraphicsPipeline(VulkanContext *context, std::unique_ptr<vk::raii::RenderPass> renderPass)
    : context(context), ownedDirectRenderPass(std::move(renderPass)) {

}
