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
    for (auto& binding : bindings) {
        auto descriptorSet = binding.descriptorSet;
        auto layout = binding.layout;
        auto set = binding.set;
        if (descriptorSet != nullptr && layout != nullptr) {
            descriptorSet->bindToCommandBuffer(commandBuffer, *layout, set);
        }
    }
    uint32_t i = 0;
    for (auto& pipeline: pipelines) {
        if (i - 1 > 0 && i - 1 < dependencies.size()) {
            commandBuffer.pipelineBarrier2KHR(dependencies[i - 1]);
        }
        GraphicsPipeline::RenderArguments arguments {
            .commandBuffer = commandBuffer,
            .imageIndex = swapChainImageIndex,
            .vertexBuffers = vertexBuffers,
        };

        pipeline->renderPipeline(arguments);
        i++;
    }

    commandBuffer.end();
}

void GraphicsCommandBuffer::beginCommandBuffer() const {
    vk::CommandBufferBeginInfo beginInfo {};
    commandBuffer.begin(beginInfo);
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
    beginSwapchainRender();
    finishSwapchainRender();
}

void GraphicsCommandBuffer::finishSwapchainRender() {
    recordCommandBuffer();
    submitCommandBuffer();
    sendToSwapchain();
}

void GraphicsCommandBuffer::beginSwapchainRender() {
    waitForFence();
    fetchSwapchain();
    beginCommandBuffer();
}

GraphicsCommandBuffer::GraphicsCommandBuffer(VulkanContext &context) : context(context) {}

void GraphicsCommandBuffer::destroy() {
    for (auto& buffer: vertexBuffers) {
        buffer->destroy();
    }
}

