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
    VulkanContext& context;
    VulkanAllocator::VulkanBufferAllocation bufferAllocation;
protected:
    vk::DescriptorType descriptorType = vk::DescriptorType::eUniformBuffer;
    VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
public:
    explicit UniformBuffer(VulkanContext &context) : context(context) {}

    void allocateBuffer();
    void updateBuffer(const T&);
    void writeToDescriptor(DescriptorSet& descriptorSet, uint32_t binding = 0);

    void destroy();
};

template<typename T>
void UniformBuffer<T>::destroy() {
    bufferAllocation.destroy();
}

template<typename T>
void UniformBuffer<T>::writeToDescriptor(DescriptorSet& descriptorSet, uint32_t binding) {
    vk::WriteDescriptorSet writeDescriptorSet {};
    vk::DescriptorBufferInfo bufferInfo {};
    bufferInfo.setOffset(0);
    bufferInfo.setRange(sizeof(T));
    bufferInfo.setBuffer(bufferAllocation.buffer);

    writeDescriptorSet.setDescriptorCount(1);
    writeDescriptorSet.setDstBinding(binding);
    writeDescriptorSet.setDstArrayElement(0);
    writeDescriptorSet.setDescriptorType(vk::DescriptorType::eUniformBuffer);
    writeDescriptorSet.setDstSet(descriptorSet.getDescriptorSet());
    writeDescriptorSet.setBufferInfo(bufferInfo);
//    writeDescriptorSet.setDstSet();

    context.device.updateDescriptorSets(writeDescriptorSet, {});
}

template<typename T>
void UniformBuffer<T>::updateBuffer(const T & data) {
    void* address = bufferAllocation.mapMemory();
    memcpy(address, &data, sizeof(T));
    bufferAllocation.unmapMemory();
}

template<typename T>
void UniformBuffer<T>::allocateBuffer() {
    VkBufferCreateInfo bufferCreateInfo {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (usageFlags == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    }
    bufferCreateInfo.size = sizeof(T);
    bufferCreateInfo.usage = usageFlags;

    context.allocator->allocateBuffer(&bufferCreateInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, &bufferAllocation);
}


#endif //VULKAN_ENGINE_UNIFORMBUFFER_H
