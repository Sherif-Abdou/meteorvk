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

PipelineBarrier PipelineBarrierBuilder::build() {
    PipelineBarrier barrier {};
    if (this->image.has_value()) {
        auto image_barrier = createImageBarrier();
        barrier.image_memory_barriers.push_back(image_barrier);
    }
    barrier.memory_barriers.push_back(coreBarrier);
    return barrier;
}

vk::ImageMemoryBarrier2 PipelineBarrierBuilder::createImageBarrier() {
    vk::ImageMemoryBarrier2 image_memory_barrier2 {};
    image_memory_barrier2.oldLayout = *this->initialLayout;
    image_memory_barrier2.newLayout = *this->finalLayout;
    image_memory_barrier2.setDstAccessMask(coreBarrier.dstAccessMask);
    image_memory_barrier2.setSrcAccessMask(coreBarrier.srcAccessMask);
    image_memory_barrier2.setSrcStageMask(coreBarrier.srcStageMask);
    image_memory_barrier2.setDstStageMask(coreBarrier.dstStageMask);
    image_memory_barrier2.setImage(image->image);
    image_memory_barrier2.setSubresourceRange(image->subresource_layout);

    return image_memory_barrier2;
}
