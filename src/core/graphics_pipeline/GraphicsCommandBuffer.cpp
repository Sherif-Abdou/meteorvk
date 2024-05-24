//
// Created by Sherif Abdou on 8/5/23.
//

#include "GraphicsCommandBuffer.h"
#include "vulkan/vulkan_enums.hpp"
#include <unordered_set>

void GraphicsCommandBuffer::createCommandPool() {
    vk::CommandPoolCreateInfo commandPoolCreateInfo{};
    auto graphicsIndex = context->findQueueFamilies(context->physicalDevice).graphicsFamily.value();
    commandPoolCreateInfo.setQueueFamilyIndex(graphicsIndex);
    commandPoolCreateInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    pool = context->device.createCommandPool(commandPoolCreateInfo);
}

void GraphicsCommandBuffer::createCommandBuffer() {
    vk::CommandBufferAllocateInfo allocateInfo{};
    allocateInfo.commandPool = *pool;
    allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocateInfo.setCommandBufferCount(FRAMES_IN_FLIGHT);

    auto array = context->device.allocateCommandBuffers(allocateInfo);
    for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        commandBuffer[i] = std::move(array[i]);
    }
}

void GraphicsCommandBuffer::createSyncObjects() {
    vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    vk::FenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        imageAvailableSemaphore[i] = context->device.createSemaphore(semaphoreCreateInfo);
        renderFinishedSemaphore[i] = context->device.createSemaphore(semaphoreCreateInfo);
        inFlightFence[i] = context->device.createFence(fenceCreateInfo);
    }

}

void GraphicsCommandBuffer::init() {
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
}

void GraphicsCommandBuffer::recordCommandBuffer() {
    uint32_t i = 0;
    for (auto &pipeline: pipelines) {
        GraphicsPipeline::RenderArguments arguments{
                .commandBuffer = commandBuffer[current_frame],
                .imageIndex = swapChainImageIndex,
                .vertexBuffers = vertexBuffers,
        };
        pipeline->prepareRender(arguments);
        i++;
    }

    i = 0;
    for (auto pipeline: pipelines) {
        if (dependencies.contains(i)) {
            auto dependency_list = dependencies[i];
            for (auto& dependency : dependency_list) {
                dependency.applyBarrier(*commandBuffer[current_frame]);
            }
        }
        GraphicsPipeline::RenderArguments arguments{
                .commandBuffer = commandBuffer[current_frame],
                .imageIndex = swapChainImageIndex,
                .vertexBuffers = vertexBuffers,
        };

        if (bindings.contains(i)) {
            auto binding = bindings[i];
            auto descriptorSet = binding.descriptorSet;
            auto layout = binding.layout;
            auto set = binding.set;
            if (descriptorSet != nullptr && layout != nullptr) {
                descriptorSet->bindToCommandBuffer(&commandBuffer[current_frame], *layout, set);
        }
        }

        pipeline->renderPipeline(arguments);
        i++;
    }

    commandBuffer[current_frame].end();
}

void GraphicsCommandBuffer::beginCommandBuffer() const {
    vk::CommandBufferBeginInfo beginInfo{};
    commandBuffer[current_frame].begin(beginInfo);
}

void GraphicsCommandBuffer::submitCommandBuffer(bool waitOnSwapchain) {
    vk::SubmitInfo submitInfo{};
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo.setCommandBuffers(*commandBuffer[current_frame]);
    if (waitOnSwapchain) {
        submitInfo.setWaitSemaphores(*imageAvailableSemaphore[current_frame]);
        submitInfo.setWaitDstStageMask(waitStage);
    } else {
        // submitInfo.setWaitSemaphores(*renderFinishedSemaphore[current_frame]);
        submitInfo.setWaitSemaphores({});
        submitInfo.setWaitSemaphoreCount(0);
    }
    if (waitOnSwapchain) {
        submitInfo.setSignalSemaphores(*renderFinishedSemaphore[current_frame]);
    } else {
        submitInfo.setSignalSemaphores({});
        submitInfo.setSignalSemaphoreCount(0);
    }

    context->graphicsQueue.submit(submitInfo, *inFlightFence[current_frame]);
}

void GraphicsCommandBuffer::waitForFence() {
    auto result = context->device.waitForFences(*inFlightFence[current_frame], true, UINT32_MAX);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Fence failure");
    }
    context->device.resetFences(*inFlightFence[current_frame]);
}

void GraphicsCommandBuffer::sendToSwapchain() {
    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphores(*renderFinishedSemaphore[current_frame]);
    presentInfo.setSwapchains(*context->swapChain);
    presentInfo.setImageIndices(swapChainImageIndex);

    auto result = context->presentQueue.presentKHR(presentInfo);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Fence failure");
    }

    current_frame = (current_frame + 1) % FRAMES_IN_FLIGHT;

    // Make sure not to ever tick the same descriptor set twice using a set
    /*
    std::unordered_set<DescriptorSet*> visited_sets {};
    for (auto pair: bindings) {
        auto& binding = pair.second;
        if (binding.descriptorSet != nullptr && !visited_sets.contains(binding.descriptorSet)) {
            visited_sets.insert(binding.descriptorSet);
            binding.descriptorSet->nextFrame();
        }
    }
    if (frameDescriptorSet != nullptr && !visited_sets.contains(frameDescriptorSet)) {
        visited_sets.insert(frameDescriptorSet);
        frameDescriptorSet->nextFrame();
    }
    */
    if (descriptorManager != nullptr) {
        descriptorManager->tickAllDescriptors();
    }
}

void GraphicsCommandBuffer::fetchSwapchain() {
    auto [result, index] = context->swapChain.acquireNextImage(UINT32_MAX, *imageAvailableSemaphore[current_frame],
                                                              nullptr);
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
    submitCommandBuffer();
    sendToSwapchain();
}       

void GraphicsCommandBuffer::beginSwapchainRender() {
    waitForFence();
    fetchSwapchain();
    beginCommandBuffer();
}

void GraphicsCommandBuffer::beginSimpleRender() {
    waitForFence();
    beginCommandBuffer();
}

void GraphicsCommandBuffer::finishSimpleRender() {
    submitCommandBuffer(false);
}

GraphicsCommandBuffer::GraphicsCommandBuffer(VulkanContext *context) : context(context) {}

void GraphicsCommandBuffer::destroy() {
    for (auto &buffer: vertexBuffers) {
        buffer->destroy();
    }
}

vk::raii::Semaphore* GraphicsCommandBuffer::getCurrentImageAvailableSemaphore() {
    return &this->imageAvailableSemaphore[current_frame];
}
vk::raii::Semaphore* GraphicsCommandBuffer::getCurrentRenderFinishedSemaphore() {
    return &this->renderFinishedSemaphore[current_frame];
}

void GraphicsCommandBuffer::recordSwapchainRender() {
    recordCommandBuffer();
}
void GraphicsCommandBuffer::recordSimpleRender() {
    recordCommandBuffer();
}
