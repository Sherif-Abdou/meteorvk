//
// Created by Sherif Abdou on 9/19/23.
//

#ifndef VULKAN_ENGINE_DYNAMICUNIFORMBUFFER_H
#define VULKAN_ENGINE_DYNAMICUNIFORMBUFFER_H

#include <glm/glm.hpp>
#include "UniformBuffer.h"

/// Wrapper around a Dynamic Uniform Buffer
/// Handles buffer storage and generating a dynamic offset
template<typename T>
class DynamicUniformBuffer {
    VulkanContext& context;
    VulkanAllocator::VulkanBufferAllocation bufferAllocation;
    unsigned int buffer_count = 0;
    size_t dynamicAlignment = 0;

public:
    explicit DynamicUniformBuffer(VulkanContext &context) : context(context) {}

    void allocateBuffer(unsigned int buffer_count) {
        auto device_properties = this->context.physicalDevice.getProperties();
        size_t minUboAlignment = device_properties.limits.minUniformBufferOffsetAlignment;
        dynamicAlignment = sizeof(T);
        if (minUboAlignment > 0) {
            // Black magic math from Sascha Willems, need to check what this actually does
            dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
        }


        VkBufferCreateInfo bufferCreateInfo {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.size = buffer_count * dynamicAlignment;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        this->context.allocator->allocateBuffer(&bufferCreateInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, &this->bufferAllocation);
        this->buffer_count = buffer_count;
    }

    void updateBuffer(const T & data, unsigned int index) {
        assert(index < buffer_count);

        char* address = (char*)bufferAllocation.mapMemory(); // Use char* to allow for pointer arithmetic
        memcpy((address + dynamicAlignment * index), &data, sizeof(T)); // This could go horribly wrong
        bufferAllocation.unmapMemory();
    }

    void writeToDescriptor(DescriptorSet& descriptorSet, uint32_t binding) {
        vk::WriteDescriptorSet writeDescriptorSet {};
        vk::DescriptorBufferInfo bufferInfo {};
        bufferInfo.setOffset(0); // Writing to the entire descriptor, should optimize this in the future
        bufferInfo.setRange(dynamicAlignment);
        bufferInfo.setBuffer(bufferAllocation.buffer);

        writeDescriptorSet.setDescriptorCount(1);
        writeDescriptorSet.setDstBinding(binding);
        writeDescriptorSet.setDstArrayElement(0);
        writeDescriptorSet.setDescriptorType(vk::DescriptorType::eUniformBufferDynamic);
        writeDescriptorSet.setDstSet(descriptorSet.getDescriptorSet());
        writeDescriptorSet.setBufferInfo(bufferInfo);
//    writeDescriptorSet.setDstSet();

        context.device.updateDescriptorSets(writeDescriptorSet, {});
    }

    /// Gets the dynamic offset to use to a particular index in the dynamic buffer
    unsigned long getOffsetForIndex(unsigned int index) {
        return dynamicAlignment * index;
    }

    void destroy() {
        bufferAllocation.destroy();
    }
};


#endif //VULKAN_ENGINE_DYNAMICUNIFORMBUFFER_H
