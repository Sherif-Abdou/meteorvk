//
// Created by Sherif Abdou on 11/20/23.
//

#ifndef IMAGETEXTURE_H
#define IMAGETEXTURE_H
#include "../../core/storage/VulkanAllocator.h"


class ImageTextureLoader {
private:
    int width, height, channels;

    VulkanAllocator::VulkanBufferAllocation stagingBuffer;
    VulkanAllocator::VulkanImageAllocation imageAllocation;
    VulkanContext& context;
public:
    explicit ImageTextureLoader(VulkanContext &context);

private:
    void* pixels;

    void loadTransferQueue();
public:
    VulkanAllocator::VulkanImageAllocation loadImageFromFile(const char* file_path);

};



#endif //IMAGETEXTURE_H
