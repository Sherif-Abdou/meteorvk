//
// Created by Sherif Abdou on 11/3/23.
//

#include "ComputeCommandBuffer.h"

ComputeCommandBuffer::ComputeCommandBuffer(VulkanContext &context) : context(context) {}

void ComputeCommandBuffer::createCommandPool() {
    vk::CommandPoolCreateInfo createInfo {};
    createInfo.setQueueFamilyIndex(context.findQueueFamilies(context.physicalDevice).computeFamily.value());

    commandPool = context.device.createCommandPool(createInfo);
}

void ComputeCommandBuffer::createCommandBuffer() {
    vk::CommandBufferAllocateInfo allocateInfo {};
    allocateInfo.setCommandPool(*this->commandPool);
    allocateInfo.setCommandBufferCount(1);
    allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);

    commandBuffer = context.device.allocateCommandBuffers(allocateInfo);
}

void ComputeCommandBuffer::init() {
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
}

void ComputeCommandBuffer::begin() {
    vk::CommandBufferBeginInfo beginInfo {};

    commandBuffer[currentFrame].begin(beginInfo);
}

void ComputeCommandBuffer::end() {
    commandBuffer.end();
}

void ComputeCommandBuffer::submit() {
    vk::SubmitInfo submitInfo {};
    submitInfo.setCommandBuffers(*commandBuffer[currentFrame]);
    submitInfo.setSignalSemaphores(*signalSemaphores[currentFrame]);

    context.computeQueue.submit(submitInfo, *inFlightFences[currentFrame]);
}

void ComputeCommandBuffer::nextFrame() {
    currentFrame = (currentFrame + 1) % 2;
}

uint32_t ComputeCommandBuffer::getCurrentFrame() const {
    return currentFrame;
}

void ComputeCommandBuffer::createSyncObjects() {
    vk::FenceCreateInfo fenceCreateInfo {};
    fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    vk::SemaphoreCreateInfo semaphoreCreateInfo {};

    for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
        inFlightFences.push_back(context.device.createFence(fenceCreateInfo));
        signalSemaphores.push_back(context.device.createSemaphore(semaphoreCreateInfo));
    }
}

void ComputeCommandBuffer::bindAndDispatch(ComputePipeline &pipeline) {
    pipeline.bind(*commandBuffer[currentFrame]);
    pipeline.dispatch(*commandBuffer[currentFrame]);
}

vk::Semaphore ComputeCommandBuffer::getCurrentSignalSemaphore() {
    return *signalSemaphores[currentFrame];
}
