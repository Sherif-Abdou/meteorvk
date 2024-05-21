//
// Created by Sherif Abdou on 11/20/23.
//

#ifndef IMAGETEXTURE_H
#define IMAGETEXTURE_H
#include "core/storage/VulkanAllocator.h"


class ImageTextureLoader {
private:
    int width, height, channels;

    VulkanAllocator::VulkanBufferAllocation stagingBuffer;
    VulkanAllocator::VulkanImageAllocation imageAllocation;
    VulkanContext* context;
public:
    ImageTextureLoader(VulkanContext *context);

private:
    void* pixels;

    void loadTransferQueue();
public:
    VulkanAllocator::VulkanImageAllocation loadImageFromFile(const char* file_path);
    VulkanAllocator::VulkanImageAllocation createImageFromBuffer(VkFormat, uint32_t width, uint32_t height, void* data, size_t n);

};



#endif //IMAGETEXTURE_H
