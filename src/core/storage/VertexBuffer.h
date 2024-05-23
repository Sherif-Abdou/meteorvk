//
// Created by Sherif Abdou on 8/1/23.
//

#ifndef VULKAN_RENDERER_VERTEXBUFFER_H
#define VULKAN_RENDERER_VERTEXBUFFER_H

#include "Vertex.h"
#include "core/interfaces/IndirectCallStruct.h"
#include "core/storage/DescriptorSet.h"
#include "core_v2/NewDescriptorManager.h"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_raii.hpp"

#include <mutex>

class VertexBuffer {
public:
    VulkanContext* context;
    bool canBeStorage = true;

    explicit VertexBuffer(VulkanContext *context, bool staging_buffer = false);

    std::vector<Vertex> vertices {};
    std::optional<std::vector<uint32_t>> indices {};
    unsigned long mask = 0;
    void init();
    void updateVertexBuffer();
    void attachToCommandBuffer(vk::raii::CommandBuffer &buffer);
    void draw(vk::raii::CommandBuffer &buffer);
    void draw_indirect(vk::raii::CommandBuffer& command_buffer, vk::Buffer& draw_buffer, vk::DeviceSize offset = 0);
    IndirectCallStruct createBasicIndirectCall();
    void destroy();

    vk::Buffer* getBuffer();
    vk::DeviceSize getSize();
    uint32_t getVertexCount();

    VertexBuffer(VertexBuffer&&) = default;

    virtual ~VertexBuffer();

    // Mask Constants
    constexpr static unsigned long DeferredQuadBit = 1 << 1;
    constexpr static unsigned long TransparentBit = 1 << 2;

    static std::mutex submission_mutex;

    void createModelDescriptorSet(NewDescriptorManager* manager);
    void tryBindModelDescriptorSet(vk::raii::CommandBuffer* buffer, vk::raii::PipelineLayout* layout);

    DescriptorSet* getModelDescriptorSet();
private:
    unsigned long vertex_count = 0;
    unsigned long index_count = 0;
    void initializeVertexBuffer();
    void initializeIndexBuffer();
    VulkanAllocator::VulkanBufferAllocation vertexBuffer;
    VulkanAllocator::VulkanBufferAllocation indexBuffer;
    VulkanAllocator::VulkanBufferAllocation vertexStagingBuffer;
    VulkanAllocator::VulkanBufferAllocation indexStagingBuffer;

    DescriptorSet* model_descriptor_set = nullptr;

    bool use_staging_buffer = false;
    bool use_index_buffer = false;

    void updateIndexBuffer();
};


#endif //VULKAN_RENDERER_VERTEXBUFFER_H
