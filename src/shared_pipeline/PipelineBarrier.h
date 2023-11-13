//
// Created by Sherif Abdou on 11/11/23.
//

#ifndef VULKAN_ENGINE_PIPELINEBARRIER_H
#define VULKAN_ENGINE_PIPELINEBARRIER_H

#include <vulkan/vulkan.hpp>

class PipelineBarrier {
public:
    void applyBarrier(vk::CommandBuffer buffer);
    vk::DependencyInfo info {};
    std::vector<vk::MemoryBarrier2> memory_barriers {};
    std::vector<vk::ImageMemoryBarrier2> image_memory_barriers {};
    std::vector<vk::BufferMemoryBarrier2> buffer_memory_barriers {};
};


#endif //VULKAN_ENGINE_PIPELINEBARRIER_H
