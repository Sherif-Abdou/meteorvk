//
// Created by Sherif Abdou on 8/1/23.
//

#include "VertexBuffer.h"

void VertexBuffer::init() {
    initializeVertexBuffer();
}

void VertexBuffer::attachToCommandBuffer(vk::raii::CommandBuffer &buffer) {
    vk::Buffer vertexBuffers[] = {vertexBuffer.buffer};
    vk::DeviceSize offsets[] = {0};
    buffer.bindVertexBuffers(0, vertexBuffers, offsets);
}

void VertexBuffer::initializeVertexBuffer() {
    if (vertices.size() == 0) {
        throw std::runtime_error("Can't initialize empty buffer(missing vertices)");
    }
    VkBufferCreateInfo createInfo {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    createInfo.size = sizeof (vertices[0]) * vertices.size();
    createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    allocator.allocateBuffer(&createInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, &vertexBuffer);
}

void VertexBuffer::updateVertexBuffer() {
    Vertex::writeVectorToBuffer(vertices, vertexBuffer);
}

void VertexBuffer::destroy() {
    vertexBuffer.destroy();
}

VertexBuffer::VertexBuffer(VulkanAllocator &allocator) : allocator(allocator) {}

void VertexBuffer::draw(vk::raii::CommandBuffer &buffer) {
    attachToCommandBuffer(buffer);
    buffer.draw(vertices.size(), 1, 0, 0);
}

VertexBuffer::~VertexBuffer() {
}
