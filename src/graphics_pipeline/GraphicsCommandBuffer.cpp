//
// Created by Sherif Abdou on 8/5/23.
//

#include "GraphicsCommandBuffer.h"

void GraphicsCommandBuffer::createCommandPool() {
    vk::CommandPoolCreateInfo commandPoolCreateInfo {};
    auto graphicsIndex = context.findQueueFamilies(context.physicalDevice).graphicsFamily.value();
    commandPoolCreateInfo.setQueueFamilyIndex(graphicsIndex);
    commandPoolCreateInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    pool = context.device.createCommandPool(commandPoolCreateInfo);
}

void GraphicsCommandBuffer::createCommandBuffer() {
    vk::CommandBufferAllocateInfo allocateInfo {};
    allocateInfo.commandPool = *pool;
    allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocateInfo.setCommandBufferCount(1);

    commandBuffer = std::move(context.device.allocateCommandBuffers(allocateInfo)[0]);
}

void GraphicsCommandBuffer::createSyncObjects() {
    vk::SemaphoreCreateInfo semaphoreCreateInfo {};
    vk::FenceCreateInfo fenceCreateInfo {};
    fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    imageAvailableSemaphore = context.device.createSemaphore(semaphoreCreateInfo);
    renderFinishedSemaphore = context.device.createSemaphore(semaphoreCreateInfo);
    inFlightFence = context.device.createFence(fenceCreateInfo);

}
void GraphicsCommandBuffer::init() {
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
}

void GraphicsCommandBuffer::recordCommandBuffer() {
    vk::CommandBufferBeginInfo beginInfo {};
    commandBuffer.begin(beginInfo);

    for (auto& pipeline: pipelines) {
        GraphicsPipeline::RenderArguments arguments {
            .commandBuffer = commandBuffer,
            .imageIndex = swapChainImageIndex,
            .vertexBuffers = vertexBuffers,
        };

        pipeline.renderPipeline(arguments);
    }

    commandBuffer.end();
}

void GraphicsCommandBuffer::submitCommandBuffer() {
    vk::SubmitInfo submitInfo {};
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo.setCommandBuffers(*commandBuffer);
    submitInfo.setWaitSemaphores(*imageAvailableSemaphore);
    submitInfo.setSignalSemaphores(*renderFinishedSemaphore);
    submitInfo.setWaitDstStageMask(waitStage);

    context.graphicsQueue.submit(submitInfo, *inFlightFence);
}

void GraphicsCommandBuffer::waitForFence() {
    auto result = context.device.waitForFences(*inFlightFence, true, UINT32_MAX);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Fence failure");
    }
    context.device.resetFences(*inFlightFence);
}

void GraphicsCommandBuffer::sendToSwapchain() {
    vk::PresentInfoKHR presentInfo {};
    presentInfo.setWaitSemaphores(*renderFinishedSemaphore);
    presentInfo.setSwapchains(*context.swapChain);
    presentInfo.setImageIndices(swapChainImageIndex);

    auto result = context.presentQueue.presentKHR(presentInfo);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Fence failure");
    }
}

void GraphicsCommandBuffer::fetchSwapchain() {
    auto [result, index] = context.swapChain.acquireNextImage(UINT32_MAX, *imageAvailableSemaphore, nullptr);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Fence failure");
    }
    swapChainImageIndex = index;
}

void GraphicsCommandBuffer::renderToSwapchain() {
    waitForFence();
    fetchSwapchain();
    recordCommandBuffer();
    submitCommandBuffer();
    sendToSwapchain();
}

GraphicsCommandBuffer::GraphicsCommandBuffer(VulkanContext &context) : context(context) {}

void GraphicsCommandBuffer::destroy() {
    for (auto& pipeline: pipelines) {
        pipeline.destroy();
    }
    for (auto& buffer: vertexBuffers) {
        buffer.destroy();
    }
}
