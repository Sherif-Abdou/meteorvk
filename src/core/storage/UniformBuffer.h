//
// Created by Sherif Abdou on 8/31/23.
//

#ifndef VULKAN_ENGINE_UNIFORMBUFFER_H
#define VULKAN_ENGINE_UNIFORMBUFFER_H

#include <vulkan/vulkan_raii.hpp>
#include "../VulkanContext.h"
#include "DescriptorSet.h"

template<typename T>
class UniformBuffer {
    VulkanContext* context;
    VulkanAllocator::VulkanBufferAllocation bufferAllocation;
protected:
    vk::DescriptorType descriptorType = vk::DescriptorType::eUniformBuffer;
    VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
public:
    uint32_t count = 1;
    explicit UniformBuffer(VulkanContext *context) : context(context) {}
//    UniformBuffer(UniformBuffer&&) = default;
//    UniformBuffer& operator=(UniformBuffer&&) = default;

    void allocateBuffer();
    void updateBuffer(const T&, uint32_t index = 0);
    void writeToDescriptor(DescriptorSet& descriptorSet, uint32_t binding = 0);
    void writePartialToDescriptor(DescriptorSet& descriptorSet, vk::DeviceSize range, vk::DeviceSize offset, uint32_t binding = 0);

    void* mapMemory();
    void unMapMemory();

    vk::Buffer* getBuffer();

    void destroy();
};

template<typename T>
vk::Buffer *UniformBuffer<T>::getBuffer() {
    return &bufferAllocation.buffer;
}

template<typename T>
void
UniformBuffer<T>::writePartialToDescriptor(DescriptorSet &descriptorSet, vk::DeviceSize range, vk::DeviceSize offset,
                                           uint32_t binding) {
    vk::WriteDescriptorSet writeDescriptorSet {};
    vk::DescriptorBufferInfo bufferInfo {};
    bufferInfo.setOffset(offset);
    bufferInfo.setRange(range);
    bufferInfo.setBuffer(bufferAllocation.buffer);

    writeDescriptorSet.setDescriptorCount(1);
    writeDescriptorSet.setDstBinding(binding);
    writeDescriptorSet.setDstArrayElement(0);
    writeDescriptorSet.setDescriptorType(descriptorType);
    writeDescriptorSet.setDstSet(descriptorSet.getDescriptorSet());
    writeDescriptorSet.setBufferInfo(bufferInfo);
//    writeDescriptorSet.setDstSet();

    context->device.updateDescriptorSets(writeDescriptorSet, {});
}

template<typename T>
void UniformBuffer<T>::destroy() {
    bufferAllocation.destroy();
}

template<typename T>
void UniformBuffer<T>::writeToDescriptor(DescriptorSet& descriptorSet, uint32_t binding) {
    writePartialToDescriptor(descriptorSet, sizeof (T), 0, binding);
}

template<typename T>
void* UniformBuffer<T>::mapMemory() {
    return bufferAllocation.mapMemory();
}

template<typename T>
void UniformBuffer<T>::unMapMemory() {
    bufferAllocation.unmapMemory();
}

template<typename T>
void UniformBuffer<T>::updateBuffer(const T & data, uint32_t index) {
    T* address = (T*)bufferAllocation.mapMemory();
    memcpy(address + index, &data, sizeof(T));
    bufferAllocation.unmapMemory();
}

template<typename T>
void UniformBuffer<T>::allocateBuffer() {
    VkBufferCreateInfo bufferCreateInfo {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (usageFlags == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        // bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    }
    bufferCreateInfo.size = sizeof(T) * count;
    bufferCreateInfo.usage = usageFlags;

    context->allocator->allocateBuffer(&bufferCreateInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, &bufferAllocation);
}


#endif //VULKAN_ENGINE_UNIFORMBUFFER_H
