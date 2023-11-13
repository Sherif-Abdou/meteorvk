//
// Created by Sherif Abdou on 11/13/23.
//

#ifndef PIPELINEBARRIERBUILDER_H
#define PIPELINEBARRIERBUILDER_H
#include "../compute_pipeline/ComputePipeline.h"
#include "PipelineBarrier.h"


class PipelineBarrierBuilder {
public:
    PipelineBarrierBuilder& forImage(vk::Image, vk::ImageSubresourceRange);
    PipelineBarrierBuilder& waitFor(vk::PipelineStageFlags2);
    PipelineBarrierBuilder& whichUses(vk::AccessFlags2);
    PipelineBarrierBuilder& beforeDoing(vk::PipelineStageFlags2);
    PipelineBarrierBuilder& withInitialLayout(vk::ImageLayout);
    PipelineBarrierBuilder& withFinalLayout(vk::ImageLayout);

    PipelineBarrier build();
private:
    struct ImageData {
        vk::Image image;
        vk::ImageSubresourceRange subresource_layout;
    };
    bool dstLastUsed = false;
    vk::MemoryBarrier2 coreBarrier {};
    std::optional<vk::ImageLayout> initialLayout;
    std::optional<vk::ImageLayout> finalLayout;
    std::optional<ImageData> image;

    vk::ImageMemoryBarrier2 createImageBarrier();
};



#endif //PIPELINEBARRIERBUILDER_H
