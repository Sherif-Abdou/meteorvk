//
// Created by Sherif Abdou on 7/30/23.
//

#include "VulkanAllocator.h"

void VulkanAllocator::initAllocator() {
    VmaAllocatorCreateInfo createInfo {};
    createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    createInfo.device = *context->device;
    createInfo.physicalDevice = *context->physicalDevice;
    createInfo.instance = *context->instance;

    if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't initialize VMA allocator");
    }
}

void VulkanAllocator::allocateBuffer(VkBufferCreateInfo* bufferCreateInfo, VmaMemoryUsage memoryUsage, VulkanAllocator::VulkanBufferAllocation* allocation) {
    *allocation = {};
    allocation->allocator = &allocator;

    VmaAllocationCreateInfo allocationCreateInfo {};
    if (memoryUsage == VMA_MEMORY_USAGE_CPU_TO_GPU) {
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    } else {
        allocationCreateInfo.usage = memoryUsage;
    }

    VkBuffer tmpBuffer;
    if (vmaCreateBuffer(allocator, bufferCreateInfo, &allocationCreateInfo, &tmpBuffer, &allocation->allocation,
                        nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate a buffer");
    }
    allocation->buffer = tmpBuffer;
}

void VulkanAllocator::init() {
    initAllocator();
}

void VulkanAllocator::allocateImage(VkImageCreateInfo * imageCreateInfo, VmaMemoryUsage memoryUsage, VulkanAllocator::VulkanImageAllocation * allocation) {
    *allocation = {};
    allocation->allocator = &allocator;

    VmaAllocationCreateInfo allocationCreateInfo {};
    allocationCreateInfo.usage = memoryUsage;

    VkImage tmpImage;
    if (vmaCreateImage(allocator, imageCreateInfo, &allocationCreateInfo, &tmpImage, &allocation->allocation,
                       nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate an image");
    }
    allocation->image = tmpImage;
}

