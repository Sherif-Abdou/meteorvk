//
// Created by Sherif Abdou on 11/20/23.
//

#include "TransferQueue.h"

void TransferQueue::init() {
    vk::CommandPoolCreateInfo command_pool_create_info {};
    command_pool_create_info.setQueueFamilyIndex(*context->findQueueFamilies(context->physicalDevice).transferFamily);

    pool = context->device.createCommandPool(command_pool_create_info);

    vk::CommandBufferAllocateInfo command_buffer_allocate_info {};
    command_buffer_allocate_info.setCommandPool(*pool);
    command_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);
    command_buffer_allocate_info.setCommandBufferCount(1);

    command_buffer = std::move(context->device.allocateCommandBuffers(command_buffer_allocate_info)[0]);
}

void TransferQueue::begin() {
    if (!ran) {
        vk::CommandBufferBeginInfo begin_info {};
        begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        command_buffer.begin(begin_info);
    }
}

void TransferQueue::applyBarrier(PipelineBarrier barrier) {
    barrier.applyBarrier(*command_buffer);
}

void TransferQueue::copy(vk::Buffer src, vk::Buffer dst, vk::BufferCopy region) {
    command_buffer.copyBuffer(src, dst, region);
}

void TransferQueue::copyBufferToImage(vk::Buffer src, vk::Image dst, vk::ImageLayout layout, vk::BufferImageCopy region) {
    command_buffer.copyBufferToImage(src, dst, layout, region);
}


void TransferQueue::submit() {
    command_buffer.end();

    vk::SubmitInfo submit_info {};
    submit_info.setCommandBuffers(*command_buffer);
    context->transferQueue.submit(submit_info, nullptr);

    ran = true;

    context->device.waitIdle();
}
