//
// Created by Sherif Abdou on 8/1/23.
//

#ifndef VULKAN_RENDERER_VERTEXBUFFER_H
#define VULKAN_RENDERER_VERTEXBUFFER_H

#include "Vertex.h"

class VertexBuffer {
public:
    VulkanContext& context;

    explicit VertexBuffer(VulkanContext &context, bool staging_buffer = false);

    std::vector<Vertex> vertices {};
    unsigned long mask = 0;
    void init();
    void updateVertexBuffer();
    void attachToCommandBuffer(vk::raii::CommandBuffer &buffer);
    void draw(vk::raii::CommandBuffer &buffer);
    void destroy();

    VertexBuffer(VertexBuffer&&) = default;

    virtual ~VertexBuffer();

    // Mask Constants
    constexpr static unsigned long DeferredQuadBit = 1 << 1;
    constexpr static unsigned long TransparentBit = 1 << 2;
private:
    void initializeVertexBuffer();
    VulkanAllocator::VulkanBufferAllocation vertexBuffer;
    VulkanAllocator::VulkanBufferAllocation stagingBuffer;

    void loadStagingBuffer();

    bool use_staging_buffer = false;
};


#endif //VULKAN_RENDERER_VERTEXBUFFER_H
