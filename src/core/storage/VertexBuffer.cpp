//
// Created by Sherif Abdou on 8/1/23.
//

#include "VertexBuffer.h"

#include "../shared_pipeline/PipelineBarrierBuilder.h"
#include "../shared_pipeline/TransferQueue.h"

std::mutex VertexBuffer::submission_mutex {};

void VertexBuffer::init() {
    initializeVertexBuffer();
    updateVertexBuffer();
}

void VertexBuffer::attachToCommandBuffer(vk::raii::CommandBuffer &buffer) {
    vk::Buffer vertexBuffers[] = {vertexBuffer.buffer};
    vk::DeviceSize offsets[] = {0};
    buffer.bindVertexBuffers(0, vertexBuffers, offsets);
    if (use_index_buffer) {
        buffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);
    }
}

void VertexBuffer::initializeVertexBuffer() {
    if (vertices.size() == 0) {
        throw std::runtime_error("Can't initialize empty buffer(missing vertices)");
    }
    vertex_count = vertices.size();
    VkBufferCreateInfo coreBuffer {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    coreBuffer.size = sizeof (vertices[0]) * vertices.size();
    coreBuffer.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (canBeStorage) {
        coreBuffer.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    if (use_staging_buffer) {
        coreBuffer.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    coreBuffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (!use_staging_buffer) {
        context->allocator->allocateBuffer(&coreBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, &vertexBuffer);
    } else {
        context->allocator->allocateBuffer(&coreBuffer, VMA_MEMORY_USAGE_GPU_ONLY, &vertexBuffer);
        coreBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        context->allocator->allocateBuffer(&coreBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, &vertexStagingBuffer);
    }

    if (indices.has_value()) {
        initializeIndexBuffer();
        use_index_buffer = true;
    }
}

void VertexBuffer::initializeIndexBuffer() {
    if (indices->size() == 0) {
        throw std::runtime_error("Can't initialize empty buffer(missing indices)");
    }
    index_count = indices->size();
    VkBufferCreateInfo coreBuffer {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    coreBuffer.size = sizeof ((*indices)[0]) * indices->size();
    coreBuffer.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (canBeStorage) {
        coreBuffer.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    if (use_staging_buffer) {
        coreBuffer.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    coreBuffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (!use_staging_buffer) {
        context->allocator->allocateBuffer(&coreBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, &indexBuffer);
    } else {
        context->allocator->allocateBuffer(&coreBuffer, VMA_MEMORY_USAGE_GPU_ONLY, &indexBuffer);
        coreBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        context->allocator->allocateBuffer(&coreBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, &indexStagingBuffer);
    }
}

void VertexBuffer::updateVertexBuffer() {
    if (use_staging_buffer) {
        Vertex::writeVectorToBuffer(vertices, vertexStagingBuffer);
        TransferQueue transfer_queue(context);
        transfer_queue.begin();

        vk::BufferCopy copy_region {};

        copy_region.setSize(sizeof (Vertex) * vertex_count);
        copy_region.setDstOffset(0);
        copy_region.setSrcOffset(0);

        transfer_queue.copy(vertexStagingBuffer.buffer, vertexBuffer.buffer, copy_region);

        submission_mutex.lock();
        transfer_queue.submit();
        submission_mutex.unlock();
    } else {
        Vertex::writeVectorToBuffer(vertices, vertexBuffer);
    }

    if (indices.has_value()) {
        updateIndexBuffer();
    }

    vertices.clear();
}

void VertexBuffer::updateIndexBuffer() {
    if (use_staging_buffer) {
        void* memory = indexStagingBuffer.mapMemory();
        auto bytes = sizeof((*indices)[0]) * indices->size();
        memcpy(memory, indices->data(), bytes);
        indexStagingBuffer.unmapMemory();

        TransferQueue transfer_queue(context);
        transfer_queue.begin();

        vk::BufferCopy copy_region {};

        copy_region.setSize(sizeof ((*indices)[0]) * index_count);
        copy_region.setDstOffset(0);
        copy_region.setSrcOffset(0);

        transfer_queue.copy(indexStagingBuffer.buffer, indexBuffer.buffer, copy_region);

        submission_mutex.lock();
        transfer_queue.submit();
        submission_mutex.unlock();
    } else {
        void* memory = indexBuffer.mapMemory();
        auto bytes = sizeof((*indices)[0]) * indices->size();
        memcpy(memory, indices->data(), bytes);
        indexBuffer.unmapMemory();
    }

    indices->clear();
}

void VertexBuffer::destroy() {
    vertexBuffer.destroy();
    if (use_staging_buffer) {
        vertexStagingBuffer.destroy();
    }
    if (use_index_buffer) {
        if (use_staging_buffer) {
            indexStagingBuffer.destroy();
        }
        indexBuffer.destroy();
    }
}

VertexBuffer::VertexBuffer(VulkanContext *context, bool staging_buffer) : context(context), use_staging_buffer(staging_buffer) {}

void VertexBuffer::draw(vk::raii::CommandBuffer &buffer) {
    attachToCommandBuffer(buffer);
    if (use_index_buffer) {
        buffer.drawIndexed(index_count, 1, 0, 0, 0);
    } else {
        buffer.draw(vertex_count, 1, 0, 0);
    }
}

IndirectCallStruct VertexBuffer::createBasicIndirectCall() {
    return {
        static_cast<uint32_t>(index_count),
        1,
        0,
        0,
        0
    };
}

VertexBuffer::~VertexBuffer() {
}

void VertexBuffer::draw_indirect(vk::raii::CommandBuffer &command_buffer, vk::Buffer &draw_buffer, uint64_t offset) {
    attachToCommandBuffer(command_buffer);
    if (use_index_buffer) {
        command_buffer.drawIndexedIndirect(draw_buffer, offset, 1, 0);
    } else {
        command_buffer.drawIndirect(draw_buffer, offset, 1, 0);
    }
}

vk::Buffer *VertexBuffer::getBuffer() {
    return &vertexBuffer.buffer;
}

vk::DeviceSize VertexBuffer::getSize() {
    return sizeof (Vertex) * vertex_count;
}

uint32_t VertexBuffer::getVertexCount() {
    if (use_index_buffer) {
        return index_count;
    }
    return vertex_count;
}

