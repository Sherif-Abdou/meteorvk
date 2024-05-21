//
// Created by Sherif Abdou on 11/13/23.
//

#ifndef PIPELINEBARRIERBUILDER_H
#define PIPELINEBARRIERBUILDER_H
#include "core/compute_pipeline/ComputePipeline.h"
#include "PipelineBarrier.h"


class PipelineBarrierBuilder {
public:
    /// Specifies the image that the barrier is for
    PipelineBarrierBuilder& forImage(vk::Image, vk::ImageSubresourceRange);
    /// Specifies the buffer that the barrier is for
    PipelineBarrierBuilder& forBuffer(vk::Buffer buffer, vk::DeviceSize size, vk::DeviceSize offset);
    /// Specifies which pipeline stages the barrier waits for
    PipelineBarrierBuilder& waitFor(vk::PipelineStageFlags2);
    /// Specifies what memory resources the source or destination of the barrier uses
    PipelineBarrierBuilder& whichUses(vk::AccessFlags2);
    /// Specifies which pipeline stages the barrier waits before starting
    PipelineBarrierBuilder& beforeDoing(vk::PipelineStageFlags2);
    /// Specifies the initial layout of an image for a layout change
    PipelineBarrierBuilder& withInitialLayout(vk::ImageLayout);
    /// Specifies the final layout of an image for a layout change
    PipelineBarrierBuilder& withFinalLayout(vk::ImageLayout);
    /// Specifies the initial queue of a resource for a quuee ownership change
    PipelineBarrierBuilder& withSrcQueueFamily(uint32_t);
    /// Specifies the final queue of a resource for a quuee ownership change
    PipelineBarrierBuilder& withDstQueueFamily(uint32_t);

    PipelineBarrier build();
private:
    struct ImageData {
        vk::Image image;
        vk::ImageSubresourceRange subresource_layout;
    };

    struct BufferData {
        vk::Buffer buffer;
        vk::DeviceSize offset;
        vk::DeviceSize size;
    };
    bool dstLastUsed = false;
    vk::MemoryBarrier2 coreBarrier {};
    std::optional<uint32_t> initialQueueFamilyIndex;
    std::optional<uint32_t> finalQueueFamilyIndex;
    std::optional<vk::ImageLayout> initialLayout;
    std::optional<vk::ImageLayout> finalLayout;
    std::optional<ImageData> image;
    std::optional<BufferData> buffer;

    vk::ImageMemoryBarrier2 createImageBarrier();
    vk::BufferMemoryBarrier2 createBufferBarrer();
};



#endif //PIPELINEBARRIERBUILDER_H
