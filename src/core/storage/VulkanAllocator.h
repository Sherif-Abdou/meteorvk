//
// Created by Sherif Abdou on 7/30/23.
//

#ifndef VULKAN_TEST_VULKANALLOCATOR_H
#define VULKAN_TEST_VULKANALLOCATOR_H

#include <vma/vk_mem_alloc.h>
#include "core/VulkanContext.h"

static int allocationCount;
static int destructionCount;

class VulkanAllocator {
public:
    void init();

    VulkanContext* context;
    struct VulkanBufferAllocation {
        vk::Buffer buffer;
        VmaAllocation allocation;

        VmaAllocator* allocator;

        void* mapMemory() {
            void* data;
            vmaMapMemory(*allocator, allocation, &data);
            return data;
        }

        void unmapMemory() {
            vmaUnmapMemory(*allocator, allocation);
        }

        void destroy() {
            vmaDestroyBuffer(*allocator, buffer, allocation);
            destructionCount += 1;
        };
    };
    struct VulkanImageAllocation {
        vk::Image image;
        VmaAllocation allocation;

        VmaAllocator* allocator;

        void* mapMemory() {
            void* data;
            vmaMapMemory(*allocator, allocation, &data);
            return data;
        }

        void unmapMemory() {
            vmaUnmapMemory(*allocator, allocation);
        }

        void destroy() {
            vmaDestroyImage(*allocator, image, allocation);
            destructionCount += 1;
        }
    };
    void allocateBuffer(VkBufferCreateInfo*, VmaMemoryUsage, VulkanBufferAllocation*);
    void allocateImage(VkImageCreateInfo*, VmaMemoryUsage, VulkanImageAllocation*);

    void destroy() {
        vmaDestroyAllocator(allocator);
    }
private:
    VmaAllocator allocator;
    void initAllocator();
};


#endif //VULKAN_TEST_VULKANALLOCATOR_H
