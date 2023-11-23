//
// Created by Sherif Abdou on 11/13/23.
//

#include "PipelineBarrierBuilder.h"

PipelineBarrierBuilder& PipelineBarrierBuilder::forImage(vk::Image image, vk::ImageSubresourceRange subresource) {
    this->image = {
        .image =  image,
        .subresource_layout = subresource
    };
    return *this;
}

PipelineBarrierBuilder& PipelineBarrierBuilder::forBuffer(vk::Buffer buffer, vk::DeviceSize size, vk::DeviceSize offset) {
    this->buffer = {
        .buffer = buffer,
        .offset = offset,
        .size = size,
    };
    return *this;
}

PipelineBarrierBuilder& PipelineBarrierBuilder::waitFor(vk::PipelineStageFlags2 stages) {
    coreBarrier.setSrcStageMask(stages);
    this->dstLastUsed = false;
    return *this;
}

PipelineBarrierBuilder& PipelineBarrierBuilder::whichUses(vk::AccessFlags2 flags) {
    if (this->dstLastUsed) {
        coreBarrier.setDstAccessMask(flags);
    } else {
        coreBarrier.setSrcAccessMask(flags);
    }
    return *this;
}

PipelineBarrierBuilder& PipelineBarrierBuilder::beforeDoing(vk::PipelineStageFlags2 stages) {
    coreBarrier.setDstStageMask(stages);
    this->dstLastUsed = true;
    return *this;
}

PipelineBarrierBuilder& PipelineBarrierBuilder::withInitialLayout(vk::ImageLayout initial_layout) {
    this->initialLayout = initial_layout;
    return *this;
}

PipelineBarrierBuilder& PipelineBarrierBuilder::withFinalLayout(vk::ImageLayout final_layout) {
    this->finalLayout = final_layout;
    return *this;
}

PipelineBarrierBuilder& PipelineBarrierBuilder::withSrcQueueFamily(uint32_t index) {
    this->initialQueueFamilyIndex = index;
    return *this;
}

PipelineBarrierBuilder& PipelineBarrierBuilder::withDstQueueFamily(uint32_t index) {
    this->finalQueueFamilyIndex = index;
    return *this;
}

PipelineBarrier PipelineBarrierBuilder::build() {
    PipelineBarrier barrier {};
    if (this->image.has_value()) {
        auto image_barrier = createImageBarrier();
        barrier.image_memory_barriers.push_back(image_barrier);
    } else if (this->buffer.has_value()) {
        auto buffer_barrier = createBufferBarrer();
        barrier.buffer_memory_barriers.push_back(buffer_barrier);
    }
    barrier.memory_barriers.push_back(coreBarrier);
    return barrier;
}

vk::ImageMemoryBarrier2 PipelineBarrierBuilder::createImageBarrier() {
    vk::ImageMemoryBarrier2 image_memory_barrier2 {};
    if (this->initialLayout.has_value() && this->finalLayout.has_value()) {
        image_memory_barrier2.oldLayout = *this->initialLayout;
        image_memory_barrier2.newLayout = *this->finalLayout;
    }
    image_memory_barrier2.setDstAccessMask(coreBarrier.dstAccessMask);
    image_memory_barrier2.setSrcAccessMask(coreBarrier.srcAccessMask);
    image_memory_barrier2.setSrcStageMask(coreBarrier.srcStageMask);
    image_memory_barrier2.setDstStageMask(coreBarrier.dstStageMask);
    image_memory_barrier2.setImage(image->image);
    image_memory_barrier2.setSubresourceRange(image->subresource_layout);
    if (initialQueueFamilyIndex.has_value() && finalQueueFamilyIndex.has_value()) {
        image_memory_barrier2.setSrcQueueFamilyIndex(*initialQueueFamilyIndex);
        image_memory_barrier2.setDstQueueFamilyIndex(*finalQueueFamilyIndex);
    }

    return image_memory_barrier2;
}

vk::BufferMemoryBarrier2 PipelineBarrierBuilder::createBufferBarrer() {
    vk::BufferMemoryBarrier2 buffer_memory_barrier2 {};
    buffer_memory_barrier2.setBuffer(buffer->buffer);
    buffer_memory_barrier2.setSize(buffer->size);
    buffer_memory_barrier2.setOffset(buffer->offset);
    buffer_memory_barrier2.setDstAccessMask(coreBarrier.dstAccessMask);
    buffer_memory_barrier2.setSrcAccessMask(coreBarrier.srcAccessMask);
    buffer_memory_barrier2.setSrcStageMask(coreBarrier.srcStageMask);
    buffer_memory_barrier2.setDstStageMask(coreBarrier.dstStageMask);
    if (initialQueueFamilyIndex.has_value() && finalQueueFamilyIndex.has_value()) {
        buffer_memory_barrier2.setSrcQueueFamilyIndex(*initialQueueFamilyIndex);
        buffer_memory_barrier2.setDstQueueFamilyIndex(*finalQueueFamilyIndex);
    }

    return buffer_memory_barrier2;
}
