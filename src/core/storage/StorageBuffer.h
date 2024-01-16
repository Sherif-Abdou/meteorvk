//
// Created by Sherif Abdou on 11/3/23.
//

#ifndef VULKAN_ENGINE_STORAGEBUFFER_H
#define VULKAN_ENGINE_STORAGEBUFFER_H


#include "UniformBuffer.h"

template <typename T>
class StorageBuffer: public UniformBuffer<T> {
public:
    explicit StorageBuffer(VulkanContext *context, bool indirectBuffer = false) : UniformBuffer<T>(context) {
        this->descriptorType = vk::DescriptorType::eStorageBuffer;
        this->usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (indirectBuffer) {
            this->usageFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }
    }
};


#endif //VULKAN_ENGINE_STORAGEBUFFER_H
