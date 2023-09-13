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
    pipelineSemaphore = context.device.createSemaphore(semaphoreCreateInfo);
    pipelineFence = context.device.createFence(fenceCreateInfo);
}
GraphicsPipeline::GraphicsPipeline(VulkanContext &context, GraphicsRenderPass&& renderPass)
        : context(context), renderPass(std::move(renderPass)) {}

vk::raii::Semaphore & GraphicsPipeline::getPipelineSemaphore() {
    return pipelineSemaphore;
}

vk::raii::Fence & GraphicsPipeline::getPipelineFence() {
    return pipelineFence;
}

void GraphicsPipeline::renderPipeline(GraphicsPipeline::RenderArguments renderArguments) {
    assert(targetFramebuffers.size() > 0);
    auto imageIndex = renderArguments.imageIndex;
    vk::RenderPassBeginInfo beginInfo {};
    imageIndex = std::min((long)imageIndex, (long)targetFramebuffers.size()-1);

    vk::ClearValue clearValues[2];
    clearValues[0].setColor(vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f));
    clearValues[1].setDepthStencil(vk::ClearDepthStencilValue(1.0, 0.0));

    beginInfo.setFramebuffer(*targetFramebuffers[imageIndex]);
    beginInfo.setRenderPass(*renderPass.getRenderPass());
    beginInfo.setClearValues(clearValues);
    auto rect = vk::Rect2D {};
    rect.setOffset({0, 0});
    rect.setExtent(context.swapChainExtent);
    beginInfo.setRenderArea(rect);

    renderArguments.commandBuffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
    renderArguments.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
    for (auto& buffer: renderArguments.vertexBuffers) {
        buffer.updateVertexBuffer();
        buffer.attachToCommandBuffer(renderArguments.commandBuffer);
    }

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(context.swapChainExtent.height);;
    viewport.width = static_cast<float>(context.swapChainExtent.width);
    viewport.height = -static_cast<float>(context.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    renderArguments.commandBuffer.setViewport(0, viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D {0, 0};
    scissor.extent = context.swapChainExtent;
    renderArguments.commandBuffer.setScissor(0, scissor);
    for (auto& vbo: renderArguments.vertexBuffers) {
        vbo.draw(renderArguments.commandBuffer);
    }
    renderArguments.commandBuffer.endRenderPass();
}

GraphicsPipeline::~GraphicsPipeline() {
    for (auto& ownedImage: ownedImages) {
        ownedImage.imageAllocation.destroy();
    }
}

void GraphicsPipeline::destroy() {
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
