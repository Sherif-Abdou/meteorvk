//
// Created by Sherif Abdou on 8/1/23.
//

#ifndef VULKAN_RENDERER_VERTEXBUFFER_H
#define VULKAN_RENDERER_VERTEXBUFFER_H

#include "Vertex.h"

class VertexBuffer {
public:
    VulkanAllocator& allocator;

    explicit VertexBuffer(VulkanAllocator &allocator);

    std::vector<Vertex> vertices {};
    void init();
    void updateVertexBuffer();
    void attachToCommandBuffer(vk::raii::CommandBuffer &buffer);
    void draw(vk::raii::CommandBuffer &buffer);
    void destroy();
private:
    void initializeVertexBuffer();
    VulkanAllocator::VulkanBufferAllocation vertexBuffer;
};


#endif //VULKAN_RENDERER_VERTEXBUFFER_H
